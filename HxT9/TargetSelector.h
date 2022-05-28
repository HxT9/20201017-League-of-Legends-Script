#pragma once
#include "EntityBase.h"

namespace TargetSelector
{
	EntityBase* GetBestMinion(bool onlyLastHit, float dmg, float range, float speed, float castTime, bool isSpell, bool isAP);
	EntityBase* GetBestChampion(float range, std::string buff = "");
	EntityBase* GetBestChampion(float range, bool aa, std::string buff = "");
	EntityBase* GetTurret();
	EntityBase* GetClickedChampion(Vector3 pos, float maxRange);
};

