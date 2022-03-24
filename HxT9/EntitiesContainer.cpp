#include "EntitiesContainer.h"
#include "globalVars.h"
#include "offsets.h"
#include "Enums.h"

EntitiesContainer::EntitiesContainer() {
	EntityListStartAddress = NULL;
	EntityListEndAddress = NULL;
	MaxIndex = 0;	
}

void EntitiesContainer::Init() {
	gui.print("Initiating EntitiesContainer");
	ObjManager = *(DWORD*)(baseAddress + oObjectsManager);

	entities.clear();
	for (int i = 0; i < 0x10000; i++) {
		entities.push_back(new EntityBase());
	}
}

void EntitiesContainer::resetEntities() {
	heroesIndex.clear();
	minionsIndex.clear();
	turretsIndex.clear();
	missilesIndex.clear();
	troysIndex.clear();
}

EntityBase* EntitiesContainer::GetEntityFromIndex(int index) {
	if (entities[index]->PCObject)
		return entities[index];

	return &EntityBase();
}

void EntitiesContainer::tick() {
	if (ObjManager == NULL)
		return;

	updateEL();

	resetEntities();

	int index;
	CObject *CurrentObj;

	for(DWORD i = EntityListStartAddress; i < EntityListEndAddress; i += 4){
		index = (i - EntityListStartAddress) / 4;

		if (isValidObject(i)) {
			CurrentObj = *(CObject**)i;

			/*if (GH.isTroy(CurrentObj)) {
				if (entities[index]->PCObject != CurrentObj)
					entities[index] = &EntityBase(CurrentObj, EntityType::Troy);
				troysIndex.push_back(index);
				entities[index]->UpdateAttributes();
				continue;
			}*/
			if (GH.isMinion(CurrentObj)) {
				if (entities[index]->PCObject != CurrentObj)
					entities[index]->Init(CurrentObj, EntityType::Minion);
				minionsIndex.push_back(index);
				entities[index]->UpdateAttributes();
				continue;
			}
			if (GH.isMissile(CurrentObj)) {
				if (entities[index]->PCObject != CurrentObj)
					entities[index]->Init(CurrentObj, EntityType::Missile);
				missilesIndex.push_back(index);
				entities[index]->UpdateAttributes();
				continue;
			}
			if (GH.isTurret(CurrentObj)) {
				if (entities[index]->PCObject != CurrentObj)
					entities[index]->Init(CurrentObj, EntityType::Turret);
				turretsIndex.push_back(index);
				entities[index]->UpdateAttributes();
				continue;
			}
			if (GH.isHero(CurrentObj)) {
				if (entities[index]->PCObject != CurrentObj)
					entities[index]->Init(CurrentObj, EntityType::Hero);
				heroesIndex.push_back(index);
				entities[index]->UpdateAttributes();
				continue;
			}
			entities[index]->Delete();
		}
		else {
			entities[index]->Delete();
		}
	}
}

void EntitiesContainer::updateEL() {
	EntityListStartAddress = ((DWORD*)ObjManager)[5];
	EntityListEndAddress = ((DWORD*)ObjManager)[6];
	MaxIndex = (EntityListEndAddress - EntityListStartAddress) / 4;
}

bool EntitiesContainer::isValidObject(DWORD obj) {
	return *(DWORD*)obj && !(*(BYTE*)obj & 1);
}

/*CObject* EntitiesContainer::getNextObject(CObject* curObj) {
	int nextIndex = curObj->Index + 1;
	DWORD nextAddr;

	if (nextIndex < MaxIndex) { //maxIndex escluso
		nextAddr = EntityListStartAddress + nextIndex * 4;
		
		while (!isValidObject(nextAddr)) {
			nextIndex++;
			nextAddr += 4;

			if (nextIndex >= MaxIndex)
				break;
		}
	}
	return NULL;
}*/