#include "utilities.h"

#include <Psapi.h>


DWORD findPattern(const char* module, const char* pattern) {

#define in_range(x, a, b) (x >= a && x <= b)
#define get_bits(x) (in_range((x & (~0x20)), 'A', 'F') ? ((x & (~0x20)) - 'A' + 0xA): (in_range(x, '0', '9') ? x - '0': 0))
#define get_byte(x) (get_bits(x[0]) << 4 | get_bits(x[1]))

	HMODULE hdll = GetModuleHandleA(module);
	MODULEINFO mod;
	if (!K32GetModuleInformation(GetCurrentProcess(), hdll, &mod, sizeof(MODULEINFO))) {
		return NULL;
	}

	uintptr_t start = (uintptr_t)mod.lpBaseOfDll;
	uintptr_t end = (uintptr_t)mod.lpBaseOfDll + (uintptr_t)mod.SizeOfImage;
	uintptr_t match = (uintptr_t)nullptr;
	const char* current = pattern;

	for (uintptr_t pCur = start; pCur < end; pCur++) {

		if (!*current)
			return (DWORD)match;

		if (*(PBYTE)current == ('\?') || *(BYTE*)pCur == get_byte(current)) {
			if (!match)
				match = pCur;

			if (!current[2])
				return (DWORD)match;

			if (*(PWORD)current == ('\?\?') || *(PBYTE)current != ('\?'))
				current += 3;
			else
				current += 2;
		}
		else {
			current = pattern;
			match = 0;
		}
	}
	return NULL;
}

template <typename ReturnType>
__declspec(naked) ReturnType __stdcall StackSpoofForCdecl(DWORD FuncAddr, unsigned int NumberOfParams, DWORD ROPGadget, ...) {
	__asm {
		push esi
		push edi
		mov edx, esp
		mov ecx, [edx + 16]
		mov edi, ecx
		inc exc
		LoopStart :
		mov eax, [edx + ecx * 4 + 16]
			push eax
			loop LoopStart
			mov esi, ReturnHere
			mov eax, [edx + 12]
			jmp eax,
			ReturnHere:
		imul edi, 4
			add esp, edi
			pop edi
			pop esi
			ret
	}
}

template <typename ReturnType>
__declspec(naked) ReturnType __stdcall StackSpoofForThiscall(DWORD FuncAddr, unsigned int NumberOfParams, void* thisptr, DWORD ROPGadget, ...) {
	__asm {
		push esi
		mov edx, esp
		mov ecx, [edx + 12]
		inc ecx
		LoopStart :
		mov eax, [edx + ecx * 4 + 16]
			push eax
			loop LoopStart
			mov esi, ReturnHere
			mov eax, [edx + 8]
			mov ecx, [edx + 16]
			jmp eax
			ReturnHere :
		pop esi
			ret
	}
}
