#include <iostream>

#include "Cheat.hpp"
#include "PipeMessageStruct.hpp"

#include <Windows.h>
#include <psapi.h>

using namespace std;
using namespace TrEngine;


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
	constexpr auto& pipe_name = L"\\\\.\\pipe\\OverlordTrainer";

	if (!process.openProcessByName(L"Overlord.exe"))
	{
		return false;
	}
	wcout << "<Cheat::open>Info: Process opened" << endl;

	module_adr = process.findModuleBaseAddress(L"Overlord.exe");

	if (!pipe.connect(pipe_name))
	{
		if (!process.loadLibrary(L"OverlordDLL.dll"))
		{
			cout << "<Cheat:open>Error: Failed to inject dll." << endl;
		}
		Sleep(10);
		if (!pipe.connect(pipe_name))
			cout << "<Cheat:open>Error: Failed to connet to the pipe" << endl;
	}

	PipeMessage ms;
	strcpy_s(ms.message, "Ping message");
	pipe.write(&ms, sizeof(ms));

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

bool Cheat::activeInfiniteHP()
{
	PipeNoArgs m{ PipeMessageId::OverlordImmortalTrue };
	return pipe.write(&m, sizeof m);
}
void Cheat::deactiveInfiniteHP()
{
	PipeNoArgs m{ PipeMessageId::OverlordImmortalFalse };
	pipe.write(&m, sizeof m);
}

bool Cheat::activeInfiniteMinionsHP()
{
	PipeNoArgs m{ PipeMessageId::MinionsImmortalTrue };
	return pipe.write(&m, sizeof m);
}
void Cheat::deactiveInfiniteMinionsHP()
{
	PipeNoArgs m{ PipeMessageId::MinionsImmortalFalse };
	pipe.write(&m, sizeof m);
}

bool Cheat::activeOverlordSetMaxHP()
{
	PipeNoArgs m{ PipeMessageId::OverlordSetMaxHPTrue };
	return pipe.write(&m, sizeof m);
}
void Cheat::deactiveOverlordSetMaxHP()
{
	PipeNoArgs m{ PipeMessageId::OverlordSetMaxHPFalse };
	pipe.write(&m, sizeof m);
}
bool Cheat::setOverlordSetMaxHP(float value)
{
	PipeSetMaxHP m{ PipeMessageId::OverlordSetMaxHP, value };
	return pipe.write(&m, sizeof m);
}

bool Cheat::activeMinionsSetMaxHP()
{
	PipeNoArgs m{ PipeMessageId::MinionsSetMaxHPTrue };
	return pipe.write(&m, sizeof m);
}
void Cheat::deactiveMinionsSetMaxHP()
{
	PipeNoArgs m{ PipeMessageId::MinionsSetMaxHPFalse };
	pipe.write(&m, sizeof m);
}
bool Cheat::setMinionsSetMaxHP(float value)
{
	PipeSetMaxHP m{ PipeMessageId::MinionsSetMaxHP, value };
	return pipe.write(&m, sizeof m);
}

// MP
bool Cheat::activeInfiniteMP()
{
	PipeNoArgs m{ PipeMessageId::InfiniteMagickTrue };
	return pipe.write(&m, sizeof m);
}
void Cheat::deactiveInfiniteMP()
{
	PipeNoArgs m{ PipeMessageId::InfiniteMagickFalse };
	pipe.write(&m, sizeof m);
}

bool Cheat::activeMPMultipler()
{
	PipeNoArgs m{ PipeMessageId::MagicSpendingMultiplerTrue };
	return pipe.write(&m, sizeof m);
}
void Cheat::deactiveMPMultipler()
{
	PipeNoArgs m{ PipeMessageId::MagicSpendingMultiplerFalse };
	pipe.write(&m, sizeof m);
}
bool Cheat::setMPMultipler(float value)
{
	PipeSetMPMultipler m{ PipeMessageId::MagicSpendingMultipler, value };
	return pipe.write(&m, sizeof m);
}

//private
void Cheat::destructor()
{
	deactiveInfiniteHP();

	PipeNoArgs d;
	d.type = PipeMessageId::Deattach;
	pipe.write(&d, sizeof(d));

	cout << "<Cheat::Cheat>Info: Destructed" << endl;
}