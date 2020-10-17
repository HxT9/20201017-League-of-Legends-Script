#pragma once
#include "SpellInfo.h"
#include "Vector3.h"

class ActiveSpell {
public:

	SpellInfo* GetSpellInfo();
	float GetCastingTime();
	Vector3 GetStartPos();
	Vector3 GetEndPos();
	short GetTargetIndex();
	float GetCastTime();
	float GetChannelingTime();
	float GetSpellCooldown();
	bool IsSpell();
	bool IsAutoAttack();
	float GetSpellManaCost();
};