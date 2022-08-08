#pragma once

#include <TrEngine/Process.hpp>
#include <TrEngine/HookInstruction.hpp>
#include <TrEngine/Pipe.hpp>

class Cheat
{
public:
	Cheat();
	~Cheat();

	bool open();
	bool isOpen() const;
	int32_t getID() const;

	bool activeInfiniteHP();
	void deactiveInfiniteHP();

	bool activeInfiniteMinionsHP();
	void deactiveInfiniteMinionsHP();

	bool activeOverlordSetMaxHP();
	void deactiveOverlordSetMaxHP();
	bool setOverlordSetMaxHP(float value);

	bool activeMinionsSetMaxHP();
	void deactiveMinionsSetMaxHP();
	bool setMinionsSetMaxHP(float value);

	// MP
	bool activeInfiniteMP();
	void deactiveInfiniteMP();

	bool activeMPMultipler();
	void deactiveMPMultipler();
	bool setMPMultipler(float value);

	// One hit kill
	bool activeOneHitKill();
	void deactiveOneHitKill();

private:
	void destructor();

	mutable TrEngine::Process process;
	mutable TrEngine::PipeClient pipe;
	size_t module_adr{ 0 };

	TrEngine::Offset hp_inst_offset{ 0x2CD245 };
	TrEngine::ExternPointer hp_inst_mem{ nullptr };
	TrEngine::HookInstructionX32 hp;
};