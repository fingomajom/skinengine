// lsectl.cpp : Implementation of DLL Exports.


#include "stdafx.h"
#include "resource.h"
#include "..\public\lsectl.h"
#include "SvrCaller.h"


#ifdef _MANAGED
#pragma managed(push, off)
#endif

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{

    switch ( dwReason )
    {
    case DLL_PROCESS_ATTACH:
        CoInitialize( NULL );
        break;
    case DLL_PROCESS_DETACH:
        CoUninitialize();
        break;
    }
    return TRUE;
    //return _AtlModule.DllMain(dwReason, lpReserved); 
}

#ifdef _MANAGED
#pragma managed(pop)
#endif


STDAPI DllCanUnloadNow(void)
{
    return S_OK;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    if ( ppv != NULL && riid == __uuidof(ILSECaller) )
    {
        ILSECaller* piSvrCaller = new CSvrCaller();

        *ppv = piSvrCaller;

        return *ppv != NULL ? S_OK : E_FAIL;
    }

    return S_OK;
}

