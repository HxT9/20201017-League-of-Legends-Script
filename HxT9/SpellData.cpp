#include "SpellData.h"
#include "offsets.h"
#include <Windows.h>
#include "globalVars.h"

char* GetStr(DWORD address) {
	if (*(int*)(address + 0x10) > 15)
		return (char*)(*(DWORD*)address);
	else
		return (char*)address;
}

char* SpellData::GetName() {
	return *(char**)((DWORD)this + oSpellDataName);
}

float SpellData::GetSpellSpeed() {
	return *(float*)((DWORD)this + oSpellDataSpellSpeed);
}