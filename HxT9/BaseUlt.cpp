#include "BaseUlt.h"
#include "globalVars.h"

void BaseUlt::init() {
	if (myHero.ChampionName == "Ashe") {
		speed = 1600;
		castTime = 0.25;
		width = 260;
	}
	if (myHero.ChampionName == "Ezreal") {
		speed = 2000;
		castTime = 1;
		width = 320;
	}
	if (myHero.ChampionName == "Draven") {
		speed = 2000;
		width = 320;
		castTime = 0.5;
	}
	if (myHero.ChampionName == "Jinx") {
		speed = 1700; //to fix
		castTime = 0.6;
		width = 280;
	}

	Vector3 blueSpawnPosition = Vector3(400, 60, 420);
	Vector3 redSpawnPosition = Vector3(14290, 60, 14370);

	if (myHero.Team == 100)
		spawnPoint = redSpawnPosition;
	else
		spawnPoint = blueSpawnPosition;
}

void BaseUlt::tick()
{
	float dmg, baseDmg;
	float time;
	EntityBase* temp;

	//Controllo che non abbia stoppato il recall
	for (int i = 0; i < entitiesContainer.heroesIndex.size(); i++) {
		temp = entitiesContainer.entities[entitiesContainer.heroesIndex[i]];
		if (temp != NULL &&
			temp->Index == targetIndex &&
			temp->Visible){
			if (temp->ActiveSpell) {
				if (strcmp(temp->ActiveSpell->GetSpellInfo()->GetSpellData()->GetSpellName(), "Recall") != 0) {
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

	time = castTime + spawnPoint.distTo(myHero.Pos) / speed;

	if (myHero.ChampionName == "Jinx") {
		time = castTime;
		if (spawnPoint.distTo(myHero.Pos) > 1350) {
			time += 1350 / 1700;
			time += (spawnPoint.distTo(myHero.Pos) - 1350) / 2200;
		}
		else {
			time += spawnPoint.distTo(myHero.Pos) / 1700;
		}
	}

	if (targetIndex != 0) {
		if (abs(calculatedBaseUlt - gameTime - time) < 0.2) {
			//gui.print(utils.stringf("BASEULT CAST"));
			if (myHero.ChampionName == "Ezreal") {
				myHero.nextActionTime = 0;
				myHero.nextSpellTime = 0;
				myHero.castBaseUlt();
			}
			else {
				if (utils.heroesColliding(myHero.Pos, spawnPoint, width) == 0) {
					myHero.nextActionTime = 0;
					myHero.nextSpellTime = 0;
					myHero.castBaseUlt();
				}
			}
		}
	}

	if (gameTime > calculatedBaseUlt - time + 1) targetIndex = 0;

	if (targetIndex == 0){
		if (myHero.ChampionName == "Ashe") {
			baseDmg = 200 * myHero.PCObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl();
			baseDmg += myHero.AbilityPower;
		}
		if (myHero.ChampionName == "Ezreal") {
			baseDmg = 200 + 150 * myHero.PCObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl();
			baseDmg += myHero.AbilityPower * 0.9;
			baseDmg += myHero.GetTotalAttackDamage();
		}
		if (myHero.ChampionName == "Draven") {
			baseDmg = 75 + 100 * myHero.PCObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl();
			baseDmg += myHero.BonusAttackDamage * (0.9 + 0.2 * myHero.PCObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl());
		}
		if (myHero.ChampionName == "Jinx") {
			baseDmg = 150 + 100 * myHero.PCObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl();
			baseDmg += myHero.BonusAttackDamage * 1.5;
		}

		for (int i = 0; i < entitiesContainer.heroesIndex.size(); i++) {
			temp = entitiesContainer.entities[entitiesContainer.heroesIndex[i]];
			if (temp != NULL && temp->Index != myHero.Index && temp->ActiveSpell && strcmp(temp->ActiveSpell->GetSpellInfo()->GetSpellData()->GetSpellName(), "Recall") == 0) {
				//Calcolo danno AD/AP
				if (myHero.ChampionName == "Ashe" || myHero.ChampionName == "Ezreal") {
					dmg = utils.calcEffectiveDamage(baseDmg, temp->MagicResist);
				}
				else {
					if (myHero.ChampionName == "Jinx") {
						baseDmg += (0.2 + 0.5 * myHero.PCObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl()) * (temp->MaxHealth - temp->Health);
					}
					dmg = utils.calcEffectiveDamage(baseDmg, temp->Armor);
				}

				//Controllo sulla vita e sul recall
				if (dmg > temp->Health) {
					calculatedBaseUlt = temp->ActiveSpell->GetCastingStartTime() + temp->ActiveSpell->GetChannelingTime();
					targetIndex = temp->Index;
					//gui.print(utils.stringf("BASEULT START GameTime: %f, CalculatedTime: %f", gameTime, calculatedBaseUlt));
				}
			}
		}
	}
}