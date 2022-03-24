#pragma once
#include "EntityBase.h"

class TargetSelector
{
public:
	EntityBase* getBestMinion(bool onlyLastHit, float dmg, float range, float speed, float castTime, bool isSpell);
	EntityBase* getBestChampion(float range);
	EntityBase* getBestChampion(float range, bool aa);
	EntityBase* getTurret();
	EntityBase* getClickedChampion(Vector3 pos, float maxRange);
};

