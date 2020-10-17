#pragma once
#include "Enums.h"
#include "Vector3.h"

typedef struct {
	const char* name;
	SkillShotType skillShotType;
	float range;
	float speed;
	float radius;
	float angle;
}SkillShotProperties, *PSkillShotProperties;

typedef struct
{
	Vector3 pathPoints[100];
	int nPathPoints = 0;
}Path,  *PPath;