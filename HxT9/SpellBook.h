#pragma once
#include "ActiveSpell.h"
#include "SpellSlot.h"
#include "Enums.h"

class SpellBook {
public:

	ActiveSpell* GetActiveSpell();
	SpellSlot* GetSpellSlot(Spells slot);
};