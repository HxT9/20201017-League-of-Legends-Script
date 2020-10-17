#pragma once
#include "SpellInfo.h"
#include "CastingStruct.h"

class SpellSlot {
public:

	int GetSpellLvl();
	bool IsSpellReady();
	SpellInfo* GetSpellInfo();
	CastingStruct* GetCastingStruct();
};