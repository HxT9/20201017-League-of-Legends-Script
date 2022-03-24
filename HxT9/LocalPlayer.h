#pragma once
#include "Enums.h"
#include "CObject.h"
#include "Vector3.h"
#include "EntityBase.h"

class LocalPlayer : public EntityBase
{
public:
	float AAMissileSpeed;
	float AACastTime; //Autoattack casting time
	float AADelay; //Delay between autoattacks

	float NextAATime;
	float LastAAEndTime;
	float LastAABeginTime;
	float nextActionTime;
	float nextSpellTime;
	Behaviour behaviour;
	bool afterAA;
	float delay;
	float spellDelay;
	bool chargingSpell;
	float chargingStartTime;
	float chargingEndTime;
	int selectedTargetIndex;
	bool useSpell;
	float lastRandomCheck;
	bool ApheliosCrescendumMis;

	LocalPlayer();
	LocalPlayer(CObject* obj);
	void Init();
	void tick();
	void AutoAttack(EntityBase* target);
	void MoveTo(Vector3 position);
	bool castSpellMaster(SpellBook* thisPtr, SpellSlot* slot, Spells slotID, Vector3* pos1, Vector3* pos2, DWORD networkID);
	bool CastSpellAtPos(Spells spell, Vector3 position);
	bool CastSpellAtTarget(Spells spell, EntityBase* target);
	void StartChargingSpell(Spells spell, float maxDuration);
	void ReleaseChargeableSpell(Spells spell, Vector3 position);
	bool CastSpellSelf(Spells spell);
	void myIssueOrder(void* thisPtr, int Order, Vector3* Loc, EntityBase* Target, bool IsAttackMove, bool IsMinion, DWORD Unknown);
	void myCastSpell(Spells spell, Vector3* pos);
	void myCastSpellScreen(Spells spell, Vector3 screenPos);
	void castBaseUlt();
	float Humanize();
};

