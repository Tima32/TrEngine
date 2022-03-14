#include <SFML/Graphics.hpp>
#include "MainWindow.hpp"

#include <windows.h>
#include "conio.h"

#include <iostream>

#include <TrEngine\Process.hpp>
#include <TrEngine/HookInstruction.hpp>

using namespace std;
using namespace TrEngine;

MainWindow mv;

struct Entity
{
	uint8_t Unknown1[0x198];
	int32_t hp;
	int32_t shields;
};

extern "C"
{
	void ShieldsHookF();
	void ShieldsHookFEnd();

	void HPHookF();
	void HPHookFEnd();

	void EnergyHookF();
	void EnergyHookFEnd();
}

int main1()
{
	Process process;
	if (!process.openProcessByName(L"SC2_x64.exe"))
	{
		cout << "Error open process" << endl;
		return EXIT_FAILURE;
	}
	size_t module_adr = process.findModuleBaseAddress(L"SC2_x64.exe");

	Offset minerals_offset { 0x206F9F8, 0x0, 0x958};
	Offset vespen_offset { 0x206F9F8, 0x0, 0x960 };

	Offset shields_inst_offset;
	shields_inst_offset = {0x592CC7};
	auto shields_inst_addr = shields_inst_offset.fingAdderess(module_adr, process);
	size_t shield_inst_size = size_t(ShieldsHookFEnd) - size_t(ShieldsHookF);
	ExternPointer shield_inst_mem = process.virtualAlloc(nullptr, shield_inst_size);
	if (!shield_inst_mem)
	{
		cout << "Error alloc mem " << GetLastError() << endl;
		return EXIT_FAILURE;
	}
	cout << std::hex;
	cout << "Mem adr " << shield_inst_mem.p() << endl;
	cout << std::dec;
	process.write(shield_inst_mem, ShieldsHookF, shield_inst_size);
	HookInstruction shields;
	shields.init(process, shields_inst_addr, 6, HookInstruction::HookFunction(shield_inst_mem.p()));
	shields.attach();


	Offset hp_inst_offset = {0x593C3D};
	auto hp_inst_addr = hp_inst_offset.fingAdderess(module_adr, process);
	size_t hp_inst_size = size_t(HPHookFEnd) - size_t(HPHookF);
	ExternPointer hp_inst_mem = process.virtualAlloc(nullptr, hp_inst_size);
	if (!hp_inst_mem)
	{
		cout << "Error alloc mem " << GetLastError() << endl;
		return EXIT_FAILURE;
	}
	process.write(hp_inst_mem, HPHookF, hp_inst_size);
	HookInstruction hp;
	hp.init(process, hp_inst_addr, 6, HookInstruction::HookFunction(hp_inst_mem.p()));
	hp.attach();

	Offset energy_inst_offset = { 0x59209E };
	auto energy_inst_addr = energy_inst_offset.fingAdderess(module_adr, process);
	size_t energy_inst_size = size_t(EnergyHookFEnd) - size_t(EnergyHookF);
	ExternPointer energy_inst_mem = process.virtualAlloc(nullptr, energy_inst_size);
	if (!energy_inst_mem)
	{
		cout << "Error alloc mem " << GetLastError() << endl;
		return EXIT_FAILURE;
	}
	process.write(energy_inst_mem, EnergyHookF, energy_inst_size);
	HookInstruction energy;
	energy.init(process, energy_inst_addr, 6, HookInstruction::HookFunction(energy_inst_mem.p()));
	energy.attach();


	while (!(GetAsyncKeyState(0x6b /*r+*/) & 0x8000))
	{
		auto minerals_offset_addr = minerals_offset.fingAdderess(module_adr, process);
		uint32_t minerals_value = 100000;
		process.write(minerals_offset_addr, &minerals_value, sizeof(minerals_value));


		auto vespen_offset_addr = vespen_offset.fingAdderess(module_adr, process);
		uint32_t vespen_value = 100000;
		process.write(vespen_offset_addr, &vespen_value, sizeof(vespen_value));


		Sleep(1);
	}

	process.virtualFree(shield_inst_mem);
	process.virtualFree(hp_inst_mem);
	process.virtualFree(energy_inst_mem);

	return 0;
}

int main()
{
	mv.autoStep();

	return 0;
}