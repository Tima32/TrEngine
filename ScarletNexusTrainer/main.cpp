#include <windows.h>
#include <conio.h>

#include <iostream>

#include <TrEngine\Process.hpp>

using namespace std;
using namespace TrEngine;

struct Skill
{
	float usage_time { 3600 };
	float max_usage_time { 3600 };
	uint32_t flags { 0 }; //0 - автоматическое вост.
};
Skill sas_skills[8];

struct UsageTimeLeftSAS
{
	float utl{ 3600 };
	float max_time{ 3600 };
	uint32_t unknown[10]{ 0 };
};
UsageTimeLeftSAS usage_time_left_sas[8];

bool IsAddressValid(const Offset& offset, size_t base_address, const Process& process)
{
	size_t addr = offset.fingAdderess(base_address, process);
	if (!addr)
		return false;

	uint8_t data;
	process.read(PVOID(addr), &data, 1);
}

int SetGameValueWhile()
{
	Process process;
	if (!process.openProcessByName(L"ScarletNexus-Win64-Shipping.exe"))
	{
		cout << "Error" << endl;
		return 1;
	}

	size_t module_adr = process.findModuleBaseAddress(L"ScarletNexus-Win64-Shipping.exe");
	if (module_adr == 0)
	{
		cout << "Error find module" << endl;
		return 1;
	}

	Offset offset_hp;
	offset_hp = { 0x04FC4580, 0x30, 0x2B0, 0x948, 0x310, 0x0 };
	Offset offset_mana;
	offset_mana = { 0x04FC4580, 0x30, 0x2B0, 0x208, 0x1A8, 0xC8 };
	Offset offset_sas;
	offset_sas = { 0x04FC4580, 0x30, 0x2B0, 0x208, 0x180, 0xE8, 0x18 };
	Offset offset_utl;
	offset_utl = { 0x04FC4580, 0x30, 0x2B0, 0x208, 0x360, 0xD0, 0x4 };
	Offset offset_br_acel;
	offset_br_acel = { 0x04FC4580, 0x30, 0x2B0, 0x8B8, 0x180 };
	Offset offset_thoughtfield;
	offset_thoughtfield = { 0x04FC4580, 0x30, 0x2B0, 0x8A8, 0x53C };

	cout << std::hex;
	cout << "offset_hp: " << offset_hp.fingAdderess(module_adr, process) << " " << IsAddressValid(offset_hp, module_adr, process) << endl;
	cout << "offset_mana: " << offset_mana.fingAdderess(module_adr, process) << " " << IsAddressValid(offset_mana, module_adr, process) << endl;
	cout << "offset_sas: " << offset_sas.fingAdderess(module_adr, process) << " " << IsAddressValid(offset_sas, module_adr, process) << endl;
	cout << "offset_utl: " << offset_utl.fingAdderess(module_adr, process) << " " << IsAddressValid(offset_utl, module_adr, process) << endl;
	cout << "offset_br_acel: " << offset_br_acel.fingAdderess(module_adr, process) << " " << IsAddressValid(offset_br_acel, module_adr, process) << endl;
	cout << "offset_thoughtfield: " << offset_thoughtfield.fingAdderess(module_adr, process) << " " << IsAddressValid(offset_thoughtfield, module_adr, process) << endl;
	cout << std::dec;

	_getch();

	while (1)
	{
		size_t mana_addr = offset_mana.fingAdderess(module_adr, process);
		constexpr float mana = 200;
		process.write(PVOID(mana_addr), &mana, sizeof(mana));

		size_t hp_addr = offset_hp.fingAdderess(module_adr, process);
		constexpr uint32_t hp = 2000;
		process.write(PVOID(hp_addr), &hp, sizeof(hp));

		uint8_t j = 0xEB;//inf
		//uint8_t j = 0x74;//no inf
		//cout << PVOID(module_adr + 0xE7DF04) << endl;
		process.write(PVOID(module_adr + 0xE7DF04), &j, sizeof(j));

		//sas
		size_t sas_addr = offset_sas.fingAdderess(module_adr, process);
		process.write(PVOID(sas_addr), sas_skills, sizeof(sas_skills));

		//Usage time left
		size_t usage_time_left_sas_addr = offset_utl.fingAdderess(module_adr, process);
		for (size_t i = 0; i < 8; ++i)
			process.write(
				PVOID(usage_time_left_sas_addr + (sizeof(UsageTimeLeftSAS) * i)),
				&usage_time_left_sas[i].utl,
				sizeof(usage_time_left_sas[i].utl));

		//Brain acceleration
		size_t offset_br_acel_adr = offset_br_acel.fingAdderess(module_adr, process);
		constexpr float br_acel = 400;
		process.write(PVOID(offset_br_acel_adr), &br_acel, sizeof(br_acel));

		size_t offset_br_acel_usage_adr = offset_br_acel_adr + 51 * 4;
		float br_acel_usage = 31;
		//process.read(PVOID(offset_br_acel_usage_adr), &br_acel_usage, sizeof(br_acel_usage));
		process.write(PVOID(offset_br_acel_usage_adr), &br_acel_usage, sizeof(br_acel_usage));
		//cout << br_acel_usage << endl;

		//Мыслеполе
		size_t offset_thoughtfield_addr = offset_thoughtfield.fingAdderess(module_adr, process);
		constexpr float thoughtfield = 40;
		process.write(PVOID(offset_thoughtfield_addr), &thoughtfield, sizeof(thoughtfield));

		Sleep(1);
	}
	return 0;
}

int main()
{
	SetGameValueWhile();

	Process process;
	if (!process.openProcessByName(L"ScarletNexus-Win64-Shipping.exe"))
	{
		cout << "Error" << endl;
		return 1;
	}

	size_t module_adr = process.findModuleBaseAddress(L"ScarletNexus-Win64-Shipping.exe");
	if (module_adr == 0)
	{
		cout << "Error find module" << endl;
		return 1;
	}

	



	return 0;
}

//jmp E9 F1EBFEFF - 32 бита
//jmp E9 05140100

//7ff635cfdf04