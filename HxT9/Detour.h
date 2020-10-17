#pragma once
#include <Windows.h>
#include <Psapi.h>

void* DetourFunction(BYTE* src, const BYTE* dst, const int len);
