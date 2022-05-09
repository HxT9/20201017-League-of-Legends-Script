#pragma once
#include "CObject.h"
#include "SpellBook.h"
#include <string>
#include "Enums.h"

class GameFunctions
{
public:
	void	issueClick(Vector3 screenPos);
	void*	getAIManager(CObject* obj);
	float	getBoundingRadius(CObject* obj);
	bool	isHero(CObject* obj);
	bool	isMinion(CObject* obj);
	bool	isTurret(CObject* obj);
	bool	isTroy(CObject* obj);
	bool	isMissile(CObject* obj);
	bool	isAlive(CObject* obj);
	SpellState		getSpellState(SpellBook* obj, int slot, void* zero);
	void	worldToScreen(Vector3* in, Vector3* out);
	float	getAttackCastDelay(CObject* obj);
	float	getAttackDelay(CObject* obj);
	int onProcessSpell(void* spellBook, DWORD spellInfo);
	Vector3 getMouseWorldPosition();
	CObject* getFirstCObject(void* objManager);
	CObject* getNextCObject(void* objManager, CObject* obj);
};

