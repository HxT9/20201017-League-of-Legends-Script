#include "LocalPlayer.h"
#include "globalVars.h"
#include "offsets.h"

float LocalPlayer::Humanize() {
	return ((rand() % 41) / 1000) - 0.02;
}

char getKey(Spells slot) {
	switch (slot) {
	case Spells::Q:
		return 'Q';
	case Spells::W:
		return 'W';
	case Spells::E:
		return 'E';
	case Spells::R:
		return 'R';
	case Spells::Item1:
		return '1';
	case Spells::Item2:
		return '2';
	case Spells::Item3:
		return '3';
	case Spells::Item4:
		return '4';
	case Spells::Item5:
		return '5';
	case Spells::Item6:
		return '6';
	}
}

void LocalPlayer::myIssueOrder(void* thisPtr, int Order, Vector3* Loc, EntityBase* Target, bool IsAttackMove, bool IsMinion, DWORD Unknown) {
	PERS_INPUT in;
	in.isClick = true;
	in.championOnly = !Target || Target->Type == EntityType::Hero;
	GH.worldToScreen(Loc, &in.screenPos);

	inputManager.addInput(in);
}
void LocalPlayer::myCastSpell(Spells spell, Vector3* pos) {
	Vector3 sp;
	GH.worldToScreen(pos, &sp);

	myCastSpellScreen(spell, sp);
}
bool LocalPlayer::castSpellMaster(SpellBook* thisPtr, SpellSlot* slot, Spells slotID, Vector3* pos1, Vector3* pos2, DWORD networkID) {
	int zero = 0;
	if (!chargingSpell && gameTime >= nextActionTime && gameTime >= nextSpellTime) {
		if (GH.getSpellState(myHero.PCObject->GetSpellBook(), (int)slotID, &zero) == SpellState::Ready) {
			myCastSpell(slotID, pos1);
			nextActionTime = gameTime + delay + Humanize();
			nextSpellTime = gameTime + spellDelay;

			return true;
		}
		else {
			nextSpellTime = gameTime + spellDelay;
		}
	}

	return false;
}
bool LocalPlayer::CastSpellAtPos(Spells spell, Vector3 position) {
	return castSpellMaster(PCObject->GetSpellBook(), PCObject->GetSpellBook()->GetSpellSlot(spell), spell, &position, &Vector3(0, 0, 0), 0);
}
bool LocalPlayer::CastSpellAtTarget(Spells spell, EntityBase* target) {
	return castSpellMaster(PCObject->GetSpellBook(), PCObject->GetSpellBook()->GetSpellSlot(spell), spell, &(target->Pos), &(PCObject->GetPos()), target->NetworkID);
}
bool LocalPlayer::CastSpellSelf(Spells spell) {
	return castSpellMaster(PCObject->GetSpellBook(), PCObject->GetSpellBook()->GetSpellSlot(spell), spell, &(PCObject->GetPos()), &(PCObject->GetPos()), 0);
}
void LocalPlayer::myCastSpellScreen(Spells spell, Vector3 screenPos) {
	inputManager.addHookedInput(getKey(spell), screenPos, false);
}
void myStartChargingSpell(Spells slotID) {
	inputManager.addKey(getKey(slotID), false);
}
void LocalPlayer::castBaseUlt() {
	myCastSpell(Spells::R, &baseUlt.spawnPoint);
}
void LocalPlayer::StartChargingSpell(Spells spell, float maxDuration) {
	myStartChargingSpell(spell);
	chargingSpell = true;
	chargingStartTime = gameTime;
	chargingEndTime = gameTime + maxDuration;
}
void LocalPlayer::ReleaseChargeableSpell(Spells spell, Vector3 position) {
	if (chargingSpell && gameTime >= nextActionTime) {
		myCastSpell(spell, &position);
		chargingSpell = false;
	}
}

LocalPlayer::LocalPlayer()
{
	Init();
}

LocalPlayer::LocalPlayer(CObject* PObj) {
	EntityBase::Init(PObj, EntityType::Hero);

	Init();

	UpdateAttributes();
}

void LocalPlayer::Init() {
	AAMissileSpeed = 2000;
	AACastTime = 0.5;
	NextAATime = 0; 
	LastAAEndTime = 0; //Settato a quando l'AA finisce il cast
	LastAABeginTime = 0; //Settato a quando l'AA inizia il cast
	nextActionTime = 0;
	nextSpellTime = 0;
	behaviour = Behaviour::Nothing;
	delay = 0.1;
	spellDelay = 0.1;
	useSpell = true;
	chargingSpell = false;
	chargingStartTime = 0;
	chargingEndTime = 0;
	selectedTargetIndex = -1;
	lastRandomCheck = 0;
	ApheliosCrescendumMis = false;
}

void LocalPlayer::tick() {
	UpdateAttributes();

	if (this->ActiveSpell != NULL && gameTime >= NextAATime &&
		std::string(this->ActiveSpell->GetSpellInfo()->GetSpellData()->GetMissileName()).find("Attack") != std::string::npos) {
		AAMissileSpeed = this->ActiveSpell->GetSpellInfo()->GetSpellData()->GetSpellSpeed();

		LastAABeginTime = gameTime;
		NextAATime = LastAABeginTime + AADelay;
		LastAAEndTime = LastAABeginTime + AACastTime;
	}

	if (chargingStartTime >= chargingEndTime) {
		chargingSpell = false; chargingStartTime = 0; chargingEndTime = 0;
	}

	if (selectedTargetIndex != -1 && !entitiesContainer.GetEntityFromIndex(selectedTargetIndex)->Health > 1.f)
		selectedTargetIndex = -1;

	if (gameTime > lastRandomCheck) {
		AACastTime = GH.getAttackCastDelay(PCObject);
		AADelay = GH.getAttackDelay(PCObject);

		if (NextAATime > gameTime && ChampionName == "Aphelios") {
			for (int i = 0; i < PCObject->getBuffManager()->BuffCount(); i++) {
				if (!strcmp(PCObject->getBuffManager()->getBuff(i)->GetBuffName(), "ApheliosCrescendumManager")) {
					bool found = false;
					for (int j = 0; j < entitiesContainer.missilesIndex.size(); j++) {
						EntityBase* mis = entitiesContainer.entities[entitiesContainer.missilesIndex[j]];
						if (mis->MissileName == "ApheliosCrescendumAttackMisIn") {
							ApheliosCrescendumMis = true;
							found = true;
							break;
						}
					}
					if (ApheliosCrescendumMis && !found) {
						ApheliosCrescendumMis = false;
						NextAATime = gameTime;
					}
				}
			}
		}

		lastRandomCheck = gameTime + 0.1;
	}
}
void LocalPlayer::AutoAttack(EntityBase* target){
	if (gameTime >= nextActionTime) {
		if (!chargingSpell && gameTime > NextAATime && target != NULL && !PCObject->isDashing()) {
			myIssueOrder(this->PCObject, 3, &(target->Pos), target, false, target->Type == EntityType::Minion, true);
			nextActionTime = gameTime + delay + Humanize();
		}
		else if (gameTime >= LastAAEndTime) {
			if (gameTime < NextAATime) {
				afterAA = true;
			}
			else {
				afterAA = false;
			}
			MoveTo(GH.getMouseWorldPosition());
		}
	}
}
void LocalPlayer::MoveTo(Vector3 position){
	if (gameTime >= nextActionTime) {
		myIssueOrder(PCObject, 2, &position, NULL, false, false, true);
		nextActionTime = gameTime + delay + Humanize();
	}
}