#pragma once
#include <string>
#include <vector>

#include "ImportExport.hpp"
#include "ExternPointer.hpp"

typedef unsigned long       DWORD;
typedef void* HANDLE;

namespace TrEngine
{
	class Process
	{
	public:
		Process();
		~Process();

		bool openProcessByName(const std::wstring& name);
		bool openProcessByWindow(const std::wstring& window_name, const std::wstring& class_name);
		bool openProcessByPID(DWORD pid);
		bool openProcessByHandle(HANDLE handle);
		void close();

		bool read(ExternPointer addr, void* buff, SIZE_T size, SIZE_T* read_size = nullptr) const;
		bool write(ExternPointer addr, const void* buff, SIZE_T size, SIZE_T* write_size = nullptr) const;
		ExternPointer virtualAlloc(ExternPointer address, SIZE_T size, uint32_t protect);
		ExternPointer virtualAlloc(ExternPointer address, SIZE_T size, DWORD allocation_type, uint32_t protect);
		bool virtualFree(ExternPointer address);

		SIZE_T findModuleBaseAddress(const std::wstring& module_name);

		bool loadLibrary(const wchar_t* dll_path);

		HANDLE getHandle() const;
		DWORD getID() const;
		bool isX64() const;
	private:
		DWORD pid{ 0 };
		HANDLE h_process{ 0 };

		bool is_64_process{};
	};


	struct Offset : public std::vector<ExternPointer>
	{
		Offset() = default;
		Offset(std::initializer_list<ExternPointer> parameters);
		Offset(std::initializer_list<SIZE_T> parameters);

		ExternPointer fingAdderess(ExternPointer base_address, const Process& process) const;
		Offset& operator=(std::initializer_list<ExternPointer> parameters);
		Offset& operator=(std::initializer_list<SIZE_T> parameters);
	};
	inline Offset::Offset(std::initializer_list<ExternPointer> parameters)
	{
		*this = parameters;
	}
	inline Offset::Offset(std::initializer_list<SIZE_T> parameters)
	{
		*this = parameters;
	}
	inline Offset& Offset::operator=(std::initializer_list<ExternPointer> parameters)
	{
		vector::operator=(parameters);
	}
	inline Offset& Offset::operator=(std::initializer_list<SIZE_T> parameters)
	{
		reserve(parameters.size());
		for (auto& address : parameters)
			push_back(address);
		return *this;
	}
}