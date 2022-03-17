#include "InputUtils.h"
#include <Windows.h>

void InputUtils::sendKey(char key)
{
	pressKey(key);
	releaseKey(key);
}

void InputUtils::pressKey(char key)
{
	INPUT in;

	ZeroMemory(&in.ki, sizeof(KEYBDINPUT));

	in.type = INPUT_KEYBOARD;
	in.ki.wVk = key;
	in.ki.wScan = MapVirtualKey(key, 0);
	in.ki.dwFlags = KEYEVENTF_SCANCODE;

	SendInput(1, &in, sizeof(INPUT));
}


void InputUtils::releaseKey(char key)
{
	INPUT in;

	ZeroMemory(&in.ki, sizeof(KEYBDINPUT));

	in.type = INPUT_KEYBOARD;
	in.ki.wVk = key;
	in.ki.wScan = MapVirtualKey(key, 0);
	in.ki.dwFlags = KEYEVENTF_KEYUP;

	SendInput(1, &in, sizeof(INPUT));
}
