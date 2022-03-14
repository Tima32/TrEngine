#include <windows.h>
#include <Memoryapi.h>

#include <iostream>
#include <vector>

#include "..\include\TrEngine\HookInstruction.hpp"


using namespace std;
using namespace TrEngine;

HookInstruction::HookInstruction()
{}
HookInstruction::~HookInstruction()
{
	deattach();
	free();
}

bool HookInstruction::init(void* addr, uint8_t tramplin_size, HookFunction function)
{
	if (tramplin_size < jumpInstLen || MaxInstructionLen * 2 < tramplin_size)
		return false;

	this->addr = addr;
	this->original_opcode_size = tramplin_size;
	this->function = function;

	readOriginalInstructin();
	tramplin = reinterpret_cast<Tramplin*>(findSpaceForTramplin());
	cout << "tramplin: " << tramplin << endl;
	if (tramplin == nullptr)
		return false;

	hook_jmp.offset = int32_t(size_t(tramplin) - size_t(addr) - sizeof(HookJump));
	*tramplin = Tramplin();
	tramplin->addr = uint64_t(function);
	tramplin->ret.offset = int32_t(size_t(addr) - size_t(tramplin) - 8*2 + tramplin_size - sizeof(HookJump));

	return true;
}
bool HookInstruction::attach()
{
	is_attach = true;
	
	DWORD old_protect;
	VirtualProtectEx(GetCurrentProcess(), addr, original_opcode_size, PAGE_EXECUTE_READWRITE, &old_protect);
	uint8_t* jmp = reinterpret_cast<uint8_t*>(addr);
	for (uint8_t i = 0; i < original_opcode_size; ++i)
	{
		jmp[i] = Nop;
	}
	WriteProcessMemory(GetCurrentProcess(), addr, &hook_jmp, sizeof(hook_jmp), nullptr);

	VirtualProtectEx(GetCurrentProcess(), (LPVOID)addr, original_opcode_size, old_protect, &old_protect);

	return true;
}
void HookInstruction::deattach()
{
	if (!is_attach)
		return;

	is_attach = false;
	
	DWORD old_protect;
	VirtualProtectEx(GetCurrentProcess(), addr, original_opcode_size, PAGE_EXECUTE_READWRITE, &old_protect);
	WriteProcessMemory(GetCurrentProcess(), addr, &original_opcode, original_opcode_size, nullptr);
	VirtualProtectEx(GetCurrentProcess(), (LPVOID)addr, original_opcode_size, old_protect, &old_protect);
}

//private

void HookInstruction::readOriginalInstructin()
{
	uint8_t const * const cursor = reinterpret_cast<uint8_t*>(addr);

	for (uint8_t i = 0; i < original_opcode_size; ++i)
		original_opcode[i] = cursor[i];
}
void* HookInstruction::findSpaceForTramplin()
{
	SYSTEM_INFO sys_inf;
	GetSystemInfo(&sys_inf);
	cout << "dwPageSize: " << sys_inf.dwPageSize << endl;

	unsigned char* p = NULL;
	MEMORY_BASIC_INFORMATION info;
	std::vector<MEMORY_BASIC_INFORMATION> mem_list;
	size_t target_region = -1;

	for (p = NULL; auto r = VirtualQuery(p, &info, sizeof(info)); p += info.RegionSize)
	{
		if (info.BaseAddress <= addr && size_t(addr) <= size_t(info.BaseAddress) + info.RegionSize)
		{
			target_region = mem_list.size();
		}
		mem_list.push_back(info);
	}

	void* alloc_mem = nullptr;
	for (size_t i = target_region; i != -1; --i)
	{
		alloc_mem = VirtualAlloc(
			PVOID(size_t(mem_list[i].BaseAddress) - sizeof(Tramplin)), 
			sizeof(Tramplin),
			MEM_RESERVE | MEM_COMMIT,
			PAGE_EXECUTE_READWRITE);

		if (alloc_mem)
			break;
	}
	if (alloc_mem == nullptr)
		return nullptr;

	//проверка переполнения
	if (int64_t(alloc_mem) - int64_t(addr) != int64_t(int32_t(int64_t(alloc_mem) - int64_t(addr))))
	{
		VirtualFree(alloc_mem, 0, MEM_RELEASE);
		return nullptr;
	}

	return alloc_mem;
}

void HookInstruction::free()
{
	VirtualFree(tramplin, 0, MEM_RELEASE);
}