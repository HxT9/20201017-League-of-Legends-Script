#pragma once
#include "Enums.h"
#include "CObject.h"

class ChampionScript
{
public:
	void tick();
	float getKalistaSpearDamage(CObject* target);
};

