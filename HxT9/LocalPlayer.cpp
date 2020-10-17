#include "LocalPlayer.h"
#include "globalVars.h"
#include "offsets.h"
#include "utilities.h"

template <typename ReturnType>
__declspec(naked) ReturnType __stdcall StackSpoofForCdecl(DWORD FuncAddr, unsigned int NumberOfParams, DWORD ROPGadget, ...) {
	__asm {
		push esi
		push edi
		mov edx, esp
		mov ecx, [edx + 16]
		mov edi, ecx
		inc exc
		LoopStart :
		mov eax, [edx + ecx * 4 + 16]
		push eax
		loop LoopStart
		mov esi, ReturnHere
		mov eax, [edx + 12]
		jmp eax,
		ReturnHere:
		imul edi, 4
		add esp, edi
		pop edi
		pop esi
		ret
	}
}

template <typename ReturnType>
__declspec(naked) ReturnType __stdcall StackSpoofForThiscall(DWORD FuncAddr, unsigned int NumberOfParams, void* thisptr, DWORD ROPGadget, ...) {
	__asm {
		push esi
		mov edx, esp
		mov ecx, [edx + 12]
		inc ecx
		LoopStart :
		mov eax, [edx + ecx * 4 + 16]
		push eax
		loop LoopStart
		mov esi, ReturnHere
		mov eax, [edx + 8]
		mov ecx, [edx + 16]
		jmp eax
		ReturnHere :
		pop esi
		ret
	}
}

typedef void (*GenericFunction)();
void myIssueOrder(void* thisPtr, int Order, Vector3* Loc, CObject* Target, bool IsAttackMove, bool IsMinion, DWORD Unknown) {
	/*static DWORD retnAddr = baseAddress + oRetnAddr;
	static DWORD issueOrder = baseAddress + oIssueOrder;
	__asm {
		push retnHere
		mov ecx, thisPtr
		push Unknown
		push IsMinion
		push IsAttackMove
		push Target
		push Loc
		push Order
		push retnAddr
		jmp issueOrder
		retnHere :
	}*/

	/*typedef int(__thiscall* fnIssueOrder)(void* thisPtr, int Order, Vector3* Loc, CObject* Target, bool IsAttackMove, bool IsMinion, DWORD Unknown);
	fnIssueOrder fn = (fnIssueOrder)(baseAddress + oIssueOrder); fn(thisPtr, Order, Loc, Target, IsAttackMove, IsMinion, Unknown);*/

	StackSpoofForThiscall<int*>((baseAddress + oIssueOrder), 6, thisPtr, (baseAddress + oSpoof), Order, Loc, Target, IsAttackMove, IsMinion, Unknown);

	return;
}
void myOldCastSpell(SpellBook* thisPtr, SpellSlot* slot, Spells slotID, Vector3* myPos, Vector3* pos, DWORD networkID) {
	/*static DWORD retnAddr = baseAddress + oRetnAddr;
	static DWORD castSpell = baseAddress + oCastSpell;
	__asm {
		push retnHere
		mov ecx, thisPtr
		push networkID
		push myPos
		push pos
		push slotID
		push slot
		push retnAddr
		jmp castSpell
		retnHere :
	}*/

	StackSpoofForThiscall<int*>((baseAddress + oOldCastSpell), 5, thisPtr, (baseAddress + oSpoof), slot, slotID, myPos, pos, networkID);

	return;
}
void myCastSpell(SpellBook* thisPtr, SpellSlot* slot, Spells slotID, CastingStruct* pSpellTargetingClient, DWORD networkID) {
	StackSpoofForThiscall<int*>((baseAddress + oCastSpell), 5, thisPtr, (baseAddress + oSpoof), slot, slotID, pSpellTargetingClient, networkID);

	return;
}
void myCastChargeableSpell(SpellBook* thisPtr, SpellSlot* slot, Spells slotID, Vector3* pos, bool release) {
	StackSpoofForThiscall<int*>((baseAddress + oUpdateChargeableSpell), 4, thisPtr, (baseAddress + oSpoof), slot, slotID, pos, release);

	return;
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
	LastAATime = 0; //Settato a quando l'AA finisce il cast
	nextActionTime = 0;
	nextSpellTime = 0;
	behaviour = Behaviour::Nothing;
	delay = 0.1;
	spellDelay = 0.1;
	LPObject = 0;
	useSpell = true;
	curPathIndex = 0;
	curPath.nPathPoints = 1;
	curPath.pathPoints;
	chargingSpell = false;
	chargingStartTime = 0;
	chargingEndTime = 0;
}
void LocalPlayer::tick() {
	AACastTime = GH.getAttackCastDelay(LPObject);
	AADelay = GH.getAttackDelay(LPObject);

	if (LPObject->GetActiveSpell() != NULL && gameTime > NextAATime &&
		std::string(LPObject->GetActiveSpell()->GetSpellInfo()->GetSpellData()->GetMissileName()).find("Attack") != std::string::npos) {

		AAMissileSpeed = LPObject->GetActiveSpell()->GetSpellInfo()->GetSpellData()->GetSpellSpeed();
		if (gameTime > LastAATime) {
			LastAATime = gameTime + AACastTime;
			NextAATime = gameTime + AADelay;
			afterAACalled = false;
		}
	}

	if (chargingStartTime >= chargingEndTime) {
		chargingSpell = false; chargingStartTime = 0; chargingEndTime = 0;
	}
}
void LocalPlayer::AutoAttack(CObject* target){
	if (gameTime >= nextActionTime) {
		if (!chargingSpell && gameTime > NextAATime && target != NULL) {
			myIssueOrder(this->LPObject, 3, &(target->GetPos()), target, false, GH.isMinion(target), true);
			nextActionTime = gameTime + delay + Humanize();
		}
		else if (gameTime >= LastAATime) {
			if (!afterAACalled) {
				afterAACalled = true;
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
			myOldCastSpell(thisPtr, slot, slotID, pos1, pos2, networkID);
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
	return castSpellMaster(LPObject->GetSpellBook(), LPObject->GetSpellBook()->GetSpellSlot(spell), spell, &(LPObject->GetPos()), &(target->GetPos()), target->GetNetworkID());
}
bool LocalPlayer::CastSpellSelf(Spells spell){
	return castSpellMaster(LPObject->GetSpellBook(), LPObject->GetSpellBook()->GetSpellSlot(spell), spell, &(LPObject->GetPos()), &(LPObject->GetPos()), 0);
}
void LocalPlayer::StartChargingSpell(Spells spell, float maxDuration) {
	if (CastSpellSelf(spell)) {
		chargingSpell = true;
		chargingStartTime = gameTime;
		chargingEndTime = gameTime + maxDuration;
	}
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