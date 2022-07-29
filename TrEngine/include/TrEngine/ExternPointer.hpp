#pragma once
#include <Windows.h>

namespace TrEngine
{
	class ExternPointer
	{
	public:
		ExternPointer() = default;
		ExternPointer(SIZE_T address);
		ExternPointer(void* p);

		SIZE_T getAddr() const;
		void* getPointer() const;
		SIZE_T a() const; //getAddr
		void* p() const;  //getPointer

		ExternPointer operator+(const ExternPointer p);
		ExternPointer operator+(const uint8_t p);
		ExternPointer operator-(const ExternPointer p);

		ExternPointer operator+=(const ExternPointer p);
		ExternPointer operator-=(const ExternPointer p);

		ExternPointer operator=(SIZE_T address);
		ExternPointer operator=(void* address);

		operator SIZE_T();
		operator void* ();

	private:
		SIZE_T pointer;
	};
}

void* operator<<(void* pointer, const TrEngine::ExternPointer p);

#include "ExternPointer.inl"