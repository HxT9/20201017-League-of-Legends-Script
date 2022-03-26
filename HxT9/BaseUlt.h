#pragma once
#include "Vector3.h"

class BaseUlt
{
public:
	float lastBaseUlt;
	float speed;
	float width;
	float castTime;
	Vector3 spawnPoint;

	float calculatedBaseUlt = -100;
	short targetIndex = 0;

	void init();

	void tick();
};

