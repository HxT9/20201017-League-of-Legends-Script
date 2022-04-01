#pragma once
#include "d3d9.h"
#include "d3dx9.h"
#include <string>
#include <vector>

class IMGUIManager
{
public:
	bool ShowLog = false;
	bool ShowMain = false;
	int MaxConsoleLines = 1000;
	std::vector<std::string> ConsoleLines;

	void init(LPDIRECT3DDEVICE9 Device);
	void tick();
	void destroy();
	void print(const char* fmt, ...);
	void print(std::string in);
	void resetConsole();
	void renderLogWindow();
	void renderMainWindow();
};

