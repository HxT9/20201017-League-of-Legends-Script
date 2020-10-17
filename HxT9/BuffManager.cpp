#include "BuffManager.h"
#include <Windows.h>

int BuffManager::BuffCount() {
	DWORD FirstBuff = *(DWORD*)((DWORD)this + 0x10);
	DWORD LastBuff = *(DWORD*)((DWORD)this + 0x14);

	return (LastBuff - FirstBuff) / 0x8;
}

BuffInfo* BuffManager::getBuff(int index) {
	DWORD FirstBuff = *(DWORD*)((DWORD)this + 0x10);
	return *(BuffInfo**)(FirstBuff + 0x8 * index);
}