#pragma once
#include "EntitiesContainer.h"

namespace UtilityFunctions
{
	extern bool UseCustomPrediction;
	extern DWORD ChatOpenAddress;

	void		Init();
	void		Draw();
	float		CalcEffectiveDamage(float damage, float armor);
	bool		IsValidTarget(EntityBase* target);
	void		DbgStream(std::string msg);
	void		DbgStreamChrono(std::string msg);
	void		DbgStreamFile(std::string fileName, std::string msg);
	void		DrawDebug();
	bool		IsCollision(Vector3 start, Vector3 end, float width, Vector3 center, float radius);
	int			MinionsColliding(Vector3 start, Vector3 end, float width);
	int			HeroesColliding(Vector3 start, Vector3 end, float width);
	Vector3		GetPredictedPos(EntityBase* hero, float seconds, float width, SkillShotType spellType = SkillShotType::Linear);
	void		ChampionCustomDraw();
	bool		IsChatOpen();
	std::string VFormat(const char* fmt, va_list ap);
	std::string	Stringf(const char* fmt, ...);
	void		MB(const char* fmt, ...);
	char*		GetString(DWORD Ptr);
	void		DebugLog(const char* fmt, ...);
};

