#include "StdAfx.h"
#include "ProtectDevC.h"

CProtectDevC::CProtectDevC(void) :
    m_hDevcHandle(INVALID_HANDLE_VALUE)
{
}

CProtectDevC::~CProtectDevC(void)
{
}


BOOL __stdcall CProtectDevC::InitDevC()
{
    if (m_hDevcHandle != INVALID_HANDLE_VALUE)
        return TRUE;

    DWORD	DesiredAccess;
    DWORD	ShareMode;
    LPSECURITY_ATTRIBUTES	lpSecurityAttributes = NULL;

    DWORD	CreationDistribution;
    DWORD	FlagsAndAttributes;
    HANDLE	TemplateFile;

    BOOL	 bResult = FALSE;


    DesiredAccess = MAXIMUM_ALLOWED;//GENERIC_READ|GENERIC_WRITE;
    ShareMode = 0;
    CreationDistribution = OPEN_EXISTING;
    FlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
    TemplateFile = NULL; //(HANDLE)INVALID_HANDLE_VALUE;
    lpSecurityAttributes = NULL;

    m_hDevcHandle = CreateFile(
        L"\\\\.\\" PTTDRV_DEVICE_NAME ,
        DesiredAccess,
        ShareMode,
        lpSecurityAttributes,
        CreationDistribution,
        FlagsAndAttributes,
        TemplateFile
        );

    if( m_hDevcHandle == INVALID_HANDLE_VALUE )
    {
        m_hDevcHandle = CreateFile(
            L"\\\\.\\Globals\\" PTTDRV_DEVICE_NAME ,
            DesiredAccess,
            ShareMode,
            lpSecurityAttributes,
            CreationDistribution,
            FlagsAndAttributes,
            TemplateFile
            );
    }

    if (m_hDevcHandle == NULL || m_hDevcHandle == INVALID_HANDLE_VALUE)
    {
        //return StartResetDriver();

        goto Exit0;
    }

    return TRUE;

Exit0:

    UninitDevC();

    return FALSE;
}


void __stdcall CProtectDevC::UninitDevC()
{
    if (m_hDevcHandle != NULL && m_hDevcHandle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hDevcHandle);
        m_hDevcHandle = INVALID_HANDLE_VALUE;
    }

}


unsigned int __stdcall CProtectDevC::GetDriverVersion()
{
    int nResult = 0;

    if (m_hDevcHandle == INVALID_HANDLE_VALUE)
    {
        return GetDllVersion();
    }

    BOOL  bResult   = FALSE;
    DWORD dwVersion = 0;
    DWORD dwBuffLen = sizeof(DWORD);

    bResult = DeviceIoControl(
        m_hDevcHandle,
        IOCTL_PTTDRV_GET_VERSION,
        NULL,
        0,
        &dwVersion,
        dwBuffLen,
        &dwBuffLen,
        NULL
        );

    if (bResult)
        nResult = dwVersion;

    return nResult;
}


unsigned int __stdcall CProtectDevC::GetDllVersion()
{
    return PTTDRV_DRIVER_VERSION_1;
}


int __stdcall CProtectDevC::GetStatus()
{
    int nResult = DRIVER_STATUS_INVALID;

    if (m_hDevcHandle == INVALID_HANDLE_VALUE)
        return nResult;

    BOOL  bResult   = FALSE;
    int   nValue    = 0;
    DWORD dwBuffLen = sizeof(DWORD);

    bResult = DeviceIoControl(
        m_hDevcHandle,
        IOCTL_PTTDRV_GET_STATUS,
        NULL,
        0,
        &nValue,
        dwBuffLen,
        &dwBuffLen,
        NULL
        );

    if (bResult)
        nResult = nValue;

    return nResult;
}

BOOL __stdcall CProtectDevC::SetStatus(int nStatus)
{
    BOOL  bResult = FALSE;

    if (m_hDevcHandle == INVALID_HANDLE_VALUE)
        return bResult;

    int   nValue    = nStatus;
    DWORD dwBuffLen = sizeof(DWORD);

    bResult = DeviceIoControl(
        m_hDevcHandle,
        IOCTL_PTTDRV_SET_STATUS,
        &nStatus,
        sizeof(int),
        &nValue,
        dwBuffLen,
        &dwBuffLen,
        NULL
        );

    return bResult;
}


BOOL __stdcall CProtectDevC::AppendRule( LP_DRIVER_RULE_INFO RuleInfo )
{
    BOOL bResult = FALSE;

    DWORD dwBuffLen = sizeof(DRIVER_RULE_INFO);

    bResult = DeviceIoControl(
        m_hDevcHandle,
        IOCTL_PTTDRV_APPEND_RULE_INFO,
        RuleInfo,
        sizeof(DRIVER_RULE_INFO),
        RuleInfo,
        dwBuffLen,
        &dwBuffLen,
        NULL
        );

    return bResult;    
}

BOOL __stdcall CProtectDevC::ClearRule( UINT uRuleType )
{
    BOOL bResult = FALSE;

    DWORD dwBuffLen = sizeof(uRuleType);

    bResult = DeviceIoControl(
        m_hDevcHandle,
        IOCTL_PTTDRV_CLEAR_RULE,
        &uRuleType,
        sizeof(uRuleType),
        &uRuleType,
        dwBuffLen,
        &dwBuffLen,
        NULL
        );

    return bResult;    
}
