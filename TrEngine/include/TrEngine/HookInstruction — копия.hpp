#pragma once
#include <stdint.h>

#include <vector>


//for DLL

namespace TrEngine
{
	class HookInstruction
	{
	public:
		typedef void (*HookFunction)();//asm func

		HookInstruction();
		~HookInstruction();

		//tramplin_size min 5 max 30
		bool init(void* addr, uint8_t tramplin_size, HookFunction function);
		bool attach();
		void deattach();

	private:
		void readOriginalInstructin();
		void* findSpaceForTramplin();

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
		Tramplin* tramplin{ nullptr };

		void* addr{ nullptr };
		HookFunction function{ nullptr };

		bool is_attach{ false };
	};
}