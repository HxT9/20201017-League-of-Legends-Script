#pragma once
#include <windows.h>

namespace Memory
{
	bool AreInSamePage(DWORD addr1, DWORD addr2);
}