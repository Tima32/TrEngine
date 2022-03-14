#pragma once
#include <Windows.h>

namespace TrEngine
{
	class ExternPointer
	{
	public:
		ExternPointer() = default;
		ExternPointer(size_t address);
		ExternPointer(void* p);

		size_t getAddr() const;
		void* getPointer() const;
		size_t a() const; //getAddr
		void* p() const;  //getPointer

		ExternPointer operator+(const ExternPointer p);
		ExternPointer operator+(const uint8_t p);
		ExternPointer operator-(const ExternPointer p);

		ExternPointer operator+=(const ExternPointer p);
		ExternPointer operator-=(const ExternPointer p);

		ExternPointer operator=(size_t address);
		ExternPointer operator=(void* address);

		operator size_t();
		operator void* ();

	private:
		size_t pointer;
	};
}

void* operator<<(void* pointer, const TrEngine::ExternPointer p);

#include "ExternPointer.inl"