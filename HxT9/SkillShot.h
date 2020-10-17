#pragma once
#include "Vector3.h"
#include "Enums.h"

class SkillShot
{
public:
	Vector3 startPosition;
	Vector3 endPosition;
	Vector3 centerPosition;
	float speed;
	float angle;
	SkillShotType skillShotType;
	float startTime;
	float duration;

	SkillShot(Vector3 startPos, Vector3 endPos, Vector3 centerPos, float spd, float ang, SkillShotType type, float start, float dur);

	SkillShot linearSkillShot(Vector3 startPos, Vector3 endPos, float spd);
	SkillShot circularSkillShot(Vector3 centerPos, float start, float dur);
	SkillShot conicSkillShot(Vector3 centerPos, float ang);
};

