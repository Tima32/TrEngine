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

		bool read(ExternPointer addr, void* buff, size_t size, size_t* read_size = nullptr) const;
		bool write(ExternPointer addr, const void* buff, size_t size, size_t* write_size = nullptr) const;
		ExternPointer virtualAlloc(ExternPointer address, size_t size, uint32_t protect = PAGE_EXECUTE_READWRITE);
		bool virtualFree(ExternPointer address);

		size_t findModuleBaseAddress(const std::wstring& module_name);

		HANDLE getHandle();
		DWORD getID();
	private:
		DWORD pid{ 0 };
		HANDLE h_process{ 0 };
	};


	struct Offset : public std::vector<ExternPointer>
	{
		Offset() = default;
		Offset(std::initializer_list<ExternPointer> parameters);
		Offset(std::initializer_list<size_t> parameters);

		ExternPointer fingAdderess(ExternPointer base_address, const Process& process) const;
		Offset& operator=(std::initializer_list<ExternPointer> parameters);
		Offset& operator=(std::initializer_list<size_t> parameters);
	};
	inline Offset::Offset(std::initializer_list<ExternPointer> parameters)
	{
		*this = parameters;
	}
	inline Offset::Offset(std::initializer_list<size_t> parameters)
	{
		*this = parameters;
	}
	inline Offset& Offset::operator=(std::initializer_list<ExternPointer> parameters)
	{
		vector::operator=(parameters);
	}
	inline Offset& Offset::operator=(std::initializer_list<size_t> parameters)
	{
		reserve(parameters.size());
		for (auto& address : parameters)
			push_back(address);
		return *this;
	}
}