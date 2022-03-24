#pragma once
#include <Windows.h>
#include <vector>
#include "EntityBase.h"

class EntitiesContainer
{
public:
	DWORD ObjManager = NULL;
	DWORD EntityListStartAddress = NULL;
	DWORD EntityListEndAddress = NULL;
	DWORD MaxIndex = 0;

	std::vector<EntityBase*> entities;

	std::vector<int> heroesIndex;
	std::vector<int> minionsIndex;
	std::vector<int> turretsIndex;
	std::vector<int> missilesIndex;
	std::vector<int> troysIndex;

	EntitiesContainer();
	void Init();
	void resetEntities();
	EntityBase* GetEntityFromIndex(int index);
	void tick();
	void updateEL();
	bool isValidObject(DWORD obj);
};

