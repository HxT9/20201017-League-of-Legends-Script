#include "SpellInfo.h"
#include "offsets.h"
#include <Windows.h>

SpellData* SpellInfo::GetSpellData() {
	return *(SpellData**)((DWORD)this + oSpellInfoSpellData);
}