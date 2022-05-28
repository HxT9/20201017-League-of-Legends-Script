#include "InputManager.h"
#include "globalVars.h"
#include "GameFunctions.h"

namespace InputManager {
	char C_championOnlyKey = 'M';
	bool UseScan = false;
	float XOffset;
	float YOffset;
	float Delay;
	float LastExecution;
	bool HookingMousePos;
	Vector3 HookedMousePos;
	std::deque<PERS_INPUT> Inputs;
	INPUT ChampionOnlyDown;
	INPUT ChampionOnlyUp;

	void Init()
	{
		Inputs = std::deque<PERS_INPUT>();
		LastExecution = 0.f;
		HookingMousePos = false;
		HookedMousePos = Vector3();

		ChampionOnlyDown = GetInput(C_championOnlyKey, false);
		ChampionOnlyUp = GetInput(C_championOnlyKey, true);

		YOffset = (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXPADDEDBORDER));
		XOffset = (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER));
	}

	INPUT GetInput(char key, bool up) {
		INPUT in;
		ZeroMemory(&in, sizeof(INPUT));

		in.type = INPUT_KEYBOARD;
		in.ki.wVk = key;
		in.ki.wScan = MapVirtualKey(in.ki.wVk, 0);
		in.ki.dwFlags = 0;
		if (UseScan)
			in.ki.dwFlags = KEYEVENTF_SCANCODE;
		if (up)
			in.ki.dwFlags |= KEYEVENTF_KEYUP;
		in.ki.time = 0;

		return in;
	}

	void AddKey(char key, bool up) {
		AddInput(GetInput(key, up));
	}

	void AddInput(PERS_INPUT in) {
		if (in.isAA)
			for (int i = 0; i < Inputs.size(); i++)
				if (Inputs[i].isAA)
					return;

		if (in.championOnly)
			AddInput(ChampionOnlyDown);

		Inputs.push_back(in);

		if (in.championOnly)
			AddInput(ChampionOnlyUp);
	}

	void AddInput(INPUT in)
	{
		AddInput(in, false);
	}

	void AddInput(INPUT in, bool championOnly)
	{
		PERS_INPUT pi;
		pi.in = in;
		pi.championOnly = championOnly;

		AddInput(pi);
	}

	void AddHookedInput(char key, Vector3 hookedPos, bool onlyUp) {
		PERS_INPUT pi1, pi2;
		pi1.hookMouse = true;
		pi1.hookMouseChange = true;
		pi1.screenPos = hookedPos;
		AddInput(pi1);

		if (!onlyUp)
			AddInput(GetInput(key, false));
		AddInput(GetInput(key, true));

		pi2.hookMouse = false;
		pi2.hookMouseChange = true;
		AddInput(pi2);
	}

	void InputManager::ResetInputs()
	{
		Inputs.clear();
	}

	void InputManager::Tick()
	{
		RECT windowPos;
		INPUT inputs[10];
		if (!Inputs.empty()) {
			if (Inputs.front().isClick) {
				GameFunctions::IssueClick(Inputs.front().screenPos);

				if (Inputs.front().isAA)
					myHero.CastedAA();
			}
			else {
				if (Inputs.front().hookMouseChange) {
					GetWindowRect(GetActiveWindow(), &windowPos);
					Inputs.front().screenPos.x += windowPos.left;
					Inputs.front().screenPos.y += windowPos.top;

					HookedMousePos = Inputs.front().screenPos;
					HookedMousePos.x += XOffset;
					HookedMousePos.y += YOffset;
					HookingMousePos = Inputs.front().hookMouse;
				}
				else {
					SendInput(1, &Inputs.front().in, sizeof(INPUT));
				}
			}

			Inputs.pop_front();

			//if (scriptManager.Debugging)
			//	IMGUIManager::Print("[InputManager] lastExecution: %f, gameTime: %f, delta: %f, inputs left: %d", lastExecution, gameTime, gameTime - lastExecution, Inputs.size());

			LastExecution = gameTime;
		}
	}
}