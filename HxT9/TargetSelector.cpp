#include "TargetSelector.h"
#include "globalVars.h"

EntityBase* TargetSelector::getBestMinion(bool onlyLastHit, float dmg, float range, float speed, float castTime, bool isSpell) {
	//Cerco un minion per il lasthit
	float dmgIncoming, AATimeNeeded = 0;
	int dyingMinions = 0;
	EntityBase* randomMinion = NULL;
	EntityBase* temp;

	for (int i = 0; i < entitiesContainer.minionsIndex.size(); i++) {
		temp = entitiesContainer.entities[entitiesContainer.minionsIndex[i]];
		if (temp->Team == myHero.Team
			|| temp->Pos.distTo(myHero.Pos) >= (range + myHero.BoundingRadius + temp->BoundingRadius)
			|| !temp->PCObject->isTargetable() || !GH.isAlive(temp->PCObject) || temp->MaxHealth < 10)
			continue;

		AATimeNeeded = (temp->Pos.distTo(myHero.Pos) / speed) + castTime;

		dmgIncoming = 0;
		/*for (int j = 0; j < entitiesContainer.missiles.size(); j++) {
			if (entitiesContainer.missiles[j]->GetMissileTargetIndex() != temp->Index)
				continue;

			if (entitiesContainer.missiles[j]->GetPos().distTo(temp->GetPos()) / 300 < AATimeNeeded)
				dmgIncoming += utils.getMissileSourceEntity(entitiesContainer.missiles[j])->GetTotalAttackDamage();
		}*/

		/*for (int j = 0; j < entitiesContainer.iMinions; j++) {
			if (std::string(entitiesContainer.minions[j]->GetMinionName()).find("melee") != std::string::npos || entitiesContainer.minions[j]->GetActiveSpell() == NULL || entitiesContainer.minions[j]->GetActiveSpell()->GetTargetIndex() != entitiesContainer.minions[i]->Index)
				continue;
			if ((entitiesContainer.minions[j]->GetActiveSpell()->GetCastingTime() + entitiesContainer.minions[j]->GetActiveSpell()->GetCastTime() - gameTime + entitiesContainer.minions[j]->GetPos().distTo(entitiesContainer.minions[i]->GetPos()) / 500) < AATimeNeeded)
				dmgIncoming += entitiesContainer.minions[j]->GetTotalAttackDamage();
		}*/ //Questo serve per prendere anche quelli che stanno castando l'AA

		if ((temp->Health - (dmgIncoming) > 0) && dmg > temp->Health - (dmgIncoming)) {
			return temp;
		}
		else if (temp->Health - (dmgIncoming) < dmg + 150 && gameTime < 900 && temp->PCObject->getCloseEnemyMinions(550) > 2) {
			dyingMinions++;
		}
		else {
			randomMinion = temp;
		}
	}
	if (onlyLastHit || dyingMinions > 0)
		randomMinion = NULL;

	return randomMinion;
}

EntityBase* TargetSelector::getBestChampion(float range) {
	return getBestChampion(range, false);
}

EntityBase* TargetSelector::getBestChampion(float range, bool aa) {
	float minHP = 100000;
	EntityBase* bestHero = NULL;
	EntityBase* temp;
	float r = range;
	for (int i = 0; i < entitiesContainer.heroesIndex.size(); i++) {
		temp = entitiesContainer.entities[entitiesContainer.heroesIndex[i]];
		if (utils.isValidTarget(temp)){
			if (aa)
				r = range + temp->BoundingRadius;

			if (temp->Team != myHero.Team &&
				temp->Pos.distTo(myHero.Pos) <= r &&
				temp->MaxHealth > 10) {
				if (myHero.selectedTargetIndex == temp->Index)
					return temp;

					if (temp->Health < minHP) {
						minHP = temp->Health;
							bestHero = temp;
					}
			}
		}
	}
	return bestHero;
}
EntityBase* TargetSelector::getTurret() {
	EntityBase* temp;
	for (int i = 0; i < entitiesContainer.turretsIndex.size(); i++) {
		temp = entitiesContainer.entities[entitiesContainer.turretsIndex[i]];
		if (utils.isValidTarget(temp) &&
			temp->Team != myHero.Team &&
			temp->Pos.distTo(myHero.Pos) < (myHero.AttackRange + myHero.BoundingRadius + temp->BoundingRadius))

			return temp;
	}
	return NULL;
}

EntityBase* TargetSelector::getClickedChampion(Vector3 pos, float maxRange) {
	EntityBase*temp, *selected = NULL;
	float min = maxRange, tempDist;
	for (int i = 0; i < entitiesContainer.heroesIndex.size(); i++) {
		temp = entitiesContainer.entities[entitiesContainer.heroesIndex[i]];
		tempDist = temp->Pos.distTo(pos);
		if (utils.isValidTarget(temp) && temp->IsEnemyTo(&myHero) && tempDist < min) {
			selected = temp;
			min = tempDist;
		}
	}
	return selected;
}