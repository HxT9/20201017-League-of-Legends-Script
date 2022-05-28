#pragma once
#include "d3d9.h"
#include "d3dx9.h"
#include <string>
#include <vector>

namespace IMGUIManager
{
	extern bool ShowLog;
	extern bool ShowMain;
	extern int MaxConsoleLines;
	extern std::vector<std::string> ConsoleLines;

	void Init(LPDIRECT3DDEVICE9 Device);
	void Tick();
	void Dispose();
	void Print(const char* fmt, ...);
	void Print(std::string in);
	void ResetConsole();
};

