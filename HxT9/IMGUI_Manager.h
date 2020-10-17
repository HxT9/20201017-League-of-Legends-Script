#pragma once
#include "d3d9.h"
#pragma comment(lib, "d3d9.lib")
#include "d3dx9.h"
#include <string>
#pragma comment(lib, "d3dx9.lib")

class IMGUI_Manager
{
public:
	const char* text[10] = {"","","","","","","","","",""};

	void init(LPDIRECT3DDEVICE9 Device);
	void tick(LPDIRECT3DDEVICE9 pDevice);
	void destroy();
	void print(const char* in);
	void print(std::string in);
	void resetConsole();
};

