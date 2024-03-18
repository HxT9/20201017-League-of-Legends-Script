// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <TlHelp32.h>
#include <thread>



/*
Inject in a process that interacts with lol (like blitz)

Thread hijack injection
*/
HMODULE thisDll;

DWORD WINAPI Unload(LPVOID lpParam) {
	Sleep(1000);
	FreeLibraryAndExitThread(thisDll, 0);
	Sleep(1000);
}

void do_things(HMODULE hModule) {
	bool injected = false;

	// path to our dll
	LPCSTR DllPath = "E:\\alber\\source\\repos\\20201017-League-of-Legends-Script\\Release\\Blitz.dll";
	LPWSTR ProcessName = (LPWSTR)L"League of Legends.exe";

	PROCESSENTRY32 entry;

	DWORD Pid;
	HANDLE hProcess;
	LPVOID pDllPath;
	HANDLE hLoadThread;

	thisDll = hModule;

	entry.dwSize = sizeof(PROCESSENTRY32);
	while (!injected) {
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (Process32First(snapshot, &entry) == TRUE)
		{
			if (wcscmp(entry.szExeFile, ProcessName) == 0)
			{
				Pid = entry.th32ProcessID;
			}
			while (Process32Next(snapshot, &entry) == TRUE)
			{
				if (wcscmp(entry.szExeFile, ProcessName) == 0)
				{
					Pid = entry.th32ProcessID;
					hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pid);
					if (hProcess) {
						pDllPath = VirtualAllocEx(hProcess, 0, strlen(DllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
						if (pDllPath) {
							if (WriteProcessMemory(hProcess, pDllPath, (LPVOID)DllPath, strlen(DllPath) + 1, 0)) {
								hLoadThread = CreateRemoteThread(hProcess, 0, 0,
									(LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32.dll"),
										"LoadLibraryA"), pDllPath, 0, 0);
								if (hLoadThread) {
									WaitForSingleObject(hLoadThread, INFINITE);
									//MessageBoxA(NULL, "Injected.", NULL, MB_OK);
									VirtualFreeEx(hProcess, pDllPath, strlen(DllPath) + 1, MEM_RELEASE);
									injected = true;
									Sleep(2000);
								}
							}
							else {
								MessageBoxA(NULL, "Can't write dll path to process.", NULL, MB_OK);
							}
						}
						else {
							MessageBoxA(NULL, "Can't allocate memory to process.", NULL, MB_OK);
						}
					}
					else {
						MessageBoxA(NULL, "Can't get handle to process.", NULL, MB_OK);
					}
				}
			}
		}
		CloseHandle(snapshot);
	}

	CreateThread(NULL, 0, Unload, NULL, 0, NULL);
}



BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
		do_things(hModule);
	return TRUE;
}

