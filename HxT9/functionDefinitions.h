#pragma once
#include <Windows.h>
#include "d3d9.h"
#include "d3dx9.h"

typedef HRESULT(WINAPI* Prototype_Present)(LPDIRECT3DDEVICE9 pDevice, CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*);