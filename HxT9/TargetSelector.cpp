#include "TargetSelector.h"
#include "globalVars.h"

EntityBase* TargetSelector::getBestMinion(bool onlyLastHit, float dmg, float range, float speed, float castTime, bool isSpell, bool isAP) {
	//Cerco un minion per il lasthit
	float dmgIncoming, AATimeNeeded = 0, effDamage;
	EntityBase* retMinion = NULL;
	EntityBase* temp;
	std::vector<EntityBase*> goodMinions;
	std::vector<EntityBase*> almostGoodMinions;

	for (int i = 0; i < entitiesContainer.minionsIndex.size(); i++) {
		temp = entitiesContainer.entities[entitiesContainer.minionsIndex[i]];
		if (temp->Team == myHero.Team
			|| temp->Pos.distTo(myHero.Pos) >= (range + myHero.BoundingRadius + temp->BoundingRadius)
			|| !temp->PCObject->isTargetable() || !GH.isAlive(temp->PCObject) || temp->MaxHealth < 10)
			continue;

		if (isAP)
			effDamage = utils.calcEffectiveDamage(dmg, temp->MagicResist);
		else
			effDamage = utils.calcEffectiveDamage(dmg, temp->Armor);

		AATimeNeeded = (temp->Pos.distTo(myHero.Pos) / speed) + castTime;

		dmgIncoming = temp->IncomingDamage(AATimeNeeded);

		if ((temp->Health - (dmgIncoming) > 0) && effDamage > temp->Health - dmgIncoming) {
			goodMinions.push_back(temp);
		}
		else if (temp->Health - dmgIncoming < effDamage + 150 && gameTime < 900 && temp->PCObject->getCloseEnemyMinions(550) > 2) {
			almostGoodMinions.push_back(temp);
		}
		else {
			retMinion = temp;
		}
	}
	if (onlyLastHit || almostGoodMinions.size() + goodMinions.size() > 0)
		retMinion = NULL;

	/*Adesso ciclo i goodMinion
	* Se trovo un siege allora ritorno quello, altrimenti mi metto buono un melee se c'è, altrimenti quello che rimane
	* Se non ho trovato subito un siege, controllo tra gli almostGood se c'è un siege. In quel caso non ritorno nulla
	* */
	for (EntityBase* e : goodMinions) {
		if (e->ObjectName.find("Siege") != std::string::npos)
			return e;

		if (e->ObjectName.find("Melee") != std::string::npos) {
			retMinion = e;
		}

		if (!retMinion)
			retMinion = e;
	}

	for (EntityBase* e : almostGoodMinions) {
		if (e->ObjectName.find("Siege") != std::string::npos)
			return NULL;
	}

	return retMinion;
}

EntityBase* TargetSelector::getBestChampion(float range, std::string buff) {
	return getBestChampion(range, false, buff);
}

EntityBase* TargetSelector::getBestChampion(float range, bool aa, std::string buff) {
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
				temp->MaxHealth > 10 &&
				(buff == "" || temp->GetBuff(buff))) {
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