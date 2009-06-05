#include "stdafx.h"
#include "DWAPIHooks.h"
#include "detours/detours.h"


static wchar_t g_lpszCurrentOpenSubKeyW[2048] = { 0 };
static char g_lpszCurrentOpenSubKeyA[2048] = { 0 };

//----------------------------------------------------------------
//
// IE 6				
//
static LONG (WINAPI * OldRegOpenKeyExA)(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult) = RegOpenKeyExA; 
LONG WINAPI SogouRegOpenKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
{
    if (lpSubKey)
        StrCpyA(g_lpszCurrentOpenSubKeyA, lpSubKey);
    return OldRegOpenKeyExA(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}

static LONG (WINAPI * OldRegQueryValueExA)(HKEY hKey,  LPCSTR lpValueName,  LPDWORD lpReserved,  LPDWORD lpType,  LPBYTE lpData,  LPDWORD lpcbData) = RegQueryValueExA; 
LONG WINAPI SogouRegQueryValueExA(HKEY hKey,  LPCSTR lpValueName,  LPDWORD lpReserved,  LPDWORD lpType,  LPBYTE lpData,  LPDWORD lpcbData)
{
    LONG lRet = OldRegQueryValueExA(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);

    // 优化 http 连接数
    if (lpValueName && lpData && StrCmpIA(lpValueName, "MaxConnectionsPerServer") == 0)
    {
        if (*lpData < 6)
            *lpData = 6;
        return lRet;
    }
    if (lpValueName && lpData && StrCmpIA(lpValueName, "MaxConnectionsPer1_0Server") == 0)
    {
        if (*lpData < 6)
            *lpData = 6;
        return lRet;
    }

    // 处理脚本调试对话框，不让其弹出来。
    if (lpValueName && lpData && lpcbData && (StrCmpIA(lpValueName, "DisableScriptDebuggerIE") == 0 || StrCmpIA(lpValueName, "Disable Script Debugger") == 0))
    {
        StrCpyA((char *)lpData, "yes");
        *lpcbData = 3;
        return lRet;
    }

    // 处理广告拦截，欺骗 se 广告拦截的状态，这样可以在不修改注册表的情况下，达到利用 ie 的广告拦截 
    if (lpValueName && lpData && lpcbData && StrCmpIA(lpValueName, "PopupMgr") == 0)
    {
        StrCpyA((char *)lpData, "yes");
        *lpcbData = 3;
        return lRet;
    }

    // 处理 ActiveX 控件拦截
    static const char * lpszRegPathActiveXPrompt = "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3";
    static const int nLenRegPathActiveXPrompt = lstrlenA(lpszRegPathActiveXPrompt);
    if (lpValueName && lpData && StrCmpIA(lpValueName, "2201") == 0 && StrCmpNA(g_lpszCurrentOpenSubKeyA, lpszRegPathActiveXPrompt, nLenRegPathActiveXPrompt) == 0)
    {
        DWORD *lpDword = (DWORD *)lpData;
        *lpDword = 3;
        return lRet;
    }

    return lRet;
}

static LONG (WINAPI * OldRegOpenKeyExW)(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult) = RegOpenKeyExW; 
LONG WINAPI SogouRegOpenKeyExW(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
{
    if (lpSubKey)
        StrCpy(g_lpszCurrentOpenSubKeyW, lpSubKey);
    return OldRegOpenKeyExW(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}

//----------------------------------------------------------------
//
//	IE 7			
//
static LONG (WINAPI * OldRegQueryValueExW)(HKEY hKey,  LPCWSTR lpValueName,  LPDWORD lpReserved,  LPDWORD lpType,  LPBYTE lpData,  LPDWORD lpcbData) = RegQueryValueExW; 
LONG WINAPI SogouRegQueryValueExW(HKEY hKey,  LPCWSTR lpValueName,  LPDWORD lpReserved,  LPDWORD lpType,  LPBYTE lpData,  LPDWORD lpcbData)
{
    LONG lRet = OldRegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);

    // 优化 http 连接数
    if (lpValueName && lpData && StrCmpI(lpValueName, L"MaxConnectionsPerServer") == 0)
    {
        if (*lpData < 6)
            *lpData = 6;
        return lRet;
    }

    if (lpValueName && lpData && StrCmpI(lpValueName, L"MaxConnectionsPer1_0Server") == 0)
    {
        if (*lpData < 6)
            *lpData = 6;
        return lRet;
    }

    // 处理脚本调试对话框，不让其弹出来。
    if (lpValueName && lpData && lpcbData && (StrCmpI(lpValueName, L"DisableScriptDebuggerIE") == 0 || StrCmpI(lpValueName, L"Disable Script Debugger") == 0))
    {
        StrCpy((WCHAR *)lpData, L"yes");
        *lpcbData = 3 * sizeof(WCHAR);
        return lRet;
    }

    // 处理广告拦截，欺骗 se 广告拦截的状态，这样可以在不修改注册表的情况下，达到利用 ie 的广告拦截 
    if (lpValueName && lpData && lpcbData && StrCmpI(lpValueName, L"PopupMgr") == 0)
    {
        StrCpy((WCHAR *)lpData, L"yes");
        *lpcbData = 3 * sizeof(WCHAR);
        return lRet;
    }

    // 处理 ActiveX 控件拦截
    static const TCHAR * lpszRegPathActiveXPrompt = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3");
    if (lpValueName && lpData && StrCmpI(lpValueName, L"2201") == 0 && StrCmp(g_lpszCurrentOpenSubKeyW, lpszRegPathActiveXPrompt) == 0)
    {
        DWORD *lpDword = (DWORD *)lpData;
        *lpDword = 3;
        return lRet;
    }

    return lRet;
}



CDWAPIHooks::CDWAPIHooks()
{
    LONG lRes = ::DetourTransactionBegin();
    ATLASSERT(lRes == NO_ERROR);

    lRes = ::DetourUpdateThread(::GetCurrentThread());
    ATLASSERT(lRes == NO_ERROR);

    lRes = ::DetourAttach((PVOID*)&OldRegOpenKeyExA, SogouRegOpenKeyExA);
    ATLASSERT(lRes == NO_ERROR);
    lRes = ::DetourAttach((PVOID*)&OldRegQueryValueExA, SogouRegQueryValueExA);
    ATLASSERT(lRes == NO_ERROR);
    lRes = ::DetourAttach((PVOID*)&OldRegOpenKeyExW, SogouRegOpenKeyExW);
    ATLASSERT(lRes == NO_ERROR);
    lRes = ::DetourAttach((PVOID*)&OldRegQueryValueExW, SogouRegQueryValueExW);
    ATLASSERT(lRes == NO_ERROR);

    lRes = ::DetourTransactionCommit();
    ATLASSERT(lRes == NO_ERROR);

}
CDWAPIHooks::~CDWAPIHooks()
{

}
