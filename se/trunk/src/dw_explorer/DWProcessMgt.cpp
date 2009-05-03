#include "StdAfx.h"
#include "DWProcessMgt.h"
#include "RpcMsg.h"
#include "DWComDef.h"


#define PMAM_CREATE_WEBWND   (WM_USER + 1000)
#define PMAM_DESTROY_WEBWND  (WM_USER + 1001)
#define PMAM_WEBWND_OPENURL  (WM_USER + 1002)

#define PMAM_GETWEBWNDINFO   (WM_USER + 1003)

CDWProcessMgt::CDWProcessMgt(void)
{
    m_dwMaxWndCreate = 1;

    CloseHandle(
        CreateThread( NULL, 0, AsynMsgLoopThread, this, 0, &m_dwThreadId) );
}

CDWProcessMgt::~CDWProcessMgt(void)
{
    ::PostThreadMessage(m_dwThreadId, WM_QUIT, 0, 0);
}

CDWProcessMgt& CDWProcessMgt::Instance()
{
    static CDWProcessMgt* __CDWProcessMgt_Instance__ = new CDWProcessMgt();
    return *__CDWProcessMgt_Instance__;
}

BOOL CDWProcessMgt::CreateWebWnd(HWND hParent, LPARAM lParam, LPCTSTR pszOpenURL )
{
    if ( pszOpenURL == NULL )
        return FALSE;

    int nLen = wcslen(pszOpenURL) + 4;
    WCHAR* pParam = new WCHAR[ nLen ];

    if ( pParam == NULL )
        return FALSE;

    pParam[0] = nLen;
    *((LPARAM*)(pParam+1)) = lParam;
    wcscpy_s(pParam+3, nLen - 3, pszOpenURL);

    BOOL bRet = ::PostThreadMessage( m_dwThreadId,
        PMAM_CREATE_WEBWND, (WPARAM)hParent, (LPARAM)pParam );
    
    ATLASSERT(bRet);

    return bRet;
}

BOOL CDWProcessMgt::DestryWebWnd(HWND hWnd)
{
    return ::PostThreadMessage( m_dwThreadId,
        PMAM_DESTROY_WEBWND, (WPARAM)hWnd, 0 );
}

BOOL CDWProcessMgt::WebWndOpenURL( HWND hWnd, LPCTSTR pszOpenURL )
{
    if ( pszOpenURL == NULL || hWnd == NULL )
        return FALSE;

    int nLen = wcslen(pszOpenURL) + 4;
    WCHAR* pParam = new WCHAR[ nLen ];

    if ( pParam == NULL )
        return FALSE;

    pParam[0] = nLen;
    *((HWND*)(pParam+1)) = hWnd;
    wcscpy_s(pParam+3, nLen - 3, pszOpenURL);

    BOOL bRet = ::PostThreadMessage( m_dwThreadId,
        PMAM_WEBWND_OPENURL, 00, (LPARAM)pParam );

    ATLASSERT(bRet);

    return bRet;
}

BOOL CDWProcessMgt::GetWebWndInfo( HWND hParent, HWND hWnd )
{
    return ::PostThreadMessage( m_dwThreadId,
        PMAM_GETWEBWNDINFO, (WPARAM)hParent, (LPARAM)hWnd );
}


ProcessInfo* CDWProcessMgt::_FindProcessInfo(HWND hWnd)
{
    ProcessInfo* pPInfo = NULL;

    m_cs.Lock();
    for ( POSITION pos = m_listProcess.GetHeadPosition(); pos != NULL; )
    {
        pPInfo = m_listProcess.GetNext(pos);
        if ( pPInfo != NULL && pPInfo->m_listWnd.Find(hWnd) != NULL )
            break;
        pPInfo = NULL;
    }
    m_cs.Unlock();
    
    return pPInfo;
}

HWND CDWProcessMgt::_CreateWebWnd(HWND hParent, LPARAM lParam)
{
    ATLASSERT( ::IsWindow(hParent) );

    WCHAR* pParam = (WCHAR*)lParam;
    ProcessInfo* pPInfo = NULL;

    m_cs.Lock();
    for ( POSITION pos = m_listProcess.GetHeadPosition(); pos != NULL; )
    {
        pPInfo = m_listProcess.GetNext(pos);
        if ( pPInfo != NULL && pPInfo->dwWndCreated < m_dwMaxWndCreate )
            break;
        pPInfo = NULL;
    }
    m_cs.Unlock();

    if ( pPInfo == NULL )
        pPInfo = CreateSEProcess();
    if ( pPInfo == NULL )
    {
        delete []pParam;
        ATLASSERT( FALSE );
        return NULL;
    }

    LSEDataBuffer createParamBuf((pParam[0]-1) * sizeof(WCHAR));
    WCHAR* pBuffer  = (WCHAR*)createParamBuf.GetDataBuffer();
    *(HWND*)pBuffer = hParent;
    wcscpy_s( pBuffer+2, pParam[0]-3, pParam+3 );

    HWND hWnd = NULL;
    CComPtr<IDataBuffer> spResult;
    int nRet = pPInfo->rpcClt.SendRpcMsg( s2c_create_webwnd, 
        &createParamBuf, &spResult );
    if (spResult.p != NULL)
        spResult.p->AddRef();

    if ( nRet == 0 )
    {
        if ( spResult.p != NULL && spResult->GetBufferSize() == 4 )
        {
            hWnd = (HWND)(*((ULONG*)spResult->GetDataBuffer()));
            if ( hWnd != NULL )
            {
                pPInfo->m_listWnd.AddTail( hWnd );
                pPInfo->dwWndCreated++;
            }
        }
    }

    ::PostMessage( hParent, WM_CREATE_WEB_WND, (WPARAM)hWnd, *(LPARAM*)(pParam+1));

    delete []pParam;

    return NULL;
}

BOOL CDWProcessMgt::_DestryWebWnd( HWND hWnd )
{
    ProcessInfo* pPInfo = _FindProcessInfo(hWnd);
    ATLASSERT(pPInfo != NULL);
    if ( pPInfo == NULL )
        return FALSE;

    static LSEDataBufferImpl<ULONG> destroyParamBuf;
    destroyParamBuf = (ULONG)hWnd;

    int nRet = pPInfo->rpcClt.SendRpcMsg( s2c_destroy_webwnd, 
        destroyParamBuf.GetDataBuffer(), NULL );

    pPInfo->m_listWnd.RemoveAt( pPInfo->m_listWnd.Find(hWnd) );

    if ( pPInfo->m_listWnd.GetCount() <= 0 && 
        pPInfo->dwWndCreated >= m_dwMaxWndCreate )
    {
        m_cs.Lock();        
        m_listProcess.RemoveAt( m_listProcess.Find( pPInfo ) );
        m_cs.Unlock();
        nRet = pPInfo->rpcClt.SendRpcMsg( s2c_quit, 0, 0 );
        delete pPInfo;
    }

    return TRUE;
}

BOOL CDWProcessMgt::_WebWndOpenURL( LPARAM lParam )
{
    ATLASSERT( lParam );

    WCHAR* pParam = (WCHAR*)lParam;

    ProcessInfo* pPInfo = _FindProcessInfo( *(HWND*)(pParam+1));
    ATLASSERT(pPInfo != NULL);
    if ( pPInfo == NULL )
        return FALSE;

    LSEDataBuffer createParamBuf((pParam[0]-1) * sizeof(WCHAR));
    WCHAR* pBuffer  = (WCHAR*)createParamBuf.GetDataBuffer();
    memcpy( pBuffer, pParam+1, (pParam[0]-1)*2 );

    HWND hWnd = NULL;
    int nRet = pPInfo->rpcClt.SendRpcMsg( s2c_webwnd_openurl, 
        &createParamBuf, 0 );

    delete []pParam;

    return nRet == 0;
}

BOOL CDWProcessMgt::_GetWebWndInfo( HWND hParent, HWND hWnd )
{
    ProcessInfo* pPInfo = _FindProcessInfo(hWnd);
    ATLASSERT(pPInfo != NULL);
    if ( pPInfo == NULL )
        return FALSE;


    CComPtr<IDataBuffer> spResult;

    static LSEDataBufferImpl<ULONG> wndBuf;
    wndBuf = (ULONG)hWnd;

    int nRet = pPInfo->rpcClt.SendRpcMsg( s2c_webwnd_info, 
        wndBuf.GetDataBuffer(), &spResult );

    if ( nRet == 0 && spResult.p )
    {
        nRet = ::PostMessage(hParent, WM_WEBWND_INFO_CHANGED, (WPARAM)hWnd, (LPARAM)spResult.p);
        if ( nRet )
            spResult.p = NULL;
        else
            spResult.p->AddRef();
    }


    return TRUE;
}

DWORD WINAPI CDWProcessMgt::AsynMsgLoopThread( LPVOID p )
{
    MSG msg;

    CDWProcessMgt& prtmgt = CDWProcessMgt::Instance();

    while ( ::GetMessage(&msg, NULL, 0, 0) )
    {
        switch (msg.message)
        {
        case PMAM_CREATE_WEBWND:
            prtmgt._CreateWebWnd( (HWND)msg.wParam, msg.lParam );
            break;
        case PMAM_DESTROY_WEBWND:
            prtmgt._DestryWebWnd((HWND)msg.wParam);
            break;
        case PMAM_WEBWND_OPENURL:
            prtmgt._WebWndOpenURL(msg.lParam);
            break;
        case PMAM_GETWEBWNDINFO:
            prtmgt._GetWebWndInfo((HWND)msg.wParam, (HWND)msg.lParam);
            break;
        }
    }

    return 0L;
}


ProcessInfo* CDWProcessMgt::CreateSEProcess()
{
    ProcessInfo* pRet = NULL;

    ATL::CString strCmdLine;
    ATL::CString strRpcEPoint;

    strRpcEPoint.Format(L"%s%x", Rpc_Clt_EPoint_Name, GetTickCount());
    strCmdLine.Format(L"[csp]:%s", strRpcEPoint);

    DWORD dwRet = CreateSEProcess(strCmdLine);
    
    if ( dwRet != 0 )
    {
        pRet = new ProcessInfo;

        pRet->dwWndCreated = 0;

        BOOL bRet = pRet->rpcClt.InitRpcClient( strRpcEPoint );
        int  nLoop = 0;
        while ( !bRet && ++nLoop < 100 )
        {
            bRet = pRet->rpcClt.InitRpcClient( strRpcEPoint );
            Sleep( 20 );
        }

        if ( !bRet )
        {
            ATLASSERT(FALSE);
            delete pRet;
            pRet = NULL;
        }
    }

    m_listProcess.AddTail( pRet );
    
    return pRet;
}

DWORD CDWProcessMgt::CreateSEProcess( LPCTSTR pszCmdLine )
{
    DWORD dwPID = 0;

    ATL::CString strCmdLine;

    STARTUPINFO			stInfo		= { 0 };
    PROCESS_INFORMATION psInfo		= { 0 };
    stInfo.cb = sizeof(STARTUPINFO);

    TCHAR szModuleFile[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, szModuleFile, MAX_PATH);

    if ( pszCmdLine != NULL && _tcslen(pszCmdLine) > 0 )
        strCmdLine.Format(L"\"%s\" %s", szModuleFile, pszCmdLine);
    else
        strCmdLine = szModuleFile;

    BOOL bCreate = CreateProcess(
        NULL, 
        strCmdLine.GetBuffer(0), 
        NULL, 
        NULL, 
        TRUE, 
        0, 
        NULL, 
        NULL, 
        &stInfo, 
        &psInfo);

    ATLASSERT( bCreate );

    WaitForInputIdle(
        psInfo.hProcess,
        INFINITE);

    ::CloseHandle(psInfo.hProcess);
    ::CloseHandle(psInfo.hThread);    

    return psInfo.dwProcessId;
}

