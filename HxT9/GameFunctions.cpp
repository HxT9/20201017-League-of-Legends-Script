#include "GameFunctions.h"
#include "offsets.h"
#include "globalVars.h"

typedef CObject* (__thiscall* fnGetFirstCObject)(void*);
typedef CObject* (__thiscall* fnGetNextCObject)(void*, CObject*);

typedef bool(__cdecl* fnIsHero)(CObject* pObj);
typedef bool(__cdecl* fnIsMinion)(CObject* pObj);
typedef bool(__cdecl* fnIsTurret)(CObject* pObj);
typedef bool(__cdecl* fnIsTroy)(CObject* pObj);
typedef bool(__cdecl* fnIsMissile)(CObject* pObj);

typedef bool(__thiscall* fnIsAlive)(CObject* pObj);
typedef bool(__thiscall* fnIsTargetable)(CObject* pObj);

typedef SpellState(__thiscall* fnGetSpellState)(SpellBook* pObj, int slot, void* a3);

typedef void(__thiscall* fnPrintChat)(DWORD* ChatClientPtr, const char* cMessage, int iColor);
typedef bool(__cdecl* fnW2S)(Vector3* world, Vector3* screen);

typedef float(__cdecl* fnGetAttackCastDelay)(CObject* pObj);
typedef float(__cdecl* fnGetAttackDelay)(CObject* pObj);

typedef int(__thiscall* fnIssueOrderNew)(DWORD a1, int a2, int a3, bool a4, int a5, int a6, char a7);

typedef void*(__thiscall* fnGetAIManager)(DWORD obj);
typedef float(__thiscall* fnGetBoundingRadius)(DWORD obj);

typedef int(__thiscall* event_OnProcessSpell)(void* spellBook, /* SpellCastInfo* */DWORD spellInfo);

void GameFunctions::issueClick(Vector3 screenPos) {
	fnIssueOrderNew fun = (fnIssueOrderNew)(baseAddress + oIssueClickNew);
	fun(*(DWORD*)(*(DWORD*)(baseAddress + oHudInstance) + 0x24), 0, 0, true, screenPos.x, screenPos.y, 0);
	fun(*(DWORD*)(*(DWORD*)(baseAddress + oHudInstance) + 0x24), 1, 0, true, screenPos.x, screenPos.y, 0);
}

/*void GameFunctions::issueKey(Vector3 screenPos) {
	fnIssueOrderNew fun = (fnIssueOrderNew)(baseAddress + oIssueClickNew);
	fun(*(DWORD*)(*(DWORD*)(baseAddress + oHudInstance) + 0x34), 0, 0, 0, 0, 0);
}*/

void* GameFunctions::getAIManager(CObject* obj) {
	fnGetAIManager fun = (fnGetAIManager)(baseAddress + oGetAIManager);
	return fun((DWORD)obj);
}

float GameFunctions::getBoundingRadius(CObject* obj) {
	fnGetBoundingRadius fun = (fnGetBoundingRadius)(baseAddress + oGetBoundingRadius);
	return fun((DWORD)obj);
}

bool	GameFunctions::isHero(CObject* obj) {
	fnIsHero fun = (fnIsHero)(baseAddress + oIsHero); return obj && fun(obj);
}
bool	GameFunctions::isMinion(CObject* obj) {
	fnIsMinion fun = (fnIsMinion)(baseAddress + oIsMinion); return obj && fun(obj);
}
bool	GameFunctions::isTurret(CObject* obj) {
	fnIsTurret fun = (fnIsTurret)(baseAddress + oIsTurret); return obj && fun(obj);
}
bool	GameFunctions::isTroy(CObject* obj) {
	fnIsTroy fun = (fnIsTroy)(baseAddress + oIsTroy); return obj && fun(obj);
}
bool	GameFunctions::isMissile(CObject* obj) {
	fnIsMissile fun = (fnIsMissile)(baseAddress + oIsMissile); return obj && fun(obj);
}
bool	GameFunctions::isAlive(CObject* obj) {
	fnIsAlive fun = (fnIsAlive)(baseAddress + oIsAlive); return obj && fun(obj);
}
SpellState		GameFunctions::getSpellState(SpellBook* obj, int slot, void* zero) {
	fnGetSpellState fun = (fnGetSpellState)(baseAddress + oGetSpellState); return fun(obj, slot, zero);
}
void	GameFunctions::worldToScreen(Vector3* in, Vector3* out) {
	fnW2S fun = (fnW2S)(baseAddress + oW2S); fun(in, out);
}
Vector3	GameFunctions::getMouseWorldPosition() {
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
float	GameFunctions::getAttackCastDelay(CObject* obj) {
	fnGetAttackCastDelay fun = (fnGetAttackCastDelay)(baseAddress + oGetAttackCastDelay); return fun(obj);
}
float	GameFunctions::getAttackDelay(CObject* obj) {
	fnGetAttackDelay fun = (fnGetAttackDelay)(baseAddress + oGetAttackDelay); return fun(obj);
}

int GameFunctions::onProcessSpell(void* spellBook, /* SpellCastInfo* */DWORD spellInfo) {
	event_OnProcessSpell fun = (event_OnProcessSpell)(baseAddress + oOnProcessSpell); return fun(spellBook, spellInfo);
}