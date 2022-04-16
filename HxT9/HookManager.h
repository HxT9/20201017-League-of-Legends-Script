#pragma once
#include <Windows.h>
#include "d3d9.h"

typedef HRESULT(WINAPI* Prototype_Present)(LPDIRECT3DDEVICE9, CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*);
typedef BOOL(WINAPI* Prototype_GetCursorPos)(LPPOINT);

HRESULT	WINAPI HkD3DPresent(LPDIRECT3DDEVICE9 pDevice, CONST RECT* pSrcRect, CONST RECT* pDstRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion);
LRESULT	WINAPI HkWndProc(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param);
BOOL	WINAPI HkGetCursorPos(LPPOINT lpPoint);

class HookManager
{
public:
	PVOID OriginalD3DPresent;
	PVOID OriginalGetCursorPos;

	Prototype_Present NewD3DPresent;
	Prototype_GetCursorPos NewGetCursorPos;
	WNDPROC NewWndProc;

public:
	HookManager();
	void Init();
	void Dispose();
};
