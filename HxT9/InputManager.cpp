#include "InputManager.h"
#include "globalVars.h"
#include "Constants.h"

InputManager::InputManager()
{
	Inputs = std::deque<PERS_INPUT>();
	lastExecution = 0.f;
	hookingMousePos = false;
	hookedMousePos = Vector3();

	ChampionOnlyDown = getInput(C_championOnlyKey, false);
	ChampionOnlyUp = getInput(C_championOnlyKey, true);

	yOffset = (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXPADDEDBORDER));
	xOffset = (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER));
}

INPUT InputManager::getInput(char key, bool up) {
	INPUT in;
	ZeroMemory(&in, sizeof(INPUT));

	in.type = INPUT_KEYBOARD;
	in.ki.wVk = key;
	in.ki.wScan = MapVirtualKey(in.ki.wVk, 0);
	in.ki.dwFlags = 0;
	if (useScan)
		in.ki.dwFlags = KEYEVENTF_SCANCODE;
	if (up)
		in.ki.dwFlags |= KEYEVENTF_KEYUP;
	in.ki.time = 0;

	return in;
}

void InputManager::addKey(char key, bool up) {
	addInput(getInput(key, up));
}

void InputManager::addInput(PERS_INPUT in) {
	if (in.isAA)
		for (int i = 0; i < Inputs.size(); i++)
			if (Inputs[i].isAA)
				return;

	if (in.championOnly)
		addInput(ChampionOnlyDown);

	Inputs.push_back(in);

	if (in.championOnly)
		addInput(ChampionOnlyUp);
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

	addInput(pi);
}

void InputManager::addHookedInput(char key, Vector3 hookedPos, bool onlyUp) {
	PERS_INPUT pi1, pi2;
	pi1.hookMouse = true;
	pi1.hookMouseChange = true;
	pi1.screenPos = hookedPos;
	addInput(pi1);

	if(!onlyUp)
		addInput(getInput(key, false));
	addInput(getInput(key, true));

	pi2.hookMouse = false;
	pi2.hookMouseChange = true;
	addInput(pi2);
}

void InputManager::resetInputs()
{
	Inputs.clear();
}

void InputManager::tick()
{
	RECT windowPos;
	INPUT inputs[10];
	if (!Inputs.empty()) {
		if (Inputs.front().isClick) {
			GH.issueClick(Inputs.front().screenPos);
			
			if (Inputs.front().isAA)
				myHero.CastedAA();
		}
		else {
			if (Inputs.front().hookMouseChange) {
				GetWindowRect(GetActiveWindow(), &windowPos);
				Inputs.front().screenPos.x += windowPos.left;
				Inputs.front().screenPos.y += windowPos.top;

				hookedMousePos = Inputs.front().screenPos;
				hookedMousePos.x += xOffset;
				hookedMousePos.y += yOffset;
				hookingMousePos = Inputs.front().hookMouse;
			}
			else {
				SendInput(1, &Inputs.front().in, sizeof(INPUT));
			}
		}

		Inputs.pop_front();

		//if (scriptManager.Debugging)
		//	gui.print("[InputManager] lastExecution: %f, gameTime: %f, delta: %f, inputs left: %d", lastExecution, gameTime, gameTime - lastExecution, Inputs.size());

		lastExecution = gameTime;
	}
}