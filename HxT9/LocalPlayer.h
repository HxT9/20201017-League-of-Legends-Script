#pragma once
#include "Enums.h"
#include "CObject.h"
#include "Vector3.h"

class LocalPlayer
{
public:
	CObject* LPObject;

	float AAMissileSpeed;
	float AACastTime; //Autoattack casting time
	float AADelay; //Delay between autoattacks

	float NextAATime;
	float LastAAEndTime;
	float LastAABeginTime;
	float AACheckDelay;
	float lastAACheck;
	float nextActionTime;
	float nextSpellTime;
	Behaviour behaviour;
	bool afterAA;
	float delay;
	float spellDelay;
	char* championName;

	bool chargingSpell;
	float chargingStartTime;
	float chargingEndTime;

	CObject* selectedTarget;

	bool useSpell;

	Path curPath;
	int curPathIndex;

	float randomCheckDelay;
	float lastRandomCheck;

	float boundingRadius;

	LocalPlayer();
	LocalPlayer(CObject* obj);
	void init();
	void tick();
	void AutoAttack(CObject* target);
	void MoveTo(Vector3 position);
	bool castSpellMaster(SpellBook* thisPtr, SpellSlot* slot, Spells slotID, Vector3* pos1, Vector3* pos2, DWORD networkID);
	bool CastSpellAtPos(Spells spell, Vector3 position);
	bool CastSpellAtTarget(Spells spell, CObject* target);
	void StartChargingSpell(Spells spell, float maxDuration);
	void ReleaseChargeableSpell(Spells spell, Vector3 position);
	bool CastSpellSelf(Spells spell);
	void myIssueOrder(void* thisPtr, int Order, Vector3* Loc, CObject* Target, bool IsAttackMove, bool IsMinion, DWORD Unknown);
	void myCastSpell(Spells spell, Vector3* pos);
	void myCastSpellScreen(Spells spell, Vector3 screenPos);
	void castBaseUlt();
	float Humanize();
};

