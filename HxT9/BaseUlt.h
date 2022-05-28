#pragma once
#include "Vector3.h"

namespace BaseUlt
{
	extern bool Enabled;
	extern float LastBaseUlt;
	extern float Speed;
	extern float Width;
	extern float CastTime;
	extern Vector3 SpawnPoint;
	extern float CalculatedBaseUlt;
	extern short TargetIndex;

	void Init();
	void Tick();
};

