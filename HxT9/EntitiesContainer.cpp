#include "EntitiesContainer.h"
#include "globalVars.h"
#include "offsets.h"
#include "Enums.h"

EntitiesContainer::EntitiesContainer() {
	resetEntities();
}
void EntitiesContainer::resetEntities() {
	heroes.clear();
	minions.clear();
	turrets.clear();
	troys.clear();
	missiles.clear();

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
			heroes.insert(heroes.end(), CurrentObj);
			continue;
		}
		if (GH.isMinion(CurrentObj)) {
			minions.insert(minions.end(), CurrentObj);
			continue;
		}
		if (GH.isTurret(CurrentObj)) {
			turrets.insert(turrets.end(), CurrentObj);
			continue;
		}
		if (GH.isMissile(CurrentObj)) {
			missiles.insert(missiles.end(), CurrentObj);
			continue;
		}
		/*if (GH.isTroy(CurrentObj)) {
			troys.insert(troys.end(), CurrentObj);
			continue;
		}*/
	}
}