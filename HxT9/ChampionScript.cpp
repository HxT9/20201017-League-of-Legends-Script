#include "ChampionScript.h"
#include "globalVars.h"

//Caytlin
//Miss Fortune
//Tristana
//Ezreal
//Veigar
//Sona
//Morgana

int zero = 0;
void ChampionScript::tick() {
	if (!myHero.useSpell) return;
	EntityBase *target, *temp;
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
				if (myHero.behaviour == Behaviour::Combo) {
					if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Item1 + i, &zero) == SpellState::Ready) {
						target = targetSelector.getBestChampion(550);
						if (target != NULL)
							myHero.CastSpellAtTarget((Spells)((int)Spells::Item1 + i), target);
					}
				}
			}
		}
	}*/

	if (myHero.ChampionName == "Cassiopeia") {
		switch (myHero.behaviour) {
		case Behaviour::LastHit:
			if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
				damage = 48 + myHero.Level * 4;
				damage += myHero.AbilityPower * 0.1;

				target = targetSelector.getBestMinion(true, damage, 700, 2000, 0.1, false);
				if (target != NULL)
					myHero.CastSpellAtTarget(Spells::E, target);
			}
			break;
		case Behaviour::Push:
			if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
				damage = 48 + myHero.Level * 4;
				damage += myHero.AbilityPower * 0.1;

				target = targetSelector.getBestMinion(true, damage, 700, 2000, 0.1, false);
				if (target != NULL)
					myHero.CastSpellAtTarget(Spells::E, target);
			}
			break;
		case Behaviour::Combo:
			if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(850);
				if (target != NULL) {
					predictedPos = utils.getPredictedPos(target, 0.75, 150);
					if (predictedPos.distTo(myHero.PCObject->GetPos()) <= 850)
						myHero.CastSpellAtPos(Spells::Q, predictedPos);
				}
			}
			if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::W, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(600);
				if (target != NULL) {
					predictedPos = utils.getPredictedPos(target, 0.75 + (target->Pos.distTo(myHero.Pos) / 2500), 320);
					if (predictedPos.distTo(myHero.Pos) <= 600)
						myHero.CastSpellAtPos(Spells::W, predictedPos);
				}
			}
			if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(700);
				if (target != NULL)
					myHero.CastSpellAtTarget(Spells::E, target);
			}
			break;
		}
		return;
	}

	if (myHero.ChampionName == "Ezreal") {
		switch (myHero.behaviour) {
		case Behaviour::Combo:
			if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::W, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(1150);
				if (target != NULL) {
					predictedPos = utils.getPredictedPos(target, 0.25 + (target->Pos.distTo(myHero.Pos) / 1200), 160);
					if (predictedPos.distTo(myHero.Pos) < 1150)
						myHero.CastSpellAtPos(Spells::W, predictedPos);
				}
			}
			if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(1150);
				if (target != NULL) {
					predictedPos = utils.getPredictedPos(target, 0.25 + (target->Pos.distTo(myHero.Pos) / 2000), 120);
					if (predictedPos.distTo(myHero.Pos) < 1150 && utils.minionsColliding(myHero.Pos, predictedPos, 120) == 0)
						myHero.CastSpellAtPos(Spells::Q, predictedPos);
				}
			}
			break;
		}
		return;
	}

	if (myHero.ChampionName == "Kalista") {
		//BuffName: kalistaexpungemarker
		switch (myHero.behaviour) {
		case Behaviour::LastHit:
			if (myHero.PCObject->GetSpellBook()->GetSpellSlot(Spells::E)->GetSpellLvl() == 5) {
				for (int i = 0; i < entitiesContainer.minionsIndex.size(); i++) {
					temp = entitiesContainer.entities[entitiesContainer.minionsIndex[i]];
					if (utils.isValidTarget(temp) && temp->Pos.distTo(myHero.Pos) <= 1100) {
						if (getKalistaSpearDamage(temp) > temp->Health) {
							if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
								myHero.CastSpellSelf(Spells::E);
							}
						}
					}
				}
			}
			break;
		case Behaviour::Push:
			if (myHero.PCObject->GetSpellBook()->GetSpellSlot(Spells::E)->GetSpellLvl() == 5) {
				for (int i = 0; i < entitiesContainer.minionsIndex.size(); i++) {
					temp = entitiesContainer.entities[entitiesContainer.minionsIndex[i]];
					if (utils.isValidTarget(temp) && temp->Pos.distTo(myHero.Pos) <= 1100) {
						if (getKalistaSpearDamage(temp) > temp->Health) {
							if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
								myHero.CastSpellSelf(Spells::E);
							}
						}
					}
				}
			}
			break;
		case Behaviour::Combo:
			for (int i = 0; i < entitiesContainer.heroesIndex.size(); i++) {
				temp = entitiesContainer.entities[entitiesContainer.heroesIndex[i]];
				if (utils.isValidTarget(temp) && temp->Pos.distTo(myHero.Pos) <= 1100) {
					if (getKalistaSpearDamage(temp) > temp->Health) {
						if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
							myHero.CastSpellSelf(Spells::E);
						}
					}
				}
			}
			if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(1150);
				if (target != NULL) {
					predictedPos = utils.getPredictedPos(target, 0.25 + (target->Pos.distTo(myHero.Pos) / 2100), 80);
					if (predictedPos.distTo(myHero.Pos) < 1150 && utils.minionsColliding(myHero.Pos, predictedPos, 120) == 0)
						myHero.CastSpellAtPos(Spells::Q, predictedPos);
				}
			}
			break;
		}
		return;
	}

	if (myHero.ChampionName == "Jinx") {
		switch (myHero.behaviour) {
		case Behaviour::Combo:
			if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::W, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(1400); //1500
				if (target != NULL) {
					predictedPos = utils.getPredictedPos(target, 0.5 + (target->Pos.distTo(myHero.Pos) / 3300), 120);
					if (predictedPos.distTo(myHero.Pos) < 1500 && utils.minionsColliding(myHero.Pos, predictedPos, 120) == 0)
						myHero.CastSpellAtPos(Spells::W, predictedPos);
				}
			}
			break;
		}
		return;
	}

	if (myHero.ChampionName == "KogMaw") {
		switch (myHero.behaviour) {
		case Behaviour::Combo:
			if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(1130);
				if (target != NULL) {
					predictedPos = utils.getPredictedPos(target, 0.25 + (target->Pos.distTo(myHero.Pos) / 1660), 140);
					if (predictedPos.distTo(myHero.Pos) < 1130 && utils.minionsColliding(myHero.Pos, predictedPos, 140) == 0)
						myHero.CastSpellAtPos(Spells::Q, predictedPos);
				}
			}
			if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(1280);
				if (target != NULL) {
					predictedPos = utils.getPredictedPos(target, 0.25 + (target->Pos.distTo(myHero.Pos) / 1350), 250);
					if (predictedPos.distTo(myHero.Pos) < 1280)
						myHero.CastSpellAtPos(Spells::E, predictedPos);
				}
			}
			if (targetSelector.getBestChampion(700) && GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::W, &zero) == SpellState::Ready) {
				myHero.CastSpellSelf(Spells::W);
			}
			if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::R, &zero) == SpellState::Ready) {
				if (myHero.Level < 11) {
					range = 1300;
				}
				else if (myHero.Level < 16) {
					range = 1550;
				}
				else {
					range = 1800;
				}

				target = targetSelector.getBestChampion(range);
				if (target != NULL && target->Health / target->MaxHealth < 0.4 && target->Health < utils.calcEffectiveDamage(1.5 * (60 + 40 * myHero.PCObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl() + myHero.GetTotalAttackDamage() * 0.65 + myHero.AbilityPower * 0.35), target->MagicResist)) {
					predictedPos = utils.getPredictedPos(target, 1.1, 350);
					if(predictedPos.distTo(myHero.Pos) <= range)
						myHero.CastSpellAtPos(Spells::R, predictedPos);
				}
			}
			break;
		}
		return;
	}

	if (myHero.ChampionName == "Tristana") {
		switch (myHero.behaviour) {
		case Behaviour::Combo:
			if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::R, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(myHero.AttackRange + myHero.BoundingRadius);
				if (target != NULL && target->Health < utils.calcEffectiveDamage(200 + 100 * myHero.PCObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl(), target->MagicResist))
					myHero.CastSpellAtTarget(Spells::R, target);
			}
			if (targetSelector.getBestChampion(600) && GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
				myHero.CastSpellSelf(Spells::Q);
			}
			if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(myHero.AttackRange + myHero.BoundingRadius);
				if (target != NULL)
					myHero.CastSpellAtTarget(Spells::E, target);
			}
			break;
		}
		return;
	}

	if (myHero.ChampionName == "Varus") {
		switch (myHero.behaviour) {
		case Behaviour::Combo:
			if (!myHero.chargingSpell && GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(925);
				if (target != NULL) {
					predictedPos = utils.getPredictedPos(target, 0.2419, 300);
					if (predictedPos.distTo(myHero.Pos) <= 925)
						myHero.CastSpellAtPos(Spells::E, predictedPos);
				}
			}
			//While charging spellstate is still ready
			if (!myHero.chargingSpell && GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) { 
				target = targetSelector.getBestChampion(1625);
				if (target != NULL) {
					myHero.StartChargingSpell(Spells::Q, 4);
				}
			}
			if (myHero.chargingSpell) {
				range = 925.f + ((gameTime - myHero.chargingStartTime) / 0.2f) * 70.f;
				range = min(range, 1625);
				//gui.print(utils.stringf("Q range: %f", range));
				target = targetSelector.getBestChampion(range);
				if (target != NULL) {
					predictedPos = utils.getPredictedPos(target, myHero.Pos.distTo(target->Pos) / 1850, 50);
					if (predictedPos.distTo(myHero.Pos) <= range)
						myHero.ReleaseChargeableSpell(Spells::Q, predictedPos);
				}
			}
			break;
		}
	}

	if (myHero.ChampionName == "Xerath") {
		if (myHero.ActiveSpell != NULL && std::string(myHero.ActiveSpell->GetSpellInfo()->GetSpellData()->GetSpellName()).find("XerathLocusOfPower") != std::string::npos ) {
			if (ticker.leftButtonDown && GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::R, &zero) == SpellState::Ready) {
				range = 5000;
				target = targetSelector.getClickedChampion(GH.getMouseWorldPosition(), 300);
				/*if (target == NULL) {
					target = targetSelector.getBestChampion(range);
				}*/
				if (target != NULL) {
					predictedPos = utils.getPredictedPos(target, 0.627, 270);
					if (predictedPos.distTo(myHero.Pos) <= range)
						myHero.CastSpellAtPos(Spells::R, predictedPos);
				}
			}
		}
		else {
			switch (myHero.behaviour) {
			case Behaviour::Combo:
				if (!myHero.chargingSpell && GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
					target = targetSelector.getBestChampion(1000);
					if (target != NULL) {
						predictedPos = utils.getPredictedPos(target, 0.25 + target->Pos.distTo(myHero.Pos) / 1600, 120);
						if (predictedPos.distTo(myHero.Pos) <= 1000 && utils.minionsColliding(myHero.Pos, predictedPos, 60) == 0)
							myHero.CastSpellAtPos(Spells::E, predictedPos);
					}
				}

				//While charging spellstate is still ready
				if (!myHero.chargingSpell && GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
					target = targetSelector.getBestChampion(1400);
					if (target != NULL) {
						myHero.StartChargingSpell(Spells::Q, 3);
					}
				}
				if (myHero.chargingSpell) {
					range = 750 + ((gameTime - myHero.chargingStartTime) / 0.15f) * 65;
					range = min(range, 1400);
					//gui.print(utils.stringf("Q range: %f", range));
					target = targetSelector.getBestChampion(range);
					if (target != NULL) {
						predictedPos = utils.getPredictedPos(target, 0.5, 290);
						if (predictedPos.distTo(myHero.Pos) <= range)
							myHero.ReleaseChargeableSpell(Spells::Q, predictedPos);
					}
				}

				if (!myHero.chargingSpell && GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::W, &zero) == SpellState::Ready) {
					target = targetSelector.getBestChampion(1000);
					if (target != NULL) {
						predictedPos = utils.getPredictedPos(target, 0.25, 420);
						if (predictedPos.distTo(myHero.Pos) <= 1000)
							myHero.CastSpellAtPos(Spells::W, predictedPos);
					}
				}

				break;
			}
		}
	}

	//URF
	/*
	if (myHero.ChampionName == "Ryze") {
		switch (myHero.behaviour) {
		case Behaviour::Combo:
			if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::W, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(550);
				if (target != NULL)
					myHero.CastSpellAtTarget(Spells::W, target);
			}
			if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(550);
				if (target != NULL)
					myHero.CastSpellAtTarget(Spells::E, target);
			}
			if (!myHero.chargingSpell && GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(1000);
				if (target != NULL) {
					predictedPos = utils.getPredictedPos(target, 0.25 + myHero.Pos.distTo(target->Pos) / 1700, 110);
					if (predictedPos.distTo(myHero.Pos) <= 1000)
						myHero.CastSpellAtPos(Spells::Q, predictedPos);
				}
			}
			break;
		}
	}
	
	if (myHero.ChampionName == "Karthus") {
		switch (myHero.behaviour) {
		case Behaviour::Combo:
			if (!myHero.chargingSpell && GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(875);
				if (target != NULL) {
					predictedPos = utils.getPredictedPos(target, 0.75, 160);
					if (predictedPos.distTo(myHero.Pos) <= 875)
						myHero.CastSpellAtPos(Spells::Q, predictedPos);
				}
			}
			break;
		}
	}
	if (myHero.ChampionName == "Kayle") {
		switch (myHero.behaviour) {
		case Behaviour::Combo:
			if (targetSelector.getBestChampion(700) && GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
				myHero.CastSpellSelf(Spells::E);
			}
			if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::W, &zero) == SpellState::Ready) {
				myHero.CastSpellSelf(Spells::W);
			}
			break;
		}
	}
	if (myHero.ChampionName == "Lucian") {
		switch (myHero.behaviour) {
		case Behaviour::Combo:
			if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(500);
				if (target != NULL)
					myHero.CastSpellAtTarget(Spells::Q, target);
			}
			if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready && targetSelector.getBestChampion(600) != NULL) {
				myHero.CastSpellAtPos(Spells::E, GH.getMouseWorldPosition());
			}
			break;
		}
	}
	if (myHero.ChampionName == "Vayne") {
		switch (myHero.behaviour) {
		case Behaviour::Combo:
			if (myHero.afterAA && GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready && targetSelector.getBestChampion(600) != NULL) {
				myHero.CastSpellAtPos(Spells::Q, GH.getMouseWorldPosition());
			}
			break;
		}
	}
	if (myHero.ChampionName == "Draven") {
		switch (myHero.behaviour) {
		case Behaviour::Combo:
			if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::W, &zero) == SpellState::Ready) {
				myHero.CastSpellSelf(Spells::W);
			}
			if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
				myHero.CastSpellSelf(Spells::Q);
			}
			break;
		}
	}
	*/
}


int getKalistaSpears(CObject* target) {
	for (int i = 0; i < target->getBuffManager()->BuffCount(); i++) {
		if (strcmp(target->getBuffManager()->getBuff(i)->GetBuffName(), "kalistaexpungemarker") == 0) {
			if (target->getBuffManager()->getBuff(i)->GetBuffEndTime() > gameTime) {
				return target->getBuffManager()->getBuff(i)->GetBuffCountAlt();
			}
		}
	}
	return 0;
}

float ChampionScript::getKalistaSpearDamage(EntityBase* target) {
	int eLvl = myHero.PCObject->GetSpellBook()->GetSpellSlot(Spells::E)->GetSpellLvl();
	float armor = target->Armor;
	float dmg = 10 + 10 * eLvl + myHero.GetTotalAttackDamage() * 0.6;
	if (getKalistaSpears(target->PCObject) > 0)
		dmg += ((0.5 * pow(eLvl, 2) + 2.5 * eLvl + 7) + (myHero.GetTotalAttackDamage() * ((16.25 + 3.75 * eLvl) / 100))) * (getKalistaSpears(target->PCObject) - 1);
	else
		dmg = 0;

	return utils.calcEffectiveDamage(dmg, armor);
}


/*if (myHero.ChampionName == "Annie") {
	if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
		target = targetSelector.getBestChampion(625);
		if (target != NULL)
			myHero.CastSpellAtTarget(Spells::Q, target);
	}
	if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::W, &zero) == SpellState::Ready) {
		target = targetSelector.getBestChampion(600);
		if (target != NULL)
			myHero.CastSpellAtPos(Spells::W, utils.getPredictedPos(target, 0.25));
	}
	if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
		myHero.CastSpellSelf(Spells::E);
	}
	if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)Spells::R, &zero) == SpellState::Ready) {
		target = targetSelector.getBestChampion(600);
		if (target != NULL)
			myHero.CastSpellAtPos(Spells::R, utils.getPredictedPos(target, 0.25));
	}
}*/