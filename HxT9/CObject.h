#pragma once
#include <Windows.h>
#include "Vector3.h"
#include "ActiveSpell.h"
#include "SpellInfo.h"
#include "SpellBook.h"
#include "Structs.h"
#include "BuffManager.h"
#include "Enums.h"

class CObject {
public:

	short GetIndex();
	DWORD GetNetworkID();
	Vector3 GetPos();
	bool isTargetable();
	int GetLevel();
	float GetHealth();
	float GetBaseAttackDamage();
	float GetBonusAttackDamage();
	float GetTotalAttackDamage();
	float GetArmor();
	float GetMagicResist();
	float GetMaxHealth();
	float GetAttackRange();
	bool IsVisible();
	float GetBoundingRadius();
	bool IsEnemyTo(CObject* Obj);
	char* GetName();
	char* GetRefName();
	char* GetObjectName();
	int GetTeam();
	float GetFlatArmorReduction();
	float GetPercArmorReduction();
	float GetPercArmorPen();
	float GetPercBonusArmorPen();
	float GetLethality();
	void* GetAIManager();
	void* GetAIManager2();
	float GetAP();
	ActiveSpell* GetActiveSpell();
	SpellInfo* GetMissileSpellInfo();
	short GetMissileSourceIndex();
	boolean GetMissileHasTarget();
	short GetMissileTargetIndex();
	Vector3 GetMissileStartPos();
	Vector3 GetMissileEndPos();
	SpellBook* GetSpellBook();
	Vector3 GetDestinationPos();
	bool isMoving();
	bool isDashing();
	int getAIMgrPassedWaypoints();
	Path GetPath();
	Vector3 GetCurrentPos();
	Vector3 GetVelocity();
	float GetMovementSpeed();
	int getCloseEnemyMinions(float range);
	BuffManager* getBuffManager();
	bool isWindingUp();
	bool isObjType(int type);
};
