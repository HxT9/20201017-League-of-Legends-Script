#pragma once
#include "Vector3.h"
#include <queue>
#include <Windows.h>

typedef struct {
	INPUT in;
	bool championOnly;
}PERS_INPUT;

class InputManager {
public:
	float delay;
	float lastExecution;
	std::queue<PERS_INPUT> Inputs;
	INPUT ChampionOnlyIn;
	InputManager();
	void addInput(INPUT in);
	void addInput(INPUT in, bool championOnly);
	void resetInputs();
	void tick();
};