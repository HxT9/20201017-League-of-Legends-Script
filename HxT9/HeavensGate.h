#pragma once
#include <Windows.h>

DWORD_PTR GetGateAddress();

void hk_NtProtectVirtualMemory();

NTSTATUS hk_NtTerminateProcess(HANDLE ProcessHandle, NTSTATUS ExitStatus);

void hk_Wow64Trampoline();

LPVOID CreateNewJump(DWORD Gate);

void WriteJump(DWORD dwWow64Address, void* pBuffer, size_t ulSize);

void EnableHeavensGateHook();

void DisableHeavensGateHook();