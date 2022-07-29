#pragma once
#include <stdint.h>

enum class PipeMessageId : uint32_t
{
	None = 0,
	Message,
	Deattach,


	OverlordImmortalTrue,
	OverlordImmortalFalse,

	MinionsImmortalTrue,
	MinionsImmortalFalse,
	
	OverlordSetMaxHPTrue,
	OverlordSetMaxHPFalse,
	OverlordSetMaxHP,

	MinionsSetMaxHPTrue,
	MinionsSetMaxHPFalse,
	MinionsSetMaxHP,

	InfiniteMagickTrue,
	InfiniteMagickFalse,

	MagicSpendingMultiplerTrue,
	MagicSpendingMultiplerFalse,
	MagicSpendingMultipler,
};

struct PipeNoArgs
{
	PipeMessageId type{ PipeMessageId::None };
};
struct PipeMessage
{
	PipeMessageId type{ PipeMessageId::Message };
	char message[256];
};

struct PipeSetMaxHP
{
	PipeMessageId type{ PipeMessageId::None};
	float max_hp;
};
struct PipeSetMPMultipler
{
	PipeMessageId type{ PipeMessageId::MagicSpendingMultipler };
	float mp_multipler;
};