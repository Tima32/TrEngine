#include <windows.h>

#include <iostream>

#include <TrEngine/HookInstruction.hpp>
#include <TrEngine/Process.hpp>

using namespace std;
using namespace TrEngine;

Process process;
HookInstruction test_hook;

int32_t* hp_player_addr = nullptr;
extern "C" __declspec(dllexport) void _fastcall DamageFilter(int32_t hp_val, int32_t * hp_addr)
{
    //cout << "hp: " << hp_addr << endl;
    if (hp_addr == hp_player_addr)
    {
        return;
    }
    *hp_addr = hp_val;
    return;
}

extern "C" {
    void HookFunc();
    //extern hook_funk_ret: qword
}

FILE* conin_in = nullptr;
FILE* conout_out = nullptr;
FILE* conout_err = nullptr;
void Attach()
{
    AllocConsole();
    freopen_s(&conin_in, "conin$", "r", stdin);
    freopen_s(&conout_out, "conout$", "w", stdout);
    freopen_s(&conout_err,"conout$", "w", stderr);
    std::cout.clear();
    std::cin.clear();

    cout << "<Attach>Info: Attaching ..." << endl;

    process.openProcessByHandle(GetCurrentProcess());

    Offset offset_hp;
    offset_hp = { 0x04FC4580, 0x30, 0x2B0, 0x948, 0x310, 0x0 };

    //"VCRUNTIME140.dll"+140A
    size_t attach_addr = process.findModuleBaseAddress(L"VCRUNTIME140.dll") + 0x140A;
    cout << "attach_addr: " << attach_addr << endl;
    test_hook.init(PVOID(attach_addr), 7, HookFunc);
    test_hook.attach();


    cout << "<Attach>Info: Attach success" << endl;
}
void Deatach()
{
    cout << "<Attach>Info: Deatach..." << endl;
    //FreeConsole();
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
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        Deatach();
        break;
    }
    return TRUE;
}

