// lsesvr.cpp : Implementation of WinMain


#include "stdafx.h"
#include "resource.h"
#include "lsesvrmodule.h"


CLSESvrModule _AtlModule;


LONG WINAPI LSEUnhandledExceptionFilter(
    __in struct _EXCEPTION_POINTERS *ExceptionInfo );


extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR /*lpCmdLine*/, int nShowCmd)
{
    int nResult = E_FAIL;

#ifndef _DEBUG
    SetUnhandledExceptionFilter( LSEUnhandledExceptionFilter ); 
#endif

    nResult = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    ATLASSERT( SUCCEEDED(nResult) );

    nResult = _AtlModule.WinMain(nShowCmd);

    CoUninitialize();

#ifndef _DEBUG
    SetUnhandledExceptionFilter( NULL ); 
#endif

    return nResult;
}



//////////////////////////////////////////////////////////////////////////
// Òì³£¹ýÂËÆ÷
//////////////////////////////////////////////////////////////////////////
LONG WINAPI LSEUnhandledExceptionFilter(
    __in struct _EXCEPTION_POINTERS *ExceptionInfo )
{
    MessageBox( NULL, _T("LSE ±ÀÀ£Äñ..."), _T("³ö´íÀ²£¡£¡"), MB_OK );

    return EXCEPTION_EXECUTE_HANDLER;
}

