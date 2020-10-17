#pragma once
#include "functionDefinitions.h"
#include "CObject.h"
#include "SpellBook.h"
#include <string>
#include "Enums.h"

class GameFunctions
{
public:
	bool	isHero(CObject* obj);
	bool	isMinion(CObject* obj);
	bool	isTurret(CObject* obj);
	bool	isTroy(CObject* obj);
	bool	isMissile(CObject* obj);
	bool	isAlive(CObject* obj);
	bool	isTargetable(CObject* obj);
	SpellState		getSpellState(SpellBook* obj, int slot, void* zero);
	void	printChat(const char* cMessage);
	void	printChat(std::string cMessage);
	void	printChat(Vector3 vec);
	void	worldToScreen(Vector3* in, Vector3* out);
	float	getAttackCastDelay(CObject* obj);
	float	getAttackDelay(CObject* obj);
	Vector3 getMouseWorldPosition();
	CObject* getFirstCObject(void* objManager);
	CObject* getNextCObject(void* objManager, CObject* obj);
};

