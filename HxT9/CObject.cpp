#include "CObject.h"
#include "offsets.h"
#include "globalVars.h"
#include "GameFunctions.h"
#include "UtilityFunctions.h"


template< typename Function > Function CallVirtual(PVOID Base, DWORD Index)
{
	PDWORD* VTablePointer = (PDWORD*)Base;
	PDWORD VTableFunctionBase = *VTablePointer;
	DWORD dwAddress = VTableFunctionBase[Index];

	return (Function)(dwAddress);
}

short CObject::GetIndex() {
	return *(short*)((DWORD)this + oObjIndex);
}

DWORD CObject::GetNetworkID() {
	return *(DWORD*)((DWORD)this + oObjNetworkID);
}

Vector3 CObject::GetPos() {
	return *(Vector3*)((DWORD)this + oObjPos);
}

bool CObject::IsTargetable() {
	return *(bool*)((DWORD)this + oObjTargetable);
}

int CObject::GetLevel() {
	return *(int*)((DWORD)this + oObjLevel);
}

float CObject::GetHealth() {
	return *(float*)((DWORD)this + oObjHP);
}

float CObject::GetBaseAttackDamage() {
	return *(float*)((DWORD)this + oObjBaseAtk);
}

float CObject::GetBonusAttackDamage() {
	return *(float*)((DWORD)this + oObjBonusAtk);
}

float CObject::GetTotalAttackDamage() {
	return this->GetBonusAttackDamage() + this->GetBaseAttackDamage();
}

float CObject::GetArmor() {
	return *(float*)((DWORD)this + oObjArmor);
}

float CObject::GetMagicResist() {
	return *(float*)((DWORD)this + oObjMagicResist);
}

float CObject::GetMaxHealth() {
	return *(float*)((DWORD)this + oObjMaxHP);
}

float CObject::GetAttackRange() {
	return *(float*)((DWORD)this + oObjAtkRange);
}

bool CObject::IsVisible() {
	return *(bool*)((DWORD)this + oObjVisibility);
}

float CObject::GetBoundingRadius() {
	return GameFunctions::GetBoundingRadius(this);
}

void* CObject::GetAIManager() {
	return GameFunctions::GetAIManager(this);
}

bool CObject::IsWindingUp() {
	typedef void* (__thiscall* OriginalFn)(PVOID);
	return CallVirtual<OriginalFn>(this, 231)(this);
}

float CObject::GetAP() {
	return *(float*)((DWORD)this + oObjAP);
}

bool CObject::IsEnemyTo(CObject* Obj) {
	if (Obj->GetTeam() == 100 && this->GetTeam() == 200)
		return true;

	else if (Obj->GetTeam() == 200 && this->GetTeam() == 100)
		return true;

	return false;
}

char* CObject::GetName() {
	return UtilityFunctions::GetString((DWORD)this + oObjName);
}

char* CObject::GetObjectName() {
	return UtilityFunctions::GetString((DWORD)this + oObjObjectName);
}

int CObject::GetTeam() {
	return *(int*)((DWORD)this + oObjTeam);
}

float CObject::GetFlatArmorReduction() {
	return 0;
}

float CObject::GetPercArmorReduction() {
	return 0;
}

float CObject::GetPercArmorPen() {
	return 0;
}

float CObject::GetPercBonusArmorPen() {
	return 0;
}

float CObject::GetLethality() {
	return 0;
}

ActiveSpell* CObject::GetActiveSpell() {
	return *(ActiveSpell**)((DWORD)this + oObjSpellBook + oSpellBookActiveSpell);
}

SpellInfo* CObject::GetMissileSpellInfo() {
	return *(SpellInfo**)((DWORD)this + oMissileSpellInfo);
}

short CObject::GetMissileSourceIndex() {
	return *(short*)((DWORD)this + oMissileSourceIndex);
}

boolean CObject::GetMissileHasTarget() {
	return *(bool*)((DWORD)this + oMissileHasTarget);
}

short CObject::GetMissileTargetIndex() {
	if (GetMissileHasTarget())
		return **(short**)((DWORD)this + oMissileTargetIndex);

	return NULL;
}

Vector3 CObject::GetMissileStartPos() {
	return *(Vector3*)((DWORD)this + oMissileStartPos);
}

Vector3 CObject::GetMissileEndPos() {
	return *(Vector3*)((DWORD)this + oMissileEndPos);
}

SpellBook* CObject::GetSpellBook() {
	return (SpellBook*)((DWORD)this + oObjSpellBook);
}

Vector3 CObject::GetDestinationPos() {
	return *(Vector3*)((DWORD)GetAIManager() + oAIMGR_TargetPos);
}

bool CObject::IsMoving() {
	return *(bool*)((DWORD)GetAIManager() + oAIMGR_IsMoving);
}

bool CObject::IsDashing() {
	return *(bool*)((DWORD)GetAIManager() + oAIMGR_IsDashing) && IsMoving();
}

int CObject::GetAIMgrPassedWaypoints() {
	return *(int*)((DWORD)GetAIManager() + oAIMGR_PassedWaypoints);
}

Path CObject::GetPath() {
	Path ret;
	ret.nPathPoints = 0;

	if (GetAIManager()) {
		DWORD vNavBegin = *(DWORD*)((DWORD)GetAIManager() + oAIMGR_NavBegin);
		DWORD vNavEnd = (*(DWORD*)((DWORD)GetAIManager() + oAIMGR_NavEnd) - 0xC);

		for (DWORD i = vNavBegin; i <= vNavEnd; i += 0xc) {
			ret.pathPoints[ret.nPathPoints++] = *(Vector3*)i;
		}
	}
	return ret;
}

Vector3 CObject::GetCurrentPos() {
	return *(Vector3*)((DWORD)GetAIManager() + oAIMGR_CurrentPos);
}

Vector3 CObject::GetVelocity() {
	return *(Vector3*)((DWORD)GetAIManager() + oAIMGR_Velocity);
}

float CObject::GetMovementSpeed() {
	return *(float*)((DWORD)this + oObjMoveSpeed);
}

int CObject::GetCloseEnemyMinions(float range) {
	int ret = 0;
	EntityBase* temp;
	for (int i = 0; i < EntitiesContainer::MinionsIndex.size(); i++) {
		temp = EntitiesContainer::Entities[EntitiesContainer::MinionsIndex[i]];
		if (UtilityFunctions::IsValidTarget(temp) && temp->Team != GetTeam() && temp->Pos.distTo(GetPos()) < range) {
			ret++;
		}
	}
	return ret;
}

BuffManager* CObject::GetBuffManager() {
	return (BuffManager*)((DWORD)this + oObjBuffMgr);
}