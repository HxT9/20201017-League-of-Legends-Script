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

bool	GameFunctions::isHero(CObject* obj) {
	fnIsHero fun = (fnIsHero)(baseAddress + oIsHero); return fun(obj);
}
bool	GameFunctions::isMinion(CObject* obj) {
	fnIsMinion fun = (fnIsMinion)(baseAddress + oIsMinion); return fun(obj);
}
bool	GameFunctions::isTurret(CObject* obj) {
	fnIsTurret fun = (fnIsTurret)(baseAddress + oIsTurret); return fun(obj);
}
bool	GameFunctions::isTroy(CObject* obj) {
	fnIsTroy fun = (fnIsTroy)(baseAddress + oIsTroy); return fun(obj);
}
bool	GameFunctions::isMissile(CObject* obj) {
	fnIsMissile fun = (fnIsMissile)(baseAddress + oIsMissile); return fun(obj);
}
bool	GameFunctions::isAlive(CObject* obj) {
	fnIsAlive fun = (fnIsAlive)(baseAddress + oIsAlive); return fun(obj);
}
bool	GameFunctions::isTargetable(CObject* obj) {
	fnIsTargetable fun = (fnIsTargetable)(baseAddress + oIsTargetable); return fun(obj);
}
SpellState		GameFunctions::getSpellState(SpellBook* obj, int slot, void* zero) {
	fnGetSpellState fun = (fnGetSpellState)(baseAddress + oGetSpellState); return fun(obj, slot, zero);
}
void	GameFunctions::printChat(const char* cMessage) {
	DWORD chatClientPtr = *(unsigned long*)(baseAddress + oChatClientPtr);
	if (chatClientPtr != NULL) {
		fnPrintChat fun = (fnPrintChat)(baseAddress + oPrintChat); fun((DWORD*)chatClientPtr, cMessage, 2);
	}
}
void	GameFunctions::printChat(std::string cMessage) {
	char* out = new char[cMessage.size() + 1];
	copy(cMessage.begin(), cMessage.end(), out);
	printChat(out);
	delete[] out;
}
void	GameFunctions::printChat(Vector3 vec) {
	printChat("x: " + std::to_string(vec.x) + ", y: " + std::to_string(vec.y) + ", z: " + std::to_string(vec.z));
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
CObject* GameFunctions::getFirstCObject(void* objManager)
{
	fnGetFirstCObject fun = (fnGetFirstCObject)(baseAddress + oGetFirstCObject); return fun(objManager);
}
CObject* GameFunctions::getNextCObject(void* objManager, CObject* obj)
{
	fnGetNextCObject fun = (fnGetNextCObject)(baseAddress + oGetNextCObject); return fun(objManager, obj);
}