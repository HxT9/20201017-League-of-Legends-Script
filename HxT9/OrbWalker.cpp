#include "OrbWalker.h"
#include <Windows.h>
#include "Vector3.h"
#include "globalVars.h"
#include "offsets.h"
#include "EntityBase.h"

void OrbWalker::tick() {
	bool chatOpened = utils.isChatOpen();
	dmg = myHero.GetBasicAttackDamage(0, 0);

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
	myHero.AutoAttack(targetSelector.getBestChampion(myHero.AttackRange, true));
}
void OrbWalker::PushLogic() {
	EntityBase* target = targetSelector.getBestMinion(false, dmg, myHero.AttackRange, myHero.AAMissileSpeed, myHero.AACastTime, false, false);
	if (target == NULL)
		target = targetSelector.getTurret();
	myHero.AutoAttack(target);
}
void OrbWalker::LastHitLogic() {
	myHero.AutoAttack(targetSelector.getBestMinion(true, dmg, myHero.AttackRange, myHero.AAMissileSpeed, myHero.AACastTime, false, false));
}
void OrbWalker::FleeLogic() {
	myHero.MoveTo(GH.getMouseWorldPosition());
}