#pragma once
#include "Vector3.h"
#include <queue>
#include <Windows.h>

namespace InputManager{
	typedef struct PERS_INPUT {
		INPUT in = INPUT();
		bool championOnly = false;
		bool isClick = false;
		bool isAA = false;
		Vector3 screenPos = Vector3();
		bool hookMouse = false;
		bool hookMouseChange = false;
	}PERS_INPUT;


	extern char C_championOnlyKey;
	extern bool UseScan;
	extern float XOffset;
	extern float YOffset;
	extern float Delay;
	extern float LastExecution;
	extern bool HookingMousePos;
	extern Vector3 HookedMousePos;
	extern std::deque<PERS_INPUT> Inputs;
	extern INPUT ChampionOnlyDown;
	extern INPUT ChampionOnlyUp;

	void Init();
	INPUT GetInput(char key, bool up);
	void AddKey(char key, bool up);
	void AddInput(PERS_INPUT in);
	void AddInput(INPUT in);
	void AddInput(INPUT in, bool championOnly);
	void AddHookedInput(char key, Vector3 hookedPos, bool onlyUp);
	void ResetInputs();
	void Tick();
};