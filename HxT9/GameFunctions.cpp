#include "GameFunctions.h"
#include "offsets.h"
#include "globalVars.h"

namespace GameFunctions {
	fnIsType isType;
	fnIsTroy isTroy;
	fnIsAlive isAlive;
	fnGetSpellState getSpellState;
	fnW2S W2S;
	fnGetAttackCastDelay getAttackCastDelay;
	fnGetAttackDelay getAttackDelay;
	fnIssueOrderNew issueOrderNew;
	fnGetAIManager getAIManager;
	fnGetBoundingRadius getBoundingRadius;

	void Init() {
		isType = (fnIsType)(baseAddress + oIsType);
		isTroy = (fnIsTroy)(baseAddress + oIsTroy);
		isAlive = (fnIsAlive)(baseAddress + oIsAlive);
		getSpellState = (fnGetSpellState)(baseAddress + oGetSpellState);
		W2S = (fnW2S)(baseAddress + oW2S);
		getAttackCastDelay = (fnGetAttackCastDelay)(baseAddress + oGetAttackCastDelay);
		getAttackDelay = (fnGetAttackDelay)(baseAddress + oGetAttackDelay);
		issueOrderNew = (fnIssueOrderNew)(baseAddress + oIssueClickNew);
		getAIManager = (fnGetAIManager)(baseAddress + oGetAIManager);
		getBoundingRadius = (fnGetBoundingRadius)(baseAddress + oGetBoundingRadius);
	}

	bool IsType(CObject* pObj, ObjectTypeFlags type) {
		return pObj && isType(pObj, (DWORD)type);
	}

	bool IsTroy(CObject* pObj) {
		return pObj && isTroy(pObj);
	}

	bool IsAlive(CObject* obj) {
		 return obj && isAlive(obj);
	}

	SpellState GetSpellState(SpellBook* obj, int slot, void* zero) {
		return getSpellState(obj, slot, zero);
	}

	void WorldToScreen(Vector3* in, Vector3* out) {
		W2S(in, out);
	}

	float GetAttackCastDelay(CObject* obj) {
		return getAttackCastDelay(obj);
	}
	float GetAttackDelay(CObject* obj) {
		return getAttackDelay(obj);
	}

	void IssueClick(Vector3 screenPos) {
		issueOrderNew(*(DWORD*)(*(DWORD*)(baseAddress + oHudInstance) + 0x24), 0, 0, true, screenPos.x, screenPos.y, 0);
		issueOrderNew(*(DWORD*)(*(DWORD*)(baseAddress + oHudInstance) + 0x24), 1, 0, true, screenPos.x, screenPos.y, 0);
	}

	void* GetAIManager(CObject* obj) {
		return getAIManager((DWORD)obj);
	}

	float GetBoundingRadius(CObject* obj) {
		return getBoundingRadius((DWORD)obj);
	}

	Vector3	GetMouseWorldPosition() {
		unsigned long MousePtr = baseAddress + oHudInstance;

		auto aux1 = *(unsigned long*)MousePtr;
		aux1 += 0x14;
		auto aux2 = *(unsigned long*)aux1;
		aux2 += 0x1C;

		float X = *(float*)(aux2 + 0x0);
		float Y = *(float*)(aux2 + 0x4);
		float Z = *(float*)(aux2 + 0x8);

		return Vector3{ X, Y, Z };
	}
}