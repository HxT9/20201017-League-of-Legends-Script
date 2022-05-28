#include "BaseUlt.h"
#include "globalVars.h"
#include "EntitiesContainer.h"
#include "UtilityFunctions.h"

namespace BaseUlt{
	bool Enabled = true;
	float LastBaseUlt;
	float Speed;
	float Width;
	float CastTime;
	Vector3 SpawnPoint;
	float CalculatedBaseUlt = -100;
	short TargetIndex = 0;

	void Init() {
		if (myHero.ObjectName == "Ashe") {
			Speed = 1600;
			CastTime = 0.25;
			Width = 260;
		}
		if (myHero.ObjectName == "Ezreal") {
			Speed = 2000;
			CastTime = 1;
			Width = 320;
		}
		if (myHero.ObjectName == "Draven") {
			Speed = 2000;
			Width = 320;
			CastTime = 0.5;
		}
		if (myHero.ObjectName == "Jinx") {
			Speed = 1700; //to fix
			CastTime = 0.6;
			Width = 280;
		}

		Vector3 blueSpawnPosition = Vector3(400, 60, 420);
		Vector3 redSpawnPosition = Vector3(14290, 60, 14370);

		if (myHero.Team == 100)
			SpawnPoint = redSpawnPosition;
		else
			SpawnPoint = blueSpawnPosition;

		LastBaseUlt = 0;
	}

	void Tick()
	{
		float dmg, baseDmg;
		float time;
		EntityBase* temp;

		if (!Enabled)
			return;

		//Controllo che non abbia stoppato il recall
		for (int i = 0; i < EntitiesContainer::HeroesIndex.size(); i++) {
			temp = EntitiesContainer::Entities[EntitiesContainer::HeroesIndex[i]];
			if (temp != NULL &&
				temp->Index == TargetIndex &&
				temp->Visible) {
				if (temp->ActiveSpell) {
					if (strcmp(temp->ActiveSpell->GetSpellInfo()->GetSpellData()->GetName(), "Recall") != 0) {
						TargetIndex = 0;
					}
				}
				else {
					TargetIndex = 0;
				}
			}
		}

		time = CastTime + SpawnPoint.distTo(myHero.Pos) / Speed;

		if (myHero.ObjectName == "Jinx") {
			time = CastTime;
			if (SpawnPoint.distTo(myHero.Pos) > 1350) {
				time += 1350 / 1700;
				time += (SpawnPoint.distTo(myHero.Pos) - 1350) / 2200;
			}
			else {
				time += SpawnPoint.distTo(myHero.Pos) / 1700;
			}
		}

		if (TargetIndex) {
			if (CalculatedBaseUlt - gameTime - time < 0 && gameTime > LastBaseUlt + 10) {
				//IMGUIManager::Print(UtilityFunctions::\U\1tringf("BASEULT CAST"));
				if (myHero.ObjectName == "Ezreal") {
					myHero.NextActionTime = 0;
					myHero.NextSpellTime = 0;
					myHero.CastBaseUlt();
				}
				else {
					if (UtilityFunctions::HeroesColliding(myHero.Pos, SpawnPoint, Width) == 0) {
						myHero.NextActionTime = 0;
						myHero.NextSpellTime = 0;
						myHero.CastBaseUlt();
					}
				}
				LastBaseUlt = gameTime;
				TargetIndex = 0;
			}
		}

		if (TargetIndex == 0) {
			if (myHero.ObjectName == "Ashe") {
				baseDmg = 200 * myHero.PCObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl();
				baseDmg += myHero.AbilityPower;
			}
			if (myHero.ObjectName == "Ezreal") {
				baseDmg = 200 + 150 * myHero.PCObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl();
				baseDmg += myHero.AbilityPower * 0.9;
				baseDmg += myHero.GetTotalAttackDamage();
			}
			if (myHero.ObjectName == "Draven") {
				baseDmg = 75 + 100 * myHero.PCObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl();
				baseDmg += myHero.BonusAttackDamage * (0.9 + 0.2 * myHero.PCObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl());
			}
			if (myHero.ObjectName == "Jinx") {
				baseDmg = 150 + 100 * myHero.PCObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl();
				baseDmg += myHero.BonusAttackDamage * 1.5;
			}

			for (int i = 0; i < EntitiesContainer::HeroesIndex.size(); i++) {
				temp = EntitiesContainer::Entities[EntitiesContainer::HeroesIndex[i]];
				if (temp != NULL && temp->Index != myHero.Index && temp->ActiveSpell && strcmp(temp->ActiveSpell->GetSpellInfo()->GetSpellData()->GetName(), "Recall") == 0) {
					//Calcolo danno AD/AP
					if (myHero.ObjectName == "Ashe" || myHero.ObjectName == "Ezreal") {
						dmg = UtilityFunctions::CalcEffectiveDamage(baseDmg, temp->MagicResist);
					}
					else {
						if (myHero.ObjectName == "Jinx") {
							baseDmg += (0.2 + 0.5 * myHero.PCObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl()) * (temp->MaxHealth - temp->Health);
						}
						dmg = UtilityFunctions::CalcEffectiveDamage(baseDmg, temp->Armor);
					}

					//Controllo sulla vita e sul recall
					if (dmg > temp->Health) {
						CalculatedBaseUlt = temp->ActiveSpell->GetChannelEndTime();
						TargetIndex = temp->Index;
					}
				}
			}
		}
	}
}