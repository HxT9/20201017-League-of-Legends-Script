#include "SpellBook.h"
#include "offsets.h"
#include <Windows.h>

ActiveSpell* SpellBook::GetActiveSpell() {
	return *(ActiveSpell**)((DWORD)this + oSpellBookActiveSpell);
}

float SpellBook::GetManaCost(int slot) {
	return *(float*)((DWORD)this + oSpellBookManaCost + (0x10 * slot));
}

SpellSlot* SpellBook::GetSpellSlot(Spells slot) {
	return *(SpellSlot**)((DWORD)this + oSpellBookSpellSlots + ((int)slot * sizeof(DWORD)));
}