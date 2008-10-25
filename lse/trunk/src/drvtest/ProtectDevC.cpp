#include "StdAfx.h"
#include "ProtectDevC.h"

CProtectDevC::CProtectDevC(void) :
    m_hDevcHandle(INVALID_HANDLE_VALUE),
    m_piCallback(NULL),
    m_hReadEventThread(NULL),
    m_hExitEvent(NULL),
    m_hReportEvent(NULL)
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

    m_hExitEvent   = CreateEvent( NULL, FALSE, FALSE, NULL );
    m_hReportEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
    if ( m_hReportEvent == NULL || !SetReportEvent(m_hReportEvent) )
        goto Exit0;

    if ( m_piCallback != NULL )
        StartReadEventThread();

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

    SetEvent(m_hExitEvent);

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

BOOL __stdcall CProtectDevC::GetConfig(LP_DRIVER_CONFIG pconfig)
{
    BOOL  bResult = FALSE;

    if (m_hDevcHandle == INVALID_HANDLE_VALUE)
        return bResult;

    DWORD dwBuffLen = sizeof(DRIVER_CONFIG);

    bResult = DeviceIoControl(
        m_hDevcHandle,
        IOCTL_PTTDRV_GET_CONFIG,
        pconfig,
        sizeof(DRIVER_CONFIG),
        pconfig,
        dwBuffLen,
        &dwBuffLen,
        NULL
        );

    return bResult;
}

BOOL __stdcall CProtectDevC::SetConfig(LP_DRIVER_CONFIG pconfig)
{
    BOOL  bResult = FALSE;

    if (m_hDevcHandle == INVALID_HANDLE_VALUE)
        return bResult;

    DWORD dwBuffLen = sizeof(DRIVER_CONFIG);

    bResult = DeviceIoControl(
        m_hDevcHandle,
        IOCTL_PTTDRV_SET_CONFIG,
        pconfig,
        sizeof(DRIVER_CONFIG),
        pconfig,
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


BOOL __stdcall CProtectDevC::SetReportEvent( HANDLE hReportEvent )
{
    BOOL bResult = FALSE;

    DWORD dwBuffLen = sizeof(DWORD);

    DWORD dwSet = (DWORD)(LONG_PTR)hReportEvent;

    bResult = DeviceIoControl(
        m_hDevcHandle,
        IOCTL_PTTDRV_SET_REPORT_EVENT,
        &dwSet,
        sizeof(DWORD),
        &dwSet,
        dwBuffLen,
        &dwBuffLen,
        NULL
        );

    return bResult;    
}

BOOL __stdcall CProtectDevC::GetReportLog( LP_DRIVER_EVENT_INFO EventInfo )
{
    int nResult = DRIVER_STATUS_INVALID;

    if (m_hDevcHandle == INVALID_HANDLE_VALUE)
        return nResult;

    DWORD dwBuffLen = sizeof(DRIVER_EVENT_INFO);

    nResult = DeviceIoControl(
        m_hDevcHandle,
        IOCTL_PTTDRV_GET_REPORT_INFO,
        NULL,
        0,
        EventInfo,
        dwBuffLen,
        &dwBuffLen,
        NULL
        );


    return nResult;
}


void __stdcall CProtectDevC::SetCallback(IProtectDevCallback* piCallback)
{
    m_piCallback = piCallback;

    if ( m_piCallback != NULL )
        StartReadEventThread();
}


BOOL CProtectDevC::StartReadEventThread()
{
    BOOL bResult = FALSE;

    if (m_hReadEventThread != NULL)
        return TRUE;

    DWORD dwThread = NULL;

    m_hReadEventThread = CreateThread(NULL, 1024, ThreadReadEventFunc, this, 0, &dwThread);
    if (m_hReadEventThread == NULL)
        return FALSE;

    bResult = TRUE;

    return bResult;

}

DWORD WINAPI CProtectDevC::ThreadReadEventFunc(LPVOID lpParam)
{
    CProtectDevC* pThis = (CProtectDevC*)lpParam;

    pThis->RunReadEventFunc();

    return 0;
}

void CProtectDevC::RunReadEventFunc()
{
    DWORD  dwResult       = 0;
    HANDLE hWaitEvents[2] = { m_hReportEvent, m_hExitEvent };

    DRIVER_EVENT_INFO EventInfo  = { 0 };

    while (true)
    {

        Sleep(20);

        dwResult = WaitForMultipleObjects(2, hWaitEvents, FALSE, INFINITE);

        switch ( dwResult )
        {

        case WAIT_OBJECT_0 + 0:
            {
                while ( GetReportLog(&EventInfo) )
                {
                    if ( m_piCallback != NULL)
                        m_piCallback->ReportLog(&EventInfo);
                }
            }
            break;

        case WAIT_OBJECT_0 + 1:

            return;

        case WAIT_TIMEOUT:
        default:

            return;

        }
    }
}
