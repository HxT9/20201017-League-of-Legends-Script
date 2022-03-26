#pragma once
#include "d3d9.h"
#include "d3dx9.h"

class ScriptManager
{
	bool initLP = false;
	bool initHelpers = false;

public:
	void Tick(LPDIRECT3DDEVICE9 pDevice);
	void Init(LPDIRECT3DDEVICE9 pDevice);
};

