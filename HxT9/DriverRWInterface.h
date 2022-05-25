#pragma once
#include <Windows.h>

// Request Code To Read
#define IO_READ_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x910, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
// Request Code To Write
#define IO_WRITE_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x911, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
// Request Code To Request Base Address
#define IO_BASE_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x912, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
// Request Code To Hook
#define IO_HOOK_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x913, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
// Request Code To Protect
#define IO_PROTECT_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x914, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
// Request Code To Copy Memory
#define IO_MEMCPY_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x915, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

typedef struct _KERNEL_REQUEST
{
	UINT64 ProcessId;
	UINT64 Address;
	UINT64 Data;
	UINT64 Size;
} KERNEL_REQUEST, * PKERNEL_REQUEST;

// interface for our driver
class DriverRWInterface
{
public:
	BOOLEAN Connected = FALSE;
	HANDLE hDriver; // Handle to driver
	UINT64 ProcessId;
	PKERNEL_REQUEST Request;

	DriverRWInterface() {
		hDriver = NULL;
		ProcessId = NULL;
		Request = NULL;
	}

	DriverRWInterface(LPCSTR RegistryPath, ULONG Pid)
	{
		hDriver = CreateFileA(RegistryPath, GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
		ProcessId = Pid;

		if (hDriver != INVALID_HANDLE_VALUE) {
			Connected = TRUE;
			Request = new KERNEL_REQUEST;
		}
	}

	void Dispose() {
		if (hDriver != INVALID_HANDLE_VALUE)
			CloseHandle(hDriver);
	}

	UINT64 GetBaseAddress() {
		if (hDriver == INVALID_HANDLE_VALUE)
			return NULL;

		UINT64 BaseAddress = NULL;

		Request->ProcessId = ProcessId;
		Request->Data = (UINT64)&BaseAddress;
		Request->Size = sizeof(UINT64);
		Request->Address = 0;

		DeviceIoControl(hDriver, IO_BASE_REQUEST, Request, sizeof(KERNEL_REQUEST), Request, sizeof(KERNEL_REQUEST), 0, 0);

		return BaseAddress;
	}

	template <typename type>
	type ReadVirtualMemory(UINT64 ReadAddress)
	{
		if (hDriver == INVALID_HANDLE_VALUE) {
			return NULL;
		}

		type boh;

		Request->ProcessId = ProcessId;
		Request->Address = ReadAddress;
		Request->Size = sizeof(type);
		Request->Data = (UINT64)&boh;

		// send code to our driver with the arguments
		DeviceIoControl(hDriver, IO_READ_REQUEST, Request, sizeof(KERNEL_REQUEST), Request, sizeof(KERNEL_REQUEST), 0, 0);

		return boh;
	}

	bool ReadVirtualMemory(UINT64 ReadAddress, UINT64 ReadValue, UINT64 size)
	{
		if (hDriver == INVALID_HANDLE_VALUE) {
			return false;
		}

		Request->ProcessId = ProcessId;
		Request->Address = ReadAddress;
		Request->Size = size;
		Request->Data = ReadValue;

		// send code to our driver with the arguments
		DeviceIoControl(hDriver, IO_READ_REQUEST, Request, sizeof(KERNEL_REQUEST), Request, sizeof(KERNEL_REQUEST), 0, 0);

		return true;
	}

	template <typename type>
	bool WriteVirtualMemory(UINT64 WriteAddress, type WriteValue)
	{
		if (hDriver == INVALID_HANDLE_VALUE)
			return false;

		bool ret;

		Request->ProcessId = ProcessId;
		Request->Address = WriteAddress;
		Request->Data = (UINT64) & WriteValue;
		Request->Size = sizeof(type);

		ret = DeviceIoControl(hDriver, IO_WRITE_REQUEST, Request, sizeof(KERNEL_REQUEST), Request, sizeof(KERNEL_REQUEST), 0, 0);

		return ret;
	}

	bool WriteVirtualMemory(UINT64 WriteAddress, UINT64 WriteValue, UINT64 size)
	{
		if (hDriver == INVALID_HANDLE_VALUE)
			return false;

		bool ret;

		Request->ProcessId = ProcessId;
		Request->Address = WriteAddress;
		Request->Data = WriteValue;
		Request->Size = size;

		ret = DeviceIoControl(hDriver, IO_WRITE_REQUEST, Request, sizeof(KERNEL_REQUEST), Request, sizeof(KERNEL_REQUEST), 0, 0);

		return ret;
	}

	UINT64 HookFunction(UINT64 Function, UINT64 Hk) {
		if (hDriver == INVALID_HANDLE_VALUE)
			return false;

		Request->ProcessId = ProcessId;
		Request->Address = Function;
		Request->Data = Hk;
		Request->Size = 0;

		DeviceIoControl(hDriver, IO_HOOK_REQUEST, Request, sizeof(KERNEL_REQUEST), Request, sizeof(KERNEL_REQUEST), 0, 0);

		return Request->Size;
	}

	bool Protect(UINT64 Address, UINT64 Size, DWORD Protection) {
		if (hDriver == INVALID_HANDLE_VALUE)
			return false;

		Request->ProcessId = ProcessId;
		Request->Address = Address;
		Request->Data = Protection;
		Request->Size = Size;

		DeviceIoControl(hDriver, IO_PROTECT_REQUEST, Request, sizeof(KERNEL_REQUEST), Request, sizeof(KERNEL_REQUEST), 0, 0);

		return Request->Data;
	}

	bool Memcpy(UINT64 Destination, UINT64 Source, UINT64 Size) {
		if (hDriver == INVALID_HANDLE_VALUE)
			return false;

		Request->ProcessId = ProcessId;
		Request->Address = Source;
		Request->Data = Destination;
		Request->Size = Size;

		return DeviceIoControl(hDriver, IO_PROTECT_REQUEST, Request, sizeof(KERNEL_REQUEST), Request, sizeof(KERNEL_REQUEST), 0, 0);
	}
};