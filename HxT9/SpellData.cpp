#include "SpellData.h"
#include "offsets.h"
#include <Windows.h>

char* GetStr(DWORD address) {
	if (*(int*)(address + 0x10) > 15)
		return (char*)(*(DWORD*)address);
	else
		return (char*)address;
}

char* SpellData::GetMissileName() {
	return *(char**)((DWORD)this + oSpellDataMissileName);
}

char* SpellData::GetSpellName() {
	return *(char**)((DWORD)this + oSpellDataSpellName);
}

float SpellData::GetSpellWidth() {
	return *(float*)((DWORD)this + oSpellDataSpellWidth);
}

float SpellData::GetSpellSpeed() {
	return *(float*)((DWORD)this + oSpellDataSpellSpeed);
}

float SpellData::GetSpellMaxCD() {
	return *(float*)((DWORD)this + oSpellDataSpellMaxCooldown);
}