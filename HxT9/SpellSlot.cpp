#include "SpellSlot.h"
#include "offsets.h"
#include "globalVars.h"
#include <Windows.h>

int SpellSlot::GetSpellLvl() {
	return *(int*)((DWORD)this + oSpellSlotSpellLvl);
}

bool SpellSlot::IsSpellReady() {
	return *(float*)((DWORD)this + oSpellSlotSpellReady) <= gameTime;
}

float SpellSlot::GetSpellReady() {
	return *(float*)((DWORD)this + oSpellSlotSpellReady);
}

SpellInfo* SpellSlot::GetSpellInfo() {
	return *(SpellInfo**)((DWORD)this + oSpellSlotSpellInfo);
}

CastingStruct* SpellSlot::GetCastingStruct() {
	return *(CastingStruct**)((DWORD)this + oSpellSlotCastingStruct);
}