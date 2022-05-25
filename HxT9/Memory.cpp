#include "Memory.h"

bool Memory::AreInSamePage(DWORD Addr1, DWORD Addr2)
{
    MEMORY_BASIC_INFORMATION mbi1;
    if (!VirtualQuery((PVOID)Addr1, &mbi1, sizeof(mbi1))) //Get Page information for Addr1
        return true;

    MEMORY_BASIC_INFORMATION mbi2;
    if (!VirtualQuery((PVOID)Addr2, &mbi2, sizeof(mbi2))) //Get Page information for Addr1
        return true;

    if (mbi1.BaseAddress == mbi2.BaseAddress) //See if the two pages start at the same Base Address
        return true; //Both addresses are in the same page, abort hooking!

    return false;
}