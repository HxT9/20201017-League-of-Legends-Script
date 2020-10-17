#pragma once
#include "CObject.h"

class TargetSelector
{
public:
	CObject* getBestMinion(bool onlyLastHit, float dmg, float range, float speed, float castTime, bool isSpell);
	CObject* getBestChampion();
	CObject* getBestChampion(float range);
	CObject* getTurret();
};

