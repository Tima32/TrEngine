#pragma once
#include <stdint.h>

#include <vector>

#include "ExternPointer.hpp"
#include "Process.hpp"


//for DLL

namespace TrEngine
{
	
	class HookInstructionX64
	{
	public:
		typedef void (*HookFunction)();//asm func

		HookInstructionX64();
		~HookInstructionX64();

		//tramplin_size min 5 max 30
		bool init(Process& process, ExternPointer addr, uint8_t tramplin_size, HookFunction function);
		bool attach();
		void deattach();

	private:
		void readOriginalInstructin();
		ExternPointer findSpaceForTramplin();

		void free();

#pragma pack(push, 1)
		struct HookJump
		{
			uint8_t jmp = { 0xE9 };
			int32_t offset;
		};
		struct Tramplin
		{
			uint8_t tramplin[8] = {0xFF, 0x15, 0x02, 00, 00, 00, 0xEB, 0x08};
			uint64_t addr = { 0 };
			HookJump ret;
		};
#pragma pack(pop)

		constexpr static uint8_t MaxInstructionLen{ 15 };
		constexpr static uint8_t OriginalOpcodeSize{ MaxInstructionLen * 2 };
		constexpr static uint8_t jumpInstLen{ sizeof(HookJump) };
		constexpr static uint8_t Nop{ 0x90 };

		uint8_t original_opcode[MaxInstructionLen * 2]{ Nop };
		uint8_t original_opcode_size{ 0 };
		HookJump hook_jmp;
		Tramplin tramplin;
		ExternPointer tramplin_addr{ nullptr };

		Process* process{ nullptr };
		ExternPointer addr{ nullptr };
		HookFunction function{ nullptr };

		bool is_attach{ false };
	};

	class HookInstructionX32
	{
	public:
		typedef void (*HookFunction)();//asm func

		HookInstructionX32();
		~HookInstructionX32();

		//tramplin_size min 5 max 30
		bool init(Process& process, ExternPointer addr, uint8_t tramplin_size, HookFunction function);
		bool attach();
		void deattach();

	private:
		void readOriginalInstructin();

#pragma pack(push, 1)
		struct HookJump
		{
			uint8_t jmp = { 0xE8 };
			int32_t offset;
		};
#pragma pack(pop)

		constexpr static uint8_t MaxInstructionLen{ 15 };
		constexpr static uint8_t OriginalOpcodeSize{ MaxInstructionLen * 2 };
		constexpr static uint8_t jumpInstLen{ sizeof(HookJump) };
		constexpr static uint8_t Nop{ 0x90 };

		uint8_t original_opcode[MaxInstructionLen * 2]{ Nop };
		uint8_t original_opcode_size{ 0 };
		HookJump hook_jmp;

		Process* process{ nullptr };
		ExternPointer addr{ nullptr };
		HookFunction function{ nullptr };

		bool is_attach{ false };
	};

	inline void* GetRealAddressFunction(const void* addr)
	{
	#ifdef _DEBUG
#pragma pack(push, 1)
		struct Opcode
		{
			uint8_t command;
			int32_t offset;
		};
#pragma pack(pop)
		const Opcode* opcode = reinterpret_cast<const Opcode*>(addr);
		if (opcode->command != 0xE9)
			return nullptr;
		size_t ret;
		if (opcode->offset >= 0)
			ret = size_t(addr) + opcode->offset + sizeof(Opcode);
		else
			ret = size_t(addr) - size_t(opcode->offset * int32_t(-1)) + sizeof(Opcode);
		return PVOID(ret);
	#else
		return PVOID(addr);
	#endif // _DEBUG

	}
}