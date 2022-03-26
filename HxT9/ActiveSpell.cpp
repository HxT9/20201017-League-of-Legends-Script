#include "ActiveSpell.h"
#include "offsets.h"
#include <Windows.h>

SpellInfo* ActiveSpell::GetSpellInfo() {
	return *(SpellInfo**)((DWORD)this + oActiveSpellSpellInfoPtr);
}

float ActiveSpell::GetCastingStartTime() {
	return *(float*)((DWORD)this + oActiveSpellCastTime);
}

float ActiveSpell::GetCastingEndTime() {
	return *(float*)((DWORD)this + oActiveSpellEndCastTime);	
}

Vector3 ActiveSpell::GetStartPos() {
	return *(Vector3*)((DWORD)this + oActiveSpellStartPos);
}

Vector3 ActiveSpell::GetEndPos() {
	return *(Vector3*)((DWORD)this + oActiveSpellEndPos);
}

short ActiveSpell::GetTargetIndex() {
	short* targets = *(short**)((DWORD)this + oActiveSpellTargetArray);
	int lastTarget;
	if (targets) {
		lastTarget = *(int*)((DWORD)this + oActiveSpellTargetArraySize);
		return targets[lastTarget - 1];
	}
	return 0;
}

float ActiveSpell::GetCastTime() {
	return *(float*)((DWORD)this + oActiveSpellCastTime);
}

float ActiveSpell::GetChannelStartTime() {
	return *(float*)((DWORD)this + oActiveSpellChannelStartTime);
}

float ActiveSpell::GetChannelEndTime() {
	return *(float*)((DWORD)this + oActiveSpellChannelEndTime);
}

float ActiveSpell::GetChannelingTime() {
	return GetChannelEndTime() - GetChannelStartTime();
}