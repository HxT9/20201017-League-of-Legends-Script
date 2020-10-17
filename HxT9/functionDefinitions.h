#pragma once
#include <Windows.h>
#include "d3d9.h"
#pragma comment(lib, "d3d9.lib")
#include "d3dx9.h"
#pragma comment(lib, "d3dx9.lib")

typedef HRESULT(WINAPI* Prototype_Present)(LPDIRECT3DDEVICE9 pDevice, CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*);