// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <stdlib.h>
#include <psapi.h>
#include <tchar.h>

#define SAFE_PROCESS TEXT("chrome.exe")


BOOL shouldPause = FALSE;

DWORD WINAPI annoyingThread(LPVOID lpParam) 
{
    while (1)
    {
        if (!shouldPause) 
        {
            POINT point;
            GetCursorPos(&point);
            SetCursorPos(point.x + 10, point.y);
        }
        Sleep(1000);
    }
    return 0;
}
BOOL getForegroundProcessName(TCHAR* processName)
{
    HWND hWindow;
    HMODULE hModule;
    DWORD cbNeeded, pid = 0;
    BOOL result = FALSE;
    HANDLE hProcess;

    hWindow = GetForegroundWindow();
    if (!hWindow)
    {
        return FALSE;
    }
    
    GetWindowThreadProcessId(hWindow, &pid);
    if (!pid)
    {
        return FALSE;
    }

    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!hProcess)
    {
        return FALSE;
    }

    if (EnumProcessModules(hProcess, &hModule, sizeof(hModule), &cbNeeded))
    {
        if (GetModuleBaseName(hProcess, hModule, processName, MAX_PATH))
        {
            result = TRUE;
        }
    }

    CloseHandle(hProcess);
    return result;
}

DWORD WINAPI monitorThread(LPVOID lpParam) {
    while (1) 
    {
        TCHAR processName[MAX_PATH];
        if (getForegroundProcessName(processName)) 
        {
            if (_tcsicmp(processName, SAFE_PROCESS) == 0)
            {
                shouldPause = TRUE;
            }
            else 
            {
                shouldPause = FALSE;
            }
        }
        Sleep(300); 
    }
    return 0;
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, 0, annoyingThread, NULL, 0, NULL);
        CreateThread(NULL, 0, monitorThread, NULL, 0, NULL);
        break;
    }
    return TRUE;
}
/* switch (ul_reason_for_call)
 {
 case DLL_PROCESS_ATTACH:
 case DLL_THREAD_ATTACH:
 case DLL_THREAD_DETACH:
 case DLL_PROCESS_DETACH:
     break;
 }*/
 //MessageBox(NULL, L"Ha Ha Ha",L"Ha",MB_OK);
