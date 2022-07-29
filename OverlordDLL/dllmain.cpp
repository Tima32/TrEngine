#include <iostream>
#include <iomanip>
#include <thread>
#include <windows.h>
#include "../Overlord/PipeMessageStruct.hpp"
#include <TrEngine/Process.hpp>
#include <TrEngine/HookInstruction.hpp>

using namespace std;
using namespace TrEngine;

extern "C"
{
	void HPHookF();
	void HPHookFEnd();

	void DamageHookF();
	void DamageHookFEnd();

	void CheckHPHookF();
	void CheckHPHookFEnd();

	void ManaHookF();
	void ManaHookFEnd();
}
void Deattach();

HANDLE h_pipe{ nullptr };
thread mt;
bool stop_thread{ false };
TrEngine::Process process;
size_t module_adr{ 0 };


// Cheat flugs
bool overlord_immortal{ false };
bool minions_immortal{ false };

bool overlord_set_max_hp{ false };
float overlord_set_max_hp_value{ 150 };
bool minions_set_max_hp{ false };
float minions_set_max_hp_value{ 150 };

bool infinite_magick{ false };
bool magic_spending_multipler{ false };
float magic_spending_multipler_value{ 1 };


#pragma pack(push, 1)
struct GameEntity
{
	uint8_t r1[0xE4];
	float hp;           // E4
	float max_hp;       // E8
	uint8_t r2[0x3C];
	float mp;           //128
	uint8_t r3[0x8];
	uint32_t pointer;   //130
};
struct GameOverlord
{
	uint8_t r1[0x348];
	GameEntity ge;
};
#pragma pack(pop)
GameOverlord* g_overlord{ nullptr };

TrEngine::Offset hp_inst_offset{ 0x2018F9 };
bool hp_is_attach{ false };
TrEngine::HookInstructionX32 hp;
extern "C" void __cdecl HPFunc(GameOverlord * overlord_pointer)
{
	if (g_overlord != overlord_pointer)
	{
		g_overlord = overlord_pointer;
		cout << "overlord_pointer: " << g_overlord << endl;
		cout << "HP: " << g_overlord->ge.hp << " " << &g_overlord->ge.hp << endl;
	}

	// Infinitie MP
	if (infinite_magick)
		g_overlord->ge.mp = 600;
}
bool AttachHP()
{
	if (hp_is_attach)
		return true;

	auto hp_inst_addr = hp_inst_offset.fingAdderess(module_adr, process);
	ExternPointer func_begin;
	ExternPointer func_end;


	func_begin = TrEngine::GetRealAddressFunction(HPHookF);
	func_end = TrEngine::GetRealAddressFunction(HPHookFEnd);

	if (!hp.init(process, hp_inst_addr, 6, TrEngine::HookInstructionX32::HookFunction(&HPHookF)))
	{
		cout << "<Cheat:activeInfiniteHP>Error: Initialization error." << endl;
		return false;
	}
	if (!hp.attach())
	{
		cout << "<Cheat:activeInfiniteHP>Error: Attach error." << endl;
		return false;
	}

	hp_is_attach = true;

	return true;
}

TrEngine::Offset damage_inst_offset{ 0x2CD245 };
bool damage_is_attach{ false };
TrEngine::HookInstructionX32 damage;
extern "C" void __cdecl DamageFunc(uint8_t* pointer, float old_hp)
{
	GameEntity* o = reinterpret_cast<GameEntity*>(pointer);
	float damage = old_hp - o->hp;

	if (g_overlord == nullptr)
		return;

	uint32_t type{ 0 }; //0, 1-overlord 2-minion

	//Overlord
	if (o == &g_overlord->ge)
		type = 1;

	//Minions
	if (g_overlord->ge.pointer == o->pointer &&
		o != &g_overlord->ge)
		type = 2;

	if (type == 1 && overlord_immortal)
		o->hp = o->max_hp;
	else if (type == 2 && minions_immortal)
		o->hp = o->max_hp;

	cout << "<DamageFunc> HP: " << setw(8) << o->hp
		<< " damage: " << setw(8) << damage
		<< " MHP: " << setw(5) << o->max_hp
		<< " adr: " << setw(8) << &o->hp;
	if (type == 1)
		cout << " E: Overlord";
	else if (type == 2)
		cout << " E: Minion";
	else
		cout << " E: Another";
	cout << endl;
}
bool AttachDamage()
{
	if (damage_is_attach)
		return true;

	auto inst_addr = damage_inst_offset.fingAdderess(module_adr, process);
	ExternPointer func_begin;
	ExternPointer func_end;


	func_begin = TrEngine::GetRealAddressFunction(DamageHookF);
	func_end = TrEngine::GetRealAddressFunction(DamageHookFEnd);

	if (!damage.init(process, inst_addr, 6, TrEngine::HookInstructionX32::HookFunction(&DamageHookF)))
	{
		cout << "<Cheat:AttachDamage>Error: Initialization error." << endl;
		return false;
	}
	if (!damage.attach())
	{
		cout << "<Cheat:AttachDamage>Error: Attach error." << endl;
		return false;
	}

	damage_is_attach = true;

	return true;
}

TrEngine::Offset check_hp_inst_offset{ 0x2CB0EB };
bool check_hp_is_attach{ false };
TrEngine::HookInstructionX32 check_hp;
extern "C" void __cdecl CheckHPFunc(uint8_t * pointer)
{
	GameEntity* o = reinterpret_cast<GameEntity*>(pointer);

	if (g_overlord == nullptr)
		return;

	uint32_t type{ 0 }; //0, 1-overlord 2-minion

	//Overlord
	if (o == &g_overlord->ge)
		type = 1;

	//Minions
	if (g_overlord->ge.pointer == o->pointer &&
		o != &g_overlord->ge)
		type = 2;

	if (type == 1 && overlord_set_max_hp)
		o->max_hp = overlord_set_max_hp_value;
	else if (type == 2 && minions_set_max_hp)
		o->max_hp = minions_set_max_hp_value;
}
bool AttachCheckHP()
{
	if (check_hp_is_attach)
		return true;

	auto inst_addr = check_hp_inst_offset.fingAdderess(module_adr, process);

	if (!check_hp.init(process, inst_addr, 6, TrEngine::HookInstructionX32::HookFunction(&CheckHPHookF)))
	{
		cout << "<Cheat:activeInfiniteHP>Error: Initialization error." << endl;
		return false;
	}
	if (!check_hp.attach())
	{
		cout << "<Cheat:activeInfiniteHP>Error: Attach error." << endl;
		return false;
	}

	check_hp_is_attach = true;

	return true;
}

TrEngine::Offset mana_inst_offset{ 0x244448 };
bool mana_is_attach{ false };
TrEngine::HookInstructionX32 mana;
extern "C" void __cdecl ManaFunc(uint8_t* pointer, float old_mp)
{
	GameEntity* o = reinterpret_cast<GameEntity*>(pointer + 0x348);
	float pending_mp = old_mp - o->mp;

	if (g_overlord == nullptr)
		return;

	uint32_t type{ 0 }; //0, 1-overlord 2-minion

	//Overlord
	if (o == &g_overlord->ge)
		type = 1;

	//Minions
	if (g_overlord->ge.pointer == o->pointer &&
		o != &g_overlord->ge)
		type = 2;

	// Multipler
	if (type == 1 && magic_spending_multipler)
	{
		pending_mp *= magic_spending_multipler_value;
		o->mp = old_mp - pending_mp;
	}

	// Infinitie
	if (type == 1 && infinite_magick)
		o->mp = 600;

	cout << "<ManaFunc> MP: " << setw(8) << o->mp
		<< " pending: " << setw(8) << pending_mp
		<< " adr: " << setw(8) << &o->hp;
	if (type == 1)
		cout << " E: Overlord";
	else if (type == 2)
		cout << " E: Minion";
	else
		cout << " E: Another";
	cout << endl;
}
bool AttachMana()
{
	if (mana_is_attach)
		return true;

	auto inst_addr = mana_inst_offset.fingAdderess(module_adr, process);
	ExternPointer func_begin;
	ExternPointer func_end;


	func_begin = TrEngine::GetRealAddressFunction(ManaHookF);
	func_end = TrEngine::GetRealAddressFunction(ManaHookFEnd);

	if (!mana.init(process, inst_addr, 6, TrEngine::HookInstructionX32::HookFunction(&ManaHookF)))
	{
		cout << "<Cheat:AttachMana>Error: Initialization error." << endl;
		return false;
	}
	if (!mana.attach())
	{
		cout << "<Cheat:AttachMana>Error: Attach error." << endl;
		return false;
	}

	mana_is_attach = true;

	return true;
}

void PipeMessageFunc(uint8_t* buf, DWORD read)
{
	auto p = reinterpret_cast<PipeMessage*>(buf);
	
	/* add terminating zero */
	p->message[read - sizeof(p->type)] = '\0';

	/* do something with data in buffer */
	printf("<PipeMessageFunc>Message: %s\n", p->message);
}
void PipeFunc()
{
	cout << "<ThreadFunc> Pipe thread created." << endl;

	uint8_t buf[1204];

	while (!stop_thread)
	{
		DWORD read;
		if (ConnectNamedPipe(h_pipe, NULL) != FALSE)   // wait for someone to connect to the pipe
		{
			cout << "<PipeFunc>Info: Pipe connected." << endl;
			while (ReadFile(h_pipe, buf, sizeof(buf) - 1, &read, NULL) != FALSE)
			{
				PipeNoArgs* pn = reinterpret_cast<PipeNoArgs*>(buf);

				// Technical commands
				if (pn->type == PipeMessageId::Message)
					PipeMessageFunc(buf, read);
				else if (pn->type == PipeMessageId::Deattach)
				{
					cout << "<PipeFunc>Info: Deattach" << endl;

					overlord_immortal = false;
					minions_immortal = false;

					overlord_set_max_hp = false;
					minions_set_max_hp = false;

					infinite_magick = false;
					magic_spending_multipler = false;

					break;
				}

				// Overlord
				else if (pn->type == PipeMessageId::OverlordImmortalTrue)
					overlord_immortal = true;
				else if (pn->type == PipeMessageId::OverlordImmortalFalse)
					overlord_immortal = false;

				// Minions
				else if (pn->type == PipeMessageId::MinionsImmortalTrue)
					minions_immortal = true;
				else if (pn->type == PipeMessageId::MinionsImmortalFalse)
					minions_immortal = false;

				// Overlord set max HP
				else if (pn->type == PipeMessageId::OverlordSetMaxHPTrue)
					overlord_set_max_hp = true;
				else if (pn->type == PipeMessageId::OverlordSetMaxHPFalse)
					overlord_set_max_hp = false;
				else if (pn->type == PipeMessageId::OverlordSetMaxHP)
					overlord_set_max_hp_value = reinterpret_cast<PipeSetMaxHP*>(pn)->max_hp;

				// Minions set max HP
				else if (pn->type == PipeMessageId::MinionsSetMaxHPTrue)
					minions_set_max_hp = true;
				else if (pn->type == PipeMessageId::MinionsSetMaxHPFalse)
					minions_set_max_hp = false;
				else if (pn->type == PipeMessageId::MinionsSetMaxHP)
					minions_set_max_hp_value = reinterpret_cast<PipeSetMaxHP*>(pn)->max_hp;

				// MP
				else if (pn->type == PipeMessageId::InfiniteMagickTrue)
					infinite_magick = true;
				else if (pn->type == PipeMessageId::InfiniteMagickFalse)
					infinite_magick = false;
				else if (pn->type == PipeMessageId::MagicSpendingMultiplerTrue)
					magic_spending_multipler = true;
				else if (pn->type == PipeMessageId::MagicSpendingMultiplerFalse)
					magic_spending_multipler = false;
				else if (pn->type == PipeMessageId::MagicSpendingMultipler)
					magic_spending_multipler_value = reinterpret_cast<PipeSetMPMultipler*>(pn)->mp_multipler;

				else
					cout << "<PipeFunc>Error: Unknown message type." << endl;
			}
		}

		cout << "<PipeFunc>Info: Pipe disconnected." << endl;
		DisconnectNamedPipe(h_pipe);
	}
}

void Attach()
{
	AllocConsole();

	FILE* fDummy;
	freopen_s(&fDummy, "CONIN$", "r", stdin);
	freopen_s(&fDummy, "CONOUT$", "w", stderr);
	freopen_s(&fDummy, "CONOUT$", "w", stdout);

	cout << "Attach" << endl;

	//Injections
	process.openProcessByHandle(GetCurrentProcess());
	module_adr = process.findModuleBaseAddress(L"Overlord.exe");
	AttachHP();
	AttachDamage();
	AttachCheckHP();
	AttachMana();

	//Pipe
	h_pipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\OverlordTrainer"),
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,   // FILE_FLAG_FIRST_PIPE_INSTANCE is not needed but forces CreateNamedPipe(..) to fail if the pipe already exists...
		1,
		1024 * 16,
		1024 * 16,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);

	if (h_pipe == INVALID_HANDLE_VALUE)
	{
		cout << "<Attach>Error: Failed to create pipe." << endl;
		return;
	}

	mt = thread(PipeFunc);
	mt.detach();
}
void Deattach()
{
	CloseHandle(h_pipe);
	cout << "Deattach" << endl;
}

BOOL APIENTRY DllMain( HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Attach();
		break;
	case DLL_THREAD_ATTACH:
		//cout << "Thread created." << endl;
		break;
	case DLL_THREAD_DETACH:
		//cout << "Thread destroyed." << endl;
		break;
	case DLL_PROCESS_DETACH:
		Deattach();
		break;
	}
	return TRUE;
}

