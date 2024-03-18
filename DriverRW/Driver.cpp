#include "Driver.h"

/*
Region SysHooks
*/
static UNICODE_STRING StringNtCreateSection = RTL_CONSTANT_STRING(L"NtCreateSection");
static NtCreateSection_t originalNtCreateSection = NULL;
static UNICODE_STRING StringNtMapViewOfSection = RTL_CONSTANT_STRING(L"NtMapViewOfSection");
static NtMapViewOfSection_t originalNtMapViewOfSection = NULL;

NTSTATUS
NTAPI
DetourNtCreateSection(
	_Out_ PHANDLE SectionHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
	_In_opt_ PLARGE_INTEGER MaximumSize,
	_In_ ULONG SectionPageProtection,
	_In_ ULONG AllocationAttributes,
	_In_opt_ HANDLE FileHandle
)
{
	if ((AllocationAttributes & SEC_NO_CHANGE) != 0)
	{
		DbgPrintEx(0, 0, "[HxT9] NtCreateSection Removing SEC NO CHANGE");
		//__debugbreak(); // Probably useful when you are still debugging
		AllocationAttributes &= ~SEC_NO_CHANGE;
	}

	return originalNtCreateSection(SectionHandle, DesiredAccess, ObjectAttributes, MaximumSize, SectionPageProtection, AllocationAttributes, FileHandle);
}

NTSTATUS
NTAPI
DetourNtMapViewOfSection(
	_In_ HANDLE SectionHandle,
	_In_ HANDLE ProcessHandle,
	_In_ _Out_ PVOID* BaseAddress OPTIONAL,
	_In_ ULONG ZeroBits OPTIONAL,
	_In_ ULONG CommitSize,
	_In_ _Out_ PLARGE_INTEGER SectionOffset OPTIONAL,
	_In_ _Out_ PULONG ViewSize,
	_In_ SECTION_INHERIT InheritDisposition,
	_In_ ULONG AllocationType OPTIONAL,
	_In_ ULONG Protect)
{
	if ((AllocationType & SEC_NO_CHANGE) != 0)
	{
		DbgPrintEx(0, 0, "[HxT9] NtMapViewOfSection Removing SEC NO CHANGE");
		//__debugbreak(); // Probably useful when you are still debugging
		AllocationType &= ~SEC_NO_CHANGE;
	}

	return originalNtMapViewOfSection(SectionHandle, ProcessHandle, BaseAddress, ZeroBits, CommitSize, SectionOffset, ViewSize, InheritDisposition, AllocationType, Protect);
}

void __fastcall SyscallStub(
	_In_ unsigned int SystemCallIndex,
	_Inout_ void** SystemCallFunction) {

	UNREFERENCED_PARAMETER(SystemCallIndex);

	if (*SystemCallFunction == originalNtMapViewOfSection)
	{
		*SystemCallFunction = DetourNtCreateSection;
	}

	if (*SystemCallFunction == originalNtCreateSection) {
		*SystemCallFunction = DetourNtMapViewOfSection;
	}
}
/*
EndRegion SysHooks
*/

NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT  DriverObject,
	_In_ PUNICODE_STRING RegistryPath
)
{
	NTSTATUS        ntStatus = STATUS_SUCCESS;
	UNICODE_STRING  ntUnicodeString;    // NT Device Name "\Device\rwdriver"
	UNICODE_STRING  ntWin32NameString;    // Win32 Name "\DosDevices\rwdriver"
	PDEVICE_OBJECT  deviceObject = NULL;    // ptr to device object

	UNREFERENCED_PARAMETER(RegistryPath);

	originalNtCreateSection = (NtCreateSection_t)MmGetSystemRoutineAddress(&StringNtCreateSection);
	if (!originalNtCreateSection)
	{
		DbgPrintEx(0, 0, "[HxT9] Failed to locate export: %wZ.\n", StringNtCreateSection);
		return STATUS_ENTRYPOINT_NOT_FOUND;
	}

	originalNtMapViewOfSection = (NtMapViewOfSection_t)MmGetSystemRoutineAddress(&StringNtMapViewOfSection);
	if (!originalNtMapViewOfSection)
	{
		DbgPrintEx(0, 0, "[HxT9] Failed to locate export: %wZ.\n", StringNtMapViewOfSection);
		return STATUS_ENTRYPOINT_NOT_FOUND;
	}

	/*ntStatus = IfhInitialize(SyscallStub);
	if (!NT_SUCCESS(ntStatus))
	{
		DbgPrintEx(0, 0, "[HxT9] Failed to initialize SysHook with status: 0x%lx.\n", ntStatus);
		return ntStatus;
	}*/

	RtlInitUnicodeString(&ntUnicodeString, NT_DEVICE_NAME);

	ntStatus = IoCreateDevice(
		DriverObject,                   // Our Driver Object
		0,                              // We don't use a device extension
		&ntUnicodeString,               // Device name "\Device\IOCTL"
		FILE_DEVICE_UNKNOWN,            // Device type
		FILE_DEVICE_SECURE_OPEN,		// Device characteristics
		FALSE,                          // Not an exclusive device
		&deviceObject);					// Returned ptr to Device Object

	if (!NT_SUCCESS(ntStatus))
	{
		DbgPrintEx(0, 0, "[HxT9] Failed to initialize driver device");
		return ntStatus;
	}

	DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = CreateClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceControl;
	DriverObject->DriverUnload = UnloadDriver;

	RtlInitUnicodeString(&ntWin32NameString, DOS_DEVICE_NAME);

	ntStatus = IoCreateSymbolicLink(
		&ntWin32NameString, &ntUnicodeString);

	if (!NT_SUCCESS(ntStatus))
	{
		IoDeleteDevice(deviceObject);
	}

	DbgPrintEx(0, 0, "[HxT9] Driver Loaded\n");
	return ntStatus;
}

NTSTATUS
CreateClose(
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp
)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	PAGED_CODE();

	DbgPrintEx(0, 0, "[HxT9] Create Close\n");

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

VOID
UnloadDriver(
	_In_ PDRIVER_OBJECT DriverObject
)
{
	PDEVICE_OBJECT deviceObject = DriverObject->DeviceObject;
	UNICODE_STRING uniWin32NameString;

	PAGED_CODE();

	IfhRelease();

	RtlInitUnicodeString(&uniWin32NameString, DOS_DEVICE_NAME);
	IoDeleteSymbolicLink(&uniWin32NameString);

	if (deviceObject != NULL)
	{
		IoDeleteDevice(deviceObject);
	}
	DbgPrintEx(0, 0, "[HxT9] Unloaded\n");
}

NTSTATUS
DeviceControl(
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp
)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	PIO_STACK_LOCATION  irpSp;// Pointer to current stack location
	NTSTATUS            ntStatus = STATUS_SUCCESS, tempStatus;// Assume success
	PEPROCESS			targetProcess = NULL;
	SIZE_T				returnSize;
	PVOID				baseAddr;
	ULONG				oldProtect;
	PKAPC_STATE pkapc_state;

	irpSp = IoGetCurrentIrpStackLocation(Irp);
	PKERNEL_RW_REQUEST Input_rw = (PKERNEL_RW_REQUEST)Irp->AssociatedIrp.SystemBuffer;

	__try
	{
		switch (irpSp->Parameters.DeviceIoControl.IoControlCode)
		{
		case IO_READ_REQUEST:
			DbgPrintEx(0, 0, "[HxT9] Packet Read PID: %lu, ADD: 0x%llx, DATA: 0x%llx, SIZE: %lu\n", Input_rw->ProcessId, Input_rw->Address, Input_rw->Data, Input_rw->Size);
			Irp->IoStatus.Information = sizeof(Input_rw);
			if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)Input_rw->ProcessId, &targetProcess))) {
				ntStatus = MmCopyVirtualMemory(targetProcess, (PVOID)Input_rw->Address, PsGetCurrentProcess(), (PVOID)Input_rw->Data, Input_rw->Size, UserMode, &returnSize);
				ObDereferenceObject(targetProcess);
			}
			else {
				DbgPrintEx(0, 0, "[HxT9] Read Error\n");
				ntStatus = STATUS_INVALID_PARAMETER;
			}
			break;

		case IO_WRITE_REQUEST:
			DbgPrintEx(0, 0, "[HxT9] Packet Write PID: %lu, ADD: 0x%llx, DATA: 0x%llx, SIZE: %lu\n", Input_rw->ProcessId, Input_rw->Address, Input_rw->Data, Input_rw->Size);
			Irp->IoStatus.Information = sizeof(Input_rw);
			if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)Input_rw->ProcessId, &targetProcess))) {
				ntStatus = MmCopyVirtualMemory(PsGetCurrentProcess(), (PVOID)Input_rw->Data, targetProcess, (PVOID)Input_rw->Address, Input_rw->Size, UserMode, &returnSize);
				ObDereferenceObject(targetProcess);
			}
			else {
				DbgPrintEx(0, 0, "[HxT9] Write Error\n");
				ntStatus = STATUS_INVALID_PARAMETER;
			}
			break;

		case IO_BASE_REQUEST:
			DbgPrintEx(0, 0, "[HxT9] Packet Base PID: %lu, ADD: 0x%llx, DATA: 0x%llx, SIZE: %lu\n", Input_rw->ProcessId, Input_rw->Address, Input_rw->Data, Input_rw->Size);
			Irp->IoStatus.Information = sizeof(Input_rw);
			if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)Input_rw->ProcessId, &targetProcess))) {
				baseAddr = PsGetProcessSectionBaseAddress(targetProcess);
				RtlCopyMemory((PVOID)Input_rw->Data, &baseAddr, sizeof(baseAddr));

				ntStatus = STATUS_SUCCESS;
				ObDereferenceObject(targetProcess);
			}
			else {
				DbgPrintEx(0, 0, "[HxT9] Base Address Error\n");
				ntStatus = STATUS_INVALID_PARAMETER;
			}
			break;

		case IO_HOOK_REQUEST:
			DbgPrintEx(0, 0, "[HxT9] Packet Hook PID: %lu, ADD: 0x%llx, DATA: 0x%llx, SIZE: %lu\n", Input_rw->ProcessId, Input_rw->Address, Input_rw->Data, Input_rw->Size);
			Irp->IoStatus.Information = sizeof(Input_rw);
			if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)Input_rw->ProcessId, &targetProcess))) {
				pkapc_state = (PKAPC_STATE)MmAllocateNonCachedMemory(sizeof(KAPC_STATE));
				if (pkapc_state) {
					KeStackAttachProcess(targetProcess, pkapc_state);
					Input_rw->Size = (UINT64)hook((PVOID)Input_rw->Address, (PVOID)Input_rw->Data);
					KeUnstackDetachProcess(pkapc_state);
					MmFreeNonCachedMemory(pkapc_state, sizeof(KAPC_STATE));
				}
				ObDereferenceObject(targetProcess);
			}
			else {
				DbgPrintEx(0, 0, "[HxT9] Hook Error\n");
				ntStatus = STATUS_INVALID_PARAMETER;
			}

		case IO_PROTECT_REQUEST:
			DbgPrintEx(0, 0, "[HxT9] Packet Protect PID: %lu, ADD: 0x%llx, DATA: 0x%llx, SIZE: %lu\n", Input_rw->ProcessId, Input_rw->Address, Input_rw->Data, Input_rw->Size);
			Irp->IoStatus.Information = sizeof(Input_rw);
			if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)Input_rw->ProcessId, &targetProcess))) {
				pkapc_state = (PKAPC_STATE)MmAllocateNonCachedMemory(sizeof(KAPC_STATE));
				if (pkapc_state) {
					KeStackAttachProcess(targetProcess, pkapc_state);
					tempStatus = ZwProtectVirtualMemory(NtCurrentProcess(), (PVOID*)&Input_rw->Address, &Input_rw->Size, Input_rw->Data, &oldProtect);
					if (!NT_SUCCESS(tempStatus)) {
						DbgPrintEx(0, 0, "[HxT9] Error on ZwProtectVirtualMemory with status: %lx\n", tempStatus);
					}
					Input_rw->Data = oldProtect;
					KeUnstackDetachProcess(pkapc_state);
					MmFreeNonCachedMemory(pkapc_state, sizeof(KAPC_STATE));
				}
				ObDereferenceObject(targetProcess);
			}
			else {
				DbgPrintEx(0, 0, "[HxT9] Protect Error\n");
				ntStatus = STATUS_INVALID_PARAMETER;
			}
		case IO_MEMCPY_REQUEST:
			DbgPrintEx(0, 0, "[HxT9] Packet Memcpy PID: %lu, ADD: 0x%llx, DATA: 0x%llx, SIZE: %lu\n", Input_rw->ProcessId, Input_rw->Address, Input_rw->Data, Input_rw->Size);
			Irp->IoStatus.Information = sizeof(Input_rw);
			if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)Input_rw->ProcessId, &targetProcess))) {
				MmCopyVirtualMemory(targetProcess, (PVOID)Input_rw->Address, targetProcess, (PVOID)Input_rw->Data, Input_rw->Size, UserMode, &returnSize);
				ObDereferenceObject(targetProcess);
			}
			else {
				DbgPrintEx(0, 0, "[HxT9] Memcpy Error\n");
				ntStatus = STATUS_INVALID_PARAMETER;
			}
		case IO_REMOVEVADPROTECTION_REQUEST:
			DbgPrintEx(0, 0, "[HxT9] Packet RemoveVADProtection PID: %lu, ADD: 0x%llx, DATA: 0x%llx, SIZE: %lu\n", Input_rw->ProcessId, Input_rw->Address, Input_rw->Data, Input_rw->Size);
			Irp->IoStatus.Information = sizeof(Input_rw);
			if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)Input_rw->ProcessId, &targetProcess))) {
				PMM_AVL_TABLE pTable = (PMM_AVL_TABLE)((UINT64)targetProcess + 0x658);
				PMM_AVL_NODE pNode = pTable->BalancedRoot;

				if (MiFindNodeOrParent(pTable, Input_rw->Address >> PAGE_SHIFT, &pNode) == TableFoundNode)
				{
					PMMVAD_SHORT pResult = (PMMVAD_SHORT)pNode;

					DbgPrintEx(0, 0, "[HxT9] Vad Protection: %l", pResult->u.VadFlags.Protection);
				}
				else
				{
					DbgPrintEx(0, 0, "[HxT9] VAD entry for address 0x%p not found\n", Input_rw->Address);
					ntStatus = STATUS_NOT_FOUND;
				}
			}
			else {
				DbgPrintEx(0, 0, "[HxT9] RemoveVADProtection Error\n");
				ntStatus = STATUS_INVALID_PARAMETER;
			}

		default:
			Irp->IoStatus.Information = 0;
			ntStatus = STATUS_INVALID_DEVICE_REQUEST;
			break;
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
		DbgPrintEx(0, 0, "[HxT9] Exception\n");
	}

	Irp->IoStatus.Status = ntStatus;
	IofCompleteRequest(Irp, IO_NO_INCREMENT);

	return ntStatus;
}