#include "ActiveSpell.h"
#include "offsets.h"
#include <Windows.h>

SpellInfo* ActiveSpell::GetSpellInfo() {
	return *(SpellInfo**)((DWORD)this + oActiveSpellSpellInfoPtr);
}

float ActiveSpell::GetCastingTime() {
	return *(float*)((DWORD)this + oActiveSpellCastingStartTime);
}

Vector3 ActiveSpell::GetStartPos() {
	return *(Vector3*)((DWORD)this + oActiveSpellStartPos);
}

Vector3 ActiveSpell::GetEndPos() {
	return *(Vector3*)((DWORD)this + oActiveSpellEndPos);
}

short ActiveSpell::GetTargetIndex() {
	return *(short*)((DWORD)this + oActiveSpellTargetIndex);
}

float ActiveSpell::GetCastTime() {
	return *(float*)((DWORD)this + oActiveSpellCastTime);
}

float ActiveSpell::GetChannelingTime() {
	return *(float*)((DWORD)this + oActiveSpellChannelTime);
}

float ActiveSpell::GetSpellCooldown() {
	return *(float*)((DWORD)this + oActiveSpellSpellCooldown);
}

bool ActiveSpell::IsSpell() {
	return *(bool*)((DWORD)this + oActiveSpellIsSpell);
}

bool ActiveSpell::IsAutoAttack() {
	return *(bool*)((DWORD)this + oActiveSpellIsAA);
}

float ActiveSpell::GetSpellManaCost() {
	return *(float*)((DWORD)this + oActiveSpellManaCost);
}