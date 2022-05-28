#pragma once
#include "CObject.h"
#include "Enums.h"

namespace GameFunctions
{
	typedef bool(__thiscall* fnIsType)(CObject* pObj, DWORD type);
	typedef bool(__cdecl* fnIsTroy)(CObject* pObj);
	typedef bool(__thiscall* fnIsAlive)(CObject* pObj);
	typedef SpellState(__thiscall* fnGetSpellState)(SpellBook* pObj, int slot, void* a3);
	typedef bool(__cdecl* fnW2S)(Vector3* world, Vector3* screen);
	typedef float(__cdecl* fnGetAttackCastDelay)(CObject* pObj);
	typedef float(__cdecl* fnGetAttackDelay)(CObject* pObj);
	typedef int(__thiscall* fnIssueOrderNew)(DWORD a1, int a2, int a3, bool a4, int a5, int a6, char a7);
	typedef void* (__thiscall* fnGetAIManager)(DWORD obj);
	typedef float(__thiscall* fnGetBoundingRadius)(DWORD obj);

	extern fnIsType isType;
	extern fnIsTroy isTroy;
	extern fnIsAlive isAlive;
	extern fnGetSpellState getSpellState;
	extern fnW2S W2S;
	extern fnGetAttackCastDelay getAttackCastDelay;
	extern fnGetAttackDelay getAttackDelay;
	extern fnIssueOrderNew issueOrderNew;
	extern fnGetAIManager getAIManager;
	extern fnGetBoundingRadius getBoundingRadius;

	void	Init();
	void	IssueClick(Vector3 screenPos);
	void*	GetAIManager(CObject* obj);
	float	GetBoundingRadius(CObject* obj);
	bool	IsType(CObject* obj, ObjectTypeFlags type);
	bool	IsTroy(CObject* obj);
	bool	IsAlive(CObject* obj);
	SpellState	GetSpellState(SpellBook* obj, int slot, void* zero);
	void	WorldToScreen(Vector3* in, Vector3* out);
	float	GetAttackCastDelay(CObject* obj);
	float	GetAttackDelay(CObject* obj);
	Vector3 GetMouseWorldPosition();
};

