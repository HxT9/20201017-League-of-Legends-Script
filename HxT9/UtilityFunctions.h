#pragma once
#include "Drawer.h"
#include "EntitiesContainer.h"

class UtilityFunctions
{
public:
	void drawBoundingBox();
	float calcEffectiveDamage(float damage, float armor);
	bool isValidTarget(CObject* target);
	void drawEntities();
	void drawLastHittableMinions();
	void drawActiveSpells();
	void drawSpellCD();
	void drawMissiles();
	void dbgStream(std::string msg);
	void dbgStreamChrono(std::string msg);
	void drawDebug();
	void* __cdecl get_peb();
	bool isCollision(Vector3 start, Vector3 end, float width, Vector3 center, float radius);
	int minionsColliding(Vector3 start, Vector3 end, float width);
	int heroesColliding(Vector3 start, Vector3 end, float width);
	Vector3 getPredictedPos(CObject* hero, float seconds, float width, CObject* sender);
	void ChampionCustomDraw();
	void drawPredictedPos();
	std::string vformat(const char* fmt, va_list ap);
	std::string stringf(const char* fmt, ...);
};

