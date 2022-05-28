#include "EntitiesContainer.h"
#include "globalVars.h"
#include "offsets.h"
#include "Enums.h"
#include "GameFunctions.h"
#include "IMGUIManager.h"

namespace EntitiesContainer {
	DWORD ObjManager = NULL;
	DWORD EntityListStartAddress = NULL;
	DWORD EntityListEndAddress = NULL;
	DWORD MaxIndex = NULL;

	std::vector<EntityBase*> Entities;

	std::vector<int> HeroesIndex;
	std::vector<int> MinionsIndex;
	std::vector<int> TurretsIndex;
	std::vector<int> MissilesIndex;
	std::vector<int> TroysIndex;

	void Init() {
		EntityListStartAddress = NULL;
		EntityListEndAddress = NULL;
		MaxIndex = 0;

		ObjManager = *(DWORD*)(baseAddress + oObjectsManager);

		Entities.clear();
		for (int i = 0; i < 0x10000; i++) {
			Entities.push_back(new EntityBase());
		}
	}

	void ResetEntities() {
		HeroesIndex.clear();
		MinionsIndex.clear();
		TurretsIndex.clear();
		MissilesIndex.clear();
		TroysIndex.clear();
	}

	EntityBase* GetEntityFromIndex(int index) {
		if (Entities[index]->PCObject)
			return Entities[index];

		return &EntityBase();
	}

	void Tick() {
		if (ObjManager == NULL)
			return;

		UpdateEL();

		ResetEntities();

		int index;
		CObject* CurrentObj;

		for (DWORD i = EntityListStartAddress; i < EntityListEndAddress; i += 4) {
			index = (i - EntityListStartAddress) / 4;

			if (IsValidObject(i)) {
				CurrentObj = *(CObject**)i;

				/*if (GameFunctions::IsTroy(CurrentObj)) {
					if (entities[index]->PCObject != CurrentObj)
						entities[index] = &EntityBase(CurrentObj, EntityType::Troy);
					troysIndex.push_back(index);
					entities[index]->UpdateAttributes();
					continue;
				}*/
				if (GameFunctions::IsType(CurrentObj, ObjectTypeFlags::Minion)) {
					if (Entities[index]->PCObject != CurrentObj)
						Entities[index]->Init(CurrentObj, EntityType::Minion);
					MinionsIndex.push_back(index);
					Entities[index]->UpdateAttributes();
					continue;
				}
				if (GameFunctions::IsType(CurrentObj, ObjectTypeFlags::Missile)) {
					if (Entities[index]->PCObject != CurrentObj)
						Entities[index]->Init(CurrentObj, EntityType::Missile);
					MissilesIndex.push_back(index);
					Entities[index]->UpdateAttributes();
					continue;
				}
				if (GameFunctions::IsType(CurrentObj, ObjectTypeFlags::Turret)) {
					if (Entities[index]->PCObject != CurrentObj)
						Entities[index]->Init(CurrentObj, EntityType::Turret);
					TurretsIndex.push_back(index);
					Entities[index]->UpdateAttributes();
					continue;
				}
				if (GameFunctions::IsType(CurrentObj, ObjectTypeFlags::Hero)) {
					if (Entities[index]->PCObject != CurrentObj)
						Entities[index]->Init(CurrentObj, EntityType::Hero);
					HeroesIndex.push_back(index);
					Entities[index]->UpdateAttributes();
					continue;
				}

				Entities[index]->Delete();
			}
			else {
				Entities[index]->Delete();
				continue;
			}
		}
	}

	void UpdateEL() {
		EntityListStartAddress = ((DWORD*)ObjManager)[5];
		EntityListEndAddress = ((DWORD*)ObjManager)[6];
		MaxIndex = (EntityListEndAddress - EntityListStartAddress) / 4;
	}

	bool IsValidObject(DWORD obj) {
		return *(DWORD*)obj && !(*(BYTE*)obj & 1);
	}
}