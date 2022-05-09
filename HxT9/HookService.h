/*#pragma once
#include <Windows.h>
#include <vector>

std::vector<DWORD> Targets;
std::vector<DWORD> HookedFunctions;
std::vector<DWORD> SkipBytes;

void Init();
void AddHook(DWORD Target, DWORD HkFunction, DWORD SkipBytes);
void RemoveHook(DWORD Target);
LONG ExceptionHandler(EXCEPTION_POINTERS* pExceptionInfo);*/