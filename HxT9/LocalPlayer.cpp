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

typedef struct {
	Vector3 Pos;
	WORD Key;
}SPELLIN, *PSPELLIN;
void CastSpell(PSPELLIN PSpellin) {
	POINT oldMousePos;
	GetCursorPos(&oldMousePos);

	INPUT in;
	MOUSEINPUT mi;
	KEYBDINPUT ki;

	mi.time = ki.time = 0;

	mi.dwFlags = MOUSEEVENTF_MOVE;
	mi.dx = PSpellin->Pos.x - oldMousePos.x;
	mi.dy = PSpellin->Pos.y - oldMousePos.y;
	in.type = 0;
	in.mi = mi;
	inputManager.addInput(in);

	ki.wVk = PSpellin->Key;
	ki.wScan = MapVirtualKey(PSpellin->Key, 0);
	in.type = 1;
	in.ki = ki;
	inputManager.addInput(in);

	ki.dwFlags = KEYEVENTF_KEYUP;
	in.ki = ki;
	inputManager.addInput(in);

	mi.dx = oldMousePos.x - PSpellin->Pos.x;
	mi.dy = oldMousePos.y - PSpellin->Pos.y;
	in.type = 0;
	in.mi = mi;
	inputManager.addInput(in);
}

void myStartChargingSpell(Spells slotID) {
	INPUT in;
	KEYBDINPUT ki;
	WORD Key;

	switch (slotID) {
	case Spells::Q:
		Key = 'Q';
		break;
	case Spells::W:
		Key = 'W';
		break;
	case Spells::E:
		Key = 'E';
		break;
	case Spells::R:
		Key = 'R';
		break;
	case Spells::Item1:
		Key = '1';
		break;
	case Spells::Item2:
		Key = '2';
		break;
	case Spells::Item3:
		Key = '3';
		break;
	case Spells::Item4:
		Key = '4';
		break;
	case Spells::Item5:
		Key = '5';
		break;
	case Spells::Item6:
		Key = '6';
		break;
	}

	ki.time = 0;

	ki.wVk = Key;
	ki.wScan = MapVirtualKey(Key, 0);
	in.type = 1;
	in.ki = ki;
	inputManager.addInput(in);
}

void ReleaseChargeableSpell(PSPELLIN PSpellin) {
	POINT oldMousePos;
	GetCursorPos(&oldMousePos);

	INPUT in;
	MOUSEINPUT mi;
	KEYBDINPUT ki;

	mi.time = ki.time = 0;

	mi.dwFlags = MOUSEEVENTF_MOVE;
	mi.dx = PSpellin->Pos.x - oldMousePos.x;
	mi.dy = PSpellin->Pos.y - oldMousePos.y;
	in.type = 0;
	in.mi = mi;
	inputManager.addInput(in);

	ki.wVk = PSpellin->Key;
	ki.wScan = MapVirtualKey(PSpellin->Key, 0);
	in.type = 1;
	ki.dwFlags = KEYEVENTF_KEYUP;
	in.ki = ki;
	inputManager.addInput(in);

	mi.dx = oldMousePos.x - PSpellin->Pos.x;
	mi.dy = oldMousePos.y - PSpellin->Pos.y;
	in.type = 0;
	in.mi = mi;
	inputManager.addInput(in);
}

void RClickSpace(Vector3* Par) { //Al posto di spazio uso 'M' come attack champion only
	INPUT in;
	MOUSEINPUT mi;
	KEYBDINPUT ki;
	POINT oldMousePos;

	mi.time = ki.time = 0;

	ki.wScan = MapVirtualKey('M', 0);
	ki.wVk = 'M';
	ki.dwFlags = KEYEVENTF_SCANCODE;
	in.type = 1;
	in.ki = ki;

	//inputManager.addInput(in);

	if (Par != NULL) {
		GetCursorPos(&oldMousePos);
		mi.dwFlags = MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_MOVE;
		mi.dx = Par->x - oldMousePos.x;
		mi.dy = Par->y - oldMousePos.y;
	}
	else {
		mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	}
	in.type = 0;
	in.mi = mi;
	inputManager.addInput(in, true);

	mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	in.mi = mi;
	inputManager.addInput(in);

	ki.dwFlags = KEYEVENTF_KEYUP;
	in.type = 1;
	in.ki = ki;
	inputManager.addInput(in);

	if (Par != NULL) {
		mi.dwFlags = MOUSEEVENTF_MOVE;
		mi.dx = oldMousePos.x - Par->x;
		mi.dy = oldMousePos.y - Par->y;
		in.type = 0;
		in.mi = mi;
		inputManager.addInput(in);
	}
}

void RClickAttack(Vector3* Par) {
	INPUT in;
	MOUSEINPUT mi;
	POINT oldMousePos;

	mi.time = 0;

	GetCursorPos(&oldMousePos);
	mi.dwFlags = MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_MOVE;
	mi.dx = Par->x - oldMousePos.x;
	mi.dy = Par->y - oldMousePos.y;

	in.type = 0;
	in.mi = mi;
	inputManager.addInput(in);

	mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	in.mi = mi;
	inputManager.addInput(in);

	mi.dwFlags = MOUSEEVENTF_MOVE;
	mi.dx = oldMousePos.x - Par->x;
	mi.dy = oldMousePos.y - Par->y;
	in.type = 0;
	in.mi = mi;
	inputManager.addInput(in);
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

	//2 - Move, 3 - Auto Attack

	//StackSpoofForThiscall<int*>((baseAddress + oIssueOrder), 6, thisPtr, (baseAddress + oSpoof), Order, Loc, Target, IsAttackMove, IsMinion, Unknown);
	Vector3 screenPos;
	RECT windowPos;
	Vector3* Params = NULL;

	switch (Order) {
	case 2:
		RClickSpace(NULL);
		break;
	case 3:
		GH.worldToScreen(Loc, &screenPos);
		GetWindowRect(GetActiveWindow(), &windowPos);
		screenPos.x += windowPos.left;
		screenPos.y += windowPos.top;

		Params = new Vector3();
		if (Params) {
			Params->x = screenPos.x;
			Params->y = screenPos.y - 25;
		}

		if (GH.isHero(Target)) {
			RClickSpace(Params);
		}
		else {
			RClickAttack(Params);
		}
		break;
	default:
		break;
	}

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

	//StackSpoofForThiscall<int*>((baseAddress + oOldCastSpell), 5, thisPtr, (baseAddress + oSpoof), slot, slotID, myPos, pos, networkID);
	Vector3 screenPos;
	RECT windowPos;
	GH.worldToScreen(myPos, &screenPos);

	GetWindowRect(GetActiveWindow(), &windowPos);
	screenPos.x += windowPos.left;
	screenPos.y += windowPos.top;

	PSPELLIN Params = new SPELLIN();
	if (Params) {
		Params->Pos = screenPos;
		switch (slotID) {
		case Spells::Q:
			Params->Key = 'Q';
			break;
		case Spells::W:
			Params->Key = 'W';
			break;
		case Spells::E:
			Params->Key = 'E';
			break;
		case Spells::R:
			Params->Key = 'R';
			break;
		case Spells::Item1:
			Params->Key = '1';
			break;
		case Spells::Item2:
			Params->Key = '2';
			break;
		case Spells::Item3:
			Params->Key = '3';
			break;
		case Spells::Item4:
			Params->Key = '4';
			break;
		case Spells::Item5:
			Params->Key = '5';
			break;
		case Spells::Item6:
			Params->Key = '6';
			break;
		}
	}

	CastSpell(Params);

	return;
}
void myCastSpell(SpellBook* thisPtr, SpellSlot* slot, Spells slotID, CastingStruct* pSpellTargetingClient, DWORD networkID) {
	//StackSpoofForThiscall<int*>((baseAddress + oCastSpell), 5, thisPtr, (baseAddress + oSpoof), slot, slotID, pSpellTargetingClient, networkID);

	return;
}
void myCastChargeableSpell(SpellBook* thisPtr, SpellSlot* slot, Spells slotID, Vector3* pos, bool release) {
	//StackSpoofForThiscall<int*>((baseAddress + oUpdateChargeableSpell), 4, thisPtr, (baseAddress + oSpoof), slot, slotID, pos, release);
	Vector3 screenPos;
	RECT windowPos;
	GH.worldToScreen(pos, &screenPos);

	GetWindowRect(GetActiveWindow(), &windowPos);
	screenPos.x += windowPos.left;
	screenPos.y += windowPos.top;

	PSPELLIN Params = new SPELLIN();
	if (Params) {
		Params->Pos = screenPos;
		switch (slotID) {
		case Spells::Q:
			Params->Key = 'Q';
			break;
		case Spells::W:
			Params->Key = 'W';
			break;
		case Spells::E:
			Params->Key = 'E';
			break;
		case Spells::R:
			Params->Key = 'R';
			break;
		case Spells::Item1:
			Params->Key = '1';
			break;
		case Spells::Item2:
			Params->Key = '2';
			break;
		case Spells::Item3:
			Params->Key = '3';
			break;
		case Spells::Item4:
			Params->Key = '4';
			break;
		case Spells::Item5:
			Params->Key = '5';
			break;
		case Spells::Item6:
			Params->Key = '6';
			break;
		}
	}

	ReleaseChargeableSpell(Params);

	return;
}
void LocalPlayer::castBaseUlt() {
	Vector3 targetPos;
	RECT windowPos;
	GetWindowRect(GetActiveWindow(), &windowPos);

	targetPos.x = miniMap.getMinimapPos().x;
	targetPos.y = miniMap.getMinimapPos().y;

	if (this->LPObject->GetTeam() == 100) {
		targetPos.x += miniMap.getMinimapSize().x * 0.9655;
		targetPos.y += miniMap.getMinimapSize().y * 0.0345;
	}
	else {
		targetPos.x += miniMap.getMinimapSize().x * 0.0275;
		targetPos.y += miniMap.getMinimapSize().y * 0.9700;
	}
	targetPos.x += windowPos.left;
	targetPos.y += windowPos.top;

	PSPELLIN Params = new SPELLIN();
	Params->Key = 'R';
	Params->Pos = targetPos;

	CastSpell(Params);
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
	AACheckDelay = 0.08;
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
}
void LocalPlayer::tick() {
	if (LPObject->GetActiveSpell() != NULL && gameTime > NextAATime &&
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
}
void LocalPlayer::AutoAttack(CObject* target){
	if (gameTime >= nextActionTime) {
		if (!chargingSpell && gameTime > NextAATime && target != NULL) {
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