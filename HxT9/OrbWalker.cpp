#include "OrbWalker.h"
#include <Windows.h>
#include "Vector3.h"
#include "globalVars.h"
#include "offsets.h"

void OrbWalker::tick() {
	bool chatOpened = *(bool*)(*(DWORD*)(baseAddress + oChatClientBox) + oChatBoxChatOpened);

	dmg = myHero.LPObject->GetTotalAttackDamage();
	if (strcmp(myHero.championName, "kalista") == 0) {
		dmg *= 0.9;
	}

	myHero.behaviour = Behaviour::Nothing;
	if (focused && !chatOpened) {
		if (GetKeyState(' ') & 0x8000) {
			myHero.behaviour = Behaviour::Combo;
			ComboLogic();
		}
		else if (GetKeyState('C') & 0x8000) {
			myHero.behaviour = Behaviour::Push;
			PushLogic();
		}
		else if (GetKeyState('V') & 0x8000) {
			myHero.behaviour = Behaviour::Flee;
			FleeLogic();
		}
		else if (GetKeyState('X') & 0x8000) {
			myHero.behaviour = Behaviour::LastHit;
			LastHitLogic();
		}
	}
}

void OrbWalker::ComboLogic() {
	myHero.AutoAttack(targetSelector.getBestChampion(myHero.LPObject->GetAttackRange(), true));
}
void OrbWalker::PushLogic() {
	CObject* target = targetSelector.getBestMinion(false, dmg, myHero.LPObject->GetAttackRange(), myHero.AAMissileSpeed, myHero.AACastTime, false);
	if (target == NULL)
		target = targetSelector.getTurret();
	myHero.AutoAttack(target);
}
void OrbWalker::LastHitLogic() {
	myHero.AutoAttack(targetSelector.getBestMinion(true, dmg, myHero.LPObject->GetAttackRange(), myHero.AAMissileSpeed, myHero.AACastTime, false));
}
void OrbWalker::FleeLogic() {
	myHero.MoveTo(GH.getMouseWorldPosition());
}