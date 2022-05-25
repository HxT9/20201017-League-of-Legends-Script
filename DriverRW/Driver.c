#include "ntos.h"
#include <minwindef.h>
#include <HookLib.h>

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


DRIVER_INITIALIZE DriverEntry;
_Dispatch_type_(IRP_MJ_CREATE)
_Dispatch_type_(IRP_MJ_CLOSE)
DRIVER_DISPATCH CreateClose;
_Dispatch_type_(IRP_MJ_DEVICE_CONTROL)
DRIVER_DISPATCH DeviceControl;
DRIVER_UNLOAD UnloadDriver;

NTSTATUS NTAPI MmCopyVirtualMemory(PEPROCESS SourceProcess, PVOID SourceAddress, PEPROCESS TargetProcess, PVOID TargetAddress, SIZE_T BufferSize, KPROCESSOR_MODE PreviousMode, PSIZE_T ReturnSize);
NTKERNELAPI NTSTATUS PsLookupProcessByProcessId(IN HANDLE ProcessId, OUT PEPROCESS* Process);
NTKERNELAPI PVOID PsGetProcessSectionBaseAddress(IN PEPROCESS Process);

typedef struct _KERNEL_RW_REQUEST
{
	UINT64 ProcessId;
	UINT64 Address;
	UINT64 Data;
	UINT64 Size;
} KERNEL_RW_REQUEST, * PKERNEL_RW_REQUEST;

NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT  DriverObject,
	_In_ PUNICODE_STRING RegistryPath
)
{
	NTSTATUS        ntStatus;
	UNICODE_STRING  ntUnicodeString;    // NT Device Name "\Device\rwdriver"
	UNICODE_STRING  ntWin32NameString;    // Win32 Name "\DosDevices\rwdriver"
	PDEVICE_OBJECT  deviceObject = NULL;    // ptr to device object

	UNREFERENCED_PARAMETER(RegistryPath);

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

	DbgPrintEx(0, 0, "[HxT9] Entry\n");
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

	RtlInitUnicodeString(&uniWin32NameString, DOS_DEVICE_NAME);
	IoDeleteSymbolicLink(&uniWin32NameString);

	if (deviceObject != NULL)
	{
		IoDeleteDevice(deviceObject);
	}
	DbgPrintEx(0, 0, "[HxT9] Unload\n");
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
				ntStatus = MmCopyVirtualMemory(targetProcess, Input_rw->Address, PsGetCurrentProcess(), Input_rw->Data, Input_rw->Size, UserMode, &returnSize);
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
				ntStatus = MmCopyVirtualMemory(PsGetCurrentProcess(), Input_rw->Data, targetProcess, Input_rw->Address, Input_rw->Size, UserMode, &returnSize);
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
				RtlCopyMemory(Input_rw->Data, &baseAddr, sizeof(baseAddr));

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
				pkapc_state = MmAllocateNonCachedMemory(sizeof(KAPC_STATE));
				if (pkapc_state) {
					KeStackAttachProcess(targetProcess, pkapc_state);
					Input_rw->Size = hook(Input_rw->Address, Input_rw->Data);
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
				pkapc_state = MmAllocateNonCachedMemory(sizeof(KAPC_STATE));
				if (pkapc_state) {
					KeStackAttachProcess(targetProcess, pkapc_state);
					tempStatus = ZwProtectVirtualMemory(NtCurrentProcess(), &Input_rw->Address, &Input_rw->Size, Input_rw->Data, &oldProtect);
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
				MmCopyVirtualMemory(targetProcess, Input_rw->Address, targetProcess, Input_rw->Data, Input_rw->Size, UserMode, &returnSize);
				ObDereferenceObject(targetProcess);
			}
			else {
				DbgPrintEx(0, 0, "[HxT9] Memcpy Error\n");
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