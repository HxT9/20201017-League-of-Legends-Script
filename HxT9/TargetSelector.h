#pragma once
#include "CObject.h"

class TargetSelector
{
public:
	CObject* getBestMinion(bool onlyLastHit, float dmg, float range, float speed, float castTime, bool isSpell);
	CObject* getBestChampion(float range);
	CObject* getBestChampion(float range, bool aa);
	CObject* getTurret();
	CObject* getClickedChampion(Vector3 pos, float maxRange);
};

