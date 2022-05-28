#include "LocalPlayer.h"
#include "globalVars.h"
#include "offsets.h"
#include "InputManager.h"
#include "GameFunctions.h"
#include "BaseUlt.h"
#include "EntitiesContainer.h"

float LocalPlayer::Humanizer() {
	return ((rand() % (HumanizerMax + 1 - HumanizerMin)) + HumanizerMin) / 1000;
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

void LocalPlayer::MyIssueOrder(void* thisPtr, int Order, Vector3* Loc, EntityBase* Target, bool IsAttackMove, bool IsMinion, DWORD Unknown) {
	InputManager::PERS_INPUT in;
	in.isClick = true;
	in.championOnly = !Target || Target->Type == EntityType::Hero;
	in.isAA = Target != NULL;
	GameFunctions::WorldToScreen(Loc, &in.screenPos);

	InputManager::AddInput(in);
}
void LocalPlayer::MyCastSpell(Spells spell, Vector3* pos) {
	Vector3 sp;
	GameFunctions::WorldToScreen(pos, &sp);

	MyCastSpellScreen(spell, sp);
}
bool LocalPlayer::CastSpellMaster(SpellBook* thisPtr, SpellSlot* slot, Spells slotID, Vector3* pos1, Vector3* pos2, DWORD networkID) {
	int zero = 0;
	if (gameTime >= NextActionTime) {
		if (GameFunctions::GetSpellState(myHero.PCObject->GetSpellBook(), (int)slotID, &zero) == SpellState::Ready) {
			MyCastSpell(slotID, pos1);
			NextActionTime = gameTime + ActionDelay + Humanizer();

			return true;
		}
	}

	return false;
}
bool LocalPlayer::CastSpellAtPos(Spells spell, Vector3 position) {
	return CastSpellMaster(PCObject->GetSpellBook(), PCObject->GetSpellBook()->GetSpellSlot(spell), spell, &position, &Vector3(0, 0, 0), 0);
}
bool LocalPlayer::CastSpellAtTarget(Spells spell, EntityBase* target) {
	return CastSpellMaster(PCObject->GetSpellBook(), PCObject->GetSpellBook()->GetSpellSlot(spell), spell, &(target->Pos), &(PCObject->GetPos()), target->NetworkID);
}
bool LocalPlayer::CastSpellSelf(Spells spell) {
	return CastSpellMaster(PCObject->GetSpellBook(), PCObject->GetSpellBook()->GetSpellSlot(spell), spell, &(PCObject->GetPos()), &(PCObject->GetPos()), 0);
}
void LocalPlayer::MyCastSpellScreen(Spells spell, Vector3 screenPos) {
	InputManager::AddHookedInput(getKey(spell), screenPos, false);
}
void myStartChargingSpell(Spells slotID) {
	InputManager::AddKey(getKey(slotID), false);
}
void LocalPlayer::CastBaseUlt() {
	MyCastSpell(Spells::R, &BaseUlt::SpawnPoint);
}
void LocalPlayer::StartChargingSpell(Spells spell, float maxDuration) {
	myStartChargingSpell(spell);
}
void LocalPlayer::ReleaseChargeableSpell(Spells spell, Vector3 position) {
	if (gameTime >= NextActionTime) {
		//MyCastSpell(spell, &position);
		InputManager::AddHookedInput(getKey(spell), position.ToScreen(), true);
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
}

void LocalPlayer::CastedAA(float startTime) {
	LastAABeginTime = startTime;
	NextAATime = LastAABeginTime + AADelay;
	LastAAEndTime = LastAABeginTime + AACastTime;
}

void LocalPlayer::CastedAA() {
	CastedAA(gameTime);
}

void LocalPlayer::Tick() {
	UpdateAttributes();

	AACastTime = GameFunctions::GetAttackCastDelay(PCObject);
	AADelay = GameFunctions::GetAttackDelay(PCObject);

	if (this->ActiveSpell != NULL
		&& std::string(this->ActiveSpell->GetSpellInfo()->GetSpellData()->GetName()).find("Attack") != std::string::npos) {

		AAMissileSpeed = this->ActiveSpell->GetSpellInfo()->GetSpellData()->GetSpellSpeed();

		if(gameTime > NextAATime)
			CastedAA(this->ActiveSpell->GetChannelStartTime());
	}

	if (gameTime > LastRandomCheckTime) {
		if (NextAATime > gameTime && ObjectName == "Aphelios") {
			for (int i = 0; i < PCObject->GetBuffManager()->BuffCount(); i++) {
				if (!strcmp(PCObject->GetBuffManager()->getBuff(i)->GetBuffName(), "ApheliosCrescendumManager")) {
					bool found = false;
					for (int j = 0; j < EntitiesContainer::MissilesIndex.size(); j++) {
						EntityBase* mis = EntitiesContainer::Entities[EntitiesContainer::MissilesIndex[j]];
						if (mis->SpellName == "ApheliosCrescendumAttackMisIn") {
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

		LastRandomCheckTime = gameTime + 0.05;
	}
}

bool LocalPlayer::IsChargingSpell() {
	return this->ActiveSpell && std::string(myHero.ActiveSpell->GetSpellInfo()->GetSpellData()->GetName()).find("BasicAttack") == std::string::npos;
}

void LocalPlayer::AutoAttack(EntityBase* target){
	//AA ignore actionTime
	if (!IsChargingSpell() && gameTime > NextAATime && target != NULL && !PCObject->IsDashing() && (UseAAInCombo || Behaviour != Behaviour::Combo)) {
		MyIssueOrder(this->PCObject, 3, &(target->Pos), target, false, target->Type == EntityType::Minion, true);
	}else{
		if (gameTime >= NextActionTime && gameTime >= LastAAEndTime) {
			if (gameTime < NextAATime) {
				AfterAA = true;
			}
			else {
				AfterAA = false;
			}
			MoveTo(GameFunctions::GetMouseWorldPosition());

			NextActionTime = gameTime + ActionDelay + Humanizer();
		}
	}
}
void LocalPlayer::MoveTo(Vector3 position){
	if (gameTime >= NextActionTime) {
		MyIssueOrder(PCObject, 2, &position, NULL, false, false, true);
	}
}