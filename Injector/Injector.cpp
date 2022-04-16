#include <iostream>
#include <Windows.h>
#include <Tlhelp32.h>
#include <fstream>

using namespace std;

VOID startup(LPCTSTR lpApplicationName);

int main()
{
	bool injected = false;

	// path to our dll
	//LPCSTR DllPath = "E:\\alber\\source\\repos\\20200112 LOL Hack\\Release\\HxT9.dll";
	//LPWSTR ProcessName = (LPWSTR)L"League of Legends.exe";


	LPCTSTR BlitzPath = L"C:\\Users\\alber\\AppData\\Local\\Programs\\Blitz\\Blitz.exe";
	LPCTSTR OriginalDll = L"E:\\alber\\source\\repos\\20201017-League-of-Legends-Script\\Release\\HxT9.dll";
	LPCTSTR CustomDll = L"E:\\alber\\source\\repos\\20201017-League-of-Legends-Script\\Release\\Blitz.dll";
	LPCSTR DllPath = "E:\\alber\\source\\repos\\20201017-League-of-Legends-Script\\Release\\AutoInjector.dll";
	LPWSTR ProcessName = (LPWSTR)L"Blitz.exe";

	ifstream in(OriginalDll, fstream::binary);
	ofstream out(CustomDll, fstream::binary);

	out << in.rdbuf();
	in.close();
	out.close();

	Sleep(1000);

	PROCESSENTRY32 entry;

	DWORD Pid;
	HANDLE hProcess;
	LPVOID pDllPath;
	HANDLE hLoadThread;

	entry.dwSize = sizeof(PROCESSENTRY32);
	while (!injected) {
		startLoop:
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (Process32First(snapshot, &entry) == TRUE)
		{
			if (wcscmp(entry.szExeFile, ProcessName) == 0)
			{
				Pid = entry.th32ProcessID;
			}
			while (Process32Next(snapshot, &entry) == TRUE)
			{
				if (wcscmp(entry.szExeFile, ProcessName) == 0)
				{
					Pid = entry.th32ProcessID;
					hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pid);
					if (hProcess) {
						pDllPath = VirtualAllocEx(hProcess, 0, strlen(DllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
						if (pDllPath) {
							if (WriteProcessMemory(hProcess, pDllPath, (LPVOID)DllPath, strlen(DllPath) + 1, 0)) {
								Sleep(1000);
								hLoadThread = CreateRemoteThread(hProcess, 0, 0,
									(LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32.dll"),
										"LoadLibraryA"), pDllPath, 0, 0);
								if (hLoadThread) {
									WaitForSingleObject(hLoadThread, INFINITE);
									std::cout << "Dll path allocated at: " << std::hex << pDllPath << std::endl;
									VirtualFreeEx(hProcess, pDllPath, strlen(DllPath) + 1, MEM_RELEASE);
									injected = true;
									Sleep(2000);

									//SendMessageA(FindWindowW(NULL, L"Blitz"), WM_CLOSE, NULL, NULL);
									break;
								}
							}
							else {
								cout << "Can't write dll path to process." << endl;
								cin.get();
							}
						}
						else {
							cout << "Can't allocate memory to process." << endl;
							cin.get();
						}
					}
					else {
						cout << "Can't get handle to process." << endl;
						cin.get();
					}
				}
			}
			if (!injected) {
				startup(BlitzPath);
				Sleep(3000);
				while (!FindWindowW(NULL, L"Blitz")) {}
				goto startLoop;
			}
		}
		CloseHandle(snapshot);
	}

	return 0;
}

VOID startup(LPCTSTR lpApplicationName)
{
	// additional information
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	// set the size of the structures
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// start the program up
	CreateProcessW(lpApplicationName,   // the path
		NULL,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	);
	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}