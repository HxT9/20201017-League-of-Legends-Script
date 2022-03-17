#pragma once
#include "d3d9.h"
#include "d3dx9.h"
#include <string>

class IMGUI_Manager
{
public:
	std::string text[10];

	void init(LPDIRECT3DDEVICE9 Device);
	void tick(LPDIRECT3DDEVICE9 pDevice);
	void destroy();
	void print(const char* fmt, ...);
	void print(std::string in);
	void resetConsole();
};

