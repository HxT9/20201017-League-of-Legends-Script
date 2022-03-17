#include <Windows.h>

#include "globalVars.h"
#include "Detour.h"
#include "utilities.h"
#include "functionDefinitions.h"
#include ".\libs\MinHook.h"

#pragma comment(lib, "nod3dx9.lib")

HRESULT WINAPI myPresent(LPDIRECT3DDEVICE9 pDevice, CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*);

PVOID pTarget;

DWORD FindD3D9VTable(DWORD Len)
{
    DWORD dwObjBase = 0;
    dwObjBase = (DWORD)LoadLibrary(TEXT("d3d9.dll"));
    while (dwObjBase++ < dwObjBase + Len)
    {
        if ((*(WORD*)(dwObjBase + 0x00)) == 0x06C7
            && (*(WORD*)(dwObjBase + 0x06)) == 0x8689
            && (*(WORD*)(dwObjBase + 0x0C)) == 0x8689
            ) {
            dwObjBase += 2; break;
        }
    }
    return(dwObjBase);
}
DWORD GetD3D9VTableFunction(int VTableIndex)
{
    PDWORD VTable;
    *(DWORD*)&VTable = *(DWORD*)FindD3D9VTable(0x128000);
    return VTable[VTableIndex];
}

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

        if (adr == NULL) {
            MessageBoxA(NULL, "d3d9 not found", "Error", MB_OK);
            return FALSE;
        }

        memcpy(&VTable, (PVOID)adr, 4);

        //originalPresent = (Prototype_Present)DetourFunction((PBYTE)VTable[17], (PBYTE)myPresent, 5);
        pTarget = (PVOID)VTable[17];

        MH_Initialize();
        MH_CreateHook(pTarget, &myPresent, (LPVOID*)&originalPresent);
        MH_EnableHook(pTarget);

        break;

    case DLL_PROCESS_DETACH:
        MH_DisableHook(pTarget);
        break;
    }
    return TRUE;
}

HRESULT WINAPI myPresent(LPDIRECT3DDEVICE9 pDevice, CONST RECT* pSrcRect, CONST RECT* pDstRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion) {
    __try{
        ticker.tick(pDevice);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
    
    return originalPresent(pDevice, pSrcRect, pDstRect, hDestWindow, pDirtyRegion);
}