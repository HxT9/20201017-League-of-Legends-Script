#pragma once
#include "Vector3.h"
#include <queue>
#include <Windows.h>

typedef struct PERS_INPUT {
	INPUT in = INPUT();
	bool championOnly = false;
	bool isClick = false;
	bool isAA = false;
	Vector3 screenPos = Vector3();
	bool hookMouse = false;
	bool hookMouseChange = false;
}PERS_INPUT;

class InputManager {
public:
	char C_championOnlyKey = 'M';
	bool useScan = false;
	float xOffset;
	float yOffset;
	float delay;
	float lastExecution;
	bool hookingMousePos;
	Vector3 hookedMousePos;
	std::deque<PERS_INPUT> Inputs;
	INPUT ChampionOnlyDown;
	INPUT ChampionOnlyUp;
	InputManager();
	INPUT getInput(char key, bool up);
	void addKey(char key, bool up);
	void addInput(PERS_INPUT in);
	void addInput(INPUT in);
	void addInput(INPUT in, bool championOnly);
	void addHookedInput(char key, Vector3 hookedPos, bool onlyUp);
	void resetInputs();
	void tick();
};