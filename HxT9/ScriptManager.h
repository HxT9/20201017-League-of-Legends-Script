#pragma once
#include "d3d9.h"
#include "d3dx9.h"

class ScriptManager
{
	bool initLP = false;
	bool initHelpers = false;

public:
	bool leftButtonDown = false;

public:
	void tick(LPDIRECT3DDEVICE9 pDevice);
	void init(LPDIRECT3DDEVICE9 pDevice);
	void initWndProc();
};

