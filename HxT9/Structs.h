#pragma once
#include "Enums.h"
#include "Vector3.h"

typedef struct SkillShotProperties 
{
	const char* name;
	SkillShotType skillShotType;
	float range;
	float speed;
	float radius;
	float angle;
}SkillShotProperties, *PSkillShotProperties;

typedef struct Path
{
	Vector3 pathPoints[100];
	int nPathPoints = 0;
}Path,  *PPath;