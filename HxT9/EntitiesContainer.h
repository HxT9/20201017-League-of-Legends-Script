#pragma once
#include <Windows.h>
#include <vector>
#include "EntityBase.h"

namespace EntitiesContainer
{
	extern DWORD ObjManager;
	extern DWORD EntityListStartAddress;
	extern DWORD EntityListEndAddress;
	extern DWORD MaxIndex;

	extern std::vector<EntityBase*> Entities;

	extern std::vector<int> HeroesIndex;
	extern std::vector<int> MinionsIndex;
	extern std::vector<int> TurretsIndex;
	extern std::vector<int> MissilesIndex;
	extern std::vector<int> TroysIndex;

	void Init();
	void ResetEntities();
	EntityBase* GetEntityFromIndex(int index);
	void Tick();
	void UpdateEL();
	bool IsValidObject(DWORD obj);
};

