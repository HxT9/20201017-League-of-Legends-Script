#include "HookManager.h"
#include "globalVars.h"
#include <Psapi.h>
#include "libs/MinHook.h"
#include "imgui.h"

DWORD findPattern(const char* module, const char* pattern) {

#define in_range(x, a, b) (x >= a && x <= b)
#define get_bits(x) (in_range((x & (~0x20)), 'A', 'F') ? ((x & (~0x20)) - 'A' + 0xA): (in_range(x, '0', '9') ? x - '0': 0))
#define get_byte(x) (get_bits(x[0]) << 4 | get_bits(x[1]))

	HMODULE hdll = GetModuleHandleA(module);
	MODULEINFO mod;
	if (!K32GetModuleInformation(GetCurrentProcess(), hdll, &mod, sizeof(MODULEINFO))) {
		return NULL;
	}

	uintptr_t start = (uintptr_t)mod.lpBaseOfDll;
	uintptr_t end = (uintptr_t)mod.lpBaseOfDll + (uintptr_t)mod.SizeOfImage;
	uintptr_t match = (uintptr_t)nullptr;
	const char* current = pattern;

	for (uintptr_t pCur = start; pCur < end; pCur++) {

		if (!*current)
			return (DWORD)match;

		if (*(PBYTE)current == ('\?') || *(BYTE*)pCur == get_byte(current)) {
			if (!match)
				match = pCur;

			if (!current[2])
				return (DWORD)match;

			if (*(PWORD)current == ('\?\?') || *(PBYTE)current != ('\?'))
				current += 3;
			else
				current += 2;
		}
		else {
			current = pattern;
			match = 0;
		}
	}
	return NULL;
}

PVOID GetD3DPresent() {
	HMODULE d3dll;
	DWORD adr;
	PDWORD VTable;

	do {
		d3dll = GetModuleHandle(L"d3d9.dll");
		Sleep(10);
	} while (!d3dll);

	adr = findPattern("d3d9.dll", "C7 06 ? ? ? ? 89 86 ? ? ? ? 89 86") + 2;

	if (adr == NULL) {
		MessageBoxA(NULL, "d3d9 not found", "Error", MB_OK);
		return FALSE;
	}

	memcpy(&VTable, (PVOID)adr, 4);

	return (PVOID)VTable[17];
}

HookManager::HookManager() {
	MH_Initialize();
}

void HookManager::Init() {
	OriginalD3DPresent = GetD3DPresent();
	MH_CreateHook(OriginalD3DPresent, &HkD3DPresent, (LPVOID*)&NewD3DPresent);
	MH_EnableHook(OriginalD3DPresent);

	OriginalGetCursorPos = GetCursorPos;
	MH_CreateHook(OriginalGetCursorPos, &HkGetCursorPos, (LPVOID*)&NewGetCursorPos);
	MH_EnableHook(OriginalGetCursorPos);

	HWND hwnd = FindWindowA(0, "League of Legends (TM) Client");
	NewWndProc = WNDPROC(SetWindowLongA(hwnd, GWL_WNDPROC, (LONG)HkWndProc));
}

void HookManager::Dispose() {
	MH_DisableHook(MH_ALL_HOOKS);

	WNDPROC(SetWindowLongA(FindWindowA(0, "League of Legends (TM) Client"), GWL_WNDPROC, (LONG)NewWndProc));
}

HRESULT WINAPI HkD3DPresent(LPDIRECT3DDEVICE9 pDevice, CONST RECT* pSrcRect, CONST RECT* pDstRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion) {
    __try {
        scriptManager.Tick(pDevice);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        gui.print("[%f] Exception", gameTime);
    }

    return hookManager.NewD3DPresent(pDevice, pSrcRect, pDstRect, hDestWindow, pDirtyRegion);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param);
LRESULT WINAPI HkWndProc(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param) {
	if (gui.ShowLog) {
		ImGui_ImplWin32_WndProcHandler(hwnd, u_msg, w_param, l_param);
	}

	if (gui.ShowMain) {
		if (u_msg == WM_KEYUP && (w_param == VK_END || w_param == VK_ESCAPE)){
			gui.ShowMain = false;
			CallWindowProcA(hookManager.NewWndProc, hwnd, WM_ACTIVATE, WA_ACTIVE, 0);
		}

		ImGui_ImplWin32_WndProcHandler(hwnd, u_msg, w_param, l_param);
		return 1;
	}

	switch (u_msg) {
	case WM_SETFOCUS:
		orbWalker.focused = true;
		break;

	case WM_KILLFOCUS:
		orbWalker.focused = false;
		break;

	case WM_KEYDOWN:
		if (w_param == VK_SHIFT) {
			gui.ShowLog = true;
		}
		break;

	case WM_KEYUP:
		if (w_param == VK_SHIFT){
			gui.ShowLog = false;
		}

		if (w_param == VK_END) {
			gui.ShowMain = true;
			CallWindowProcA(hookManager.NewWndProc, hwnd, WM_ACTIVATE, WA_INACTIVE, 0);
		}

		if (w_param == VK_NUMPAD1)
			myHero.useSpell = true;

		if (w_param == VK_NUMPAD2)
			myHero.useSpell = false;

		if (w_param == VK_NUMPAD5)
			utils.MB("Debugging messagebox with NUMPAD5");

		break;

	case WM_LBUTTONUP:
		EntityBase* temp = targetSelector.getClickedChampion(GH.getMouseWorldPosition(), 150);
		if (temp)
			myHero.selectedTargetIndex = temp->Index;
		else
			myHero.selectedTargetIndex = -1;
	}

	return CallWindowProcA(hookManager.NewWndProc, hwnd, u_msg, w_param, l_param);
}

BOOL WINAPI HkGetCursorPos(LPPOINT lpPoint) {
	BOOL ret = hookManager.NewGetCursorPos(lpPoint);

	if (inputManager.hookingMousePos) {
		lpPoint->x = inputManager.hookedMousePos.x;
		lpPoint->y = inputManager.hookedMousePos.y;
	}

	return ret;
}