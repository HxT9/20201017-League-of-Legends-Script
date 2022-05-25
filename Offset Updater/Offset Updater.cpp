#include <windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <vector>
#include <fstream>

HANDLE hProcess;
DWORD PageSize;

template <typename T>
T Read(DWORD Address)
{
	T Data;
	ReadProcessMemory(hProcess, (LPVOID)Address, &Data, sizeof(T), nullptr);
	return Data;
}

enum class SearchType {
	All,
	Text,
	Data
};

DWORD GetProcessIdByName(const LPCTSTR ProcessName)
{
	HANDLE hSnapshot = INVALID_HANDLE_VALUE;
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe32{ 0 };
	DWORD ret = 0;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	for (BOOL res = Process32First(hSnapshot, &pe32); res; res = Process32Next(hSnapshot, &pe32))
	{
		if (!wcscmp(pe32.szExeFile, ProcessName))
			return pe32.th32ProcessID;
	}

	return 0;
}

MODULEENTRY32W GetRemoteModuleHandle(unsigned int PID, const TCHAR* szModule)
{
	HANDLE hSnapshot = INVALID_HANDLE_VALUE;
	MODULEENTRY32 md32{ 0 };
	md32.dwSize = sizeof(MODULEENTRY32);
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);
	MODULEENTRY32W ret;

	for (BOOL res = Module32First(hSnapshot, &md32); res; res = Module32Next(hSnapshot, &md32))
	{
		if (!wcscmp(szModule, md32.szModule))
			ret = md32;
	}

	return ret;
}

DWORD FindPattern(BYTE* base, DWORD size, const char* pattern, const char* mask, DWORD startOffset = 0) {
	size_t patternLength = strlen(mask);

	for (DWORD i = startOffset; i < size - patternLength; i++)
	{
		bool found = true;
		for (DWORD j = 0; j < patternLength; j++)
		{
			if (mask[j] != '?' && pattern[j] != *(char*)(base + i + j))
			{
				found = false;
				break;
			}
		}

		if (found)
		{
			return i;
		}
	}
	return 0;
}

DWORD FindPatternEx(MODULEENTRY32W hModule, const char* pattern, const char* mask, SearchType searchType = SearchType::All, DWORD startAddr = 0, DWORD endAddr = 0) {
	DWORD moduleStart = 0, moduleEnd = 0, curPos, ret;
	IMAGE_NT_HEADERS NTHeaders;
	MEMORY_BASIC_INFORMATION mbi;
	BYTE* buffer = NULL;
	DWORD oldProtection = 0;

	NTHeaders = Read<IMAGE_NT_HEADERS>((DWORD)hModule.modBaseAddr + Read<IMAGE_DOS_HEADER>((DWORD)hModule.modBaseAddr).e_lfanew);

	if (!startAddr){
		moduleStart = (DWORD)hModule.modBaseAddr;
		moduleEnd = moduleStart + hModule.modBaseSize;

		switch (searchType) {
		case SearchType::Text:
			moduleStart = moduleStart + NTHeaders.OptionalHeader.BaseOfCode;
			moduleEnd = moduleStart + NTHeaders.OptionalHeader.SizeOfCode;
			break;
		case SearchType::Data:
			moduleStart = moduleStart + NTHeaders.OptionalHeader.BaseOfData;
			moduleEnd = (DWORD)hModule.modBaseAddr + hModule.modBaseSize;
			break;
		}
	}
	else {
		moduleStart = startAddr;

		if (endAddr) {
			moduleEnd = endAddr;
		}
		else {
			moduleEnd = (DWORD)hModule.modBaseAddr + hModule.modBaseSize;
		}
	}

	ret = 0;

	curPos = moduleStart;
	while (curPos < moduleEnd) {
		VirtualQueryEx(hProcess, (void*)curPos, &mbi, moduleEnd - moduleStart);
		buffer = new BYTE[mbi.RegionSize];

		VirtualProtectEx(hProcess, mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &oldProtection);

		ReadProcessMemory(hProcess, mbi.BaseAddress, buffer, mbi.RegionSize, 0);

		VirtualProtectEx(hProcess, mbi.BaseAddress, mbi.RegionSize, oldProtection, 0);

		ret = FindPattern(buffer, mbi.RegionSize, pattern, mask, max((INT)(startAddr - (DWORD)mbi.BaseAddress), 0));

		delete[] buffer;

		if (ret) {
			ret = ret + (DWORD)mbi.BaseAddress;// -(DWORD)hModule.modBaseAddr;
			break;
		}

		curPos += mbi.RegionSize;
	}

	return ret;
}

DWORD getE8Address(DWORD callAddress) {
	LONG sValue = Read<LONG>(callAddress + 1);
	return callAddress + sValue + 5;
}

std::string vformat(const char* fmt, va_list ap)
{
	// Allocate a buffer on the stack that's big enough for us almost
	// all the time.
	size_t size = 2048;
	char buf[2048];

	// Try to vsnprintf into our buffer.
	va_list apcopy;
	va_copy(apcopy, ap);
	int needed = vsnprintf(&buf[0], size, fmt, ap);
	// NB. On Windows, vsnprintf returns -1 if the string didn't fit the
	// buffer.  On Linux & OSX, it returns the length it would have needed.

	if (needed <= size && needed >= 0) {
		// It fit fine the first time, we're done.
		return std::string(&buf[0]);
	}
	else {
		// vsnprintf reported that it wanted to write more characters
		// than we allotted.  So do a malloc of the right size and try again.
		// This doesn't happen very often if we chose our initial size
		// well.
		std::vector <char> buf;
		size = needed;
		buf.resize(size);
		needed = vsnprintf(&buf[0], size, fmt, apcopy);
		return std::string(&buf[0]);
	}
}

std::string stringf(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	std::string buf = vformat(fmt, ap);
	va_end(ap);
	return buf;
}

int main()
{
	MODULEENTRY32W hModule;
	DWORD Pid;
	LPWSTR processName = (LPWSTR)L"League of Legends.exe";
	DWORD tempAddr = 0;

	//Globals
	DWORD oObjectsManager = 0;
	DWORD oLocalPlayer = 0;	
	DWORD oChatClientBox = 0;
	DWORD oChatHud = 0;
	DWORD oGameTime = 0;
	DWORD oMinimapObj = 0;
	DWORD oHudInstance = 0;

	//Functions
	DWORD oGetAIManager = 0;
	DWORD oGetBoundingRadius = 0;
	DWORD oIsHero = 0;
	DWORD oIsMinion = 0;
	DWORD oIsTurret = 0;
	DWORD oIsMissile = 0;
	DWORD oIsTroy = 0;
	DWORD oIsAlive = 0;
	DWORD oGetSpellState = 0;
	DWORD oGetAttackCastDelay = 0;
	DWORD oGetAttackDelay = 0;
	DWORD oW2S = 0;
	DWORD oIssueClickNew = 0;
	DWORD oOnProcessSpell = 0;

	//Offsets
	DWORD oChatHudChatOpened = 0;
	DWORD oMinimapObjectHud = 0;
	DWORD oMinimapPosition = 0;
	DWORD oMinimapSize = 0;

	DWORD oObjIndex = 0x20;
	DWORD oObjTeam = 0x4c;
	DWORD oObjName = 0x6c;
	DWORD oObjNetworkID = 0xcc;
	DWORD oObjPos = 0x1f4;
	DWORD oObjVisibility = 0x28c;
	DWORD oObjMana = 0x2b4;
	DWORD oObjMaxManaoObjMana = oObjMana + 0x10;
	DWORD oObjSar = 0;
	DWORD oObjMaxSar = 0;
	DWORD oObjSarEnabled = 0;
	DWORD oObjTargetable = 0;
	DWORD oObjHP = 0;
	DWORD oObjMaxHP = 0;
	DWORD oObjHPMaxPenalty = 0;
	DWORD oObjAllShield = 0;
	DWORD oObjPhysicalShield = 0;
	DWORD oObjMagicalShield = 0;
	DWORD oObjStats = 0;
	DWORD oObjBonusAtk = 0;
	DWORD oObjAP = 0;
	DWORD oObjBaseAtk = 0;
	DWORD oObjArmor = 0;
	DWORD oObjMagicResist = 0;
	DWORD oObjMoveSpeed = 0;
	DWORD oObjAtkRange = 0;
	DWORD oObjBuffMgr = 0;
	DWORD oObjSpellBook = 0;
	DWORD oObjObjectName = 0;
	DWORD oObjLevel = 0;

	DWORD oSpellBookActiveSpell = 0x20;
	DWORD oActiveSpellSpellInfoPtr = 0x8;
	DWORD oActiveSpellEndCastTime = 0x10;
	DWORD oActiveSpellStartPos = 0x84;
	DWORD oActiveSpellEndPos = oActiveSpellStartPos + 0xC;
	DWORD oActiveSpellTargetArray = 0xC0;
	DWORD oActiveSpellTargetArraySize = oActiveSpellTargetArray + 0x4;
	DWORD oActiveSpellCastTime = 0xCC;
	DWORD oActiveSpellChannelStartTime = 0x13C;
	DWORD oActiveSpellChannelEndTime = oActiveSpellChannelStartTime + 0x4;

	DWORD oSpellBookSpellSlots = 0;
	DWORD oSpellSlotSpellLvl = 0;
	DWORD oSpellSlotSpellReady = 0;
	DWORD oSpellSlotSpellInfo = 0;

	DWORD oSpellInfoSpellData = 0;
	DWORD oSpellDataName = 0;
	DWORD oSpellDataSpellSpeed = 0x460;

	DWORD oAIMGR_TargetPos = 0;
	DWORD oAIMGR_IsMoving = 0;
	DWORD oAIMGR_PassedWaypoints = 0;
	DWORD oAIMGR_NavBegin = 0;
	DWORD oAIMGR_NavEnd = 0;
	DWORD oAIMGR_IsDashing = 0;
	DWORD oAIMGR_CurrentPos = 0;
	DWORD oAIMGR_Velocity = 0;

	DWORD oMissileSpellInfo = 0x278;
	DWORD oMissileSourceIndex = 0x2dc;
	DWORD oMissileStartPos = 0x2f4;
	DWORD oMissileEndPos = oMissileStartPos + 0xc;
	DWORD oMissileTargetIndex = 0x330;
	DWORD oMissileHasTarget = oMissileTargetIndex + 0x4;

	DWORD oBuffType = 0x4;
	DWORD oBuffName = 0x8;
	DWORD oBuffNameOffset = 0x4;
	DWORD oBuffStartTime = 0x10;
	DWORD oBuffEndTime = 0x14;
	DWORD oBuffCountAlt = 0;
	DWORD oBuffIntCount = 0;
	DWORD oBuffFloatCount = 0x130;

	Pid = GetProcessIdByName(processName);

	if (Pid) {
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, Pid);
		hModule = GetRemoteModuleHandle(Pid, processName);

		tempAddr = FindPatternEx(hModule, "\x8B\x0D\x00\x00\x00\x00\x56\xE8\x00\x00\x00\x00\x85\xC0\x74", "xx????xx????xxx", SearchType::Text);
		if(tempAddr)
			oObjectsManager = Read<DWORD>(tempAddr + 2) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, "\x8B\x3D\x00\x00\x00\x00\x3B\xF7\x75\x09", "xx????xxxx", SearchType::Text);
		if (tempAddr)
			oLocalPlayer = Read<DWORD>(tempAddr + 2) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, "\x8B\x0D\x00\x00\x00\x00\x85\xC9\x74\x0D\x8B\x41", "xx????xxxxxx", SearchType::Text);
		if (tempAddr)
			oChatClientBox = Read<DWORD>(tempAddr + 2) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, "\xA3\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\xC8", "x????x????x????xx", SearchType::Text);
		if (tempAddr)
			oChatHud = Read<DWORD>(tempAddr + 1) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, "\xF3\x0F\x5C\x0D\x00\x00\x00\x00\x0F\x2F\xC1\xF3", "xxxx????xxxx", SearchType::Text);
		if (tempAddr)
			oGameTime = Read<DWORD>(tempAddr + 4) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, "\x8B\x0D\x00\x00\x00\x00\x66\x0F\x6E\xC6", "xx????xxxx", SearchType::Text);
		if (tempAddr)
			oMinimapObj = Read<DWORD>(tempAddr + 2) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, "\x8B\x0D\x00\x00\x00\x00\xFF\x70\xFC\x8B\x49\x0C\xE8\x00\x00\x00\x00\xC6", "xx????xxxxxxx????x", SearchType::Text);
		if (tempAddr)
			oHudInstance = Read<DWORD>(tempAddr + 2) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, "\xE8\x00\x00\x00\x00\x6A\x00\x6A\x01\xFF\x74\x24\x14", "x????xxxxxxxx", SearchType::Text);
		if (tempAddr)
			oGetAIManager = getE8Address(tempAddr) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, "\xE8\x00\x00\x00\x00\xD8\x44\x24\x04\x8B\x44\x24\x10", "x????xxxxxxxx", SearchType::Text);
		if (tempAddr)
			oGetBoundingRadius = getE8Address(tempAddr) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, "\xE8\x00\x00\x00\x00\x83\xC4\x04\x84\xC0\x74\x61\x85", "x????xxxxxxxx", SearchType::Text);
		if (tempAddr)
			oIsHero = getE8Address(tempAddr) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, "\xE8\x00\x00\x00\x00\x83\xC4\x04\xB9\x00\x00\x00\x00\x84\xC0\x0F\x44\xCB", "x????xxxx????xxxxx", SearchType::Text);
		if (tempAddr)
			oIsMinion = getE8Address(tempAddr) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, "\xE8\x00\x00\x00\x00\x83\xC4\x04\x84\xC0\x74\x09\x5F", "x????xxxxxxxx", SearchType::Text);
		if (tempAddr)
			oIsTurret = getE8Address(tempAddr) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, "\xE8\x00\x00\x00\x00\x83\xC4\x04\x84\xC0\x74\x3F", "x????xxxxxxx", SearchType::Text);
		if (tempAddr)
			oIsMissile = getE8Address(tempAddr) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, "\xE8\x00\x00\x00\x00\x8B\xE8\x83\xC4\x04\x85\xED\x0F\x84\x00\x00\x00\x00\x6A\x08", "x????xxxxxxxxx????xx", SearchType::Text);
		if (tempAddr)
			oIsTroy = getE8Address(tempAddr) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, "\x56\x8B\xF1\x8B\x06\x8B\x80\x00\x00\x00\x00\xFF\xD0\x84\xC0\x74\x19", "xxxxxxx????xxxxxx", SearchType::Text);
		if (tempAddr)
			oIsAlive = tempAddr - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, "\x83\xEC\x14\x8B\x44\x24\x1C\x55", "xxxxxxxx", SearchType::Text);
		if (tempAddr)
			oGetSpellState = tempAddr - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, "\x83\xEC\x0C\x53\x8B\x5C\x24\x14\x8B\xCB\x56\x57\x8B\x03\xFF\x90", "xxxxxxxxxxxxxxxx", SearchType::Text);
		if (tempAddr)
			oGetAttackCastDelay = tempAddr - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, "\x8B\x44\x24\x04\x51\xF3", "xxxxxx", SearchType::Text);
		if (tempAddr)
			oGetAttackDelay = tempAddr - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, "\x83\xEC\x10\x56\xE8\x00\x00\x00\x00\x8B\x08", "xxxxx????xx", SearchType::Text);
		if (tempAddr)
			oW2S = tempAddr - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, "\x8B\x44\x24\x04\x6A\x01\xFF\x70\x40", "xxxxxxxxx", SearchType::Text);
		if (tempAddr) {
			tempAddr = FindPatternEx(hModule, "\xE8\x00\x00\x00\x00\xC2\x04", "x????xx", SearchType::Text, tempAddr, tempAddr + 0x20);
			if (tempAddr) {
				oIssueClickNew = getE8Address(tempAddr) - (DWORD)hModule.modBaseAddr;
			}
		}

		tempAddr = FindPatternEx(hModule, "\x8B\x44\x24\x04\x6A\x01\xFF\x70\x40", "xxxxxxxxx", SearchType::Text);
		if (tempAddr) {
			tempAddr = FindPatternEx(hModule, "\xE8\x00\x00\x00\x00\xC2\x04", "x????xx", SearchType::Text, tempAddr, tempAddr + 0x20);
			if (tempAddr) {
				oIssueClickNew = getE8Address(tempAddr) - (DWORD)hModule.modBaseAddr;
			}
		}

		tempAddr = FindPatternEx(hModule, "\xE8\x00\x00\x00\x00\x85\xC0\x0F\x94\x44\x24\x00\x85\xC0", "x????xxxxxx?xx", SearchType::Text);
		if (tempAddr) {
			oOnProcessSpell = getE8Address(tempAddr) - (DWORD)hModule.modBaseAddr;
		}

		tempAddr = FindPatternEx(hModule, "\xE8\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x80\xB8\x00\x00\x00\x00\x00\x0F\x85\x00\x00\x00\x00", "x????x????xx?????xx????", SearchType::Text);
		if (tempAddr) {
			tempAddr = getE8Address(tempAddr);
			tempAddr = FindPatternEx(hModule, "\x8A\x46\x00\x3A\xD8", "xx?xx", SearchType::Text, tempAddr, tempAddr + 0x100);
			if (tempAddr) {
				oChatHudChatOpened = Read<BYTE>(tempAddr + 2);
			}
		}

		tempAddr = FindPatternEx(hModule, "\xF3\x0F\x58\xC8\xF3\x0F\x00\x00\x00\xFF\x52\x00\x8B\x8E\x00\x00\x00\x00", "xxxxxx???xx?xx????", SearchType::Text);
		if (tempAddr) {
			oMinimapObjectHud = Read<DWORD>(tempAddr + 14);
		}

		tempAddr = FindPatternEx(hModule, "\x0F\x5B\xC9\xF3\x0F\x11\x56\x00", "xxxxxxx?", SearchType::Text);
		if (tempAddr) {
			oMinimapPosition = Read<BYTE>(tempAddr + 7);
		}

		tempAddr = FindPatternEx(hModule, "\xF3\x0F\x2C\xC0\xF3\x0F\x11\x4E\x00", "xxxxxxxx?", SearchType::Text);
		if (tempAddr) {
			oMinimapSize = Read<BYTE>(tempAddr + 8);
		}

		tempAddr = FindPatternEx(hModule, "\x8D\xBB\x00\x00\x00\x00\x89\xB3\x00\x00\x00\x00", "xx????xx????", SearchType::Text);
		if (tempAddr) {
			oObjSar = Read<DWORD>(tempAddr + 2);
			oObjMaxSar = oObjSar + 0x10;
			oObjSarEnabled = oObjMaxSar + 0x10;
		}

		tempAddr = FindPatternEx(hModule, "\x89\x44\x24\x14\x8D\xB0\x00\x00\x00\x00", "xxxxxx????", SearchType::Text);
		if (tempAddr) {
			oObjTargetable = Read<DWORD>(tempAddr + 6);
		}

		tempAddr = FindPatternEx(hModule, "\x81\xC1\x00\x00\x00\x00\x89\x47\x04", "xx????xxx", SearchType::Text);
		if (tempAddr) {
			oObjStats = Read<DWORD>(tempAddr + 2);
		}

		tempAddr = FindPatternEx(hModule, "\x0F\x28\xDC\xF3\x0F\x59\x99\x00\x00\x00\x00", "xxxxxxx????", SearchType::Text);
		if (tempAddr) {
			oObjBonusAtk = Read<DWORD>(tempAddr + 7) + oObjStats;
		}

		tempAddr = FindPatternEx(hModule, "\xD8\x81\x00\x00\x00\x00\xD9\x81\x00\x00\x00\x00", "xx????xx????", SearchType::Text);
		if (tempAddr) {
			oObjAP = Read<DWORD>(tempAddr + 2) + oObjStats;
		}

		tempAddr = FindPatternEx(hModule, "\xF3\x0F\x10\x91\x00\x00\x00\x00\x0F\x28\xDC", "xxxx????xxx", SearchType::Text);
		if (tempAddr) {
			oObjBaseAtk = Read<DWORD>(tempAddr + 4) + oObjStats;
		}

		tempAddr = FindPatternEx(hModule, "\x51\x56\x57\x8B\xF9\x8B\x4F\x04\x8B\x01\xFF\x90\x00\x00\x00\x00\x8B\x4F\x04\x8B\xF0\xE8\x00\x00\x00\x00\x50\x6A\x03", "xxxxxxxxxxxx????xxxxxx????xxx", SearchType::Text);
		if (tempAddr) {
			tempAddr = FindPatternEx(hModule, "\x8B\x81\x00\x00\x00\x00", "xx????", SearchType::Text, tempAddr, tempAddr + 0x50);
			if (tempAddr) {
				oObjArmor = Read<DWORD>(tempAddr + 2);
				oObjMagicResist = oObjArmor + 0x8;
			}
		}

		tempAddr = FindPatternEx(hModule, "\x8B\x81\x00\x00\x00\x00\x89\x47\x0C\x8B\x01", "xx????xxxxx", SearchType::Text);
		if (tempAddr) {
			oObjMoveSpeed = Read<DWORD>(tempAddr + 2);
		}

		tempAddr = FindPatternEx(hModule, "\xE8\x00\x00\x00\x00\x80\x7B\x54\x00", "x????xxxx", SearchType::Text);
		if (tempAddr) {
			tempAddr = getE8Address(tempAddr);
			tempAddr = FindPatternEx(hModule, "\xC3\xD9\x83\x00\x00\x00\x00", "xxx????", SearchType::Text);
			if (tempAddr) {
				oObjAtkRange = Read<DWORD>(tempAddr + 3);
			}
		}

		tempAddr = FindPatternEx(hModule, "\x68\x00\x00\x00\x00\x8D\xB0\x00\x00\x00\x00\x89\x44\x24\x14", "x????xx????xxxx", SearchType::Text);
		if (tempAddr) {
			oObjHP = Read<DWORD>(tempAddr + 7);
			oObjMaxHP = oObjHP + 0x10;
			oObjHPMaxPenalty = oObjHP + 0x20;
			oObjAllShield = oObjHP + 0x40;
			oObjPhysicalShield = oObjHP + 0x50;
			oObjMagicalShield = oObjHP + 0x60;
		}

		tempAddr = FindPatternEx(hModule, "\xFF\x74\x24\x1C\x8D\x8F\x00\x00\x00\x00", "xxxxxx????", SearchType::Text);
		if (tempAddr) {
			oObjSpellBook = Read<DWORD>(tempAddr + 6);
		}

		tempAddr = FindPatternEx(hModule, "\x8D\x8B\x00\x00\x00\x00\xFF\x30\xFF\x73\x20", "xx????xxxxx", SearchType::Text);
		if (tempAddr) {
			oObjBuffMgr = Read<DWORD>(tempAddr + 2);
		}

		tempAddr = FindPatternEx(hModule, "\xC6\x83\x00\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8D\x8B\x00\x00\x00\x00", "xx?????x????xx????", SearchType::Text);
		if (tempAddr) {
			oObjObjectName = Read<DWORD>(tempAddr + 2);
		}

		tempAddr = FindPatternEx(hModule, "\x8D\xBB\x00\x00\x00\x00\x83\xC4\x14", "xx????xxx", SearchType::Text);
		if (tempAddr) {
			oObjLevel = Read<DWORD>(tempAddr + 2);
		}

		tempAddr = FindPatternEx(hModule, "\x8B\x38\xE8\x00\x00\x00\x00\x8B\xF0", "xxx????xx", SearchType::Text);
		if (tempAddr) {
			tempAddr = getE8Address(tempAddr + 2);
			tempAddr = FindPatternEx(hModule, "\x8B\x84\x81\x00\x00\x00\x00", "xxx????", SearchType::Text, tempAddr);
			if (tempAddr) {
				oSpellBookSpellSlots = Read<DWORD>(tempAddr + 3);
			}
		}

		tempAddr = FindPatternEx(hModule, "\xE8\x00\x00\x00\x00\x3B\x47\x08", "x????xxx", SearchType::Text);
		if (tempAddr) {
			tempAddr = getE8Address(tempAddr);
			if (tempAddr) {
				oSpellSlotSpellLvl = Read<BYTE>(tempAddr + 2);
			}
		}

		tempAddr = FindPatternEx(hModule, "\xF3\x0F\x10\x4E\x00\xD9\x5C\x24\x0C", "xxxx?xxxx", SearchType::Text);
		if (tempAddr) {
			oSpellSlotSpellReady = Read<BYTE>(tempAddr + 4);
		}

		tempAddr = FindPatternEx(hModule, "\xE8\x00\x00\x00\x00\x8B\x50\x44", "x????xxx", SearchType::Text);
		if (tempAddr) {
			tempAddr = getE8Address(tempAddr);
			if (tempAddr) {
				oSpellSlotSpellInfo = Read<DWORD>(tempAddr + 2);
			}
		}

		tempAddr = FindPatternEx(hModule, "\xF3\x0F\x10\x54\x24\x00\x8B\x48\x00", "xxxxx?xx?", SearchType::Text);
		if (tempAddr) {
			oSpellInfoSpellData = Read<BYTE>(tempAddr + 8);
		}

		tempAddr = FindPatternEx(hModule, "\x8B\x44\x24\x08\x56\x57\x8B\xF9\x8D\x77\x04", "xxxxxxxxxxx", SearchType::Text);
		if (tempAddr) {
			tempAddr = FindPatternEx(hModule, "\xFF\x70\x00", "xx?", SearchType::Text, tempAddr);
			if (tempAddr) {
				oSpellDataName = Read<BYTE>(tempAddr + 2);
			}
		}

		tempAddr = FindPatternEx(hModule, "\x89\x4F\x00\x8B\x88\x00\x00\x00\x00", "xx?xx????", SearchType::Text);
		if (tempAddr) {
			oAIMGR_TargetPos = Read<BYTE>(tempAddr + 2);
		}

		tempAddr = FindPatternEx(hModule, "\x8A\x98\x00\x00\x00\x00\x8B\x06\xFF\x90\x00\x00\x00\x00\x8B\xCE", "xx????xxxx????xx", SearchType::Text);
		if (tempAddr) {
			oAIMGR_IsMoving = Read<DWORD>(tempAddr + 2);
		}

		tempAddr = FindPatternEx(hModule, "\x8D\x9E\x00\x00\x00\x00\xC6\x86\x00\x00\x00\x00\x00", "xx????xx?????", SearchType::Text);
		if (tempAddr) {
			oAIMGR_PassedWaypoints = Read<DWORD>(tempAddr + 2);
		}

		tempAddr = FindPatternEx(hModule, "\x2B\x88\x00\x00\x00\x00\xB8\x00\x00\x00\x00\xF7\xE9\xD1\xFA", "xx????x????xxxx", SearchType::Text);
		if (tempAddr) {
			oAIMGR_NavBegin = Read<DWORD>(tempAddr + 2);
			oAIMGR_NavEnd = oAIMGR_NavBegin + 0x4;
		}

		tempAddr = FindPatternEx(hModule, "\x88\x87\x00\x00\x00\x00\x8B\x44\x24\x50", "xx????xxxx", SearchType::Text);
		if (tempAddr) {
			oAIMGR_IsDashing = Read<DWORD>(tempAddr + 2);
		}

		tempAddr = FindPatternEx(hModule, "\x8B\xCB\x89\x86\x00\x00\x00\x00", "xxxx????", SearchType::Text);
		if (tempAddr) {
			oAIMGR_CurrentPos = Read<DWORD>(tempAddr + 4);
		}

		tempAddr = FindPatternEx(hModule, "\xF3\x0F\x59\xC1\xF3\x0F\x11\x96\x00\x00\x00\x00", "xxxxxxxx????", SearchType::Text);
		if (tempAddr) {
			oAIMGR_Velocity = Read<DWORD>(tempAddr + 8);
		}

		tempAddr = FindPatternEx(hModule, "\xE8\x00\x00\x00\x00\x84\xC0\x74\x2D\x8B\x0E", "x????xxxxxx", SearchType::Text);
		if (tempAddr) {
			tempAddr = getE8Address(tempAddr);
			if (tempAddr) {
				oBuffCountAlt = Read<SHORT>(tempAddr + 2);
			}
		}

		tempAddr = FindPatternEx(hModule, "\x7D\x03\x8B\x41\x74\x00", "xxxxx?", SearchType::Text);
		if (tempAddr) {
			oBuffIntCount = Read<BYTE>(tempAddr + 4);
		}

		std::fstream offsetStream;
		offsetStream.open("E:\\Downloads\\Cheats\\Lol\\OffsetsUpdater.txt", std::fstream::out);
		offsetStream << stringf(
			"#define oObjectsManager 0x%lx\n"
			"#define oLocalPlayer 0x%lx\n"
			"#define oChatClientBox 0x%lx\n"
			"#define oChatHud 0x%lx\n"
			"#define oGameTime 0x%lx\n"
			"#define oMinimapObj 0x%lx\n"
			"#define oHudInstance 0x%lx\n"
			"\n"
			"#define oGetAIManager 0x%lx\n"
			"#define oGetBoundingRadius 0x%lx\n"
			"#define oIsHero 0x%lx\n"
			"#define oIsMinion 0x%lx\n"
			"#define oIsTurret 0x%lx\n"
			"#define oIsMissile 0x%lx\n"
			"#define oIsTroy 0x%lx\n"
			"#define oIsAlive 0x%lx\n"
			"#define oGetSpellState 0x%lx\n"
			"#define oGetAttackCastDelay 0x%lx\n"
			"#define oGetAttackDelay 0x%lx\n"
			"#define oW2S 0x%lx\n"
			"#define oIssueClickNew 0x%lx\n"
			"#define oOnProcessSpell 0x%lx\n"
			"\n"
			"#define oChatHudChatOpened 0x%lx\n"
			"#define oMinimapObjectHud 0x%lx\n"
			"#define oMinimapPosition 0x%lx\n"
			"#define oMinimapSize 0x%lx\n"
			"\n"
			"#define oObjIndex 0x%lx\n"
			"#define oObjTeam 0x%lx\n"
			"#define oObjName 0x%lx\n"
			"#define oObjNetworkID 0x%lx\n"
			"#define oObjPos 0x%lx\n"
			"#define oObjVisibility 0x%lx\n"
			"#define oObjMana 0x%lx\n"
			"#define oObjMaxManaoObjMana 0x%lx\n"
			"#define oObjSar 0x%lx\n"
			"#define oObjMaxSar 0x%lx\n"
			"#define oObjSarEnabled 0x%lx\n"
			"#define oObjTargetable 0x%lx\n"
			"#define oObjHP 0x%lx\n"
			"#define oObjMaxHP 0x%lx\n"
			"#define oObjHPMaxPenalty 0x%lx\n"
			"#define oObjAllShield 0x%lx\n"
			"#define oObjPhysicalShield 0x%lx\n"
			"#define oObjMagicalShield 0x%lx\n"
			"#define oObjStats 0x%lx\n"
			"#define oObjBonusAtk 0x%lx\n"
			"#define oObjAP 0x%lx\n"
			"#define oObjBaseAtk 0x%lx\n"
			"#define oObjArmor 0x%lx\n"
			"#define oObjMagicResist 0x%lx\n"
			"#define oObjMoveSpeed 0x%lx\n"
			"#define oObjAtkRange 0x%lx\n"
			"#define oObjBuffMgr 0x%lx\n"
			"#define oObjSpellBook 0x%lx\n"
			"#define oObjObjectName 0x%lx\n"
			"#define oObjLevel 0x%lx\n"
			"\n"
			"#define oSpellBookActiveSpell 0x%lx\n"
			"#define oActiveSpellSpellInfoPtr 0x%lx\n"
			"#define oActiveSpellEndCastTime 0x%lx\n"
			"#define oActiveSpellStartPos 0x%lx\n"
			"#define oActiveSpellEndPos 0x%lx\n"
			"#define oActiveSpellTargetArray 0x%lx\n"
			"#define oActiveSpellTargetArraySize 0x%lx\n"
			"#define oActiveSpellCastTime 0x%lx\n"
			"#define oActiveSpellChannelStartTime 0x%lx\n"
			"#define oActiveSpellChannelEndTime 0x%lx\n"
			"\n"
			"#define oSpellBookSpellSlots 0x%lx\n"
			"#define oSpellSlotSpellLvl 0x%lx\n"
			"#define oSpellSlotSpellReady 0x%lx\n"
			"#define oSpellSlotSpellInfo 0x%lx\n"
			"\n"
			"#define oSpellInfoSpellData 0x%lx\n"
			"#define oSpellDataName 0x%lx\n"
			"#define oSpellDataSpellSpeed 0x%lx\n"
			"\n"
			"#define oAIMGR_TargetPos 0x%lx\n"
			"#define oAIMGR_IsMoving 0x%lx\n"
			"#define oAIMGR_PassedWaypoints 0x%lx\n"
			"#define oAIMGR_NavBegin 0x%lx\n"
			"#define oAIMGR_NavEnd 0x%lx\n"
			"#define oAIMGR_IsDashing 0x%lx\n"
			"#define oAIMGR_CurrentPos 0x%lx\n"
			"#define oAIMGR_Velocity 0x%lx\n"
			"\n"
			"#define oMissileSpellInfo 0x%lx\n"
			"#define oMissileSourceIndex 0x%lx\n"
			"#define oMissileStartPos 0x%lx\n"
			"#define oMissileEndPos 0x%lx\n"
			"#define oMissileTargetIndex 0x%lx\n"
			"#define oMissileHasTarget 0x%lx\n"
			"\n"
			"#define oBuffType 0x%lx\n"
			"#define oBuffName 0x%lx\n"
			"#define oBuffNameOffset 0x%lx\n"
			"#define oBuffStartTime 0x%lx\n"
			"#define oBuffEndTime 0x%lx\n"
			"#define oBuffCountAlt 0x%lx\n"
			"#define oBuffIntCount 0x%lx\n"
			"#define oBuffFloatCount 0x%lx\n"
			,
			oObjectsManager,
			oLocalPlayer,
			oChatClientBox,
			oChatHud,
			oGameTime,
			oMinimapObj,
			oHudInstance,

			oGetAIManager,
			oGetBoundingRadius,
			oIsHero,
			oIsMinion,
			oIsTurret,
			oIsMissile,
			oIsTroy,
			oIsAlive,
			oGetSpellState,
			oGetAttackCastDelay,
			oGetAttackDelay,
			oW2S,
			oIssueClickNew,
			oOnProcessSpell,

			oChatHudChatOpened,
			oMinimapObjectHud,
			oMinimapPosition,
			oMinimapSize,

			oObjIndex,
			oObjTeam,
			oObjName,
			oObjNetworkID,
			oObjPos,
			oObjVisibility,
			oObjMana,
			oObjMaxManaoObjMana,
			oObjSar,
			oObjMaxSar,
			oObjSarEnabled,
			oObjTargetable,
			oObjHP,
			oObjMaxHP,
			oObjHPMaxPenalty,
			oObjAllShield,
			oObjPhysicalShield,
			oObjMagicalShield,
			oObjStats,
			oObjBonusAtk,
			oObjAP,
			oObjBaseAtk,
			oObjArmor,
			oObjMagicResist,
			oObjMoveSpeed,
			oObjAtkRange,
			oObjBuffMgr,
			oObjSpellBook,
			oObjObjectName,
			oObjLevel,

			oSpellBookActiveSpell,
			oActiveSpellSpellInfoPtr,
			oActiveSpellEndCastTime,
			oActiveSpellStartPos,
			oActiveSpellStartPos,
			oActiveSpellTargetArray,
			oActiveSpellTargetArraySize,
			oActiveSpellCastTime,
			oActiveSpellChannelStartTime,
			oActiveSpellChannelEndTime,

			oSpellBookSpellSlots,
			oSpellSlotSpellLvl,
			oSpellSlotSpellReady,
			oSpellSlotSpellInfo,

			oSpellInfoSpellData,
			oSpellDataName,
			oSpellDataSpellSpeed,

			oAIMGR_TargetPos,
			oAIMGR_IsMoving,
			oAIMGR_PassedWaypoints,
			oAIMGR_NavBegin,
			oAIMGR_NavEnd,
			oAIMGR_IsDashing,
			oAIMGR_CurrentPos,
			oAIMGR_Velocity,

			oMissileSpellInfo,
			oMissileSourceIndex,
			oMissileStartPos,
			oMissileEndPos,
			oMissileTargetIndex,
			oMissileHasTarget,

			oBuffType,
			oBuffName,
			oBuffNameOffset,
			oBuffStartTime,
			oBuffEndTime,
			oBuffCountAlt,
			oBuffIntCount,
			oBuffFloatCount
			);
		offsetStream.close();
	}
}