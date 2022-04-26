#pragma once
#include "EntityBase.h"

class TargetSelector
{
public:
	EntityBase* getBestMinion(bool onlyLastHit, float dmg, float range, float speed, float castTime, bool isSpell, bool isAP);
	EntityBase* getBestChampion(float range, std::string buff = "");
	EntityBase* getBestChampion(float range, bool aa, std::string buff = "");
	EntityBase* getTurret();
	EntityBase* getClickedChampion(Vector3 pos, float maxRange);
};

