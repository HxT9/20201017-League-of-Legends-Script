#include "EntitiesContainer.h"
#include "globalVars.h"
#include "offsets.h"
#include "Enums.h"

EntitiesContainer::EntitiesContainer() {
	resetEntities();
}
void EntitiesContainer::resetEntities() {
	heroes.clear();
	vHeroes.clear();

	minions.clear();
	vMinions.clear();

	turrets.clear();
	vTurrets.clear();

	troys.clear();
	vTroys.clear();

	missiles.clear();
	vMissiles.clear();

	ObjManager = *(DWORD*)(baseAddress + oObjectsManager);
}

/*void EntitiesContainer::tick() {
	resetEntities();

	if (ObjManager == NULL)
		return;

	int i = 0;
	int nObjs = *(int*)(ObjManager + 0x8);
	int readObjs = 0;
	CObject** objArray = *(CObject***)(ObjManager + 0x14);
	CObject* CurrentObj;

	while (readObjs < nObjs) {
		CurrentObj = objArray[i++];

		if (CurrentObj == NULL || CurrentObj->GetIndex() < 0 || CurrentObj->GetIndex() > nObjs)
			continue;

		

		readObjs++;
		if (funcs.isHero(CurrentObj)) {
			heroes[iHeroes++] = CurrentObj;
			continue;
		}
		if (funcs.isMinion(CurrentObj)) {
			minions[iMinions++] = CurrentObj;
			if (funcs.isDragon(CurrentObj)) {
				dragons[iDragons++] = CurrentObj;
				continue;
			}
			continue;
		}
		if (funcs.isTurret(CurrentObj)) {
			turrets[iTurrets++] = CurrentObj;
			continue;
		}
		if (funcs.isTroy(CurrentObj)) {
			troys[iTroys++] = CurrentObj;
			continue;
		}
		if (funcs.isMissile(CurrentObj)) {
			missiles[iMissiles++] = CurrentObj;
			continue;
		}
	}
}*/

void EntitiesContainer::tick() {
	resetEntities();

	if (ObjManager == NULL)
		return;

	int readObjs = 0;
	CObject *CurrentObj, *NextObj = GH.getFirstCObject((void*)ObjManager);

	while (NextObj) {

		CurrentObj = NextObj;
		NextObj = GH.getNextCObject((void*)ObjManager, NextObj);

		if (GH.isHero(CurrentObj)) {
			heroes[CurrentObj->GetIndex()] = CurrentObj;
			vHeroes.insert(vHeroes.end(), CurrentObj->GetIndex());
			continue;
		}
		if (GH.isMinion(CurrentObj)) {
			minions[CurrentObj->GetIndex()] = CurrentObj;
			vMinions.insert(vMinions.end(), CurrentObj->GetIndex());
			continue;
		}
		if (GH.isTurret(CurrentObj)) {
			turrets[CurrentObj->GetIndex()] = CurrentObj;
			vTurrets.insert(vTurrets.end(), CurrentObj->GetIndex());
			continue;
		}
		if (GH.isMissile(CurrentObj)) {
			missiles[CurrentObj->GetIndex()] = CurrentObj;
			vMissiles.insert(vMissiles.end(), CurrentObj->GetIndex());
			continue;
		}
		if (GH.isTroy(CurrentObj)) {
			troys[CurrentObj->GetIndex()] = CurrentObj;
			vTroys.insert(vTroys.end(), CurrentObj->GetIndex());
			continue;
		}
	}
}