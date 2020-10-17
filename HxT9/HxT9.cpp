#include <Windows.h>

#include "globalVars.h"
#include "Detour.h"
#include "utilities.h"
#include "functionDefinitions.h"

HRESULT WINAPI myPresent(LPDIRECT3DDEVICE9 pDevice, CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*);

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    HMODULE d3dll;
    DWORD adr;
    PDWORD VTable;

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:

        thisDll = hModule;

        baseAddress = (DWORD)GetModuleHandle(NULL);

        do {
            d3dll = GetModuleHandle(L"d3d9.dll");
            Sleep(10);
        } while (!d3dll);

        adr = findPattern("d3d9.dll", "C7 06 ? ? ? ? 89 86 ? ? ? ? 89 86") + 2;

        if (adr == NULL)
            return FALSE;

        memcpy(&VTable, (PVOID)adr, 4);

        originalPresent = (Prototype_Present)DetourFunction((PBYTE)VTable[17], (PBYTE)myPresent, 5);

    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

HRESULT WINAPI myPresent(LPDIRECT3DDEVICE9 pDevice, CONST RECT* pSrcRect, CONST RECT* pDstRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion) {
    ticker.tick(pDevice);
    return originalPresent(pDevice, pSrcRect, pDstRect, hDestWindow, pDirtyRegion);
}