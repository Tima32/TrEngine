#pragma once

namespace TrEngine
{
	inline ExternPointer::ExternPointer(void* p)
	{
		pointer = reinterpret_cast<SIZE_T>(p);
	}
	inline ExternPointer::ExternPointer(SIZE_T address)
	{
		pointer = address;
	}

	inline SIZE_T ExternPointer::getAddr() const
	{
		return pointer;
	}
	inline void* ExternPointer::getPointer() const
	{
		return reinterpret_cast<void*>(pointer);
	}
	inline SIZE_T ExternPointer::a() const
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

	inline ExternPointer ExternPointer::operator=(SIZE_T address)
	{
		return { pointer = address };
	}
	inline ExternPointer ExternPointer::operator=(void* address)
	{
		pointer = reinterpret_cast<SIZE_T>(address);
		return { pointer };
	}

	inline ExternPointer::operator SIZE_T()
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