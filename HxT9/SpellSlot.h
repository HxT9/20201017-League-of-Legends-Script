#pragma once
#include "SpellInfo.h"

class SpellSlot {
public:

	int GetSpellLvl();
	bool IsSpellReady();
	float GetSpellReady();
	SpellInfo* GetSpellInfo();
};