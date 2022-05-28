#pragma once
#include <Windows.h>
#include <vector>
#include "d3d9.h"

namespace HookManager
{
	typedef HRESULT(WINAPI* Prototype_Present)(LPDIRECT3DDEVICE9, CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*);
	typedef BOOL(WINAPI* Prototype_GetCursorPos)(LPPOINT);
	typedef int(__thiscall* Prototype_OnProcessSpell)(void* spellBook, /* SpellCastInfo* */DWORD spellInfo);

	HRESULT	WINAPI HkD3DPresent(LPDIRECT3DDEVICE9 pDevice, CONST RECT* pSrcRect, CONST RECT* pDstRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion);
	LRESULT	WINAPI HkWndProc(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param);
	BOOL	WINAPI HkGetCursorPos(LPPOINT lpPoint);
	int __fastcall HkOnProcessSpell(void* spellBook, void* edx, /*SpellCastInfo* */DWORD spellCastInfo);

	extern PVOID VehHandle;

	extern PVOID OriginalD3DPresent;
	extern PVOID OriginalGetCursorPos;

	extern DWORD onProcessSpellOldProtection;

	extern Prototype_Present NewD3DPresent;
	extern Prototype_GetCursorPos NewGetCursorPos;
	extern Prototype_OnProcessSpell NewOnProcessSpell;
	extern WNDPROC NewWndProc;

	void Init();
	void Dispose();
	bool VehHook(DWORD Fun, DWORD hkFun, DWORD* oldProtection);
	bool VehUnHook(DWORD Fun, DWORD OldProtection);
};
