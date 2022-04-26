#include "HxT9.h"
#include <Windows.h>
#include "globalVars.h"
#include "HookManager.h"
#pragma comment(lib, "nod3dx9.lib")

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

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        thisDll = hModule;

        baseAddress = (DWORD)GetModuleHandle(NULL);

        do {
            Sleep(10);
        } while (!GetModuleHandle(L"d3d9.dll"));

        hookManager.Init();

        break;

    case DLL_PROCESS_DETACH:
        unloaded = true;
        break;
    }
    return TRUE;
}