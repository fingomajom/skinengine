#include "StdAfx.h"
#include "CookieMgt.h"
#include "detours/detours.h"


BOOL (WINAPI * OldInternetSetCookieA)(
    __in LPCSTR lpszUrl,
    __in_opt LPCSTR lpszCookieName,
    __in LPCSTR lpszCookieData) = InternetSetCookieA;

BOOL (WINAPI * OldInternetSetCookieW)(
    __in LPCWSTR lpszUrl,
    __in_opt LPCWSTR lpszCookieName,
    __in LPCWSTR lpszCookieData) = InternetSetCookieW;

DWORD (WINAPI * OldInternetSetCookieExA)(
    __in LPCSTR lpszUrl,
    __in_opt LPCSTR lpszCookieName,
    __in LPCSTR lpszCookieData,
    __in DWORD dwFlags,
    __null DWORD_PTR dwReserved) = InternetSetCookieExA;

DWORD (WINAPI * OldInternetSetCookieExW)(
    __in LPCWSTR lpszUrl,
    __in_opt LPCWSTR lpszCookieName,
    __in LPCWSTR lpszCookieData,
    __in DWORD dwFlags,
    __null DWORD_PTR dwReserved) = InternetSetCookieExW;


BOOL (WINAPI * OldInternetGetCookieA)(
    __in LPCSTR lpszUrl,
    __in_opt LPCSTR lpszCookieName,
    __out_ecount_opt(*lpdwSize) LPSTR lpszCookieData,
    __inout LPDWORD lpdwSize) = InternetGetCookieA;

BOOL (WINAPI * OldInternetGetCookieW)(
    __in LPCWSTR lpszUrl,
    __in_opt LPCWSTR lpszCookieName,
    __out_ecount_opt(*lpdwSize) LPWSTR lpszCookieData,
    __inout LPDWORD lpdwSize) = InternetGetCookieW;

BOOL (WINAPI * OldInternetGetCookieExA)(
    __in LPCSTR lpszUrl,
    __in_opt LPCSTR lpszCookieName,
    __in_ecount_opt(*lpdwSize) LPSTR lpszCookieData,
    __inout_opt LPDWORD lpdwSize,
    __in DWORD dwFlags,
    __null LPVOID lpReserved) = InternetGetCookieExA;

BOOL (WINAPI * OldInternetGetCookieExW)(
    __in LPCWSTR lpszUrl,
    __in_opt LPCWSTR lpszCookieName,
    __in_ecount_opt(*lpdwSize) LPWSTR lpszCookieData,
    __inout_opt LPDWORD lpdwSize,
    __in DWORD dwFlags,
    __null LPVOID lpReserved) = InternetGetCookieExW;



BOOL WINAPI MyInternetSetCookieA(
    __in LPCSTR lpszUrl,
    __in_opt LPCSTR lpszCookieName,
    __in LPCSTR lpszCookieData)
{
    ATL::CStringA strCookieData;

    MessageBoxA(GetActiveWindow(), lpszCookieData, lpszUrl, MB_OK);

    return OldInternetSetCookieA( lpszUrl, lpszCookieName, lpszCookieData );
}

BOOL WINAPI MyInternetSetCookieW(
    __in LPCWSTR lpszUrl,
    __in_opt LPCWSTR lpszCookieName,
    __in LPCWSTR lpszCookieData)
{
    ATL::CStringW strCookieData;

    MessageBox(GetActiveWindow(), lpszCookieData, lpszUrl, MB_OK);

    return OldInternetSetCookieW( lpszUrl, lpszCookieName, lpszCookieData );
}

DWORD WINAPI MyInternetSetCookieExA(
    __in LPCSTR lpszUrl,
    __in_opt LPCSTR lpszCookieName,
    __in LPCSTR lpszCookieData,
    __in DWORD dwFlags,
    __null DWORD_PTR dwReserved)
{
    ATL::CStringA strCookieData;

    MessageBoxA(GetActiveWindow(), lpszCookieData, lpszUrl, MB_OK);

    return OldInternetSetCookieExA(
        lpszUrl,
        lpszCookieName,
        lpszCookieData,
        dwFlags,
        dwReserved);
}

DWORD WINAPI MyInternetSetCookieExW(
    __in LPCWSTR lpszUrl,
    __in_opt LPCWSTR lpszCookieName,
    __in LPCWSTR lpszCookieData,
    __in DWORD dwFlags,
    __null DWORD_PTR dwReserved)
{
    ATL::CStringW strCookieData;

    MessageBox(GetActiveWindow(), lpszCookieData, lpszUrl, MB_OK);

    return OldInternetSetCookieExW(
        lpszUrl,
        lpszCookieName,
        lpszCookieData,
        dwFlags,
        dwReserved);
}

BOOL WINAPI MyInternetGetCookieA(
    __in LPCSTR lpszUrl,
    __in_opt LPCSTR lpszCookieName,
    __out_ecount_opt(*lpdwSize) LPSTR lpszCookieData,
    __inout LPDWORD lpdwSize)
{
    ATL::CStringW strCookieData;

    MessageBoxA(GetActiveWindow(), lpszCookieData, lpszUrl, MB_OK);

    return OldInternetGetCookieA(
        lpszUrl,
        lpszCookieName,
        lpszCookieData,
        lpdwSize);
}

BOOL WINAPI MyInternetGetCookieW(
    __in LPCWSTR lpszUrl,
    __in_opt LPCWSTR lpszCookieName,
    __out_ecount_opt(*lpdwSize) LPWSTR lpszCookieData,
    __inout LPDWORD lpdwSize)
{
    ATL::CStringW strCookieData;

    MessageBox(GetActiveWindow(), lpszCookieData, lpszUrl, MB_OK);

    return OldInternetGetCookieW(
        lpszUrl,
        lpszCookieName,
        lpszCookieData,
        lpdwSize);
}

BOOL WINAPI MyInternetGetCookieExA(
    __in LPCSTR lpszUrl,
    __in_opt LPCSTR lpszCookieName,
    __in_ecount_opt(*lpdwSize) LPSTR lpszCookieData,
    __inout_opt LPDWORD lpdwSize,
    __in DWORD dwFlags,
    __null LPVOID lpReserved)
{
    ATL::CStringW strCookieData;

    MessageBoxA(GetActiveWindow(), lpszCookieData, lpszUrl, MB_OK);


    return OldInternetGetCookieExA(
        lpszUrl,
        lpszCookieName,
        lpszCookieData,
        lpdwSize,
        dwFlags,
        lpReserved);
}

BOOL WINAPI MyInternetGetCookieExW(
    __in LPCWSTR lpszUrl,
    __in_opt LPCWSTR lpszCookieName,
    __in_ecount_opt(*lpdwSize) LPWSTR lpszCookieData,
    __inout_opt LPDWORD lpdwSize,
    __in DWORD dwFlags,
    __null LPVOID lpReserved)
{
    ATL::CStringW strCookieData;

    MessageBox(GetActiveWindow(), lpszCookieData, lpszUrl, MB_OK);

    return OldInternetGetCookieExW(
        lpszUrl,
        lpszCookieName,
        lpszCookieData,
        lpdwSize,
        dwFlags,
        lpReserved);
}




CCookieMgt::CCookieMgt(void)
{
    //DetourTransactionBegin();
    //DetourUpdateThread(GetCurrentThread());

    //DetourAttach((void**)&OldInternetSetCookieA  , MyInternetSetCookieA);
    //DetourAttach((void**)&OldInternetSetCookieW  , MyInternetSetCookieW);
    //DetourAttach((void**)&OldInternetSetCookieExW, MyInternetSetCookieExW);
    //DetourAttach((void**)&OldInternetSetCookieExW, MyInternetSetCookieExW);

    //DetourAttach((void**)&OldInternetGetCookieA  , MyInternetGetCookieA);
    //DetourAttach((void**)&OldInternetGetCookieW  , MyInternetGetCookieW);
    //DetourAttach((void**)&OldInternetGetCookieExA, MyInternetGetCookieExA);
    //DetourAttach((void**)&OldInternetGetCookieExW, MyInternetGetCookieExW);

    //OldInternetSetCookieA("http://urlspirit.spiritsoft.cn/urlcore/admin/index.php", "", "logpwd=lichenglin%^**;domain=spiritsoft.cn;path=/;");

    //DetourTransactionCommit();
}

CCookieMgt::~CCookieMgt(void)
{
    //DetourTransactionBegin();
    //DetourUpdateThread(GetCurrentThread());

    //DetourDetach((void**)&OldInternetSetCookieA  , MyInternetSetCookieA);
    //DetourDetach((void**)&OldInternetSetCookieW  , MyInternetSetCookieW);
    //DetourDetach((void**)&OldInternetSetCookieExW, MyInternetSetCookieExW);
    //DetourDetach((void**)&OldInternetSetCookieExW, MyInternetSetCookieExW);

    //DetourDetach((void**)&OldInternetGetCookieA  , MyInternetGetCookieA);
    //DetourDetach((void**)&OldInternetGetCookieW  , MyInternetGetCookieW);
    //DetourDetach((void**)&OldInternetGetCookieExA, MyInternetGetCookieExA);
    //DetourDetach((void**)&OldInternetGetCookieExW, MyInternetGetCookieExW);
    //DetourTransactionCommit();
}

CCookieMgt& CCookieMgt::Instance()
{
    static CCookieMgt* __CCookieMgt_Instance__ = new CCookieMgt();
    return *__CCookieMgt_Instance__;
}

