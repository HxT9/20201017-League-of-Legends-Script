#pragma once
#include "ActiveSpell.h"
#include "SpellSlot.h"
#include "Enums.h"

class SpellBook {
public:

	ActiveSpell* GetActiveSpell();
	float GetManaCost(int slot);
	SpellSlot* GetSpellSlot(Spells slot);
};