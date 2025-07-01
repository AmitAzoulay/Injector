#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>

#define MAX_THREADS_NUM 50
#define DLL_PATH TEXT("C:\\Hachshara\\Dll\\x64\\Debug\\Dll.dll")
#ifdef UNICODE
#define LOAD_LIBRARY_VERSION "LoadLibraryW"
#else
#define LOAD_LIBRARY_VERSION "LoadLibraryA"
#endif


INT _tmain(INT argc, TCHAR** argv)
{
	HANDLE processHandle = NULL, threadHandle = NULL;
	DWORD pid = 0, tid = 0;
	INT dllPathLen = 0;
	LPVOID ptrBuffer = NULL,loadLibaryPtr = NULL;

	_tprintf(TEXT("Enter pid:"));
	_tscanf_s(TEXT("%lu"), &pid);

	processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (processHandle == NULL)
	{
		_tprintf(TEXT("Cannot open process\n"));
		return 1;
	}

	dllPathLen = _tcslen(DLL_PATH) * sizeof(TCHAR) + sizeof(TCHAR);
	ptrBuffer = VirtualAllocEx(processHandle, NULL, dllPathLen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	
	if (ptrBuffer == NULL)
	{
		_tprintf(TEXT("Cannot allocate memory in the process\n"));
		return 1;
	}

	if (!WriteProcessMemory(processHandle, ptrBuffer, DLL_PATH, dllPathLen, NULL))
	{
		_tprintf(TEXT("Cannot write to the process memory\n"));
		return 1;
	}

	loadLibaryPtr = (LPVOID)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), LOAD_LIBRARY_VERSION);
	threadHandle = CreateRemoteThread(processHandle, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibaryPtr, ptrBuffer, 0, &tid);
	if (threadHandle == NULL)
	{
		_tprintf(TEXT("Cannot create remote thread\n"));
	}
	else
	{
		_tprintf(TEXT("Remote thread created\n"));
	}




	return 0;
}

