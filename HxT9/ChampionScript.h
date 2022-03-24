#pragma once
#include "Enums.h"
#include "EntityBase.h"

class ChampionScript
{
public:
	void tick();
	float getKalistaSpearDamage(EntityBase* target);
};

