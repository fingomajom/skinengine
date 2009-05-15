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


//WINUSERAPI
//LRESULT
//WINAPI
//SendMessageA(
//             __in HWND hWnd,
//             __in UINT Msg,
//             __in WPARAM wParam,
//             __in LPARAM lParam);

LRESULT (WINAPI * OldSendMessageW)(
    __in HWND hWnd,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam) = SendMessageW;

LRESULT WINAPI MySendMessageW(
    __in HWND hWnd,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam)
{
    DWORD dwPID = 0;

    if ( GetWindowThreadProcessId(hWnd, &dwPID) && dwPID != GetCurrentProcessId() )
        return ::PostMessageW(hWnd, Msg, wParam, lParam);

    return OldSendMessageW( hWnd, Msg, wParam, lParam );
}

LRESULT (WINAPI * OldSendMessageA)(
    __in HWND hWnd,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam) = SendMessageA;

LRESULT WINAPI MySendMessageA(
    __in HWND hWnd,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam)
{
    DWORD dwPID = 0;

    if ( GetWindowThreadProcessId(hWnd, &dwPID)  && dwPID != GetCurrentProcessId() )
        return ::PostMessageA(hWnd, Msg, wParam, lParam);

    return OldSendMessageA( hWnd, Msg, wParam, lParam );
}


LRESULT (WINAPI * OldCallWindowProcA)(
    __in WNDPROC lpPrevWndFunc,
    __in HWND hWnd,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam) = CallWindowProcA;

LRESULT (WINAPI * OldCallWindowProcW)(
    __in WNDPROC lpPrevWndFunc,
    __in HWND hWnd,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam) = CallWindowProcW;


LRESULT WINAPI MyCallWindowProcA(
    __in WNDPROC lpPrevWndFunc,
    __in HWND hWnd,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam)
{
    DWORD dwPID = 0;

    //if ( GetWindowThreadProcessId(hWnd, &dwPID) && dwPID != GetCurrentProcessId() )
    //    return ::PostMessageA(hWnd, Msg, wParam, lParam);
    if ( GetWindowThreadProcessId(hWnd, NULL) != GetCurrentThreadId() )
        return ::PostMessageA(hWnd, Msg, wParam, lParam);

    return OldCallWindowProcA( lpPrevWndFunc, hWnd, Msg, wParam, lParam );
}

LRESULT WINAPI MyCallWindowProcW(
    __in WNDPROC lpPrevWndFunc,
    __in HWND hWnd,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam)
{
    DWORD dwPID = 0;

    //if ( GetWindowThreadProcessId(hWnd, &dwPID) && dwPID != GetCurrentProcessId() )
    //    return ::PostMessageW(hWnd, Msg, wParam, lParam);
    if ( GetWindowThreadProcessId(hWnd, NULL) != GetCurrentThreadId() )
        return ::PostMessageA(hWnd, Msg, wParam, lParam);

    TCHAR szBuf[50];
    GetClassName(hWnd, szBuf, 49);
    if ( StrStrI(szBuf, L"DWExplorer_DWChildFrm") )
    {
        switch (Msg)
        {
        case WM_WINDOWPOSCHANGED:
        case WM_WINDOWPOSCHANGING:
            return 0;
        }
    }

    return OldCallWindowProcW( lpPrevWndFunc, hWnd, Msg, wParam, lParam );
}


CDWCrashMgt::CDWCrashMgt(void)
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    DetourAttach((void**)&OldCreateThread  , MyCreateThread);
    //DetourAttach((void**)&OldIsChild       , MyIsChild);
    //DetourAttach((void**)&OldSendMessageW  , MySendMessageW);
    //DetourAttach((void**)&OldSendMessageA  , MySendMessageA);
    //DetourAttach((void**)&OldCallWindowProcA  , MyCallWindowProcA);
    //DetourAttach((void**)&OldCallWindowProcW  , MyCallWindowProcW);

    DetourTransactionCommit();
}

CDWCrashMgt::~CDWCrashMgt(void)
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    DetourDetach((void**)&OldCreateThread  , MyCreateThread);
    //DetourDetach((void**)&OldIsChild  , MyIsChild);
    //DetourDetach((void**)&OldSendMessageW  , MySendMessageW);
    //DetourDetach((void**)&OldSendMessageA  , MySendMessageA);
    //DetourDetach((void**)&OldCallWindowProcA  , MyCallWindowProcA);
    //DetourDetach((void**)&OldCallWindowProcW  , MyCallWindowProcW);

    DetourTransactionCommit();
}

