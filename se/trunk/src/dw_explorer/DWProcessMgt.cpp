#include "StdAfx.h"
#include "DWProcessMgt.h"
#include "RpcMsg.h"

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

CRpcMsgClient gg_clt;

BOOL CDWProcessMgt::CreateWebWnd(HWND hParent, HWND* pCeatedWnd )
{
    return ::PostThreadMessage( m_dwThreadId,
        0, (WPARAM)pCeatedWnd, (LPARAM)hParent );
}

BOOL CDWProcessMgt::DestryWebWnd( HWND hWnd )
{
    ProcessInfo* pPInfo = _FindProcessInfo(hWnd);
    ATLASSERT(pPInfo != NULL);
    if ( pPInfo == NULL )
        return FALSE;

    static LSEDataBufferImpl<ULONG> destroyParamBuf;
    destroyParamBuf = (ULONG)hWnd;
    
    int nRet = pPInfo->rpcClt.SendRpcMsg( s2c_create_webwnd, 
        destroyParamBuf.GetDataBuffer(), NULL );

    pPInfo->m_listWnd.RemoveAt( pPInfo->m_listWnd.Find(hWnd) );
    
    if ( pPInfo->m_listWnd.GetCount() <= 0 && 
         pPInfo->dwWndCreated >= m_dwMaxWndCreate )
    {
        m_cs.Lock();        
        m_listProcess.RemoveAt( m_listProcess.Find( pPInfo ) );
        m_cs.Unlock();
        pPInfo->rpcClt.SendRpcMsg( s2c_quit, 0, 0 );
        delete pPInfo;
    }

    return TRUE;
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

HWND CDWProcessMgt::_CreateWebWnd(HWND hParent)
{
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
        return NULL;


    static LSEDataBufferImpl<ULONG> createParamBuf;
    createParamBuf = (ULONG)hParent;

    CComPtr<IDataBuffer> spResult;
    int nRet = gg_clt.SendRpcMsg( s2c_create_webwnd, 
        createParamBuf.GetDataBuffer(), &spResult );
    if (spResult.p != NULL)
        spResult.p->AddRef();

    if ( nRet == 0 )
    {
        if ( spResult.p != NULL && spResult->GetBufferSize() == 4 )
        {
            HWND hWnd = (HWND)(*((ULONG*)spResult->GetDataBuffer()));
            if ( hWnd != NULL )
            {
                pPInfo->m_listWnd.AddTail( hWnd );
                pPInfo->dwWndCreated++;
            }
            return hWnd;
        }
    }


    return NULL;
}

DWORD WINAPI CDWProcessMgt::AsynMsgLoopThread( LPVOID p )
{
    MSG msg;

    CDWProcessMgt& prtmgt = CDWProcessMgt::Instance();

    while ( ::GetMessage(&msg, NULL, 0, 0) )
    {
        *((HWND*)msg.wParam) = prtmgt._CreateWebWnd( (HWND)msg.lParam );
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
        if ( !pRet->rpcClt.InitRpcClient( strRpcEPoint ) )
        {
            delete pRet;
            pRet = NULL;
        }
    }
    
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

    WaitForInputIdle(
        psInfo.hProcess,
        INFINITE);

    ::CloseHandle(psInfo.hProcess);
    ::CloseHandle(psInfo.hThread);    

    return psInfo.dwProcessId;
}

