#include "HeavensGate.h"
#include "globalVars.h"
#include "makesyscall.h"
#include "UtilityFunctions.h"

bool IsHeavensGateInitialized = false;

LPVOID lpJmpRealloc = nullptr;
DWORD Backup_Eax;
DWORD Handle, Address, DwSize, NewProtect, OldProtect;
DWORD Ret1, Ret2;

void __declspec(naked) hk_NtProtectVirtualMemory()
{
	__asm {
		mov Backup_Eax, eax
		mov eax, [esp]
		mov Ret1, eax
		mov eax, [esp + 0x4]
		mov Ret2, eax
		mov eax, [esp + 0x8]
		mov Handle, eax
		mov eax, [esp + 0xC]
		mov Address, eax
		mov eax, [esp + 0x10]
		mov DwSize, eax
		mov eax, [esp + 0x14]
		mov NewProtect, eax
		mov eax, [esp + 0x18]
		mov OldProtect, eax

		mov eax, Backup_Eax
		pushad
	}

	UtilityFunctions::DebugLog("ProtectVirtualMemory Address: 0x%p Size: 0x%lx Protection: 0x%lx", Address, DwSize, NewProtect);

	__asm popad
	__asm jmp lpJmpRealloc
}

void __declspec(naked) hk_NtQueryVirtualMemory()
{
	__asm {
		mov Backup_Eax, eax
		mov eax, [esp] // - 4 bytes ret1
		mov Ret1, eax
		mov eax, [esp + 0x4] // - 4 bytes ret2
		mov Ret2, eax
		mov eax, [esp + 0x8] //HANDLE - 4 bytes ProcessHandle
		mov Handle, eax
		mov eax, [esp + 0xC] //PVOID - 4 bytes BaseAddress
		mov Address, eax
		/*
		mov eax, [esp + 0x10] //PVOID - 4 bytes MBI - BaseAddress
		mov BaseAddress2, eax
		mov eax, [esp + 0x14] //PVOID - 4 bytes MBI - AllocationBase
		mov AllocationBase, eax
		mov eax, [esp + 0x18] //DWORD - 4 bytes MBI - AllocationProtect
		mov AllocationProtect, eax
		mov eax, [esp + 0x1C] //WORD - 2 bytes MBI - PartitionId
		mov PartitionId, eax
		mov eax, [esp + 0x20] //SIZE_T - 4 bytes MBI - RegionSize
		mov RegionSize, eax
		mov eax, [esp + 0x24] //DWORD - 4 bytes MBI - State
		mov State, eax
		mov eax, [esp + 0x28] //DWORD - 4 bytes MBI - Protect
		mov Protect, eax
		mov eax, [esp + 0x2C] //DWORD - 4 bytes MBI - Type
		mov typex, eax
		mov eax, [esp + 0x30] //PVOID - 4 bytes - MemoryInformation
		mov MemoryInformation, eax
		mov eax, [esp + 0x34] //SIZE_T - 4 bytes - MemoryInformationLength
		mov MemoryInformationLength, eax
		mov eax, [esp + 0x38] //PSIZE_T - 4 bytes - ReturnLength
		mov ReturnLength, eax
		*/
		mov eax, Backup_Eax
		pushad
	}

	UtilityFunctions::DebugLog("QueryVirtualMemory Address: %p", Address);
	//UtilityFunctions::DbgStreamFile("HG.txt", UtilityFunctions::Stringf("QueryVirtualMemory Address: %p", Address));

	__asm popad
	__asm jmp lpJmpRealloc
}

void __declspec(naked) hk_NtReadVirtualMemory()
{
	__asm pushad

	UtilityFunctions::DebugLog("Calling NtReadVirtualMemory.\n");
	//UtilityFunctions::DbgStreamFile("HG.txt", "ReadVirtualMemory");

	__asm popad
	__asm jmp lpJmpRealloc
}

DWORD_PTR __declspec(naked) GetGateAddress()
{
	__asm
	{
		mov eax, dword ptr fs : [0xC0]
		ret
	}
}

void __declspec(naked) hk_Wow64Trampoline()
{
	__asm
	{
		cmp eax, 0x3f //64bit Syscall id of NtRVM
		je hk_NtReadVirtualMemory
		cmp eax, 0x50 //64bit Syscall id of NtPVM
		je hk_NtProtectVirtualMemory

		jmp lpJmpRealloc
	}
}

LPVOID CreateNewJump(DWORD Gate)
{
	lpJmpRealloc = VirtualAlloc(nullptr, 0x1000, MEM_RESERVE | MEM_COMMIT,
		PAGE_EXECUTE_READWRITE);
	memcpy(lpJmpRealloc, (void*)Gate, 9);

	return lpJmpRealloc;
}

void WriteJump(DWORD dwWow64Address, void* pBuffer, size_t ulSize)
{
	DWORD dwOldProtect = 0;
	VirtualProtect((LPVOID)dwWow64Address, 0x1000, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	memcpy((void*)dwWow64Address, pBuffer, ulSize);
	VirtualProtect((LPVOID)dwWow64Address, 0x1000, dwOldProtect, &dwOldProtect);
}

void EnableHeavensGateHook()
{
	if (IsHeavensGateInitialized) return;
	UtilityFunctions::DbgStreamFile("HG.txt", "Gate init");

	DWORD Gate = 0;

	_asm {
		mov eax, dword ptr fs : [0xC0]
		mov Gate, eax;
	}

	UtilityFunctions::DbgStreamFile("HG.txt", UtilityFunctions::Stringf("Gate %p", Gate));

	CreateNewJump(Gate);
	UtilityFunctions::DbgStreamFile("HG.txt", UtilityFunctions::Stringf("Created Jump %p", lpJmpRealloc));

	LPVOID Hook_Gate = &hk_Wow64Trampoline;

	char trampolineBytes[] =
	{
		0x68, 0xDD, 0xCC, 0xBB, 0xAA,       /*push 0xAABBCCDD*/
		0xC3,                               /*ret*/
		0xCC, 0xCC, 0xCC                    /*padding*/
	};
	memcpy(&trampolineBytes[1], &Hook_Gate, 4);
	UtilityFunctions::DbgStreamFile("HG.txt", "Memcpy");

	UtilityFunctions::DbgStreamFile("HG.txt", UtilityFunctions::Stringf("Gate before: %lx", *(BYTE*)Gate));

	WriteJump(Gate, trampolineBytes, sizeof(trampolineBytes));

	UtilityFunctions::DbgStreamFile("HG.txt", UtilityFunctions::Stringf("Gate after: %lx", *(BYTE*)Gate));

	IsHeavensGateInitialized = true;
}

void DisableHeavensGateHook()
{
	if (!IsHeavensGateInitialized)
		return;

	DWORD Gate = 0;

	_asm {
		mov eax, dword ptr fs : [0xC0]
		mov Gate, eax;
	}
	WriteJump(Gate, lpJmpRealloc, 9);
}