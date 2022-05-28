#pragma once
#include "d3d9.h"
#include "d3dx9.h"

namespace ScriptManager
{
	extern bool InitLP;
	extern bool InitHelpers;

	void Tick(LPDIRECT3DDEVICE9 pDevice);
	void Init(LPDIRECT3DDEVICE9 pDevice);
};

