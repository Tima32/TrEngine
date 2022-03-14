#pragma once

#include <TrEngine/Process.hpp>
#include <TrEngine/HookInstruction.hpp>

class Cheat
{
public:
	Cheat();
	~Cheat();

	bool open();
	bool isOpen() const;
	int32_t getID() const;

	bool setMinerals(int32_t minerals);
	bool getMinerals(int32_t& minerals) const;
	bool setVespen(int32_t vespen);
	bool getVespen(int32_t& vespen) const;

	bool activeInfiniteShields();
	void deactiveInfiniteShields();

	bool activeInfiniteHP();
	void deactiveInfiniteHP();

	bool activeSubjugation();
	void deactiveSubjugation();

	bool activeInfiniteEnergy();
	void deactiveInfiniteEnergy();

private:
	void destructor();

	mutable TrEngine::Process process;
	size_t module_adr{0};

	TrEngine::Offset minerals_offset{ 0x206F9F8, 0x0, 0x958 };
	TrEngine::Offset vespen_offset{ 0x206F9F8, 0x0, 0x960 };

	TrEngine::Offset shields_inst_offset{ 0x592CC7 };
	TrEngine::ExternPointer shield_inst_mem{ nullptr };
	TrEngine::HookInstruction shields;

	TrEngine::Offset hp_inst_offset{ 0x593C3D };
	TrEngine::ExternPointer hp_inst_mem{ nullptr };
	TrEngine::HookInstruction hp;

	TrEngine::Offset energy_inst_offset{ 0x59209E };
	TrEngine::ExternPointer energy_inst_mem{ nullptr };
	TrEngine::HookInstruction energy;

	bool subjugation{ false };
};