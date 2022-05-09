#include "SpellBook.h"
#include "offsets.h"
#include <Windows.h>

ActiveSpell* SpellBook::GetActiveSpell() {
	return *(ActiveSpell**)((DWORD)this + oSpellBookActiveSpell);
}

SpellSlot* SpellBook::GetSpellSlot(Spells slot) {
	return *(SpellSlot**)((DWORD)this + oSpellBookSpellSlots + ((int)slot * sizeof(DWORD)));
}