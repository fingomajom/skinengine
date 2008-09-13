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
    SetUnhandledExceptionFilter( LSEUnhandledExceptionFilter ); 

    return _AtlModule.WinMain(nShowCmd);

    SetUnhandledExceptionFilter( NULL ); 
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

