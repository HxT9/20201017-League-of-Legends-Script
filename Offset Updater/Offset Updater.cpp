#include <windows.h>
#include <TlHelp32.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

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

std::string convertPattern(std::string pattern) {
	size_t pos = 0;
	std::vector<std::string> tokens;
	while(pos = pattern.find(' '), pos != std::string::npos)
	{
		tokens.push_back(pattern.substr(0, pos));
		pattern.erase(0, pos + 1);
	}
	tokens.push_back(pattern);

	std::string ret = "";
	for (int i = 0; i < tokens.size(); i++) {
		if (tokens[i] == "?")
			ret += "0";
		else
			ret += std::stoi(tokens[i], 0, 16);
	}

	return ret;
}

std::string createMask(std::string pattern) {
	size_t pos = 0;
	std::vector<std::string> tokens;
	while (pos = pattern.find(' '), pos != std::string::npos)
	{
		tokens.push_back(pattern.substr(0, pos));
		pattern.erase(0, pos + 1);
	}
	tokens.push_back(pattern);

	std::string ret = "";
	for (int i = 0; i < tokens.size(); i++) {
		if (tokens[i] == "?")
			ret += "?";
		else
			ret += "x";
	}

	return ret;
}

DWORD FindPattern(BYTE* base, DWORD size, std::string pattern, std::string mask, DWORD startOffset = 0) {
	size_t patternLength = mask.length();

	for (DWORD i = startOffset; i < size - patternLength; i++)
	{
		bool found = true;
		for (DWORD j = 0; j < patternLength; j++)
		{
			if (mask.at(j) != '?' && pattern.at(j) != *(char*)(base + i + j))
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

DWORD FindPatternEx(MODULEENTRY32W hModule, std::string pattern, std::string mask, SearchType searchType = SearchType::All, DWORD startAddr = 0, DWORD endAddr = 0) {
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
			ret = ret + (DWORD)mbi.BaseAddress;
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
	DWORD oIsType = 0;
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

	DWORD oObjIndex = 0x8;
	DWORD oObjTeam = 0x34;
	DWORD oObjName = 0x54;
	DWORD oObjNetworkID = 0xb4;
	DWORD oObjPos = 0x1dc;
	DWORD oObjVisibility = 0x274;
	DWORD oObjMana = 0x29c;
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

	DWORD oMissileSpellInfo = 0x260;
	DWORD oMissileSourceIndex = 0x2c4;
	DWORD oMissileStartPos = 0x2dc;
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

	//Globals
	std::string patternObjectsManager = "8B 0D ? ? ? ? 56 E8 ? ? ? ? 85 C0 74";
	std::string patternLocalPlayer = "8B 3D ? ? ? ? 3B F7 75 09";
	std::string patternChatClientBox = "8B 0D ? ? ? ? 85 C9 74 0D 8B 41";
	std::string patternChatHud = "A3 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? 8B C8";
	std::string patternGameTime = "F3 0F 5C 0D ? ? ? ? 0F 2F C1 F3";
	std::string patternMinimapObj = "8B 0D ? ? ? ? 66 0F 6E C6";
	std::string patternHudInstance = "8B 0D ? ? ? ? FF 70 FC 8B 49 0C E8 ? ? ? ? C6";

	//Functions
	std::string patternGetAIManager = "E8 ? ? ? ? 6A ? 6A 01 FF 74 24 14";
	std::string patternGetBoundingRadius = "E8 ? ? ? ? D8 44 24 04 8B 44 24 10";
	std::string patternIsType = "E8 ? ? ? ? 84 C0 8B CB";
	std::string patternIsHero = "E8 ? ? ? ? 83 C4 04 84 C0 74 61 85";
	std::string patternIsMinion = "E8 ? ? ? ? 83 C4 04 B9 ? ? ? ? 84 C0 0F 44 CB";
	std::string patternIsTurret = "E8 ? ? ? ? 83 C4 04 84 C0 74 09 5F";
	std::string patternIsMissile = "57 E8 ? ? ? ? 83 C4 04 84 C0 74 0A";
	std::string patternIsTroy = "E8 ? ? ? ? 8B E8 83 C4 04 85 ED 0F 84 ? ? ? ? 6A 08";
	std::string patternIsAlive = "56 8B F1 8B 06 8B 80 ? ? ? ? FF D0 84 C0 74 19";
	std::string patternGetSpellState = "83 EC 14 8B 44 24 1C 55";
	std::string patternGetAttackCastDelay = "83 EC 0C 53 8B 5C 24 14 8B CB 56 57 8B 03 FF 90";
	std::string patternGetAttackDelay = "8B 44 24 04 51 F3";
	std::string patternW2S = "83 EC 10 56 E8 ? ? ? ? 8B 08";
	std::string patternIssueClickNew = "8B 44 24 04 6A 01 FF 70 40"; // +  E8 ? ? ? ? C2 04
	std::string patternOnProcessSpell = "E8 ? ? ? ? 85 C0 0F 94 44 24 ? 85 C0";

	//Offsets
	std::string patternChatHudChatOpened = "E8 ? ? ? ? A1 ? ? ? ? 80 B8 ? ? ? ? ? 0F 85 ? ? ? ?"; //+ 8A 46 ? 3A D8
	std::string patternMinimapObjectHud = "F3 0F 58 C8 F3 0F ? ? ? FF 52 ? 8B 8E ? ? ? ?";
	std::string patternMinimapPosition = "0F 5B C9 F3 0F 11 56 ?";
	std::string patternMinimapSize = "F3 0F 2C C0 F3 0F 11 4E ?";

	std::string patternObjIndex = "";
	std::string patternObjTeam = "";
	std::string patternObjName = "";
	std::string patternObjNetworkID = "";
	std::string patternObjPos = "";
	std::string patternObjVisibility = "";
	std::string patternObjMana = "";
	std::string patternObjMaxManaoObjMana = "";
	std::string patternObjSar = "8D BB ? ? ? ? 89 B3 ? ? ? ?";
	std::string patternObjTargetable = "89 44 24 14 8D B0 ? ? ? ?";
	std::string patternObjHP = "68 ? ? ? ? 8D B0 ? ? ? ? 89 44 24 14";
	std::string patternObjStats = "81 C1 ? ? ? ? 89 47 04";
	std::string patternObjBonusAtk = "0F 28 DC F3 0F 59 99 ? ? ? ?";
	std::string patternObjAP = "D8 81 ? ? ? ? D9 81 ? ? ? ?";
	std::string patternObjBaseAtk = "F3 0F 10 91 ? ? ? ? 0F 28 DC";
	std::string patternObjArmor = "51 56 57 8B F9 8B 4F 04 8B 01 FF 90 ? ? ? ? 8B 4F 04 8B F0 E8 ? ? ? ? 50 6A 03"; //+ 8B 81 ? ? ? ?
	std::string patternObjMoveSpeed = "8B 81 ? ? ? ? 89 47 0C 8B 01";
	std::string patternObjAtkRange = "E8 ? ? ? ? 80 7B 54 ?"; //+ C3 D9 83 ? ? ? ?
	std::string patternObjBuffMgr = "8D 8B ? ? ? ? FF 30 FF 73 ? E8";
	std::string patternObjSpellBook = "FF 74 24 1C 8D 8F ? ? ? ?";
	std::string patternObjObjectName = "C6 83 ? ? ? ? ? E8 ? ? ? ? 8D 8B ? ? ? ?";
	std::string patternObjLevel = "8D BB ? ? ? ? 83 C4 14";

	std::string patternSpellBookActiveSpell = "";
	std::string patternActiveSpellSpellInfoPtr = "";
	std::string patternActiveSpellEndCastTime = "";
	std::string patternActiveSpellStartPos = "";
	std::string patternActiveSpellEndPos = "";
	std::string patternActiveSpellTargetArray = "";
	std::string patternActiveSpellTargetArraySize = "";
	std::string patternActiveSpellCastTime = "";
	std::string patternActiveSpellChannelStartTime = "";
	std::string patternActiveSpellChannelEndTime = "";

	std::string patternSpellBookSpellSlots = "8B 38 E8 ? ? ? ? 8B F0"; //+ 8B 84 81 ? ? ? ?
	std::string patternSpellSlotSpellLvl = "E8 ? ? ? ? 3B 47 08";
	std::string patternSpellSlotSpellReady = "F3 0F 10 4E ? D9 5C 24 0C";
	std::string patternSpellSlotSpellInfo = "E8 ? ? ? ? 8B 50 ? 8B 8A";

	std::string patternSpellInfoSpellData = "F3 0F 10 54 24 ? 8B 48 ?";
	std::string patternSpellDataName = "8B 44 24 08 56 57 8B F9 8D 77 04"; //+ FF 70 ?
	std::string patternSpellDataSpellSpeed = "";

	std::string patternAIMGR_TargetPos = "89 ? ? 8B 88 ? ? ? ? 89 ? ? 8D ? ? ? ? ? 8B";
	std::string patternAIMGR_IsMoving = "8A 98 ? ? ? ? 8B 06 FF 90 ? ? ? ? 8B CE";
	std::string patternAIMGR_PassedWaypoints = "8D 9E ? ? ? ? C6 86 ? ? ? ? ?";
	std::string patternAIMGR_NavBegin = "2B 88 ? ? ? ? B8 ? ? ? ? F7 E9 D1 FA";
	std::string patternAIMGR_IsDashing = "88 ? ? ? ? ? 8B 44 ? ? 89 ? ? ? ? ? F3 0F";
	std::string patternAIMGR_CurrentPos = "8B CB 89 86 ? ? ? ?";
	std::string patternAIMGR_Velocity = "F3 0F 59 C1 F3 0F 11 96 ? ? ? ?";

	std::string patternMissileSpellInfo = "";
	std::string patternMissileSourceIndex = "";
	std::string patternMissileStartPos = "";
	std::string patternMissileEndPos = "";
	std::string patternMissileTargetIndex = "";
	std::string patternMissileHasTarget = "";

	std::string patternBuffType = "";
	std::string patternBuffName = "";
	std::string patternBuffNameOffset = "";
	std::string patternBuffStartTime = "";
	std::string patternBuffEndTime = "";
	std::string patternBuffCountAlt = "E8 ? ? ? ? 84 C0 74 2D 8B 0E";
	std::string patternBuffIntCount = "7D 03 8B 41 74 ?";
	std::string patternBuffFloatCount = "";

	Pid = GetProcessIdByName(processName);

	if (Pid) {
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, Pid);
		hModule = GetRemoteModuleHandle(Pid, processName);

		typedef LONG(NTAPI* NtSuspendProcess)(IN HANDLE ProcessHandle);
		NtSuspendProcess pNtSuspendProcess = (NtSuspendProcess)GetProcAddress(
			GetModuleHandleA("ntdll"), "NtSuspendProcess");
		typedef LONG(NTAPI* NtResumeProcess)(IN HANDLE ProcessHandle);
		NtResumeProcess pNtResumeProcess = (NtResumeProcess)GetProcAddress(
			GetModuleHandleA("ntdll"), "NtResumeProcess");

		//pNtSuspendProcess(hProcess);
		std::cout << "Process Suspended" << std::endl;

		tempAddr = FindPatternEx(hModule, convertPattern(patternObjectsManager), createMask(patternObjectsManager), SearchType::Text);
		if(tempAddr)
			oObjectsManager = Read<DWORD>(tempAddr + 2) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, convertPattern(patternLocalPlayer), createMask(patternLocalPlayer), SearchType::Text);
		if (tempAddr)
			oLocalPlayer = Read<DWORD>(tempAddr + 2) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, convertPattern(patternChatClientBox), createMask(patternChatClientBox), SearchType::Text);
		if (tempAddr)
			oChatClientBox = Read<DWORD>(tempAddr + 2) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, convertPattern(patternChatHud), createMask(patternChatHud), SearchType::Text);
		if (tempAddr)
			oChatHud = Read<DWORD>(tempAddr + 1) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, convertPattern(patternGameTime), createMask(patternGameTime), SearchType::Text);
		if (tempAddr)
			oGameTime = Read<DWORD>(tempAddr + 4) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, convertPattern(patternMinimapObj), createMask(patternMinimapObj), SearchType::Text);
		if (tempAddr)
			oMinimapObj = Read<DWORD>(tempAddr + 2) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, convertPattern(patternHudInstance), createMask(patternHudInstance), SearchType::Text);
		if (tempAddr)
			oHudInstance = Read<DWORD>(tempAddr + 2) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, convertPattern(patternGetAIManager), createMask(patternGetAIManager), SearchType::Text);
		if (tempAddr)
			oGetAIManager = getE8Address(tempAddr) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, convertPattern(patternGetBoundingRadius), createMask(patternGetBoundingRadius), SearchType::Text);
		if (tempAddr)
			oGetBoundingRadius = getE8Address(tempAddr) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, convertPattern(patternIsType), createMask(patternIsType), SearchType::Text);
		if (tempAddr)
			oIsType = getE8Address(tempAddr) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, convertPattern(patternIsHero), createMask(patternIsHero), SearchType::Text);
		if (tempAddr)
			oIsHero = getE8Address(tempAddr) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, convertPattern(patternIsMinion), createMask(patternIsMinion), SearchType::Text);
		if (tempAddr)
			oIsMinion = getE8Address(tempAddr) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, convertPattern(patternIsTurret), createMask(patternIsTurret), SearchType::Text);
		if (tempAddr)
			oIsTurret = getE8Address(tempAddr) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, convertPattern(patternIsMissile), createMask(patternIsMissile), SearchType::Text);
		if (tempAddr)
			oIsMissile = getE8Address(tempAddr + 1) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, convertPattern(patternIsTroy), createMask(patternIsTroy), SearchType::Text);
		if (tempAddr)
			oIsTroy = getE8Address(tempAddr) - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, convertPattern(patternIsAlive), createMask(patternIsAlive), SearchType::Text);
		if (tempAddr)
			oIsAlive = tempAddr - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, convertPattern(patternGetSpellState), createMask(patternGetSpellState), SearchType::Text);
		if (tempAddr)
			oGetSpellState = tempAddr - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, convertPattern(patternGetAttackCastDelay), createMask(patternGetAttackCastDelay), SearchType::Text);
		if (tempAddr)
			oGetAttackCastDelay = tempAddr - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, convertPattern(patternGetAttackDelay), createMask(patternGetAttackDelay), SearchType::Text);
		if (tempAddr)
			oGetAttackDelay = tempAddr - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, convertPattern(patternW2S), createMask(patternW2S), SearchType::Text);
		if (tempAddr)
			oW2S = tempAddr - (DWORD)hModule.modBaseAddr;

		tempAddr = FindPatternEx(hModule, convertPattern(patternIssueClickNew), createMask(patternIssueClickNew), SearchType::Text);
		if (tempAddr) {
			tempAddr = FindPatternEx(hModule, convertPattern("E8 ? ? ? ? C2 04"), "x????xx", SearchType::Text, tempAddr, tempAddr + 0x20);
			if (tempAddr) {
				oIssueClickNew = getE8Address(tempAddr) - (DWORD)hModule.modBaseAddr;
			}
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternOnProcessSpell), createMask(patternOnProcessSpell), SearchType::Text);
		if (tempAddr) {
			oOnProcessSpell = getE8Address(tempAddr) - (DWORD)hModule.modBaseAddr;
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternChatHudChatOpened), createMask(patternChatHudChatOpened), SearchType::Text);
		if (tempAddr) {
			tempAddr = getE8Address(tempAddr);
			tempAddr = FindPatternEx(hModule, convertPattern("8A 46 ? 3A D8"), "xx?xx", SearchType::Text, tempAddr, tempAddr + 0x100);
			if (tempAddr) {
				oChatHudChatOpened = Read<BYTE>(tempAddr + 2);
			}
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternMinimapObjectHud), createMask(patternMinimapObjectHud), SearchType::Text);
		if (tempAddr) {
			oMinimapObjectHud = Read<DWORD>(tempAddr + 14);
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternMinimapPosition), createMask(patternMinimapPosition), SearchType::Text);
		if (tempAddr) {
			oMinimapPosition = Read<BYTE>(tempAddr + 7);
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternMinimapSize), createMask(patternMinimapSize), SearchType::Text);
		if (tempAddr) {
			oMinimapSize = Read<BYTE>(tempAddr + 8);
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternObjSar), createMask(patternObjSar), SearchType::Text);
		if (tempAddr) {
			oObjSar = Read<DWORD>(tempAddr + 2);
			oObjMaxSar = oObjSar + 0x10;
			oObjSarEnabled = oObjMaxSar + 0x10;
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternObjTargetable), createMask(patternObjTargetable), SearchType::Text);
		if (tempAddr) {
			oObjTargetable = Read<DWORD>(tempAddr + 6);
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternObjHP), createMask(patternObjHP), SearchType::Text);
		if (tempAddr) {
			oObjHP = Read<DWORD>(tempAddr + 7);
			oObjMaxHP = oObjHP + 0x10;
			oObjHPMaxPenalty = oObjHP + 0x20;
			oObjAllShield = oObjHP + 0x40;
			oObjPhysicalShield = oObjHP + 0x50;
			oObjMagicalShield = oObjHP + 0x60;
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternObjStats), createMask(patternObjStats), SearchType::Text);
		if (tempAddr) {
			oObjStats = Read<DWORD>(tempAddr + 2);
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternObjBonusAtk), createMask(patternObjBonusAtk), SearchType::Text);
		if (tempAddr) {
			oObjBonusAtk = Read<DWORD>(tempAddr + 7) + oObjStats;
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternObjAP), createMask(patternObjAP), SearchType::Text);
		if (tempAddr) {
			oObjAP = Read<DWORD>(tempAddr + 2) + oObjStats;
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternObjBaseAtk), createMask(patternObjBaseAtk), SearchType::Text);
		if (tempAddr) {
			oObjBaseAtk = Read<DWORD>(tempAddr + 4) + oObjStats;
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternObjArmor), createMask(patternObjArmor), SearchType::Text);
		if (tempAddr) {
			tempAddr = FindPatternEx(hModule, convertPattern("8B 81 ? ? ? ?"), "xx????", SearchType::Text, tempAddr, tempAddr + 0x50);
			if (tempAddr) {
				oObjArmor = Read<DWORD>(tempAddr + 2);
				oObjMagicResist = oObjArmor + 0x8;
			}
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternObjMoveSpeed), createMask(patternObjMoveSpeed), SearchType::Text);
		if (tempAddr) {
			oObjMoveSpeed = Read<DWORD>(tempAddr + 2);
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternObjAtkRange), createMask(patternObjAtkRange), SearchType::Text);
		if (tempAddr) {
			tempAddr = getE8Address(tempAddr);
			tempAddr = FindPatternEx(hModule, convertPattern("C3 D9 83 ? ? ? ?"), "xxx????", SearchType::Text);
			if (tempAddr) {
				oObjAtkRange = Read<DWORD>(tempAddr + 3);
			}
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternObjBuffMgr), createMask(patternObjBuffMgr), SearchType::Text);
		if (tempAddr) {
			oObjBuffMgr = Read<DWORD>(tempAddr + 2);
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternObjSpellBook), createMask(patternObjSpellBook), SearchType::Text);
		if (tempAddr) {
			oObjSpellBook = Read<DWORD>(tempAddr + 6);
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternObjObjectName), createMask(patternObjObjectName), SearchType::Text);
		if (tempAddr) {
			oObjObjectName = Read<DWORD>(tempAddr + 2);
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternObjLevel), createMask(patternObjLevel), SearchType::Text);
		if (tempAddr) {
			oObjLevel = Read<DWORD>(tempAddr + 2);
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternSpellBookSpellSlots), createMask(patternSpellBookSpellSlots), SearchType::Text);
		if (tempAddr) {
			tempAddr = getE8Address(tempAddr + 2);
			tempAddr = FindPatternEx(hModule, convertPattern("8B 84 81 ? ? ? ?"), "xxx????", SearchType::Text, tempAddr);
			if (tempAddr) {
				oSpellBookSpellSlots = Read<DWORD>(tempAddr + 3);
			}
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternSpellSlotSpellLvl), createMask(patternSpellSlotSpellLvl), SearchType::Text);
		if (tempAddr) {
			tempAddr = getE8Address(tempAddr);
			if (tempAddr) {
				oSpellSlotSpellLvl = Read<BYTE>(tempAddr + 2);
			}
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternSpellSlotSpellReady), createMask(patternSpellSlotSpellReady), SearchType::Text);
		if (tempAddr) {
			oSpellSlotSpellReady = Read<BYTE>(tempAddr + 4);
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternSpellSlotSpellInfo), createMask(patternSpellSlotSpellInfo), SearchType::Text);
		if (tempAddr) {
			tempAddr = getE8Address(tempAddr);
			if (tempAddr) {
				oSpellSlotSpellInfo = Read<DWORD>(tempAddr + 2);
			}
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternSpellInfoSpellData), createMask(patternSpellInfoSpellData), SearchType::Text);
		if (tempAddr) {
			oSpellInfoSpellData = Read<BYTE>(tempAddr + 8);
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternSpellDataName), createMask(patternSpellDataName), SearchType::Text);
		if (tempAddr) {
			tempAddr = FindPatternEx(hModule, convertPattern("FF 70 ?"), "xx?", SearchType::Text, tempAddr);
			if (tempAddr) {
				oSpellDataName = Read<BYTE>(tempAddr + 2);
			}
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternAIMGR_TargetPos), createMask(patternAIMGR_TargetPos), SearchType::Text);
		if (tempAddr) {
			oAIMGR_TargetPos = Read<BYTE>(tempAddr + 2);
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternAIMGR_IsMoving), createMask(patternAIMGR_IsMoving), SearchType::Text);
		if (tempAddr) {
			oAIMGR_IsMoving = Read<DWORD>(tempAddr + 2);
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternAIMGR_PassedWaypoints), createMask(patternAIMGR_PassedWaypoints), SearchType::Text);
		if (tempAddr) {
			oAIMGR_PassedWaypoints = Read<DWORD>(tempAddr + 2);
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternAIMGR_NavBegin), createMask(patternAIMGR_NavBegin), SearchType::Text);
		if (tempAddr) {
			oAIMGR_NavBegin = Read<DWORD>(tempAddr + 2);
			oAIMGR_NavEnd = oAIMGR_NavBegin + 0x4;
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternAIMGR_IsDashing), createMask(patternAIMGR_IsDashing), SearchType::Text);
		std::cout << std::hex << tempAddr << std::endl;
		if (tempAddr) {
			oAIMGR_IsDashing = Read<DWORD>(tempAddr + 2);
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternAIMGR_CurrentPos), createMask(patternAIMGR_CurrentPos), SearchType::Text);
		if (tempAddr) {
			oAIMGR_CurrentPos = Read<DWORD>(tempAddr + 4);
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternAIMGR_Velocity), createMask(patternAIMGR_Velocity), SearchType::Text);
		if (tempAddr) {
			oAIMGR_Velocity = Read<DWORD>(tempAddr + 8);
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternBuffCountAlt), createMask(patternBuffCountAlt), SearchType::Text);
		if (tempAddr) {
			tempAddr = getE8Address(tempAddr);
			if (tempAddr) {
				oBuffCountAlt = Read<SHORT>(tempAddr + 2);
			}
		}

		tempAddr = FindPatternEx(hModule, convertPattern(patternBuffIntCount), createMask(patternBuffIntCount), SearchType::Text);
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
			"#define oIsType 0x%lx\n"
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
			oIsType,
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

		std::cout << "Process resumed" << std::endl;
	}
}