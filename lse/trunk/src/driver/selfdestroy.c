#include "precomp.h"
#pragma hdrstop

DWORD g_dwErrorCount = 0;

#define RegRootPath      L"\\REGISTRY\\MACHINE\\SYSTEM\\CurrentControlSet\\Control\\" 
#define ValueErrorCount  L"DrvErrorCount"

DWORD GetErrorCount()
{
    DWORD dwResult[10] = { 0 };
    DWORD dwType   = 0;
    DWORD dwRetLen = 0;

    WCHAR szRegKey[MAX_PATH] = RegRootPath;


    wcsncat(szRegKey, PTTDRV_DEVICE_NAME, MAX_PATH);

    DrvGetValueKey(szRegKey, 
        ValueErrorCount, 
        &dwType,
        dwResult, 
        sizeof(dwResult),
        &dwRetLen);
    
    return dwResult[0];
}

void SetErrorCount( DWORD dwCount )
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    WCHAR szRegKey[MAX_PATH] = RegRootPath;

    wcsncat(szRegKey, PTTDRV_DEVICE_NAME, MAX_PATH);

    ntStatus = DrvSetValueKey( szRegKey, 
        ValueErrorCount, 
        REG_DWORD, 
        &dwCount, 
        sizeof(dwCount) );

    if ( ntStatus != STATUS_SUCCESS )
    {
        DrvCreateKey( szRegKey );

        DrvSetValueKey( szRegKey, 
            ValueErrorCount, 
            REG_DWORD, 
            &dwCount, 
            sizeof(dwCount) );
    }
}



BOOL IsSelfDestroy()
{
    g_dwErrorCount = GetErrorCount();

    if ( g_dwErrorCount >= MAX_ERROR_COUNT ) 
        return TRUE;

    return FALSE;
}

void NormalStart()
{
    SetErrorCount( g_dwErrorCount + 1 );
}

void NormalStop()
{
    SetErrorCount( 0 );
}
