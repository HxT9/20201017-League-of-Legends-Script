#include "BaseUlt.h"
#include "globalVars.h"

void BaseUlt::init() {
	if (strcmp(myHero.championName, "Ashe") == 0) {
		speed = 1600;
		castTime = 0.25;
		width = 260;
	}
	if (strcmp(myHero.championName, "Ezreal") == 0) {
		speed = 2000;
		castTime = 1;
		width = 320;
	}
	if (strcmp(myHero.championName, "Draven") == 0) {
		speed = 2000;
		width = 320;
		castTime = 0.5;
	}
	if (strcmp(myHero.championName, "Jinx") == 0) {
		speed = 1700; //to fix
		castTime = 0.6;
		width = 280;
	}

	Vector3 blueSpawnPosition = Vector3(400, 60, 420);
	Vector3 redSpawnPosition = Vector3(14290, 60, 14370);

	if (myHero.LPObject->GetTeam() == 100)
		spawnPoint = redSpawnPosition;
	else
		spawnPoint = blueSpawnPosition;
}

void BaseUlt::tick()
{
	utils.dbgStream("baseult");
	float dmg, baseDmg;
	float time;
	CObject* temp;

	//Controllo che non abbia stoppato il recall
	for (int i = 0; i < entities.heroes.size(); i++) {
		temp = entities.heroes[i];
		if (temp != NULL &&
			temp->GetIndex() == targetIndex &&
			temp->IsVisible()){
			if (temp->GetActiveSpell() != NULL) {
				if (strcmp(temp->GetActiveSpell()->GetSpellInfo()->GetSpellData()->GetSpellName(), "Recall") != 0) {
					targetIndex = 0;

					//gui.print(utils.stringf("BASEULT CANCELLED"));
				}
			}
			else{
				targetIndex = 0;

				//gui.print(utils.stringf("BASEULT CANCELLED"));
			}
		}
	}

	time = castTime + spawnPoint.distTo(myHero.LPObject->GetPos()) / speed;

	if (strcmp(myHero.championName, "Jinx") == 0) {
		time = castTime;
		if (spawnPoint.distTo(myHero.LPObject->GetPos()) > 1350) {
			time += 1350 / 1700;
			time += (spawnPoint.distTo(myHero.LPObject->GetPos()) - 1350) / 2200;
		}
		else {
			time += spawnPoint.distTo(myHero.LPObject->GetPos()) / 1700;
		}
	}

	if (targetIndex != 0) {
		if (abs(calculatedBaseUlt - gameTime - time) < 0.2) {
			//gui.print(utils.stringf("BASEULT CAST"));
			if (strcmp(myHero.championName, "Ezreal") == 0) {
				myHero.nextActionTime = 0;
				myHero.nextSpellTime = 0;
				//myHero.CastSpellAtPos(Spells::R, spawnPoint);
				myHero.castBaseUlt();
			}
			else {
				if (utils.heroesColliding(myHero.LPObject->GetPos(), spawnPoint, width) == 0) {
					myHero.nextActionTime = 0;
					myHero.nextSpellTime = 0;
					//myHero.CastSpellAtPos(Spells::R, spawnPoint);
					myHero.castBaseUlt();
				}
			}
		}
	}

	if (gameTime > calculatedBaseUlt - time + 1) targetIndex = 0;

	if (targetIndex == 0){
		if (strcmp(myHero.championName, "Ashe") == 0) {
			baseDmg = 200 * myHero.LPObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl();
			baseDmg += myHero.LPObject->GetAP();
		}
		if (strcmp(myHero.championName, "Ezreal") == 0) {
			baseDmg = 200 + 150 * myHero.LPObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl();
			baseDmg += myHero.LPObject->GetAP() * 0.9;
			baseDmg += myHero.LPObject->GetTotalAttackDamage();
		}
		if (strcmp(myHero.championName, "Draven") == 0) {
			baseDmg = 75 + 100 * myHero.LPObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl();
			baseDmg += myHero.LPObject->GetBonusAttackDamage() * (0.9 + 0.2 * myHero.LPObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl());
		}
		if (strcmp(myHero.championName, "Jinx") == 0) {
			baseDmg = 150 + 100 * myHero.LPObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl();
			baseDmg += myHero.LPObject->GetBonusAttackDamage() * 1.5;
		}

		for (int i = 0; i < entities.heroes.size(); i++) {
			temp = entities.heroes[i];
			if (temp != NULL && temp != myHero.LPObject && temp->GetActiveSpell() != NULL && strcmp(temp->GetActiveSpell()->GetSpellInfo()->GetSpellData()->GetSpellName(), "Recall") == 0) {
				//Calcolo danno AD/AP
				if (strcmp(myHero.championName, "Ashe") == 0 || strcmp(myHero.championName, "Ezreal") == 0) {
					dmg = utils.calcEffectiveDamage(baseDmg, temp->GetMagicResist());
				}
				else {
					if (strcmp(myHero.championName, "Jinx") == 0) {
						baseDmg += (0.2 + 0.5 * myHero.LPObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl()) * (temp->GetMaxHealth() - temp->GetHealth());
					}
					dmg = utils.calcEffectiveDamage(baseDmg, temp->GetArmor());
				}

				//Controllo sulla vita e sul recall
				if (dmg > temp->GetHealth()) {
					utils.dbgStream("BU6");
					calculatedBaseUlt = temp->GetActiveSpell()->GetCastingStartTime() + temp->GetActiveSpell()->GetChannelingTime();
					targetIndex = temp->GetIndex();
					//gui.print(utils.stringf("BASEULT START GameTime: %f, CalculatedTime: %f", gameTime, calculatedBaseUlt));
				}
			}
		}
	}
}