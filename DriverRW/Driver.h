#pragma once
#include "stdafx.h"
#include <HookLib.h>
#include "libinfinityhook/infinityhook.h"

#define NT_DEVICE_NAME      L"\\Device\\HxT9"
#define DOS_DEVICE_NAME     L"\\DosDevices\\HxT9"

// Request code to read
#define IO_READ_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x910, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
// Request code to write
#define IO_WRITE_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x911, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
// Request code to get base address
#define IO_BASE_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x912, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
// Request code to hook
#define IO_HOOK_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x913, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
// Request code to page protect
#define IO_PROTECT_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x914, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
// Request code to copy memory
#define IO_MEMCPY_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x915, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
// Request code to remove vad protection
#define IO_REMOVEVADPROTECTION_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x916, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

typedef struct _KERNEL_RW_REQUEST
{
	UINT64 ProcessId;
	UINT64 Address;
	UINT64 Data;
	UINT64 Size;
} KERNEL_RW_REQUEST, * PKERNEL_RW_REQUEST;


extern "C" {
	DRIVER_INITIALIZE DriverEntry;
	DRIVER_DISPATCH CreateClose;
	DRIVER_DISPATCH DeviceControl;
	DRIVER_UNLOAD UnloadDriver;

	NTSTATUS NTAPI MmCopyVirtualMemory(PEPROCESS SourceProcess, PVOID SourceAddress, PEPROCESS TargetProcess, PVOID TargetAddress, SIZE_T BufferSize, KPROCESSOR_MODE PreviousMode, PSIZE_T ReturnSize);
	NTKERNELAPI NTSTATUS PsLookupProcessByProcessId(IN HANDLE ProcessId, OUT PEPROCESS* Process);
	NTKERNELAPI PVOID PsGetProcessSectionBaseAddress(IN PEPROCESS Process);
	NTSYSAPI NTSTATUS NTAPI ZwProtectVirtualMemory(__in HANDLE ProcessHandle, __inout PVOID* BaseAddress, __inout PSIZE_T RegionSize, __in ULONG NewProtect, __out PULONG OldProtect);
}

#define SEC_NO_CHANGE 0x400000
typedef NTSTATUS(NTAPI* NtCreateSection_t)(
	_Out_ PHANDLE SectionHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
	_In_opt_ PLARGE_INTEGER MaximumSize,
	_In_ ULONG SectionPageProtection,
	_In_ ULONG AllocationAttributes,
	_In_opt_ HANDLE FileHandle
	);

typedef NTSTATUS (NTAPI * NtMapViewOfSection_t)(
	_In_ HANDLE SectionHandle,
	_In_ HANDLE ProcessHandle,
	_In_ _Out_ PVOID* BaseAddress OPTIONAL,
	_In_ ULONG ZeroBits OPTIONAL,
	_In_ ULONG CommitSize,
	_In_ _Out_ PLARGE_INTEGER SectionOffset OPTIONAL,
	_In_ _Out_ PULONG ViewSize,
	_In_ SECTION_INHERIT InheritDisposition,
	_In_ ULONG AllocationType OPTIONAL,
	_In_ ULONG Protect);


typedef struct _MM_AVL_NODE
{
	struct _MM_AVL_NODE* LeftChild;
	struct _MM_AVL_NODE* RightChild;

	union ___unnamed1666
	{
		struct
		{
			__int64 Balance : 2;
		};
		struct _MM_AVL_NODE* Parent;
	} u1;
} MM_AVL_NODE, * PMM_AVL_NODE, * PMMADDRESS_NODE;

typedef struct _RTL_AVL_TREE
{
	PMM_AVL_NODE BalancedRoot;
	void* NodeHint;
	unsigned __int64 NumberGenericTableElements;
} RTL_AVL_TREE, * PRTL_AVL_TREE, MM_AVL_TABLE, * PMM_AVL_TABLE;

struct _MMVAD_FLAGS 
{
	unsigned long Lock : 4;
	unsigned long Type : 3;
	unsigned long Protection : 5;
	unsigned long PreferredNode : 6; 
	unsigned long PageSize : 2; 
	unsigned long PrivateMemory : 1;
};

struct _MMVAD_FLAGS1 
{
	unsigned long CommitCharge : 31; 
	unsigned long MemCommit : 1; 
};

union ___unnamed1951 
{
	unsigned long LongFlags; 
	struct _MMVAD_FLAGS VadFlags; 
};

union ___unnamed1952 
{
	unsigned long LongFlags1; 
	struct _MMVAD_FLAGS1 VadFlags1; 
};

typedef struct _MMVAD_SHORT
{
	union
	{
		struct _RTL_BALANCED_NODE VadNode;
		struct _MMVAD_SHORT* NextVad;
	};
	PVOID ExtraCreateInfo;
	UINT32 StartingVpn;
	UINT32 EndingVpn;
	unsigned char StartingVpnHigh;
	unsigned char EndingVpnHigh;
	unsigned char CommitChargeHigh;
	unsigned char SpareNT64VadUChar;
	INT32 ReferenceCount;
	PVOID PushLock;
	___unnamed1951 u;
	___unnamed1952 u1;
	PVOID EventList;
} MMVAD_SHORT, * PMMVAD_SHORT;

TABLE_SEARCH_RESULT MiFindNodeOrParent(IN PMM_AVL_TABLE Table, IN ULONG_PTR StartingVpn, OUT PMMADDRESS_NODE* NodeOrParent);