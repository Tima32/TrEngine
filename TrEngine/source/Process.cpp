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
	close();
	wcout << L"<TrEngine:openProcessByPID>Info: pid: " << pid << endl;

	h_process = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	if (h_process == NULL)
		return false;

	this->pid = pid;

	USHORT process_machine;
	USHORT native_machine;

	if (!IsWow64Process2(GetCurrentProcess(), &process_machine, &native_machine))
	{
		cerr << "<Process:openProcessByPID>Error: IsWOW64Process2 returned FALSE (failed). GetLastError returned: " << GetLastError() << endl;
		return false;
	}
	if (process_machine == IMAGE_FILE_MACHINE_UNKNOWN) //wow not found
	{
		if (native_machine == IMAGE_FILE_MACHINE_IA64 || native_machine == IMAGE_FILE_MACHINE_AMD64 || native_machine == IMAGE_FILE_MACHINE_ARM64)
			is_64_process = true;
		else if (native_machine == IMAGE_FILE_MACHINE_I386 || native_machine == IMAGE_FILE_MACHINE_ARM)
			is_64_process = false;
		else
		{
			cout << "<Process:openProcessByPID>Error: Unknown Windows Architecture." << endl;
			close();
			return false;
		}
	}
	else //wow found
	{
		is_64_process = false;
	}

	wcout << L"<TrEngine:openProcessByPID>Info: h_process: " << h_process << endl;
	return true;
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

bool Process::read(ExternPointer addr, void* buff, SIZE_T size, SIZE_T* read_size) const
{
	bool status = ReadProcessMemory(h_process, addr.getPointer(), buff, size, read_size);
	return status;
}
bool Process::write(ExternPointer addr, const void* buff, SIZE_T size, SIZE_T* write_size) const
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
ExternPointer Process::virtualAlloc(ExternPointer address, SIZE_T size, uint32_t protect)
{
	return VirtualAllocEx(h_process, address.getPointer() , size, MEM_RESERVE | MEM_COMMIT, protect);
}
ExternPointer Process::virtualAlloc(ExternPointer address, SIZE_T size, DWORD allocation_type, uint32_t protect)
{
	return VirtualAllocEx(h_process, address.getPointer(), size, allocation_type, protect);
}
bool Process::virtualFree(ExternPointer address)
{
	return VirtualFreeEx(h_process, address.getPointer(), 0, MEM_RELEASE);
}

SIZE_T Process::findModuleBaseAddress(const std::wstring& module_name)
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
				return reinterpret_cast<SIZE_T>(me.modBaseAddr);
			}
		} while (Module32Next(module_snap, &me));
	}
	CloseHandle(module_snap);
	return 0;
}

bool Process::loadLibrary(const wchar_t* dll_path)
{
	wchar_t DLL[MAX_PATH];
	HANDLE thread_handle;
	LPVOID allocated_mem;

	if (h_process == nullptr)
		return false;

	if (GetFullPathNameW(dll_path, MAX_PATH, DLL, NULL) == 0)
		return false;

	allocated_mem = VirtualAllocEx(h_process, NULL, sizeof(DLL), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (allocated_mem == nullptr)
		goto err_alloc;

	WriteProcessMemory(h_process, allocated_mem, DLL, sizeof(DLL), NULL);

	// launch DLL
	thread_handle = CreateRemoteThread(h_process, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, allocated_mem, 0, 0);
	if (thread_handle == nullptr)
		goto err_create_remote_thread;
	WaitForSingleObject(thread_handle, INFINITE);
	CloseHandle(thread_handle);

	VirtualFreeEx(h_process, allocated_mem, 0, MEM_RELEASE);

	return true;

err_create_remote_thread:
	VirtualFreeEx(h_process, allocated_mem, 0, MEM_RELEASE);
err_alloc:
	return false;
}

HANDLE Process::getHandle() const
{
	return h_process;
}
DWORD Process::getID() const
{
	return pid;
}
bool Process::isX64() const
{
	return is_64_process;
}


ExternPointer TrEngine::Offset::fingAdderess(ExternPointer base_address, const Process& process) const
{
	SIZE_T i = 0;
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