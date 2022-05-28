#include "OrbWalker.h"
#include <Windows.h>
#include "Vector3.h"
#include "globalVars.h"
#include "offsets.h"
#include "EntityBase.h"
#include "TargetSelector.h"
#include "UtilityFunctions.h"
#include "GameFunctions.h"


namespace OrbWalker {
	bool Focused = true;
	float Dmg = 0;

	void comboLogic() {
		myHero.AutoAttack(TargetSelector::GetBestChampion(myHero.AttackRange, true));
	}

	void pushLogic() {
		EntityBase* target = TargetSelector::GetBestMinion(false, Dmg, myHero.AttackRange, myHero.AAMissileSpeed, myHero.AACastTime, false, false);
		if (target == NULL)
			target = TargetSelector::GetTurret();
		myHero.AutoAttack(target);
	}

	void lastHitLogic() {
		myHero.AutoAttack(TargetSelector::GetBestMinion(true, Dmg, myHero.AttackRange, myHero.AAMissileSpeed, myHero.AACastTime, false, false));
	}

	void fleeLogic() {
		myHero.MoveTo(GameFunctions::GetMouseWorldPosition());
	}

	void Tick() {
		bool chatOpened = UtilityFunctions::IsChatOpen();
		Dmg = myHero.GetBasicAttackDamage(0, 0);

		myHero.Behaviour = Behaviour::Nothing;
		if (Focused && !chatOpened) {
			if (GetKeyState(' ') & 0x8000) {
				myHero.Behaviour = Behaviour::Combo;
				comboLogic();
			}
			else if (GetKeyState('C') & 0x8000) {
				myHero.Behaviour = Behaviour::Push;
				pushLogic();
			}
			else if (GetKeyState('V') & 0x8000) {
				myHero.Behaviour = Behaviour::Flee;
				fleeLogic();
			}
			else if (GetKeyState('X') & 0x8000) {
				myHero.Behaviour = Behaviour::LastHit;
				lastHitLogic();
			}
		}
	}
}