#include "HookManager.h"
#include "globalVars.h"
#include <Psapi.h>
#include "MinHook/MinHook.h"
#include "imgui.h"
#include "offsets.h"
#include "HeavensGate.h"
#include "Memory.h"


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
	HMODULE d3dll = NULL;
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

int reapplyProtectionIndex;
LONG WINAPI VehHandler(EXCEPTION_POINTERS* pExceptionInfo)
{
	if (pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION) //We will catch PAGE_GUARD Violation
	{
		if (pExceptionInfo->ContextRecord->Eip == (baseAddress + oOnProcessSpell)) { //Make sure we are at the address we want within the page
			pExceptionInfo->ContextRecord->Eip = (DWORD)&hkOnProcessSpell; //Modify EIP/RIP to where we want to jump to instead of the original function
		}

		pExceptionInfo->ContextRecord->EFlags |= 0x100; //Will trigger an STATUS_SINGLE_STEP exception right after the next instruction get executed to reapply page guard.
		return EXCEPTION_CONTINUE_EXECUTION; //Continue to next instruction
	}

	if (pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP) //We will also catch STATUS_SINGLE_STEP, meaning we just had a PAGE_GUARD violation
	{
		driver.Protect((baseAddress + oOnProcessSpell), 1, PAGE_EXECUTE_READ | PAGE_GUARD);

		return EXCEPTION_CONTINUE_EXECUTION; //Continue the next instruction
	}

	return EXCEPTION_CONTINUE_SEARCH; //Keep going down the exception handling list to find the right handler IF it is not PAGE_GUARD nor SINGLE_STEP
}

HookManager::HookManager() {
	MH_Initialize();
}

int __fastcall hkOnProcessSpell(void* spellBook, void* edx, /*SpellCastInfo* */DWORD spellCastInfo)
{
	utils.MB("OnProcessSpell %lx %lx", spellBook, spellCastInfo);
	return hookManager.NewOnProcessSpell(spellBook, spellCastInfo);
}

void HookManager::Init() {
	OriginalD3DPresent = GetD3DPresent();
	OriginalGetCursorPos = GetCursorPos;

	MH_CreateHook(OriginalD3DPresent, &HkD3DPresent, (LPVOID*)&NewD3DPresent);
	MH_CreateHook(OriginalGetCursorPos, &HkGetCursorPos, (LPVOID*)&NewGetCursorPos);

	MH_EnableHook(OriginalD3DPresent);
	MH_EnableHook(OriginalGetCursorPos);

	NewOnProcessSpell = (Prototype_OnProcessSpell)malloc(0x1000);
	driver.Protect((DWORD)NewOnProcessSpell, 0x1000, PAGE_EXECUTE_READWRITE);
	driver.Memcpy((DWORD)NewOnProcessSpell, (baseAddress + oOnProcessSpell), 0x100); //Occhio a non copiare troppo

	utils.DebugLog("Copied OnProcessSpell to 0x%p", (DWORD)NewOnProcessSpell);

	//Register the Custom Exception Handler
	VehHandle = AddVectoredExceptionHandler(true, (PVECTORED_EXCEPTION_HANDLER)VehHandler);

	utils.MB("Pre OnProcessSpell Hook");
	if (VehHook(baseAddress + oOnProcessSpell, (DWORD)&hkOnProcessSpell, &onProcessSpellOldProtection)) {
		utils.MB("Hooked OnProcessSpell");
	}
	else {
		utils.MB("Error Hooking OnProcessSpell");
	}
	utils.MB("Post OnProcessSpell Hook");

	//PLH::BreakPointHook onPS = PLH::BreakPointHook((char*)(baseAddress + oOnProcessSpell), (char*)&hk_OnProcessSpell);
	//PLH::VEHHook onPS = PLH::VEHHook();
	//onPS.SetupHook((BYTE*)(baseAddress + oOnProcessSpell), (BYTE*)&hk_OnProcessSpell, PLH::VEHHook::VEHMethod::GUARD_PAGE);
	//utils.MB("Set hook %d", onPS.Hook());
	//EnableHeavensGateHook();


	//utils.MB("Pre OnProcessSpell Hook");
	//NewOnProcessSpell = (Prototype_OnProcessSpell)driver.HookFunction((baseAddress + oOnProcessSpell), (UINT64)&hk_OnProcessSpell);
	//utils.MB("Post OnProcessSpell Hook");
	//MH_CreateHook((PVOID)(baseAddress + oOnProcessSpell), &hk_OnProcessSpell, (LPVOID*)&NewOnProcessSpell);
	//MH_EnableHook((PVOID)(baseAddress + oOnProcessSpell));


	HWND hwnd = FindWindowA(0, "League of Legends (TM) Client");
	NewWndProc = WNDPROC(SetWindowLongA(hwnd, GWL_WNDPROC, (LONG)HkWndProc));
}

void HookManager::Dispose() {
	MH_DisableHook(MH_ALL_HOOKS);

	DisableHeavensGateHook();

	VehUnHook(baseAddress + oOnProcessSpell, onProcessSpellOldProtection);

	RemoveVectoredExceptionHandler(VehHandle);

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

bool HookManager::VehHook(DWORD Fun, DWORD hkFun, DWORD* oldProtection)
{
	//We cannot hook two functions in the same page, because we will cause an infinite callback
	if (Memory::AreInSamePage(Fun, hkFun))
		return false;

	//Toggle PAGE_GUARD flag on the page
	*oldProtection = driver.Protect(Fun, 1, PAGE_EXECUTE_READ | PAGE_GUARD);

	return true;
}

bool HookManager::VehUnHook(DWORD Fun, DWORD OldProtection) {
	DWORD oldProtection;
	VirtualProtect((LPVOID)Fun, 1, OldProtection, &oldProtection);

	return true;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param);
LRESULT WINAPI HkWndProc(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param) {
	EntityBase* temp;

	if (gui.ShowLog) {
		ImGui_ImplWin32_WndProcHandler(hwnd, u_msg, w_param, l_param);
	}

	if (gui.ShowMain) {
		if (u_msg == WM_KEYUP && (w_param == VK_END || w_param == VK_ESCAPE)){
			gui.ShowMain = false;
			CallWindowProcA(hookManager.NewWndProc, hwnd, WM_ACTIVATE, WA_ACTIVE, 0);
			return true;
		}
		
		if (debugging) {
			if (ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard) {
				CallWindowProcA(hookManager.NewWndProc, hwnd, WM_ACTIVATE, WA_INACTIVE, 0);
				if (ImGui_ImplWin32_WndProcHandler(hwnd, u_msg, w_param, l_param))
					return true;
			}
			else {
				CallWindowProcA(hookManager.NewWndProc, hwnd, WM_ACTIVATE, WA_ACTIVE, 0);
			}
		}
		else {
			if (ImGui_ImplWin32_WndProcHandler(hwnd, u_msg, w_param, l_param))
				return true;
		}
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
		if (w_param == VK_SHIFT) {
			gui.ShowLog = false;
		}

		if (w_param == VK_END) {
			if (!gui.ShowMain) {
				gui.ShowMain = true;
				CallWindowProcA(hookManager.NewWndProc, hwnd, WM_ACTIVATE, WA_INACTIVE, 0);
			}
		}

		if (w_param == VK_NUMPAD1)
			myHero.useSpell = true;

		if (w_param == VK_NUMPAD2)
			myHero.useSpell = false;

		if (w_param == VK_NUMPAD5)
			debugging != debugging;

		if (w_param == VK_NUMPAD9)
			if (GetKeyState(VK_NUMPAD8) & 0x8000)
				unloaded = true;			

		break;

	case WM_LBUTTONUP:
		temp = targetSelector.getClickedChampion(GH.getMouseWorldPosition(), 150);
		if (temp)
			myHero.selectedTargetIndex = temp->Index;
		else
			myHero.selectedTargetIndex = -1;
		break;
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