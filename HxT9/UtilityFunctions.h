#pragma once
#include "Drawer.h"
#include "EntitiesContainer.h"

class UtilityFunctions
{
public:
	bool		UseCustomPrediction = true;
	DWORD		ChatOpenAddress = 0;

	void		init();
	void		Draw();
	void		drawMyHero();
	float		calcEffectiveDamage(float damage, float armor);
	bool		isValidTarget(EntityBase* target);
	void		drawEntities();
	void		drawLastHittableMinions();
	void		drawActiveSpells();
	void		drawSpellCD();
	void		drawMissiles();
	void		dbgStream(std::string msg);
	void		dbgStreamChrono(std::string msg);
	void		dbgStreamFile(std::string fileName, std::string msg);
	void		drawDebug();
	bool		isCollision(Vector3 start, Vector3 end, float width, Vector3 center, float radius);
	int			minionsColliding(Vector3 start, Vector3 end, float width);
	int			heroesColliding(Vector3 start, Vector3 end, float width);
	Vector3		getPredictedPos(EntityBase* hero, float seconds, float width, SkillShotType spellType = SkillShotType::Linear);
	void		ChampionCustomDraw();
	bool		isChatOpen();
	std::string	vformat(const char* fmt, va_list ap);
	std::string	stringf(const char* fmt, ...);
	void MB(const char* fmt, ...);
	char* GetString(DWORD Ptr);
	void DebugLog(const char* fmt, ...);
};

