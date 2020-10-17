#pragma once
#include "CObject.h"
#include <map>
#include <vector>

class EntitiesContainer
{
public:
	DWORD ObjManager;

	std::vector<int> vHeroes;
	std::vector<int> vMinions;
	std::vector<int> vTurrets;
	std::vector<int> vTroys;
	std::vector<int> vMissiles;

	std::map<short, CObject*> heroes;
	std::map<short, CObject*> minions;
	std::map<short, CObject*> turrets;
	std::map<short, CObject*> troys;
	std::map<short, CObject*> missiles;

	EntitiesContainer();
	void resetEntities();
	void tick();
};

