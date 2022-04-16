#pragma once
#include "Enums.h"
#include "CObject.h"
#include "Vector3.h"
#include "EntityBase.h"

class LocalPlayer : public EntityBase
{
public:
	bool	useAAInCombo = true;
	bool	useSpell = true;
	float	ActionDelay = 0.1;
	int		HumanizerMin = 40;
	int		HumanizerMax = 60;

	float	AAMissileSpeed = 2000;
	float	AACastTime = 0; //Autoattack casting time
	float	AADelay = 0; //Delay between autoattacks

	float	NextAATime = 0;
	float	LastAAEndTime = 0;
	float	LastAABeginTime = 0;
	float	nextActionTime = 0;
	float	nextSpellTime = 0;
	Behaviour behaviour = Behaviour::Nothing;
	bool	afterAA;
	bool	chargingSpell = false;
	float	chargingStartTime = 0;
	float	chargingEndTime = 0;
	int		selectedTargetIndex = -1;
	float	lastRandomCheckTime = 0;
	bool	ApheliosCrescendumMis = false;

	LocalPlayer();
	LocalPlayer(CObject* obj);
	void Init();
	void CastedAA(float startTime);
	void CastedAA();
	void Tick();
	void AutoAttack(EntityBase* target);
	void MoveTo(Vector3 position);
	bool CastSpellMaster(SpellBook* thisPtr, SpellSlot* slot, Spells slotID, Vector3* pos1, Vector3* pos2, DWORD networkID);
	bool CastSpellAtPos(Spells spell, Vector3 position);
	bool CastSpellAtTarget(Spells spell, EntityBase* target);
	void StartChargingSpell(Spells spell, float maxDuration);
	void ReleaseChargeableSpell(Spells spell, Vector3 position);
	bool CastSpellSelf(Spells spell);
	void MyIssueOrder(void* thisPtr, int Order, Vector3* Loc, EntityBase* Target, bool IsAttackMove, bool IsMinion, DWORD Unknown);
	void MyCastSpell(Spells spell, Vector3* pos);
	void MyCastSpellScreen(Spells spell, Vector3 screenPos);
	void CastBaseUlt();
	float Humanizer();
};

