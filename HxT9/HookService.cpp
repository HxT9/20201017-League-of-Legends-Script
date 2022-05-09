/*#include "HookService.h"
#include "globalVars.h"

void Init() {
	std::vector<DWORD> Targets = std::vector<DWORD>();
	std::vector<DWORD> HookedFunctions = std::vector<DWORD>();
    std::vector<DWORD> SkipBytes = std::vector<DWORD>();

	AddVectoredExceptionHandler(true, (PVECTORED_EXCEPTION_HANDLER)ExceptionHandler);
}

void AddHook(DWORD Target, DWORD HkFunction, DWORD SkipNBytes)
{
    DWORD dwOld;

    Targets.push_back(Target);
    HookedFunctions.push_back(HkFunction);
    SkipBytes.push_back(SkipNBytes);

    VirtualProtect((PVOID)Target, 1, PAGE_EXECUTE | PAGE_GUARD, &dwOld);
    gui.print("Added Hook at: %lx to %lx", Target, HkFunction);
}

void RemoveHook(DWORD Target) {
    for (int i = 0; i < Targets.size(); i++) {
        if (Targets[i] == Target) {
            Targets.erase(std::next(Targets.begin(), i));
            HookedFunctions.erase(std::next(HookedFunctions.begin(), i));
            SkipBytes.erase(std::next(SkipBytes.begin(), i));
        }
    }
}

LONG ExceptionHandler(EXCEPTION_POINTERS* pExceptionInfo)
{
    DWORD dwOld;
    if (pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION) // This is going to return true whenever any of our PAGE_GUARD'ed memory page is accessed.
    {
        for (int i = 0; i < Targets.size(); i++) {
            if (pExceptionInfo->ContextRecord->Eip == Targets[i])
            {
                dwJmpBack = (DWORD*)(pExceptionInfo->ContextRecord->Esp + SkipBytes[i]); // Find the return address for the JMP/EIP back into the target program's code.
                dwJmpBack = (DWORD)pExceptionInfo->ContextRecord->Eip + 5; // or just skip X number of bytes.
                pExceptionInfo->ContextRecord->Eip = HookedFunctions[i]; // Point EIP to hook handle.
                break;
            }
        }

        pExceptionInfo->ContextRecord->EFlags |= 0x100; //Set single step flag, causing only one line of code to be executed and then throwing the STATUS_SINGLE_STEP exception.

        return EXCEPTION_CONTINUE_EXECUTION; // When we return to the page, it will no longer be PAGE_GUARD'ed, so we rely on single stepping to re-apply it. (If we re-applied it here, we'd never move forward.)
    }

    if (pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP) // This is now going to return true on the next line of execution within our page, where we re-apply PAGE_GUARD and repeat.
    {
        for (int i = 0; i < Targets.size(); i++) {
            if (pExceptionInfo->ContextRecord->Eip == Targets[i])
            {
                VirtualProtect((PVOID)Targets[i], 1, PAGE_EXECUTE | PAGE_GUARD, &dwOld);
                break;
            }
        }

        return EXCEPTION_CONTINUE_EXECUTION;
    }

    return EXCEPTION_CONTINUE_SEARCH;
}*/