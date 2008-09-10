// lsesvr.cpp : Implementation of WinMain


#include "stdafx.h"
#include "resource.h"
#include "lsesvr.h"
#include "RunProcessMgt.h"
#include "ProcessModuleMgt.h"
#include "ModuleLoader.h"
#include "ModuleConfig.h"

#include <stdio.h>

class ClsesvrModule : public CAtlServiceModuleT< ClsesvrModule, IDS_SERVICENAME >
{
public :

    ClsesvrModule()
    {
        m_bService = (TRUE);
    }

	DECLARE_LIBID(LIBID_lsesvrLib)
	//DECLARE_REGISTRY_APPID_RESOURCEID(IDR_LSESVR, "{99EDD16E-FC5E-4881-8419-63A7A2781391}")
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

    void ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv) throw()
    {
        m_status.dwCurrentState = SERVICE_START_PENDING;

        m_hServiceStatus = RegisterServiceCtrlHandler(m_szServiceName, _Handler);
        if (m_hServiceStatus == NULL)
        {
            return;
        }
        SetServiceStatus(SERVICE_START_PENDING);

        m_status.dwWin32ExitCode = S_OK;
        m_status.dwCheckPoint = 0;
        m_status.dwWaitHint = 0;

#ifndef _ATL_NO_COM_SUPPORT

        HRESULT hr = E_FAIL;
        hr = InitializeCom();
        if (FAILED(hr))
        {
            // Ignore RPC_E_CHANGED_MODE if CLR is loaded. Error is due to CLR initializing
            // COM and InitializeCOM trying to initialize COM with different flags.
            if (hr != RPC_E_CHANGED_MODE || GetModuleHandle(_T("Mscoree.dll")) == NULL)
            {
                return;
            }
        }
        else
        {
            m_bComInitialized = true;
        }

        m_bDelayShutdown = false;
#endif //_ATL_NO_COM_SUPPORT
        // When the Run function returns, the service has stopped.
        m_status.dwWin32ExitCode = Run(SW_HIDE);

#ifndef _ATL_NO_COM_SUPPORT
        if (m_bService && m_bComInitialized)
            UninitializeCom();
#endif

        SetServiceStatus(SERVICE_STOPPED);
    }

    void OnUnknownRequest(DWORD /*dwOpcode*/) throw()
    {
    }



    HRESULT Run(int nShowCmd = SW_HIDE) throw()
    {
        HRESULT hr = S_OK;

        hr = PreMessageLoop(nShowCmd);

        if (hr == S_OK)
        {
            if (m_bService)
            {
                SetServiceStatus(SERVICE_RUNNING);
            }

            ModuleLoader loader;
            loader.LoadAllModule();

            RunMessageLoop();
        }

        if (SUCCEEDED(hr))
        {
            hr = PostMessageLoop();
        }

        return hr;
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

            // RunProcess
            if (WordCmpI(lpszToken, _T("RunProcess"))==0)
            {
                RunProcessMgt runProcessMgt;

                *pnRetCode = runProcessMgt.RunProcess( lpszToken + wcslen(L"RunProcess") + 1 );

                return false;
            }

            // ProcessModule
            if (WordCmpI(lpszToken, _T("ProcessModule"))==0)
            {
                CProcessModuleMgt ProcessModuleMgt;

                *pnRetCode = ProcessModuleMgt.ProcessModule( lpszToken + wcslen(L"ProcessModule") + 1 );

                return false;
            }

            // ProcessModule
            if (WordCmpI(lpszToken, _T("Debug"))==0)
            {
                m_bService = FALSE;               
            }

            // RegModule      xx.exe RegModule name dllfile id type
            if (WordCmpI(lpszToken, _T("RegModule"))==0)
            {
                *pnRetCode = 0;

                if ( __argc != 6 )
                    return false;

                Module_Config_Info info = { 0 };

                wcsncpy_s(info.szModuleName, __wargv[2], sizeof(info.szModuleName) / sizeof(TCHAR));
                wcsncpy_s(info.szModulePathFile, __wargv[3], sizeof(info.szModulePathFile) / sizeof(TCHAR));

                info.uModuleId = _ttoi( __wargv[4] );
                info.uModuleType = _ttoi( __wargv[5] );
                
                CModuleConfigReg reg;

                *pnRetCode = reg.SetModuleConfig( info );

                return false;
            }

            // UnregModule xx.exe UnregModule name
            if (WordCmpI(lpszToken, _T("UnregModule"))==0)
            {
                *pnRetCode = 0;

                if ( __argc != 3 )
                    return false;

                Module_Config_Info info = { 0 };

                wcsncpy_s(info.szModuleName, __wargv[2], sizeof(info.szModuleName) / sizeof(TCHAR));

                CModuleConfigReg reg;

                *pnRetCode = reg.DeleteModuleConfig( info );

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

    void OnStop() throw()
    {
        SetServiceStatus(SERVICE_STOP_PENDING);
        PostThreadMessage(m_dwThreadID, WM_QUIT, 0, 0);
    }
};

ClsesvrModule _AtlModule;

HRESULT StopService()
{
    HRESULT hResult = E_FAIL;
    
    _AtlModule.OnStop();

    return hResult;
}

//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR /*lpCmdLine*/, int nShowCmd)
{
    return _AtlModule.WinMain(nShowCmd);
}

