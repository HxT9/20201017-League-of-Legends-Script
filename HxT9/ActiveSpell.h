#pragma once
#include "SpellInfo.h"
#include "Vector3.h"

class ActiveSpell {
public:

	SpellInfo* GetSpellInfo();
	float GetCastingStartTime();
	float GetCastingEndTime();
	Vector3 GetStartPos();
	Vector3 GetEndPos();
	short GetTargetIndex();
	float GetCastTime();
	float GetChannelStartTime();
	float GetChannelEndTime();
	float GetChannelingTime();
};