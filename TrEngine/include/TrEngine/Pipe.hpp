#pragma once
#include <Windows.h>
#include "ExternPointer.hpp"

namespace TrEngine
{
	class PipeClient
	{
	public:
		PipeClient();
		~PipeClient();

		bool connect(const wchar_t* pipe_name);
		void disconnect();

		bool write(const void* buf, size_t number_of_bytes_to_write);
		bool write(const void* buf, size_t number_of_bytes_to_write, size_t& number_of_bytes_written);

	private:
		HANDLE pipe{ INVALID_HANDLE_VALUE };
	};
}