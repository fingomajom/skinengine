// lsesvr.cpp : Implementation of WinMain


#include "stdafx.h"
#include "resource.h"
#include "lsesvr.h"
#include "RunProcessMgt.h"
#include "ProcessModuleMgt.h"
#include "ModuleLoader.h"
#include "ModuleConfig.h"

#include <stdio.h>


class ClsesvrModule : 
    public CAtlServiceModuleT< ClsesvrModule, IDS_SERVICENAME >
{
public :

    ClsesvrModule()
    {
        m_bService = TRUE;
    }

    static LPCOLESTR GetAppId() throw() 
    { 
        return OLESTR("{99EDD16E-FC5E-4881-8419-63A7A2781391}"); 
    } 
    static TCHAR* GetAppIdT() throw() 
    { 
        return _T("{99EDD16E-FC5E-4881-8419-63A7A2781391}"); 
    } 


	HRESULT InitializeSecurity() throw()
	{
        HRESULT hr = CoInitializeSecurity( NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT,
            RPC_C_IMP_LEVEL_IDENTIFY, NULL, EOAC_NONE, NULL );

        ATLASSERT(SUCCEEDED(hr));

		return hr;
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
        m_status.dwCheckPoint    = 0;
        m_status.dwWaitHint      = 0;

#ifndef _ATL_NO_COM_SUPPORT

        HRESULT hr = E_FAIL;

        hr = InitializeCom();
        if (FAILED(hr))
        {
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

    HRESULT PreMessageLoop(int nShowCmd) throw()
    {
        HRESULT hr = S_OK;
        if (m_bService)
        {
            m_dwThreadID = GetCurrentThreadId();

            InitializeSecurity();

            if (FAILED(hr))
                return hr;
        }

        {

#ifndef _ATL_NO_COM_SUPPORT

#if ((_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM)) & defined(_ATL_FREE_THREADED)

            hr = RegisterClassObjects(CLSCTX_LOCAL_SERVER, 
                REGCLS_MULTIPLEUSE | REGCLS_SUSPENDED);

            if (FAILED(hr))
                return hr;

            if (hr == S_OK)
            {
                if (m_bDelayShutdown)
                {
                    CHandle h(StartMonitor());
                    if (h.m_h == NULL)
                    {
                        hr = E_FAIL;
                    }
                    else
                    {
                        hr = CoResumeClassObjects();
                        ATLASSERT(SUCCEEDED(hr));
                        if (FAILED(hr))
                        {
                            ::SetEvent(m_hEventShutdown); // tell monitor to shutdown
                            ::WaitForSingleObject(h, m_dwTimeOut * 2);
                        }
                    }
                }
                else
                {
                    hr = CoResumeClassObjects();
                    ATLASSERT(SUCCEEDED(hr));
                }

                if (FAILED(hr))
                    RevokeClassObjects();
            }
            else
            {
                m_bDelayShutdown = false;
            }

#else

            hr = RegisterClassObjects(CLSCTX_LOCAL_SERVER, 
                REGCLS_MULTIPLEUSE);
            if (hr == S_OK)
            {
                if (m_bDelayShutdown && !StartMonitor())
                {
                    hr = E_FAIL;
                }
            }
            else
            {
                m_bDelayShutdown = false;
            }


#endif

#endif	// _ATL_NO_COM_SUPPORT

            ATLASSERT(SUCCEEDED(hr));
            return hr;
        }
        if (FAILED(hr))
            return hr;

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
                
                *pnRetCode = UnregisterAppId();

                return false;
            }

            // Register as Local Server
            if (WordCmpI(lpszToken, _T("RegServer"))==0)
            {
                *pnRetCode = UnregisterAppId();

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

            // RegModule      xx.exe RegModule name id type dllfile
            if (WordCmpI(lpszToken, _T("RegModule"))==0)
            {
                *pnRetCode = E_FAIL;

                struct CStr2Int {
                    static int Str2Int(const WCHAR* p )
                    {
                        int nRet = 0;

                        while ( p != NULL && *p == ' ' )
                            p++;

                        while ( p != NULL && *p != 0 && *p>='0' && *p <= '9' )
                            nRet = nRet * 10 + (*p++ - '0');
                        return nRet;
                    }
                };

                Module_Config_Info info = { 0 };

                lpszToken += ( wcslen(L"RegModule") + 1 );

                wcsncpy_s(info.szModuleName, 
                    lpszToken, 
                    sizeof(info.szModuleName) / sizeof(TCHAR));

                LPCTSTR lpszSpace = FindOneOf( info.szModuleName, _T(" "));
                if (lpszSpace != NULL)
                    *const_cast<WCHAR*>(lpszSpace - 1) = 0;                

                lpszToken = FindOneOf( lpszToken, _T(" ") );
                info.uModuleId = CStr2Int::Str2Int( lpszToken );
                while ( lpszToken != NULL && *lpszToken == ' ' )
                    lpszToken++;
                lpszToken = FindOneOf( lpszToken, _T(" ") );
                info.uModuleType = CStr2Int::Str2Int( lpszToken );
                while ( lpszToken != NULL && *lpszToken == ' ' )
                    lpszToken++;
                lpszToken = FindOneOf( lpszToken, _T(" ") );

                if (lpszToken != NULL)
                    wcsncpy_s(info.szModulePathFile, 
                        lpszToken, 
                        sizeof(info.szModulePathFile) / sizeof(TCHAR));
                else
                    return false;
                
                CModuleConfigReg reg;

                *pnRetCode = reg.SetModuleConfig( info );

                return false;
            }

            // UnregModule xx.exe UnregModule name
            if (WordCmpI(lpszToken, _T("UnregModule"))==0)
            {
                *pnRetCode = 0;

                Module_Config_Info info = { 0 };

                wcsncpy_s(info.szModuleName, 
                    lpszToken + wcslen(L"UnregModule") + 1, 
                    sizeof(info.szModuleName) / sizeof(TCHAR));

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

        CRegKey keyAppID;
        LONG lRes = keyAppID.Open(HKEY_CLASSES_ROOT, _T("AppID"), KEY_WRITE);
        if (lRes != ERROR_SUCCESS)
            return AtlHresultFromWin32(lRes);

        CRegKey key;

        lRes = key.Create(keyAppID, GetAppIdT());
        if (lRes != ERROR_SUCCESS)
            return AtlHresultFromWin32(lRes);

        key.DeleteValue(_T("LocalService"));

        key.SetStringValue(_T("LocalService"), m_szServiceName);
        
        RegisterServer(FALSE);

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

    HRESULT UnregisterAppId() throw()
    {
        if (!Uninstall())
            return E_FAIL;

        UnregisterServer(FALSE);

        // First remove entries not in the RGS file.
        CRegKey keyAppID;
        LONG lRes = keyAppID.Open(HKEY_CLASSES_ROOT, _T("AppID"), KEY_WRITE);
        if (lRes != ERROR_SUCCESS)
            return AtlHresultFromWin32(lRes);

        CRegKey key;
        lRes = key.Open(keyAppID, GetAppIdT(), KEY_WRITE);
        if (lRes != ERROR_SUCCESS)
            return AtlHresultFromWin32(lRes);
        key.DeleteValue(_T("LocalService"));

        return S_OK;
        //return UpdateRegistryAppId(FALSE);
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

