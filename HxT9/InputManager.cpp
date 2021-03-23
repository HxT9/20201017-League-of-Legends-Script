#include "InputManager.h"
#include "globalVars.h"

InputManager::InputManager()
{
	Inputs = std::queue<PERS_INPUT>();
	lastExecution = 0.f;
	delay = 0.010f;

	ChampionOnlyIn.type = 1;
	ChampionOnlyIn.ki.wScan = MapVirtualKey('M', 0);
	ChampionOnlyIn.ki.wVk = 'M';
	ChampionOnlyIn.ki.dwFlags = KEYEVENTF_SCANCODE;
	ChampionOnlyIn.ki.time = 0;
}

void InputManager::addInput(INPUT in)
{
	addInput(in, false);
}

void InputManager::addInput(INPUT in, bool championOnly)
{
	PERS_INPUT pi;
	pi.in = in;
	pi.championOnly = championOnly;
	Inputs.push(pi);
}

void InputManager::resetInputs()
{
	while (!Inputs.empty())
		Inputs.pop();
}

void InputManager::tick()
{
	INPUT inputs[10];
	if (gameTime >= lastExecution + delay && !Inputs.empty()) {
		if (Inputs.front().championOnly) {
			inputs[0] = inputs[1] = inputs[2] = inputs[3] = inputs[4] = inputs[6] = inputs[7] = inputs[8] = inputs[9] = ChampionOnlyIn;
			inputs[5] = Inputs.front().in;
			SendInput(10, inputs, sizeof(INPUT));
		}
		else {
			SendInput(1, &Inputs.front().in, sizeof(INPUT));
		}
		Inputs.pop();
		lastExecution = gameTime;
	}
}
