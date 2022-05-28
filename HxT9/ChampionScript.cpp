#include "ChampionScript.h"
#include "globalVars.h"
#include "GameFunctions.h"
#include "TargetSelector.h"
#include "UtilityFunctions.h"

//Caytlin
//Miss Fortune
//Tristana
//Ezreal
//Veigar
//Sona
//Morgana

int zero = 0;

namespace ChampionScript {
	void Tick() {
		if (!myHero.UseSpell) return;
		EntityBase* target, * temp;
		float range;
		float damage;
		Vector3 predictedPos;

		//Ciclo gli item da usare
		/*for (int i = 0; i < 6; i++) {
			if (myHero.PCObject->GetSpellBook()->GetSpellSlot((Spells)((int)Spells::Item1 + i))->GetSpellInfo()) { //SpellInfo not NULL
				if (strcmp(myHero.PCObject->GetSpellBook()->GetSpellSlot((Spells)((int)Spells::Item1 + i))->GetSpellInfo()->GetSpellData()->GetSpellName(), "game_item_displayname_3153") == 0 //BOTRK
					|| strcmp(myHero.PCObject->GetSpellBook()->GetSpellSlot((Spells)((int)Spells::Item1 + i))->GetSpellInfo()->GetSpellData()->GetSpellName(), "game_item_displayname_3144") == 0 //Cutlass
					|| strcmp(myHero.PCObject->GetSpellBook()->GetSpellSlot((Spells)((int)Spells::Item1 + i))->GetSpellInfo()->GetSpellData()->GetSpellName(), "game_item_displayname_3146") == 0 //Hextech gun
					) {
					if (myHero.Behaviour == Behaviour::Combo) {
						if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Item1 + i, &zero) == SpellState::Ready) {
							target = TargetSelector::GetBestChampion(550);
							if (target != NULL)
								myHero.CastSpellAtTarget((Spells)((int)Spells::Item1 + i), target);
						}
					}
				}
			}
		}*/

		if (myHero.ObjectName == "Cassiopeia") {
			switch (myHero.Behaviour) {
			case Behaviour::LastHit:
				if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
					damage = 48 + myHero.Level * 4;
					damage += myHero.AbilityPower * 0.1;

					target = TargetSelector::GetBestMinion(true, damage, 700, 2000, 0.1, false, true);
					if (target != NULL)
						myHero.CastSpellAtTarget(Spells::E, target);
				}
				break;
			case Behaviour::Push:
				if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
					damage = 48 + myHero.Level * 4;
					damage += myHero.AbilityPower * 0.1;

					target = TargetSelector::GetBestMinion(true, damage, 700, 2000, 0.1, false, true);
					if (target != NULL)
						myHero.CastSpellAtTarget(Spells::E, target);
				}
				break;
			case Behaviour::Combo:
				if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
					target = TargetSelector::GetBestChampion(700);
					if (target != NULL)
						myHero.CastSpellAtTarget(Spells::E, target);
				}
				if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
					target = TargetSelector::GetBestChampion(850);
					if (target != NULL) {
						predictedPos = UtilityFunctions::GetPredictedPos(target, 0.75, 150, SkillShotType::Circular);
						if (predictedPos.distTo(myHero.PCObject->GetPos()) <= 850)
							myHero.CastSpellAtPos(Spells::Q, predictedPos);
					}
				}
				if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::W, &zero) == SpellState::Ready) {
					target = TargetSelector::GetBestChampion(600);
					if (target != NULL) {
						predictedPos = UtilityFunctions::GetPredictedPos(target, 0.75 + (target->Pos.distTo(myHero.Pos) / 2500), 320);
						if (predictedPos.distTo(myHero.Pos) <= 600)
							myHero.CastSpellAtPos(Spells::W, predictedPos);
					}
				}
				break;
			}
			return;
		}

		if (myHero.ObjectName == "Ezreal") {
			switch (myHero.Behaviour) {
			case Behaviour::Combo:
				if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::W, &zero) == SpellState::Ready) {
					target = TargetSelector::GetBestChampion(1150);
					if (target != NULL) {
						predictedPos = UtilityFunctions::GetPredictedPos(target, 0.25 + (target->Pos.distTo(myHero.Pos) / 1200), 160);
						if (predictedPos.distTo(myHero.Pos) < 1150)
							myHero.CastSpellAtPos(Spells::W, predictedPos);
					}
				}
				if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
					target = TargetSelector::GetBestChampion(1150);
					if (target != NULL) {
						predictedPos = UtilityFunctions::GetPredictedPos(target, 0.25 + (target->Pos.distTo(myHero.Pos) / 2000), 120);
						if (predictedPos.distTo(myHero.Pos) < 1150 && UtilityFunctions::MinionsColliding(myHero.Pos, predictedPos, 120) == 0)
							myHero.CastSpellAtPos(Spells::Q, predictedPos);
					}
				}
				break;
			}
			return;
		}

		if (myHero.ObjectName == "Kalista") {
			//BuffName: kalistaexpungemarker
			switch (myHero.Behaviour) {
			case Behaviour::LastHit:
				if (myHero.PCObject->GetSpellBook()->GetSpellSlot(Spells::E)->GetSpellLvl() == 5) {
					for (int i = 0; i < EntitiesContainer::MinionsIndex.size(); i++) {
						temp = EntitiesContainer::Entities[EntitiesContainer::MinionsIndex[i]];
						if (UtilityFunctions::IsValidTarget(temp) && temp->Pos.distTo(myHero.Pos) <= 1100) {
							if (GetKalistaSpearDamage(temp) > temp->Health) {
								if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
									myHero.CastSpellSelf(Spells::E);
								}
							}
						}
					}
				}
				break;
			case Behaviour::Push:
				if (myHero.PCObject->GetSpellBook()->GetSpellSlot(Spells::E)->GetSpellLvl() == 5) {
					for (int i = 0; i < EntitiesContainer::MinionsIndex.size(); i++) {
						temp = EntitiesContainer::Entities[EntitiesContainer::MinionsIndex[i]];
						if (UtilityFunctions::IsValidTarget(temp) && temp->Pos.distTo(myHero.Pos) <= 1100) {
							if (GetKalistaSpearDamage(temp) > temp->Health) {
								if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
									myHero.CastSpellSelf(Spells::E);
								}
							}
						}
					}
				}
				break;
			case Behaviour::Combo:
				for (int i = 0; i < EntitiesContainer::HeroesIndex.size(); i++) {
					temp = EntitiesContainer::Entities[EntitiesContainer::HeroesIndex[i]];
					if (UtilityFunctions::IsValidTarget(temp) && temp->Pos.distTo(myHero.Pos) <= 1100) {
						if (GetKalistaSpearDamage(temp) > temp->Health) {
							if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
								myHero.CastSpellSelf(Spells::E);
							}
						}
					}
				}
				if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
					target = TargetSelector::GetBestChampion(1150);
					if (target != NULL) {
						predictedPos = UtilityFunctions::GetPredictedPos(target, 0.25 + (target->Pos.distTo(myHero.Pos) / 2100), 80);
						if (predictedPos.distTo(myHero.Pos) < 1150 && UtilityFunctions::MinionsColliding(myHero.Pos, predictedPos, 120) == 0)
							myHero.CastSpellAtPos(Spells::Q, predictedPos);
					}
				}
				break;
			}
			return;
		}

		if (myHero.ObjectName == "Jinx") {
			switch (myHero.Behaviour) {
			case Behaviour::Combo:
				if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::W, &zero) == SpellState::Ready) {
					target = TargetSelector::GetBestChampion(1400); //1500
					if (target != NULL) {
						predictedPos = UtilityFunctions::GetPredictedPos(target, 0.5 + (target->Pos.distTo(myHero.Pos) / 3300), 120);
						if (predictedPos.distTo(myHero.Pos) < 1500 && UtilityFunctions::MinionsColliding(myHero.Pos, predictedPos, 120) == 0)
							myHero.CastSpellAtPos(Spells::W, predictedPos);
					}
				}
				break;
			}
			return;
		}

		if (myHero.ObjectName == "KogMaw") {
			switch (myHero.Behaviour) {
			case Behaviour::Combo:
				if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
					target = TargetSelector::GetBestChampion(1130);
					if (target != NULL) {
						predictedPos = UtilityFunctions::GetPredictedPos(target, 0.25 + (target->Pos.distTo(myHero.Pos) / 1660), 140);
						if (predictedPos.distTo(myHero.Pos) < 1130 && UtilityFunctions::MinionsColliding(myHero.Pos, predictedPos, 140) == 0)
							myHero.CastSpellAtPos(Spells::Q, predictedPos);
					}
				}
				if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
					target = TargetSelector::GetBestChampion(1280);
					if (target != NULL) {
						predictedPos = UtilityFunctions::GetPredictedPos(target, 0.25 + (target->Pos.distTo(myHero.Pos) / 1350), 250);
						if (predictedPos.distTo(myHero.Pos) < 1280)
							myHero.CastSpellAtPos(Spells::E, predictedPos);
					}
				}
				if (TargetSelector::GetBestChampion(700) && GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::W, &zero) == SpellState::Ready) {
					myHero.CastSpellSelf(Spells::W);
				}
				if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::R, &zero) == SpellState::Ready) {
					if (myHero.Level < 11) {
						range = 1300;
					}
					else if (myHero.Level < 16) {
						range = 1550;
					}
					else {
						range = 1800;
					}

					target = TargetSelector::GetBestChampion(range);
					if (target != NULL && target->Health / target->MaxHealth < 0.4 && target->Health < UtilityFunctions::CalcEffectiveDamage(1.5 * (60 + 40 * myHero.PCObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl() + myHero.GetTotalAttackDamage() * 0.65 + myHero.AbilityPower * 0.35), target->MagicResist)) {
						predictedPos = UtilityFunctions::GetPredictedPos(target, 1.1, 350);
						if (predictedPos.distTo(myHero.Pos) <= range)
							myHero.CastSpellAtPos(Spells::R, predictedPos);
					}
				}
				break;
			}
			return;
		}

		if (myHero.ObjectName == "Tristana") {
			switch (myHero.Behaviour) {
			case Behaviour::Combo:
				if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::R, &zero) == SpellState::Ready) {
					target = TargetSelector::GetBestChampion(myHero.AttackRange + myHero.BoundingRadius);
					if (target != NULL && target->Health < UtilityFunctions::CalcEffectiveDamage(200 + 100 * myHero.PCObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl(), target->MagicResist))
						myHero.CastSpellAtTarget(Spells::R, target);
				}
				if (TargetSelector::GetBestChampion(600) && GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
					myHero.CastSpellSelf(Spells::Q);
				}
				if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
					target = TargetSelector::GetBestChampion(myHero.AttackRange + myHero.BoundingRadius);
					if (target != NULL)
						myHero.CastSpellAtTarget(Spells::E, target);
				}
				break;
			}
			return;
		}

		if (myHero.ObjectName == "Varus") {
			switch (myHero.Behaviour) {
			case Behaviour::Combo:
				if (myHero.IsChargingSpell()) {
					if (std::string(myHero.ActiveSpell->GetSpellInfo()->GetSpellData()->GetName()).find("VarusQ") != std::string::npos) {
						range = 925.f + ((gameTime - myHero.ActiveSpell->GetChannelStartTime()) / 0.2f) * 70.f;
						range = min(range, 1625);
						target = TargetSelector::GetBestChampion(range);
						if (target != NULL) {
							predictedPos = UtilityFunctions::GetPredictedPos(target, myHero.Pos.distTo(target->Pos) / 1850, 50);
							if (predictedPos.distTo(myHero.Pos) <= range)
								myHero.ReleaseChargeableSpell(Spells::Q, predictedPos);
						}
					}
				}
				else {
					if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
						target = TargetSelector::GetBestChampion(925);
						if (target != NULL) {
							predictedPos = UtilityFunctions::GetPredictedPos(target, 0.2419, 300);
							if (predictedPos.distTo(myHero.Pos) <= 925)
								myHero.CastSpellAtPos(Spells::E, predictedPos);
						}
					}
					//While charging spellstate is still ready
					if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
						target = TargetSelector::GetBestChampion(1625);
						if (target != NULL) {
							myHero.StartChargingSpell(Spells::Q, 4);
						}
					}
				}
				break;
			}
		}

		if (myHero.ObjectName == "Xerath") {
			switch (myHero.Behaviour) {
			case Behaviour::Combo:
				if (myHero.IsChargingSpell()) {
					if (std::string(myHero.ActiveSpell->GetSpellInfo()->GetSpellData()->GetName()).find("LocusOfPower") != std::string::npos) {
						if (GetKeyState(VK_LBUTTON) & 0x8000 && GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::R, &zero) == SpellState::Ready) {
							range = 5000;
							target = TargetSelector::GetClickedChampion(GameFunctions::GetMouseWorldPosition(), 300);
							if (target != NULL) {
								predictedPos = UtilityFunctions::GetPredictedPos(target, 0.627, 400, SkillShotType::Circular);
								if (predictedPos.distTo(myHero.Pos) <= range) {
									myHero.CastSpellAtPos(Spells::R, predictedPos);
								}
							}
						}
					}
					if (std::string(myHero.ActiveSpell->GetSpellInfo()->GetSpellData()->GetName()).find("ArcanopulseChargeUp") != std::string::npos) {
						range = 750 + ((gameTime - myHero.ActiveSpell->GetChannelStartTime()) / 0.15) * 65;
						range = min(range, 1400);
						target = TargetSelector::GetBestChampion(range);
						if (target) {
							predictedPos = UtilityFunctions::GetPredictedPos(target, 0.5, 145);
							if (predictedPos.distTo(myHero.Pos) <= range)
								myHero.ReleaseChargeableSpell(Spells::Q, predictedPos);
						}
					}
				}
				else {
					if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
						target = TargetSelector::GetBestChampion(1000);
						if (target != NULL) {
							predictedPos = UtilityFunctions::GetPredictedPos(target, 0.25 + target->Pos.distTo(myHero.Pos) / 1600, 120);
							if (predictedPos.distTo(myHero.Pos) <= 1000 && UtilityFunctions::MinionsColliding(myHero.Pos, predictedPos, 60) == 0)
								myHero.CastSpellAtPos(Spells::E, predictedPos);
						}
					}

					//While charging spellstate is still ready
					if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
						target = TargetSelector::GetBestChampion(1400);
						if (target != NULL) {
							myHero.StartChargingSpell(Spells::Q, 3);
							return;
						}
					}

					if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::W, &zero) == SpellState::Ready) {
						target = TargetSelector::GetBestChampion(1000);
						if (target != NULL) {
							predictedPos = UtilityFunctions::GetPredictedPos(target, 0.25, 500, SkillShotType::Circular);
							if (predictedPos.distTo(myHero.Pos) <= 1000)
								myHero.CastSpellAtPos(Spells::W, predictedPos);
						}
					}
				}
				break;
			}
		}

		//URF
		/*
		if (myHero.ObjectName == "Ryze") {
			switch (myHero.Behaviour) {
			case Behaviour::Combo:
				if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::W, &zero) == SpellState::Ready) {
					target = TargetSelector::GetBestChampion(550);
					if (target != NULL)
						myHero.CastSpellAtTarget(Spells::W, target);
				}
				if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
					target = TargetSelector::GetBestChampion(550);
					if (target != NULL)
						myHero.CastSpellAtTarget(Spells::E, target);
				}
				if (!myHero.chargingSpell && GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
					target = TargetSelector::GetBestChampion(1000);
					if (target != NULL) {
						predictedPos = UtilityFunctions::GetPredictedPos(target, 0.25 + myHero.Pos.distTo(target->Pos) / 1700, 110);
						if (predictedPos.distTo(myHero.Pos) <= 1000)
							myHero.CastSpellAtPos(Spells::Q, predictedPos);
					}
				}
				break;
			}
		}

		if (myHero.ObjectName == "Karthus") {
			switch (myHero.Behaviour) {
			case Behaviour::Combo:
				if (!myHero.chargingSpell && GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
					target = TargetSelector::GetBestChampion(875);
					if (target != NULL) {
						predictedPos = UtilityFunctions::GetPredictedPos(target, 0.75, 160);
						if (predictedPos.distTo(myHero.Pos) <= 875)
							myHero.CastSpellAtPos(Spells::Q, predictedPos);
					}
				}
				break;
			}
		}
		if (myHero.ObjectName == "Kayle") {
			switch (myHero.Behaviour) {
			case Behaviour::Combo:
				if (TargetSelector::GetBestChampion(700) && GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
					myHero.CastSpellSelf(Spells::E);
				}
				if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::W, &zero) == SpellState::Ready) {
					myHero.CastSpellSelf(Spells::W);
				}
				break;
			}
		}
		if (myHero.ObjectName == "Lucian") {
			switch (myHero.Behaviour) {
			case Behaviour::Combo:
				if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
					target = TargetSelector::GetBestChampion(500);
					if (target != NULL)
						myHero.CastSpellAtTarget(Spells::Q, target);
				}
				if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready && TargetSelector::GetBestChampion(600) != NULL) {
					myHero.CastSpellAtPos(Spells::E, GameFunctions::GetMouseWorldPosition());
				}
				break;
			}
		}
		if (myHero.ObjectName == "Vayne") {
			switch (myHero.Behaviour) {
			case Behaviour::Combo:
				if (myHero.afterAA && GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready && TargetSelector::GetBestChampion(600) != NULL) {
					myHero.CastSpellAtPos(Spells::Q, GameFunctions::GetMouseWorldPosition());
				}
				break;
			}
		}
		if (myHero.ObjectName == "Draven") {
			switch (myHero.Behaviour) {
			case Behaviour::Combo:
				if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::W, &zero) == SpellState::Ready) {
					myHero.CastSpellSelf(Spells::W);
				}
				if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
					myHero.CastSpellSelf(Spells::Q);
				}
				break;
			}
		}
		*/
	}

	float GetKalistaSpearDamage(EntityBase* target) {
		int eLvl = myHero.SpellBk->GetSpellSlot(Spells::E)->GetSpellLvl();
		if (target->GetBuff("kalistaexpungemarker")) {
			int spears = (int)target->GetBuff("kalistaexpungemarker")->GetBuffCountAlt();
			float armor = target->Armor;
			float dmg = 10 + 10 * eLvl + myHero.GetTotalAttackDamage() * 0.6;
			if (spears)
				dmg += ((0.5 * pow(eLvl, 2) + 2.5 * eLvl + 7) + (myHero.GetTotalAttackDamage() * ((16.25 + 3.75 * eLvl) / 100))) * (spears - 1);
			else
				dmg = 0;

			return UtilityFunctions::CalcEffectiveDamage(dmg, armor);
		}
		return 0;
	}


	/*if (myHero.ObjectName == "Annie") {
		if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
			target = TargetSelector::GetBestChampion(625);
			if (target != NULL)
				myHero.CastSpellAtTarget(Spells::Q, target);
		}
		if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::W, &zero) == SpellState::Ready) {
			target = TargetSelector::GetBestChampion(600);
			if (target != NULL)
				myHero.CastSpellAtPos(Spells::W, UtilityFunctions::GetPredictedPos(target, 0.25));
		}
		if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
			myHero.CastSpellSelf(Spells::E);
		}
		if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::R, &zero) == SpellState::Ready) {
			target = TargetSelector::GetBestChampion(600);
			if (target != NULL)
				myHero.CastSpellAtPos(Spells::R, UtilityFunctions::GetPredictedPos(target, 0.25));
		}
	}*/
}