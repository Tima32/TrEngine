#pragma once

namespace TrEngine
{
	inline ExternPointer::ExternPointer(void* p)
	{
		pointer = reinterpret_cast<size_t>(p);
	}
	inline ExternPointer::ExternPointer(size_t address)
	{
		pointer = address;
	}

	inline size_t ExternPointer::getAddr() const
	{
		return pointer;
	}
	inline void* ExternPointer::getPointer() const
	{
		return reinterpret_cast<void*>(pointer);
	}
	inline size_t ExternPointer::a() const
	{
		return pointer;
	}
	inline void* ExternPointer::p() const
	{
		return reinterpret_cast<void*>(pointer);
	}

	inline ExternPointer ExternPointer::operator+(const ExternPointer p)
	{
		ExternPointer t;
		t.pointer = pointer + p.pointer;
		return t;
	}
	inline ExternPointer ExternPointer::operator+(const uint8_t p)
	{
		ExternPointer t;
		t.pointer = pointer + p;
		return t;
	}
	inline ExternPointer ExternPointer::operator-(const ExternPointer p)
	{
		ExternPointer t;
		t.pointer = pointer - p.pointer;
		return t;
	}

	inline ExternPointer ExternPointer::operator+=(const ExternPointer p)
	{
		return pointer += p.pointer;
	}
	inline ExternPointer ExternPointer::operator-=(const ExternPointer p)
	{
		return pointer -= p.pointer;
	}

	inline ExternPointer ExternPointer::operator=(size_t address)
	{
		return { pointer = address };
	}
	inline ExternPointer ExternPointer::operator=(void* address)
	{
		pointer = reinterpret_cast<size_t>(address);
		return { pointer };
	}

	inline ExternPointer::operator size_t()
	{
		return pointer;
	}
	inline ExternPointer::operator void*()
	{
		return reinterpret_cast<void*>(pointer);
	}
}

inline void* operator<<(void* pointer, const TrEngine::ExternPointer p)
{
	return pointer = p.getPointer();
}