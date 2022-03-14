#include <Windows.h>
#include <TlHelp32.h>
#include <psapi.h>

#include <iostream>

#include "..\include\TrEngine\Process.hpp"

using namespace std;
using namespace TrEngine;

Process::Process()
{}
Process::~Process()
{
	close();
}

bool Process::openProcessByName(const std::wstring& name)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hSnapshot == INVALID_HANDLE_VALUE) return false;

	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hSnapshot, &pe))
	{
		do
		{
			if (!lstrcmp(pe.szExeFile, name.c_str()))
			{
				DWORD pid = pe.th32ProcessID;
				bool result =  openProcessByPID(pid);
				
				CloseHandle(hSnapshot);
				return result;
			}
		} while (Process32Next(hSnapshot, &pe));
	}

	CloseHandle(hSnapshot);
	return false;
}
bool Process::openProcessByWindow(const std::wstring& window_name, const std::wstring& class_name)
{
	HWND prochwnd = FindWindowW(class_name.c_str(), window_name.c_str());
	if (prochwnd == 0) return false;
	DWORD pid = 0;
	GetWindowThreadProcessId(prochwnd, &pid);
	bool result = openProcessByPID(pid);
	return result;
}
bool Process::openProcessByPID(DWORD pid)
{
	if (h_process)
	{
		close();
	}
	wcout << L"<TrEngine:openProcessByPID>Info: pid: " << pid << endl;
	h_process = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	this->pid = pid;
	wcout << L"<TrEngine:openProcessByPID>Info: h_process: " << h_process << endl;
	return h_process != 0;
}
bool Process::openProcessByHandle(HANDLE handle)
{
	if (h_process)
	{
		close();
	}
	pid = GetProcessId(handle);
	return openProcessByPID(pid);
}
void Process::close()
{
	if (h_process)
	{
		CloseHandle(h_process);
		h_process = nullptr;
	}
}

bool Process::read(ExternPointer addr, void* buff, size_t size, size_t* read_size) const
{
	bool status = ReadProcessMemory(h_process, addr.getPointer(), buff, size, read_size);
	return status;
}
bool Process::write(ExternPointer addr, const void* buff, size_t size, size_t* write_size) const
{
	DWORD old_protect;
	bool status = VirtualProtectEx(h_process, addr.getPointer(), size, PAGE_EXECUTE_READWRITE, &old_protect);
	if (!status)
		return false;
	status = WriteProcessMemory(h_process, addr.getPointer(), buff, size, write_size);
	if (!status)
		return false;
	status = VirtualProtectEx(h_process, addr.getPointer(), size, old_protect, &old_protect);
	if (!status)
		return false;
	return status;

}
ExternPointer Process::virtualAlloc(ExternPointer address, size_t size, uint32_t protect)
{
	return VirtualAllocEx(h_process, address.getPointer() , size, MEM_RESERVE | MEM_COMMIT, protect);
}
bool Process::virtualFree(ExternPointer address)
{
	return VirtualFreeEx(h_process, address.getPointer(), 0, MEM_RELEASE);
}

size_t Process::findModuleBaseAddress(const std::wstring& module_name)
{
	HANDLE module_snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
	if (module_snap == nullptr)
		return 0;
	MODULEENTRY32 me = { 0 };
	me.dwSize = sizeof(MODULEENTRY32);

	if (Module32First(module_snap, &me))
	{
		do
		{
			if (!lstrcmp(me.szModule, module_name.c_str()))
			{
				CloseHandle(module_snap);
				return reinterpret_cast<size_t>(me.modBaseAddr);
			}
		} while (Module32Next(module_snap, &me));
	}
	CloseHandle(module_snap);
	return 0;
}

HANDLE Process::getHandle()
{
	return h_process;
}
DWORD Process::getID()
{
	return pid;
}


ExternPointer TrEngine::Offset::fingAdderess(ExternPointer base_address, const Process& process) const
{
	size_t i = 0;
	while (true)
	{
		base_address += operator[](i);
		++i;
		if (i == size())
			break;
		if (!process.read(base_address, &base_address, sizeof(base_address)))
			return { nullptr };
	}
	return base_address;
}