#include <windows.h>
#include <Memoryapi.h>

#include <iostream>
#include <vector>

#include "..\include\TrEngine\HookInstruction.hpp"


using namespace std;
using namespace TrEngine;

//----------------[X64]-------------------//
HookInstructionX64::HookInstructionX64()
{}
HookInstructionX64::~HookInstructionX64()
{
	deattach();
	free();
}

bool HookInstructionX64::init(Process& proc, ExternPointer addr, uint8_t tramplin_size, HookFunction function)
{
	deattach();
	free();
	if (tramplin_size < jumpInstLen || MaxInstructionLen * 2 < tramplin_size)
		return false;

	this->process = &proc;
	this->addr = addr;
	this->original_opcode_size = tramplin_size;
	this->function = function;

	readOriginalInstructin();
	tramplin_addr = findSpaceForTramplin();
	cout << "Tramplin: " << tramplin_addr.p() << endl;
	if (tramplin_addr == nullptr)
		return false;

	hook_jmp.offset = int32_t(size_t(tramplin_addr) - size_t(addr) - sizeof(HookJump));
	
	tramplin.addr = uint64_t(function);
	tramplin.ret.offset = int32_t(size_t(addr) - size_t(tramplin_addr) - 8*2 + tramplin_size - sizeof(HookJump));
	process->write(tramplin_addr, &tramplin, sizeof(tramplin));

	return true;
}
bool HookInstructionX64::attach()
{
	is_attach = true;
	
	DWORD old_protect;
	VirtualProtectEx(process->getHandle(), addr, original_opcode_size, PAGE_EXECUTE_READWRITE, &old_protect);
	//uint8_t* jmp = reinterpret_cast<uint8_t*>(addr);
	for (uint8_t i = 0; i < original_opcode_size; ++i)
	{
		//jmp[i] = Nop;
		process->write(addr + i, &Nop, sizeof(Nop));
	}
	WriteProcessMemory(process->getHandle(), addr, &hook_jmp, sizeof(hook_jmp), nullptr);

	VirtualProtectEx(process->getHandle(), (LPVOID)addr, original_opcode_size, old_protect, &old_protect);

	return true;
}
void HookInstructionX64::deattach()
{
	if (!is_attach)
		return;

	is_attach = false;
	
	DWORD old_protect;
	VirtualProtectEx(process->getHandle(), addr, original_opcode_size, PAGE_EXECUTE_READWRITE, &old_protect);
	WriteProcessMemory(process->getHandle(), addr, &original_opcode, original_opcode_size, nullptr);
	VirtualProtectEx(process->getHandle(), (LPVOID)addr, original_opcode_size, old_protect, &old_protect);
}

//private
void HookInstructionX64::readOriginalInstructin()
{
	/*uint8_t const * const cursor = reinterpret_cast<uint8_t*>(addr);

	for (uint8_t i = 0; i < original_opcode_size; ++i)
		original_opcode[i] = cursor[i];*/

	process->read(addr, original_opcode, original_opcode_size);
}
ExternPointer HookInstructionX64::findSpaceForTramplin()
{
	SYSTEM_INFO sys_inf;
	GetSystemInfo(&sys_inf);
	cout << "dwPageSize: " << sys_inf.dwPageSize << endl;

	unsigned char* p = NULL;
	MEMORY_BASIC_INFORMATION info;
	std::vector<MEMORY_BASIC_INFORMATION> mem_list;
	size_t target_region = -1;

	for (p = NULL; auto r = VirtualQueryEx(process->getHandle(), p, &info, sizeof(info)); p += info.RegionSize)
	{
		if (info.BaseAddress <= addr && size_t(addr) <= size_t(info.BaseAddress) + info.RegionSize)
		{
			target_region = mem_list.size();
		}
		mem_list.push_back(info);
	}

	ExternPointer alloc_mem = nullptr;
	for (size_t i = target_region; i != -1; --i)
	{
		alloc_mem = process->virtualAlloc(size_t(0), sizeof(Tramplin), MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN, PAGE_EXECUTE_READWRITE);
		cout << std::hex << (size_t(mem_list[i].BaseAddress) - sizeof(Tramplin)) << std::dec << " " << GetLastError() << endl;

		if (alloc_mem)
			break;
	}
	if (alloc_mem == nullptr)
		return nullptr;

	//проверка переполнения
	if (int64_t(alloc_mem) - int64_t(addr) != int64_t(int32_t(int64_t(alloc_mem) - int64_t(addr))))
	{
		process->virtualFree(alloc_mem);
		return nullptr;
	}

	return alloc_mem;
}

void HookInstructionX64::free()
{
	if (process != nullptr)
		VirtualFreeEx(process->getHandle(), tramplin_addr, 0, MEM_RELEASE);
	tramplin_addr = nullptr;
}



//----------------[X32]-------------------//
HookInstructionX32::HookInstructionX32()
{}
HookInstructionX32::~HookInstructionX32()
{
	deattach();
}

bool HookInstructionX32::init(Process& proc, ExternPointer addr, uint8_t tramplin_size, HookFunction function)
{
	deattach();
	if (tramplin_size < jumpInstLen || MaxInstructionLen * 2 < tramplin_size)
		return false;

	this->process = &proc;
	this->addr = addr;
	this->original_opcode_size = tramplin_size;
	this->function = function;

	readOriginalInstructin();

	hook_jmp.offset = int32_t(size_t(function) - size_t(addr) - sizeof(HookJump));

	return true;
}
bool HookInstructionX32::attach()
{
	is_attach = true;

	DWORD old_protect;
	VirtualProtectEx(process->getHandle(), addr, original_opcode_size, PAGE_EXECUTE_READWRITE, &old_protect);
	
	for (uint8_t i = 0; i < original_opcode_size; ++i)
	{
		process->write(addr + i, &Nop, sizeof(Nop));
	}
	WriteProcessMemory(process->getHandle(), addr, &hook_jmp, sizeof(hook_jmp), nullptr);

	VirtualProtectEx(process->getHandle(), (LPVOID)addr, original_opcode_size, old_protect, &old_protect);

	return true;
}
void HookInstructionX32::deattach()
{
	if (!is_attach)
		return;

	is_attach = false;

	DWORD old_protect;
	VirtualProtectEx(process->getHandle(), addr, original_opcode_size, PAGE_EXECUTE_READWRITE, &old_protect);
	WriteProcessMemory(process->getHandle(), addr, &original_opcode, original_opcode_size, nullptr);
	VirtualProtectEx(process->getHandle(), (LPVOID)addr, original_opcode_size, old_protect, &old_protect);
}

//private
void HookInstructionX32::readOriginalInstructin()
{
	process->read(addr, original_opcode, original_opcode_size);
}