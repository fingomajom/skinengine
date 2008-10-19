#include "precomp.h"
#pragma hdrstop

extern PULONG InitSafeBootMode;


DWORD g_dwErrorCount = 0;

#define RegRootPath      L"\\REGISTRY\\MACHINE\\SYSTEM\\CurrentControlSet\\Control\\" 
#define RegPTTPath       L"\\REGISTRY\\MACHINE\\SYSTEM\\CurrentControlSet\\Services\\" PTTDRV_DEVICE_NAME
#define ValueErrorCount  L"DrvErrorCount"

DWORD GetErrorCount()
{
    DWORD dwResult[10] = { 0 };
    DWORD dwType   = 0;
    DWORD dwRetLen = 0;

    DrvGetValueKey(RegRootPath PTTDRV_DEVICE_NAME, 
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

    ntStatus = DrvSetValueKey( RegRootPath PTTDRV_DEVICE_NAME, 
        ValueErrorCount, 
        REG_DWORD, 
        &dwCount, 
        sizeof(dwCount) );

    if ( ntStatus != STATUS_SUCCESS )
    {
        DrvCreateKey( RegRootPath PTTDRV_DEVICE_NAME );

        DrvSetValueKey( RegRootPath PTTDRV_DEVICE_NAME, 
            ValueErrorCount, 
            REG_DWORD, 
            &dwCount, 
            sizeof(dwCount) );
    }
}



BOOL IsSelfDestroy()
{
    if ( *InitSafeBootMode > 0 )
        return TRUE;

    g_dwErrorCount = GetErrorCount();

    if ( g_dwErrorCount >= MAX_ERROR_COUNT  ) 
    {
        return TRUE;
    }
    
    SetErrorCount( g_dwErrorCount + 1 );

    return FALSE;
}


void ResumeErrorCount()
{
    SetErrorCount( 0 );
}
