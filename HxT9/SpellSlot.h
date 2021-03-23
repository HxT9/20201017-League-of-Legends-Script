#pragma once
#include "SpellInfo.h"
#include "CastingStruct.h"

class SpellSlot {
public:

	int GetSpellLvl();
	bool IsSpellReady();
	float GetSpellReady();
	SpellInfo* GetSpellInfo();
	CastingStruct* GetCastingStruct();
};