#pragma once
#include "CObject.h"
#include <map>
#include <vector>

class EntitiesContainer
{
public:
	DWORD ObjManager;

	std::vector<CObject*> heroes;
	std::vector<CObject*> minions;
	std::vector<CObject*> turrets;
	std::vector<CObject*> troys;
	std::vector<CObject*> missiles;

	EntitiesContainer();
	void resetEntities();
	void tick();
};

