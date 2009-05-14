#include "StdAfx.h"
#include "DWCrashMgt.h"
#include "detours/detours.h"


DWORD WINAPI SafeThreadStartAddress( LPVOID lpThreadParameter )
{
    LPVOID* lpNewParameter = (LPVOID*)lpThreadParameter;
    ATLASSERT( lpNewParameter != NULL );
    if ( lpNewParameter == NULL )
        return 0;

    LPTHREAD_START_ROUTINE lpStartAddress = (LPTHREAD_START_ROUTINE)lpNewParameter[0];
    LPVOID                 lpParameter    = lpNewParameter[1];

    DWORD dwRet = 0;
    
#ifndef _DEBUG
    __try
    {
        dwRet = lpStartAddress(lpParameter);
    }
    __except(1)
    {
    }
#else
    dwRet = lpStartAddress(lpParameter);
#endif

    return dwRet;
}


HANDLE ( WINAPI * OldCreateThread )(
    __in_opt  LPSECURITY_ATTRIBUTES lpThreadAttributes,
    __in      SIZE_T dwStackSize,
    __in      LPTHREAD_START_ROUTINE lpStartAddress,
    __in_opt  LPVOID lpParameter,
    __in      DWORD dwCreationFlags,
    __out_opt LPDWORD lpThreadId) = CreateThread;


HANDLE WINAPI MyCreateThread(
    __in_opt  LPSECURITY_ATTRIBUTES lpThreadAttributes,
    __in      SIZE_T dwStackSize,
    __in      LPTHREAD_START_ROUTINE lpStartAddress,
    __in_opt  LPVOID lpParameter,
    __in      DWORD dwCreationFlags,
    __out_opt LPDWORD lpThreadId)
{
    LPVOID* lpNewParameter = new LPVOID[2];
    if ( lpNewParameter != NULL )
    {
        lpNewParameter[0] = lpStartAddress;
        lpNewParameter[1] = lpParameter;
        
        lpStartAddress = SafeThreadStartAddress;
        lpParameter    = lpNewParameter;
    }

    return OldCreateThread( lpThreadAttributes,
        dwStackSize,
        lpStartAddress,
        lpParameter,
        dwCreationFlags,
        lpThreadId);
}

BOOL ( WINAPI * OldIsChild)(
        __in HWND hWndParent,
        __in HWND hWnd) = IsChild;


BOOL WINAPI MyIsChild(
    __in HWND hWndParent,
    __in HWND hWnd)
{
    return OldIsChild(hWndParent, hWnd);
}


HWND (WINAPI * OldGetWindow)(
  __in HWND hWnd,
  __in UINT uCmd) = GetWindow;

HWND WINAPI MyGetWindow(
    __in HWND hWnd,
    __in UINT uCmd)
{
    return OldGetWindow(hWnd, uCmd);
}

CDWCrashMgt::CDWCrashMgt(void)
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    DetourAttach((void**)&OldCreateThread  , MyCreateThread);
    DetourAttach((void**)&OldIsChild    , MyIsChild);
    DetourAttach((void**)&OldGetWindow  , MyGetWindow);

    DetourTransactionCommit();
}

CDWCrashMgt::~CDWCrashMgt(void)
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    DetourDetach((void**)&OldCreateThread  , MyCreateThread);
    DetourDetach((void**)&OldIsChild  , MyIsChild);
    DetourDetach((void**)&OldGetWindow  , MyGetWindow);

    DetourTransactionCommit();
}

