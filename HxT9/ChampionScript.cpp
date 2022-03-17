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
	CObject *target, *temp;
	float range;
	float damage;
	Vector3 predictedPos;

	//Ciclo gli item da usare
	/*for (int i = 0; i < 6; i++) {
		if (myHero.LPObject->GetSpellBook()->GetSpellSlot((Spells)((int)Spells::Item1 + i))->GetSpellInfo()) { //SpellInfo not NULL
			if (strcmp(myHero.LPObject->GetSpellBook()->GetSpellSlot((Spells)((int)Spells::Item1 + i))->GetSpellInfo()->GetSpellData()->GetSpellName(), "game_item_displayname_3153") == 0 //BOTRK
				|| strcmp(myHero.LPObject->GetSpellBook()->GetSpellSlot((Spells)((int)Spells::Item1 + i))->GetSpellInfo()->GetSpellData()->GetSpellName(), "game_item_displayname_3144") == 0 //Cutlass
				|| strcmp(myHero.LPObject->GetSpellBook()->GetSpellSlot((Spells)((int)Spells::Item1 + i))->GetSpellInfo()->GetSpellData()->GetSpellName(), "game_item_displayname_3146") == 0 //Hextech gun
				) {
				if (myHero.behaviour == Behaviour::Combo) {
					if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::Item1 + i, &zero) == SpellState::Ready) {
						target = targetSelector.getBestChampion(550);
						if (target != NULL)
							myHero.CastSpellAtTarget((Spells)((int)Spells::Item1 + i), target);
					}
				}
			}
		}
	}*/

	if (strcmp(myHero.championName, "Cassiopeia") == 0) {
		switch (myHero.behaviour) {
		case Behaviour::LastHit:
			if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
				damage = 48 + myHero.LPObject->GetLevel() * 4;
				damage += myHero.LPObject->GetAP() * 0.1;

				target = targetSelector.getBestMinion(true, damage, 700, 2000, 0.1, false);
				if (target != NULL)
					myHero.CastSpellAtTarget(Spells::E, target);
			}
			break;
		case Behaviour::Push:
			if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
				damage = 48 + myHero.LPObject->GetLevel() * 4;
				damage += myHero.LPObject->GetAP() * 0.1;

				target = targetSelector.getBestMinion(true, damage, 700, 2000, 0.1, false);
				if (target != NULL)
					myHero.CastSpellAtTarget(Spells::E, target);
			}
			break;
		case Behaviour::Combo:
			if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(850);
				if (target != NULL) {
					predictedPos = utils.getPredictedPos(target, 0.75, 150, myHero.LPObject);
					if (predictedPos.distTo(myHero.LPObject->GetPos()) <= 850)
						myHero.CastSpellAtPos(Spells::Q, predictedPos);
				}
			}
			if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::W, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(600);
				if (target != NULL) {
					predictedPos = utils.getPredictedPos(target, 0.75 + (target->GetPos().distTo(myHero.LPObject->GetPos()) / 2500), 320, myHero.LPObject);
					if (predictedPos.distTo(myHero.LPObject->GetPos()) <= 600)
						myHero.CastSpellAtPos(Spells::W, predictedPos);
				}
			}
			if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(700);
				if (target != NULL)
					myHero.CastSpellAtTarget(Spells::E, target);
			}
			break;
		}
		return;
	}

	if (strcmp(myHero.championName, "Ezreal") == 0) {
		switch (myHero.behaviour) {
		case Behaviour::Combo:
			if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::W, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(1150);
				if (target != NULL) {
					predictedPos = utils.getPredictedPos(target, 0.25 + (target->GetPos().distTo(myHero.LPObject->GetPos()) / 1200), 160, myHero.LPObject);
					if (predictedPos.distTo(myHero.LPObject->GetPos()) < 1150)
						myHero.CastSpellAtPos(Spells::W, predictedPos);
				}
			}
			if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(1150);
				if (target != NULL) {
					predictedPos = utils.getPredictedPos(target, 0.25 + (target->GetPos().distTo(myHero.LPObject->GetPos()) / 2000), 120, myHero.LPObject);
					if (predictedPos.distTo(myHero.LPObject->GetPos()) < 1150 && utils.minionsColliding(myHero.LPObject->GetPos(), predictedPos, 120) == 0)
						myHero.CastSpellAtPos(Spells::Q, predictedPos);
				}
			}
			break;
		}
		return;
	}

	if (strcmp(myHero.championName, "Kalista") == 0) {
		//BuffName: kalistaexpungemarker
		switch (myHero.behaviour) {
		case Behaviour::LastHit:
			if (myHero.LPObject->GetSpellBook()->GetSpellSlot(Spells::E)->GetSpellLvl() == 5) {
				for (int i = 0; i < entities.minions.size(); i++) {
					temp = entities.minions[i];
					if (utils.isValidTarget(temp) && temp->GetPos().distTo(myHero.LPObject->GetPos()) <= 1100) {
						if (getKalistaSpearDamage(temp) > temp->GetHealth()) {
							if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
								myHero.CastSpellSelf(Spells::E);
							}
						}
					}
				}
			}
			break;
		case Behaviour::Push:
			if (myHero.LPObject->GetSpellBook()->GetSpellSlot(Spells::E)->GetSpellLvl() == 5) {
				for (int i = 0; i < entities.minions.size(); i++) {
					temp = entities.minions[i];
					if (utils.isValidTarget(temp) && temp->GetPos().distTo(myHero.LPObject->GetPos()) <= 1100) {
						if (getKalistaSpearDamage(temp) > temp->GetHealth()) {
							if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
								myHero.CastSpellSelf(Spells::E);
							}
						}
					}
				}
			}
			break;
		case Behaviour::Combo:
			for (int i = 0; i < entities.heroes.size(); i++) {
				temp = entities.heroes[i];
				if (utils.isValidTarget(temp) && temp->GetPos().distTo(myHero.LPObject->GetPos()) <= 1100) {
					if (getKalistaSpearDamage(temp) > temp->GetHealth()) {
						if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
							myHero.CastSpellSelf(Spells::E);
						}
					}
				}
			}
			if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(1150);
				if (target != NULL) {
					predictedPos = utils.getPredictedPos(target, 0.25 + (target->GetPos().distTo(myHero.LPObject->GetPos()) / 2100), 80, myHero.LPObject);
					if (predictedPos.distTo(myHero.LPObject->GetPos()) < 1150 && utils.minionsColliding(myHero.LPObject->GetPos(), predictedPos, 120) == 0)
						myHero.CastSpellAtPos(Spells::Q, predictedPos);
				}
			}
			break;
		}
		return;
	}

	if (strcmp(myHero.championName, "Jinx") == 0) {
		switch (myHero.behaviour) {
		case Behaviour::Combo:
			if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::W, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(1400); //1500
				if (target != NULL) {
					predictedPos = utils.getPredictedPos(target, 0.5 + (target->GetPos().distTo(myHero.LPObject->GetPos()) / 3300), 120, myHero.LPObject);
					if (predictedPos.distTo(myHero.LPObject->GetPos()) < 1500 && utils.minionsColliding(myHero.LPObject->GetPos(), predictedPos, 120) == 0)
						myHero.CastSpellAtPos(Spells::W, predictedPos);
				}
			}
			break;
		}
		return;
	}

	if (strcmp(myHero.championName, "KogMaw") == 0) {
		switch (myHero.behaviour) {
		case Behaviour::Combo:
			if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(1130);
				if (target != NULL) {
					predictedPos = utils.getPredictedPos(target, 0.25 + (target->GetPos().distTo(myHero.LPObject->GetPos()) / 1660), 140, myHero.LPObject);
					if (predictedPos.distTo(myHero.LPObject->GetPos()) < 1130 && utils.minionsColliding(myHero.LPObject->GetPos(), predictedPos, 140) == 0)
						myHero.CastSpellAtPos(Spells::Q, predictedPos);
				}
			}
			if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(1280);
				if (target != NULL) {
					predictedPos = utils.getPredictedPos(target, 0.25 + (target->GetPos().distTo(myHero.LPObject->GetPos()) / 1350), 250, myHero.LPObject);
					if (predictedPos.distTo(myHero.LPObject->GetPos()) < 1280)
						myHero.CastSpellAtPos(Spells::E, predictedPos);
				}
			}
			if (targetSelector.getBestChampion(700) && GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::W, &zero) == SpellState::Ready) {
				myHero.CastSpellSelf(Spells::W);
			}
			if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::R, &zero) == SpellState::Ready) {
				if (myHero.LPObject->GetLevel() < 11) {
					range = 1300;
				}
				else if (myHero.LPObject->GetLevel() < 16) {
					range = 1550;
				}
				else {
					range = 1800;
				}

				target = targetSelector.getBestChampion(range);
				if (target != NULL && target->GetHealth() / target->GetMaxHealth() < 0.4 && target->GetHealth() < utils.calcEffectiveDamage(1.5 * (60 + 40 * myHero.LPObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl() + myHero.LPObject->GetTotalAttackDamage() * 0.65 + myHero.LPObject->GetAP() * 0.35), target->GetMagicResist())) {
					predictedPos = utils.getPredictedPos(target, 1.1, 350, myHero.LPObject);
					if(predictedPos.distTo(myHero.LPObject->GetPos()) <= range)
						myHero.CastSpellAtPos(Spells::R, predictedPos);
				}
			}
			break;
		}
		return;
	}

	if (strcmp(myHero.championName, "Tristana") == 0) {
		switch (myHero.behaviour) {
		case Behaviour::Combo:
			if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::R, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(myHero.LPObject->GetAttackRange() + myHero.LPObject->GetBoundingRadius());
				if (target != NULL && target->GetHealth() < utils.calcEffectiveDamage(200 + 100 * myHero.LPObject->GetSpellBook()->GetSpellSlot(Spells::R)->GetSpellLvl(), target->GetMagicResist()))
					myHero.CastSpellAtTarget(Spells::R, target);
			}
			if (targetSelector.getBestChampion(600) && GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
				myHero.CastSpellSelf(Spells::Q);
			}
			if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(myHero.LPObject->GetAttackRange() + myHero.LPObject->GetBoundingRadius());
				if (target != NULL)
					myHero.CastSpellAtTarget(Spells::E, target);
			}
			break;
		}
		return;
	}

	if (strcmp(myHero.championName, "Varus") == 0) {
		switch (myHero.behaviour) {
		case Behaviour::Combo:
			if (!myHero.chargingSpell && GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(925);
				if (target != NULL) {
					predictedPos = utils.getPredictedPos(target, 0.2419, 300, myHero.LPObject);
					if (predictedPos.distTo(myHero.LPObject->GetPos()) <= 925)
						myHero.CastSpellAtPos(Spells::E, predictedPos);
				}
			}
			//While charging spellstate is still ready
			if (!myHero.chargingSpell && GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) { 
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
					predictedPos = utils.getPredictedPos(target, myHero.LPObject->GetPos().distTo(target->GetPos()) / 1850, 50, myHero.LPObject);
					if (predictedPos.distTo(myHero.LPObject->GetPos()) <= range)
						myHero.ReleaseChargeableSpell(Spells::Q, predictedPos);
				}
			}
			break;
		}
	}

	if (strcmp(myHero.championName, "Xerath") == 0) {
		if (myHero.LPObject->GetActiveSpell() != NULL && std::string(myHero.LPObject->GetActiveSpell()->GetSpellInfo()->GetSpellData()->GetSpellName()).find("XerathLocusOfPower") != std::string::npos ) {
			if (ticker.leftButtonDown && GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::R, &zero) == SpellState::Ready) {
				range = 5000;
				target = targetSelector.getClickedChampion(GH.getMouseWorldPosition(), 300);
				/*if (target == NULL) {
					target = targetSelector.getBestChampion(range);
				}*/
				if (target != NULL) {
					predictedPos = utils.getPredictedPos(target, 0.627, 270, myHero.LPObject);
					if (predictedPos.distTo(myHero.LPObject->GetPos()) <= range)
						myHero.CastSpellAtPos(Spells::R, predictedPos);
				}
			}
		}
		else {
			switch (myHero.behaviour) {
			case Behaviour::Combo:
				if (!myHero.chargingSpell && GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
					target = targetSelector.getBestChampion(1000);
					if (target != NULL) {
						predictedPos = utils.getPredictedPos(target, 0.25 + target->GetPos().distTo(myHero.LPObject->GetPos()) / 1600, 120, myHero.LPObject);
						if (predictedPos.distTo(myHero.LPObject->GetPos()) <= 1000 && utils.minionsColliding(myHero.LPObject->GetPos(), predictedPos, 60) == 0)
							myHero.CastSpellAtPos(Spells::E, predictedPos);
					}
				}

				//While charging spellstate is still ready
				if (!myHero.chargingSpell && GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
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
						predictedPos = utils.getPredictedPos(target, 0.5, 290, myHero.LPObject);
						if (predictedPos.distTo(myHero.LPObject->GetPos()) <= range)
							myHero.ReleaseChargeableSpell(Spells::Q, predictedPos);
					}
				}

				if (!myHero.chargingSpell && GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::W, &zero) == SpellState::Ready) {
					target = targetSelector.getBestChampion(1000);
					if (target != NULL) {
						predictedPos = utils.getPredictedPos(target, 0.25, 420, myHero.LPObject);
						if (predictedPos.distTo(myHero.LPObject->GetPos()) <= 1000)
							myHero.CastSpellAtPos(Spells::W, predictedPos);
					}
				}

				break;
			}
		}
	}

	//URF
	/*
	if (strcmp(myHero.championName, "Ryze") == 0) {
		switch (myHero.behaviour) {
		case Behaviour::Combo:
			if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::W, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(550);
				if (target != NULL)
					myHero.CastSpellAtTarget(Spells::W, target);
			}
			if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(550);
				if (target != NULL)
					myHero.CastSpellAtTarget(Spells::E, target);
			}
			if (!myHero.chargingSpell && GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(1000);
				if (target != NULL) {
					predictedPos = utils.getPredictedPos(target, 0.25 + myHero.LPObject->GetPos().distTo(target->GetPos()) / 1700, 110, myHero.LPObject);
					if (predictedPos.distTo(myHero.LPObject->GetPos()) <= 1000)
						myHero.CastSpellAtPos(Spells::Q, predictedPos);
				}
			}
			break;
		}
	}
	
	if (strcmp(myHero.championName, "Karthus") == 0) {
		switch (myHero.behaviour) {
		case Behaviour::Combo:
			if (!myHero.chargingSpell && GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(875);
				if (target != NULL) {
					predictedPos = utils.getPredictedPos(target, 0.75, 160, myHero.LPObject);
					if (predictedPos.distTo(myHero.LPObject->GetPos()) <= 875)
						myHero.CastSpellAtPos(Spells::Q, predictedPos);
				}
			}
			break;
		}
	}
	if (strcmp(myHero.championName, "Kayle") == 0) {
		switch (myHero.behaviour) {
		case Behaviour::Combo:
			if (targetSelector.getBestChampion(700) && GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
				myHero.CastSpellSelf(Spells::E);
			}
			if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::W, &zero) == SpellState::Ready) {
				myHero.CastSpellSelf(Spells::W);
			}
			break;
		}
	}
	if (strcmp(myHero.championName, "Lucian") == 0) {
		switch (myHero.behaviour) {
		case Behaviour::Combo:
			if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
				target = targetSelector.getBestChampion(500);
				if (target != NULL)
					myHero.CastSpellAtTarget(Spells::Q, target);
			}
			if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready && targetSelector.getBestChampion(600) != NULL) {
				myHero.CastSpellAtPos(Spells::E, GH.getMouseWorldPosition());
			}
			break;
		}
	}
	if (strcmp(myHero.championName, "Vayne") == 0) {
		switch (myHero.behaviour) {
		case Behaviour::Combo:
			if (myHero.afterAA && GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready && targetSelector.getBestChampion(600) != NULL) {
				myHero.CastSpellAtPos(Spells::Q, GH.getMouseWorldPosition());
			}
			break;
		}
	}
	if (strcmp(myHero.championName, "Draven") == 0) {
		switch (myHero.behaviour) {
		case Behaviour::Combo:
			if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::W, &zero) == SpellState::Ready) {
				myHero.CastSpellSelf(Spells::W);
			}
			if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
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

float ChampionScript::getKalistaSpearDamage(CObject* target) {
	int eLvl = myHero.LPObject->GetSpellBook()->GetSpellSlot(Spells::E)->GetSpellLvl();
	float armor = target->GetArmor();
	float dmg = 10 + 10 * eLvl + myHero.LPObject->GetTotalAttackDamage() * 0.6;
	if (getKalistaSpears(target) > 0)
		dmg += ((0.5 * pow(eLvl, 2) + 2.5 * eLvl + 7) + (myHero.LPObject->GetTotalAttackDamage() * ((16.25 + 3.75 * eLvl) / 100))) * (getKalistaSpears(target) - 1);
	else
		dmg = 0;

	return utils.calcEffectiveDamage(dmg, armor);
}


/*if (strcmp(myHero.championName, "Annie") == 0) {
	if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::Q, &zero) == SpellState::Ready) {
		target = targetSelector.getBestChampion(625);
		if (target != NULL)
			myHero.CastSpellAtTarget(Spells::Q, target);
	}
	if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::W, &zero) == SpellState::Ready) {
		target = targetSelector.getBestChampion(600);
		if (target != NULL)
			myHero.CastSpellAtPos(Spells::W, utils.getPredictedPos(target, 0.25));
	}
	if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::E, &zero) == SpellState::Ready) {
		myHero.CastSpellSelf(Spells::E);
	}
	if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)Spells::R, &zero) == SpellState::Ready) {
		target = targetSelector.getBestChampion(600);
		if (target != NULL)
			myHero.CastSpellAtPos(Spells::R, utils.getPredictedPos(target, 0.25));
	}
}*/