#include "TargetSelector.h"
#include "globalVars.h"

CObject* TargetSelector::getBestMinion(bool onlyLastHit, float dmg, float range, float speed, float castTime, bool isSpell) {
	//Cerco un minion per il lasthit
	float dmgIncoming, AATimeNeeded = 0;
	int dyingMinions = 0;
	CObject* randomMinion = NULL;
	CObject* temp;
	for (int i = 0; i < entities.minions.size(); i++) {
		temp = entities.minions[i];
		if (temp->GetTeam() == myHero.LPObject->GetTeam()
			|| temp->GetPos().distTo(myHero.LPObject->GetPos()) >= (range + myHero.LPObject->GetBoundingRadius())
			|| !GH.isTargetable(temp) || !GH.isAlive(temp) || temp->GetMaxHealth() < 10)
			continue;

		AATimeNeeded = (temp->GetPos().distTo(myHero.LPObject->GetPos()) / speed) + castTime;

		dmgIncoming = 0;
		for (int j = 0; j < entities.missiles.size(); j++) {
			if (entities.missiles[j]->GetMissileTargetIndex() != temp->GetIndex())
				continue;

			if (entities.missiles[j]->GetPos().distTo(temp->GetPos()) / 500 < AATimeNeeded)
				dmgIncoming += utils.getMissileSourceEntity(entities.missiles[j])->GetTotalAttackDamage();
		}

		/*for (int j = 0; j < entities.iMinions; j++) {
			if (std::string(entities.minions[j]->GetMinionName()).find("melee") != std::string::npos|| entities.minions[j]->GetActiveSpell() == NULL || entities.minions[j]->GetActiveSpell()->GetTargetIndex() != entities.minions[i]->GetIndex())
				continue;
			if ((entities.minions[j]->GetActiveSpell()->GetCastingTime() + entities.minions[j]->GetActiveSpell()->GetCastTime() - gameTime + entities.minions[j]->GetPos().distTo(entities.minions[i]->GetPos()) / 500) < AATimeNeeded)
				dmgIncoming += entities.minions[j]->GetTotalAttackDamage();
		}*/ //Questo serve per prendere anche quelli che stanno castando l'AA

		if ((entities.minions[i]->GetHealth() - (dmgIncoming * 0.8) > 0) && dmg > entities.minions[i]->GetHealth() - (dmgIncoming * 0.8)) {
			return entities.minions[i];
		}
		else if (entities.minions[i]->GetHealth() - (dmgIncoming) < dmg + 150 && gameTime < 900 && entities.minions[i]->getCloseEnemyMinions(550) > 2) {
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
CObject* TargetSelector::getBestChampion() {
	float minHP = 100000;
	CObject* bestHero = NULL;
	CObject* temp;
	for (int i = 0; i < entities.heroes.size(); i++) {
		temp = entities.heroes[i];
		if (utils.isValidTarget(temp) && 
			temp->GetTeam() != myHero.LPObject->GetTeam() &&
			temp->GetPos().distTo(myHero.LPObject->GetPos()) < (myHero.LPObject->GetAttackRange() + myHero.LPObject->GetBoundingRadius() + temp->GetBoundingRadius()) &&
			temp->GetMaxHealth() > 10) {

			if (temp->GetHealth() < minHP) {
				minHP = temp->GetHealth();
				bestHero = temp;
			}
		}
	}
	return bestHero;
}
CObject* TargetSelector::getBestChampion(float range) {
	float minHP = 100000;
	CObject* bestHero = NULL;
	CObject* temp;
	for (int i = 0; i < entities.heroes.size(); i++) {
		temp = entities.heroes[i];
		if (utils.isValidTarget(temp) && 
			temp->GetTeam() != myHero.LPObject->GetTeam() &&
			GH.isAlive(temp) &&
			temp->GetPos().distTo(myHero.LPObject->GetPos()) < range &&
			temp->GetMaxHealth() > 10) {

			if (temp->GetHealth() < minHP) {
				minHP = temp->GetHealth();
				bestHero = temp;
			}
		}
	}
	return bestHero;
}
CObject* TargetSelector::getTurret() {
	CObject* temp;
	for (int i = 0; i < entities.turrets.size(); i++) {
		temp = entities.turrets[i];
		if (utils.isValidTarget(temp) &&
			temp->GetTeam() != myHero.LPObject->GetTeam() &&
			temp->GetPos().distTo(myHero.LPObject->GetPos()) < (myHero.LPObject->GetAttackRange() + myHero.LPObject->GetBoundingRadius() + temp->GetBoundingRadius()))

			return temp;
	}
	return NULL;
}