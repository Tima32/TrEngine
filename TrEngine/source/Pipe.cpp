#include <limits>
#include <iostream>

constexpr size_t size_t_size = std::numeric_limits<size_t>::max();

#include "..\include\TrEngine\Pipe.hpp"

using namespace std;
using namespace TrEngine;

TrEngine::PipeClient::PipeClient()
{
}
TrEngine::PipeClient::~PipeClient()
{
	disconnect();
}

bool TrEngine::PipeClient::connect(const wchar_t* pipe_name)
{
	pipe = CreateFileW(pipe_name,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (pipe == INVALID_HANDLE_VALUE)
		return false;

	return true;
}
void TrEngine::PipeClient::disconnect()
{
	if (pipe == nullptr)
		return;

	CloseHandle(pipe);
	pipe = nullptr;
}

bool TrEngine::PipeClient::write(const void* buf, size_t number_of_bytes_to_write)
{
	return WriteFile(pipe, buf, number_of_bytes_to_write, NULL, NULL);
}
bool TrEngine::PipeClient::write(const void* buf, size_t number_of_bytes_to_write, size_t& number_of_bytes_written)
{
	DWORD d;

	bool state = WriteFile(pipe, buf, number_of_bytes_to_write, &d, NULL);
	number_of_bytes_written = size_t(d);

#ifdef _DEBUG
	if (d > size_t_size)
		cerr << "<TrEngine:PipeClient:Write>DebugWarning: The number of bytes written exceeds the range of the variable." << endl;
#endif

	return state;
}

