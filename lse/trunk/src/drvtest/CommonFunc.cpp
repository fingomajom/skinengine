#include "StdAfx.h"
#include "CommonFunc.h"
#include <winsvc.h>
#include "shlwapi.h"

#pragma comment(lib, "shlwapi.lib")



BOOL InstallDriver( LPCTSTR pszDriverName )
{
    BOOL  bResult = FALSE;
    DWORD dwError = 0;

    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService   = NULL;

    TCHAR szExeFile[MAX_PATH] = { 0 };

    GetModuleFileName( NULL, szExeFile, MAX_PATH );
    PathRemoveFileSpec( szExeFile );
    PathAppend( szExeFile, _T("pttdrv.sys") );

    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    schService = CreateService(
        schSCManager, 
        pszDriverName, 
        pszDriverName, 
        SERVICE_ALL_ACCESS,
        SERVICE_KERNEL_DRIVER, 
        SERVICE_SYSTEM_START, 
        SERVICE_ERROR_NORMAL, 
        szExeFile, 
        NULL,
        NULL, 
        NULL, 
        NULL, 
        NULL);

    dwError = GetLastError();

    if (schService == NULL && dwError != 0173)
        return FALSE;

    bResult = StartService(schService, 0, NULL);
    dwError = GetLastError();

    bResult =  bResult || dwError == ERROR_SERVICE_ALREADY_RUNNING || dwError == ERROR_SERVICE_DISABLED;

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);

    return TRUE;
}


BOOL StartDriver( LPCTSTR pszDriverName )
{
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService   = NULL;

    BOOL  ret;
    DWORD dwError;

    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    schService = OpenService(schSCManager, pszDriverName, SERVICE_ALL_ACCESS);
    if (schService == NULL)
        return FALSE;

    ret = StartService(schService, 0, NULL);
    dwError = GetLastError();

    ret =  ret || dwError == ERROR_SERVICE_ALREADY_RUNNING || dwError == ERROR_SERVICE_DISABLED;

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);

    return ret;
}

/////////////////////////////////////////////////////////////////////////////

BOOL OpenDevice(LPCTSTR DriverName, HANDLE *lphDevice)
{
    TCHAR completeDeviceName[64];
    HANDLE hDevice;

    //if ((GetVersion() & 0xFF) >= 5)
    //    wsprintf(completeDeviceName, TEXT("\\\\.\\Global\\%s"), DriverName);
    //else
        wsprintf(completeDeviceName, TEXT("\\\\.\\%s"), DriverName);

    hDevice = CreateFile(
        completeDeviceName, 
        MAXIMUM_ALLOWED, 
        0, 
        NULL,
        OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL);

    if ( hDevice == INVALID_HANDLE_VALUE )
        return FALSE;

    if (lphDevice)
        *lphDevice = hDevice;
    else
        CloseHandle(hDevice);

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

BOOL StopDriver( LPCTSTR DriverName)
{
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService   = NULL;

    BOOL ret;
    SERVICE_STATUS serviceStatus;

    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);


    schService = OpenService(schSCManager, DriverName, SERVICE_ALL_ACCESS);
    if (schService == NULL)
        return FALSE;

    ret = ControlService(schService, SERVICE_CONTROL_STOP, &serviceStatus);

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);

    return ret;
}

/////////////////////////////////////////////////////////////////////////////

BOOL RemoveDriver( LPCTSTR DriverName )
{
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService   = NULL;
    BOOL ret;

    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);


    schService = OpenService(schSCManager, DriverName, SERVICE_ALL_ACCESS);
    if (schService == NULL)
        return FALSE;

    ret = DeleteService(schService);

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);

    return ret;
}

