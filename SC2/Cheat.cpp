#include <iostream>

#include "Cheat.hpp"

#include <Windows.h>
#include <psapi.h>

using namespace std;
using namespace TrEngine;

extern "C"
{
	void ShieldsHookF();
	void ShieldsHookFEnd();

	void HPHookF();
	void HPHookFEnd();

	void HPSubjugationHookF();
	void HPSubjugationHookFEnd();

	void EnergyHookF();
	void EnergyHookFEnd();
}


Cheat::Cheat()
{
	cout << "<Cheat::Cheat>Info: Constructed" << endl;
}
Cheat::~Cheat()
{
	destructor();
}

bool Cheat::open()
{
	if (!process.openProcessByName(L"SC2_x64.exe"))
	{
		return false;
	}
	wcout << "<Cheat::open>Info: Process opened" << endl;
	module_adr = process.findModuleBaseAddress(L"SC2_x64.exe");
	return true;
}
bool Cheat::isOpen() const
{
	auto h = process.getHandle();

	TCHAR buffer[MAX_PATH];
	if (GetModuleFileNameEx(h, 0, buffer, MAX_PATH))
	{
		return true;
	}
	return false;
}
int32_t Cheat::getID() const
{
	return process.getID();
}

bool Cheat::setMinerals(int32_t minerals)
{
	auto minerals_offset_addr = minerals_offset.fingAdderess(module_adr, process);
	return process.write(minerals_offset_addr, &minerals, sizeof(minerals));
}
bool Cheat::getMinerals(int32_t& minerals) const
{
	auto minerals_offset_addr = minerals_offset.fingAdderess(module_adr, process);
	return process.read(minerals_offset_addr, &minerals, sizeof(minerals));
}
bool Cheat::setVespen(int32_t vespen)
{
	auto vespen_offset_addr = vespen_offset.fingAdderess(module_adr, process);
	return process.write(vespen_offset_addr, &vespen, sizeof(vespen));
}
bool Cheat::getVespen(int32_t& vespen) const
{
	auto vespen_offset_addr = vespen_offset.fingAdderess(module_adr, process);
	return process.read(vespen_offset_addr, &vespen, sizeof(vespen));
}

bool Cheat::activeInfiniteShields()
{
	if (shield_inst_mem)
		return true;

	auto shields_inst_addr = shields_inst_offset.fingAdderess(module_adr, process);
	ExternPointer func_begin = HookInstruction::GetRealAddressFunction(ShieldsHookF);
	ExternPointer func_end = HookInstruction::GetRealAddressFunction(ShieldsHookFEnd);
	size_t shield_inst_size = func_end - func_begin;
	shield_inst_mem = process.virtualAlloc(nullptr, shield_inst_size);
	if (!shield_inst_mem)
	{
		cout << "<Cheat:activeInfiniteShields>Error: Error alloc mem " << GetLastError() << endl;
		return false;
	}
	process.write(shield_inst_mem, func_begin, shield_inst_size);
	shields.init(process, shields_inst_addr, 6, TrEngine::HookInstruction::HookFunction(shield_inst_mem.p()));
	shields.attach();
	return true;
}
void Cheat::deactiveInfiniteShields()
{
	if (shield_inst_mem == nullptr)
		return;

	shields.deattach();
	process.virtualFree(shield_inst_mem);
	shield_inst_mem = nullptr;
}

bool Cheat::activeInfiniteHP()
{
	if (hp_inst_mem)
		return true;

	auto hp_inst_addr = hp_inst_offset.fingAdderess(module_adr, process);
	ExternPointer func_begin;
	ExternPointer func_end;

	if (subjugation == false)
	{
		func_begin = HookInstruction::GetRealAddressFunction(HPHookF);
		func_end = HookInstruction::GetRealAddressFunction(HPHookFEnd);
	}
	else
	{
		func_begin = HookInstruction::GetRealAddressFunction(HPSubjugationHookF);
		func_end = HookInstruction::GetRealAddressFunction(HPSubjugationHookFEnd);
	}

	size_t hp_inst_size = func_end - func_begin;
	hp_inst_mem = process.virtualAlloc(nullptr, hp_inst_size);
	if (!hp_inst_mem)
	{
		cout << "<Cheat:activeInfiniteHP>Error: Error alloc mem " << GetLastError() << endl;
		return false;
	}
	process.write(hp_inst_mem, func_begin, hp_inst_size);
	hp.init(process, hp_inst_addr, 6, TrEngine::HookInstruction::HookFunction(hp_inst_mem.p()));
	hp.attach();
	return true;
}
void Cheat::deactiveInfiniteHP()
{
	if (hp_inst_mem == nullptr)
		return;

	hp.deattach();
	process.virtualFree(hp_inst_mem);
	hp_inst_mem = nullptr;
}

bool Cheat::activeSubjugation()
{
	deactiveInfiniteHP();
	subjugation = true;
	return activeInfiniteHP();
}
void Cheat::deactiveSubjugation()
{
	if (hp_inst_mem)
	{
		deactiveInfiniteHP();
		subjugation = false;
		activeInfiniteHP();
	}
	subjugation = false;
}

bool Cheat::activeInfiniteEnergy()
{
	if (energy_inst_mem)
		return true;

	auto energy_inst_addr = energy_inst_offset.fingAdderess(module_adr, process);
	ExternPointer func_begin = HookInstruction::GetRealAddressFunction(EnergyHookF);
	ExternPointer func_end = HookInstruction::GetRealAddressFunction(EnergyHookFEnd);
	size_t energy_inst_size = func_end - func_begin;
	energy_inst_mem = process.virtualAlloc(nullptr, energy_inst_size);
	if (!energy_inst_mem)
	{
		cout << "<Cheat:activeInfiniteEnergy>Error: Error alloc mem " << GetLastError() << endl;
		return false;
	}
	process.write(energy_inst_mem, func_begin, energy_inst_size);
	energy.init(process, energy_inst_addr, 6, TrEngine::HookInstruction::HookFunction(energy_inst_mem.p()));
	energy.attach();
	return true;
}
void Cheat::deactiveInfiniteEnergy()
{
	if (energy_inst_mem == nullptr)
		return;

	energy.deattach();
	process.virtualFree(energy_inst_mem);
	energy_inst_mem = nullptr;
}

//private
void Cheat::destructor()
{
	deactiveInfiniteShields();
	deactiveInfiniteHP();
	cout << "<Cheat::Cheat>Info: Destructed" << endl;
}