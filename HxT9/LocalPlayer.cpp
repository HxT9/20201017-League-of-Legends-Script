#include "LocalPlayer.h"
#include "globalVars.h"
#include "offsets.h"
#include "utilities.h"

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

void LocalPlayer::myIssueOrder(void* thisPtr, int Order, Vector3* Loc, CObject* Target, bool IsAttackMove, bool IsMinion, DWORD Unknown) {
	PERS_INPUT in;
	in.isClick = true;
	in.championOnly = GH.isHero(Target) || Target == NULL;
	GH.worldToScreen(Loc, &in.screenPos);

	inputManager.addInput(in);
}
void LocalPlayer::myCastSpell(Spells spell, Vector3* pos) {
	Vector3 sp;
	GH.worldToScreen(pos, &sp);

	myCastSpellScreen(spell, sp);
}
void LocalPlayer::myCastSpellScreen(Spells spell, Vector3 screenPos) {
	inputManager.addHookedInput(getKey(spell), screenPos, false);
}
void myStartChargingSpell(Spells slotID) {
	inputManager.addKey(getKey(slotID), false);
}
void myCastChargeableSpell(SpellBook* thisPtr, SpellSlot* slot, Spells slotID, Vector3* pos, bool release) {
	//StackSpoofForThiscall<int*>((baseAddress + oUpdateChargeableSpell), 4, thisPtr, (baseAddress + oSpoof), slot, slotID, pos, release);
	Vector3 screenPos;
	GH.worldToScreen(pos, &screenPos);
	inputManager.addHookedInput(getKey(slotID), screenPos, true);
}
void LocalPlayer::castBaseUlt() {
	myCastSpell(Spells::R, &baseUlt.spawnPoint);
}

LocalPlayer::LocalPlayer() {
	init();
}
LocalPlayer::LocalPlayer(CObject* obj) {
	init();
	LPObject = obj;
}
void LocalPlayer::init() {
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
	AACheckDelay = 0.01;
	lastAACheck = 0;
	LPObject = 0;
	useSpell = true;
	curPathIndex = 0;
	curPath.nPathPoints = 1;
	curPath.pathPoints;
	chargingSpell = false;
	chargingStartTime = 0;
	chargingEndTime = 0;
	selectedTarget = NULL;
	randomCheckDelay = 1.f;
	lastRandomCheck = 0;
	boundingRadius = 0.f;
}
void LocalPlayer::tick() {
	if (LPObject->GetActiveSpell() != NULL && gameTime >= NextAATime &&
		std::string(LPObject->GetActiveSpell()->GetSpellInfo()->GetSpellData()->GetMissileName()).find("Attack") != std::string::npos) {

		AAMissileSpeed = LPObject->GetActiveSpell()->GetSpellInfo()->GetSpellData()->GetSpellSpeed();
		if (gameTime > LastAAEndTime) {
			LastAABeginTime = gameTime;
			LastAAEndTime = LastAABeginTime + GH.getAttackCastDelay(LPObject);
			NextAATime = LastAABeginTime + GH.getAttackDelay(LPObject);
			afterAA = false;
		}
	}
	else {
		if (gameTime > lastAACheck) {
			NextAATime = LastAABeginTime + GH.getAttackDelay(LPObject);
			lastAACheck = gameTime + AACheckDelay;
		}
	}

	if (chargingStartTime >= chargingEndTime) {
		chargingSpell = false; chargingStartTime = 0; chargingEndTime = 0;
	}

	if (selectedTarget != NULL && !selectedTarget->GetHealth() > 1.f)
		selectedTarget = NULL;

	if (gameTime > lastRandomCheck) {
		boundingRadius = this->LPObject->GetBoundingRadius();

		lastRandomCheck = gameTime + randomCheckDelay;
	}
}
void LocalPlayer::AutoAttack(CObject* target){
	if (gameTime >= nextActionTime) {
		if (!chargingSpell && gameTime > NextAATime && target != NULL && !LPObject->isDashing()) {
			myIssueOrder(this->LPObject, 3, &(target->GetPos()), target, false, GH.isMinion(target), true);
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
		myIssueOrder(this->LPObject, 2, &position, NULL, false, false, true);
		nextActionTime = gameTime + delay + Humanize();
	}
}

bool LocalPlayer::castSpellMaster(SpellBook* thisPtr, SpellSlot* slot, Spells slotID, Vector3* pos1, Vector3* pos2, DWORD networkID) {
	int zero = 0;
	if (!chargingSpell && gameTime >= nextActionTime && gameTime >= nextSpellTime) {
		if (GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)slotID, &zero) == SpellState::Ready) {
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

bool LocalPlayer::CastSpellAtPos(Spells spell, Vector3 position){
	return castSpellMaster(LPObject->GetSpellBook(), LPObject->GetSpellBook()->GetSpellSlot(spell), spell, &position, &Vector3(0,0,0), 0);
}
bool LocalPlayer::CastSpellAtTarget(Spells spell, CObject* target){
	// MODIFICA PER SendInput return castSpellMaster(LPObject->GetSpellBook(), LPObject->GetSpellBook()->GetSpellSlot(spell), spell, &(LPObject->GetPos()), &(target->GetPos()), target->GetNetworkID());
	return castSpellMaster(LPObject->GetSpellBook(), LPObject->GetSpellBook()->GetSpellSlot(spell), spell, &(target->GetPos()), &(LPObject->GetPos()), target->GetNetworkID());
}
bool LocalPlayer::CastSpellSelf(Spells spell){
	return castSpellMaster(LPObject->GetSpellBook(), LPObject->GetSpellBook()->GetSpellSlot(spell), spell, &(LPObject->GetPos()), &(LPObject->GetPos()), 0);
}
void LocalPlayer::StartChargingSpell(Spells spell, float maxDuration) {
	/*if (CastSpellSelf(spell)) {

		chargingSpell = true;
		chargingStartTime = gameTime;
		chargingEndTime = gameTime + maxDuration;
	}*/
	myStartChargingSpell(spell);
	chargingSpell = true;
	chargingStartTime = gameTime;
	chargingEndTime = gameTime + maxDuration;
}
void LocalPlayer::ReleaseChargeableSpell(Spells spell, Vector3 position) {
	if (chargingSpell && gameTime >= nextActionTime) {
		myCastChargeableSpell(LPObject->GetSpellBook(), LPObject->GetSpellBook()->GetSpellSlot(spell), spell, &position, true);
		chargingSpell = false;
	}
}

/*
void LocalPlayer::CastSpellAtPos(Spells spell, Vector3 position) {
	int zero = 0;
	if (gameTime > nextActionTime&& GH.getSpellState(LPObject->GetSpellBook(), (int)spell, &zero) == SpellState::Ready) {
		LPObject->GetSpellBook()->GetSpellSlot(spell)->GetCastingStruct()->setStartPosition(LPObject->GetPos());
		LPObject->GetSpellBook()->GetSpellSlot(spell)->GetCastingStruct()->setEndPosition(position);
		LPObject->GetSpellBook()->GetSpellSlot(spell)->GetCastingStruct()->setStartIndex(LPObject->GetIndex());
		LPObject->GetSpellBook()->GetSpellSlot(spell)->GetCastingStruct()->setTargetIndex(NULL);

		myCastSpell(LPObject->GetSpellBook(), (LPObject->GetSpellBook()->GetSpellSlot(spell)), spell, LPObject->GetSpellBook()->GetSpellSlot(spell)->GetCastingStruct(), 0);

		nextActionTime = gameTime + delay + Humanize();
	}
}
void LocalPlayer::CastSpellAtTarget(Spells spell, CObject* target) {
	int zero = 0;
	if (gameTime > nextActionTime&& GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)spell, &zero) == SpellState::Ready) {
		LPObject->GetSpellBook()->GetSpellSlot(spell)->GetCastingStruct()->setStartPosition(LPObject->GetPos());
		LPObject->GetSpellBook()->GetSpellSlot(spell)->GetCastingStruct()->setEndPosition(target->GetPos());
		LPObject->GetSpellBook()->GetSpellSlot(spell)->GetCastingStruct()->setStartIndex(NULL);
		LPObject->GetSpellBook()->GetSpellSlot(spell)->GetCastingStruct()->setTargetIndex(NULL);

		myCastSpell(LPObject->GetSpellBook(), (LPObject->GetSpellBook()->GetSpellSlot(spell)), spell, LPObject->GetSpellBook()->GetSpellSlot(spell)->GetCastingStruct(), target->GetNetworkID());

		nextActionTime = gameTime + delay + Humanize();
	}
}
void LocalPlayer::CastSpellSelf(Spells spell) {
	int zero = 0;
	if (gameTime > nextActionTime&& GH.getSpellState(myHero.LPObject->GetSpellBook(), (int)spell, &zero) == SpellState::Ready) {
		LPObject->GetSpellBook()->GetSpellSlot(spell)->GetCastingStruct()->setStartPosition(LPObject->GetPos());
		LPObject->GetSpellBook()->GetSpellSlot(spell)->GetCastingStruct()->setEndPosition(LPObject->GetPos());
		LPObject->GetSpellBook()->GetSpellSlot(spell)->GetCastingStruct()->setStartIndex(LPObject->GetIndex());
		LPObject->GetSpellBook()->GetSpellSlot(spell)->GetCastingStruct()->setTargetIndex(LPObject->GetIndex());

		myCastSpell(LPObject->GetSpellBook(), (LPObject->GetSpellBook()->GetSpellSlot(spell)), spell, LPObject->GetSpellBook()->GetSpellSlot(spell)->GetCastingStruct(), 0);

		nextActionTime = gameTime + delay + Humanize();
	}
}*/

float LocalPlayer::Humanize() {
	return ((rand() % 41) / 1000) - 0.02;
}