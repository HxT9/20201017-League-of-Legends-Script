#include "CObject.h"
#include "offsets.h"
#include "globalVars.h"


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

bool CObject::isTargetable() {
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
	return GH.getBoundingRadius(this);
}

void* CObject::GetAIManager() {
	return GH.getAIManager(this);
}

void* CObject::GetAIManager2()
{
	typedef void*(__thiscall* OriginalFn)(PVOID);
	return CallVirtual<OriginalFn>(this, 148)(this);
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
	return utils.GetString((DWORD)this + oObjName);
}

char* CObject::GetObjectName() {
	return utils.GetString((DWORD)this + oObjObjectName);
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

bool CObject::isMoving() {
	return *(bool*)((DWORD)GetAIManager() + oAIMGR_IsMoving);
}

bool CObject::isDashing() {
	return *(bool*)((DWORD)GetAIManager() + oAIMGR_IsDashing) && isMoving();
}

int CObject::getAIMgrPassedWaypoints() {
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

int CObject::getCloseEnemyMinions(float range) {
	int ret = 0;
	EntityBase* temp;
	for (int i = 0; i < entitiesContainer.minionsIndex.size(); i++) {
		temp = entitiesContainer.entities[entitiesContainer.minionsIndex[i]];
		if (utils.isValidTarget(temp) && temp->Team != GetTeam() && temp->Pos.distTo(GetPos()) < range) {
			ret++;
		}
	}
	return ret;
}

BuffManager* CObject::getBuffManager() {
	return (BuffManager*)((DWORD)this + oObjBuffMgr);
}

bool CObject::isWindingUp() {
	typedef void* (__thiscall* OriginalFn)(PVOID);
	return CallVirtual<OriginalFn>(this, 231)(this);
}

bool CObject::isObjType(int type)
{
	unsigned __int8* v2; // edi
	unsigned int v3; // edx
	unsigned int v4; // esi
	int* v5; // ecx
	int v6; // eax
	unsigned __int8 v7; // al
	unsigned int v8; // eax
	unsigned __int8* v9; // edx
	char v10; // cl
	int objectId; // [esp+8h] [ebp-4h]

	v2 = (unsigned __int8*)this;
	v3 = 0;
	v4 = *(BYTE*)(v2 + 81);

	objectId = *(DWORD*)&v2[4 * v2[88] + 92];
	if (v4)
	{
		v5 = (int*)(this + 84);
		do
		{
			v6 = *v5;
			++v5;
			*(&objectId + v3) ^= ~v6;
			++v3;
		} while (v3 < v4);
	}
	v7 = v2[82];
	if (v7)
	{
		v8 = 4 - v7;
		if (v8 < 4)
		{
			v9 = &v2[v8 + 84];
			do
			{
				v10 = *v9++;
				*((BYTE*)&objectId + v8++) ^= ~v10;
			} while (v8 < 4);
		}
	}

	return (objectId & type) != 0;
}