// lsesvr.cpp : Implementation of WinMain


#include "stdafx.h"
#include "resource.h"
#include "lsesvr.h"

#include <stdio.h>

class ClsesvrModule : public CAtlServiceModuleT< ClsesvrModule, IDS_SERVICENAME >
{
public :
	DECLARE_LIBID(LIBID_lsesvrLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_LSESVR, "{99EDD16E-FC5E-4881-8419-63A7A2781391}")
	HRESULT InitializeSecurity() throw()
	{
		// TODO : Call CoInitializeSecurity and provide the appropriate security settings for 
		// your service
		// Suggested - PKT Level Authentication, 
		// Impersonation Level of RPC_C_IMP_LEVEL_IDENTIFY 
		// and an appropiate Non NULL Security Descriptor.

		return S_OK;
	}

    HRESULT Start(int nShowCmd) throw()
    {
        m_bService = FALSE;

        if (m_bService)
        {
            SERVICE_TABLE_ENTRY st[] =
            {
                { m_szServiceName, _ServiceMain },
                { NULL, NULL }
            };
            if (::StartServiceCtrlDispatcher(st) == 0)
                m_status.dwWin32ExitCode = GetLastError();

            return m_status.dwWin32ExitCode;
        }

        m_status.dwWin32ExitCode = Run(nShowCmd);

        return m_status.dwWin32ExitCode;
    }


    bool ParseCommandLine(LPCTSTR lpCmdLine, HRESULT* pnRetCode) throw()
    {
        *pnRetCode = S_OK;

        TCHAR szTokens[] = _T("-/");

        LPCTSTR lpszToken = FindOneOf(lpCmdLine, szTokens);
        while (lpszToken != NULL)
        {
            if (WordCmpI(lpszToken, _T("UnregServer"))==0)
            {
                if (!Uninstall())
                    *pnRetCode = E_FAIL;

                return false;
            }

            // Register as Local Server
            if (WordCmpI(lpszToken, _T("RegServer"))==0)
            {
                if (!Uninstall())
                    *pnRetCode = E_FAIL;

                if (FAILED(*pnRetCode))
                    return false;

                if (!Install())
                    *pnRetCode = E_FAIL;

                return false;
            }

            lpszToken = FindOneOf(lpszToken, szTokens);
        }

        return true;
    }


    BOOL Install() throw()
    {
        if (IsInstalled())
            return TRUE;

        // Get the executable file path
        TCHAR szFilePath[MAX_PATH + _ATL_QUOTES_SPACE];
        DWORD dwFLen = ::GetModuleFileName(NULL, szFilePath + 1, MAX_PATH);
        if( dwFLen == 0 || dwFLen == MAX_PATH )
            return FALSE;

        // Quote the FilePath before calling CreateService
        szFilePath[0] = _T('\"');
        szFilePath[dwFLen + 1] = _T('\"');
        szFilePath[dwFLen + 2] = 0;

        SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if (hSCM == NULL)
        {
            TCHAR szBuf[1024];
            if (AtlLoadString(ATL_SERVICE_MANAGER_OPEN_ERROR, szBuf, 1024) == 0)
#ifdef UNICODE
                Checked::wcscpy_s(szBuf, _countof(szBuf), _T("Could not open Service Manager"));
#else
                Checked::strcpy_s(szBuf, _countof(szBuf), _T("Could not open Service Manager"));
#endif
            MessageBox(NULL, szBuf, m_szServiceName, MB_OK);
            return FALSE;
        }

        TCHAR szSvrDisName[MAX_PATH] = { 0 };
        TCHAR szSvrDesc[MAX_PATH] = { 0 };

        LoadString(_AtlBaseModule.GetModuleInstance(), 
            IDS_SERVICE_DISPLAYNAME, szSvrDisName, sizeof(szSvrDisName) / sizeof(TCHAR));
        LoadString(_AtlBaseModule.GetModuleInstance(), 
            IDS_SERVICE_DESCRIPTION, szSvrDesc, sizeof(szSvrDesc) / sizeof(TCHAR));


        SC_HANDLE hService = ::CreateService(
            hSCM, m_szServiceName, szSvrDisName,
            SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
            SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
            szFilePath, NULL, NULL, _T("RPCSS\0"), NULL, NULL);

        SERVICE_DESCRIPTION service_desc = { 0 };
        service_desc.lpDescription = szSvrDesc;
        BOOL bRet = ChangeServiceConfig2(hService, 
            SERVICE_CONFIG_DESCRIPTION, &service_desc);

        if (hService == NULL)
        {
            ::CloseServiceHandle(hSCM);
            TCHAR szBuf[1024];
            if (AtlLoadString(ATL_SERVICE_START_ERROR, szBuf, 1024) == 0)
#ifdef UNICODE
                Checked::wcscpy_s(szBuf, _countof(szBuf), _T("Could not start service"));
#else
                Checked::strcpy_s(szBuf, _countof(szBuf), _T("Could not start service"));
#endif
            MessageBox(NULL, szBuf, m_szServiceName, MB_OK);
            return FALSE;
        }

        ::CloseServiceHandle(hService);
        ::CloseServiceHandle(hSCM);
        return TRUE;
    }

};

ClsesvrModule _AtlModule;



//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR /*lpCmdLine*/, int nShowCmd)
{
    return _AtlModule.WinMain(nShowCmd);
}

