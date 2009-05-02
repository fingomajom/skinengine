#include "StdAfx.h"
#include "DWClientMgt.h"
#include "DWWebWnd.h"

DWORD CDWClientMgt::m_dwMainThreadId = NULL;

int OnCreateWebWnd(    
    /* [in]  */ IDataBuffer*  pParameter,
    /* [out] */ IDataBuffer** ppResult );


int DWCltReceiveRpcMsg( 
    /* [in]  */ int nMsgId,
    /* [in]  */ IDataBuffer*  pParameter,
    /* [out] */ IDataBuffer** ppResult)
{
    if ( nMsgId == s2c_create_webwnd )
        return OnCreateWebWnd(pParameter, ppResult);

    return 0;
}


int OnCreateWebWnd(    
    /* [in]  */ IDataBuffer*  pParameter,
    /* [out] */ IDataBuffer** ppResult )
{
    CDWClientMgt& cltmgt = CDWClientMgt::Instance();

    ATLASSERT(pParameter != NULL);
    ATLASSERT(ppResult != NULL);

    if ( pParameter == NULL || ppResult == NULL )
        return -1;

    if ( pParameter->GetBufferSize() != 4 )
        return -2;

    HWND hParent = cltmgt.CreateWebWnd( (HWND)(*((ULONG*)pParameter->GetDataBuffer())) );

    static LSEDataBufferImpl<ULONG> BufResult;
    BufResult = (ULONG)hParent;

    *ppResult = BufResult.GetDataBuffer();

    return 0;
}


CDWClientMgt::CDWClientMgt(void)
{
}

CDWClientMgt::~CDWClientMgt(void)
{
}

CDWClientMgt& CDWClientMgt::Instance()
{
    static CDWClientMgt* __CDWClientMgt_Instance__ = new CDWClientMgt();
    return *__CDWClientMgt_Instance__;
}


int CDWClientMgt::RunMainMsgLoop( LPTSTR lpstrCmdLine )
{
    int   nRet = 0;
    DWORD dwThreadId = 0;

    CMessageLoop theLoop;
    _Module.AddMessageLoop(&theLoop);
    CDWClientMgt& cltmgt = CDWClientMgt::Instance();


    cltmgt.m_rpcSvr.SetReceiveFunc(DWCltReceiveRpcMsg);
    //MessageBox( GetActiveWindow(), lpstrCmdLine+6, lpstrCmdLine, MB_OK);
    if ( !cltmgt.m_rpcSvr.InitRpcServer(lpstrCmdLine+6) )
        return 0;
    if ( !cltmgt.m_rpcClt.InitRpcClient(Rpc_Svr_EPoint_Name) )
        return 0;

    cltmgt.m_dwMainThreadId = GetCurrentThreadId();
    
    ::CreateThread( NULL, 0, WaitForQuitThread,0, 0, &dwThreadId);

    nRet = theLoop.Run();

    cltmgt.m_rpcSvr.UninitRpcServer();
    cltmgt.m_rpcClt.UninitRpcClient();
    _Module.RemoveMessageLoop();

    delete &cltmgt;

    return nRet;
}

HWND CDWClientMgt::CreateWebWnd( HWND hParent )
{
    HWND   hResult    = hParent;    
    DWORD  dwThreadId = 0;
    HANDLE hThread    = NULL;
    
    m_hCreateWebWndEvent = CreateEvent( NULL, FALSE, FALSE, NULL);
    if ( m_hCreateWebWndEvent == NULL )
        return NULL;

    m_CreateWebWndCS.Lock();
    hThread = CreateThread( NULL, 0, WebWndMsgLoopThread, (LPVOID)&hResult, 0, &dwThreadId);
    if ( hThread != NULL )
    {
        WaitForSingleObject(m_hCreateWebWndEvent, INFINITE);
        CloseHandle( hThread );
    }
    m_CreateWebWndCS.Unlock();
    CloseHandle(m_hCreateWebWndEvent);

    if ( hResult == hParent )
        hResult = NULL;

    return hResult;
}

void CDWClientMgt::_AddWebWnd( CDWWebWnd* pWnd )
{
    m_cs.Lock();
    m_listWebWnd.AddTail( pWnd );
    m_cs.Unlock();
}
void CDWClientMgt::_RemoveWebWnd( CDWWebWnd* pWnd )
{
    BOOL bAllQuit = FALSE;

    m_cs.Lock();

    POSITION pos = m_listWebWnd.Find( pWnd );
    if ( pos )
        m_listWebWnd.RemoveAt( pos );

    bAllQuit = (m_listWebWnd.GetCount() == 0);

    m_cs.Unlock();

    //if ( bAllQuit )
    //    PostThreadMessage(m_dwMainThreadId, WM_QUIT, 0, 0);
}

DWORD WINAPI CDWClientMgt::WebWndMsgLoopThread( LPVOID p )
{
    HWND* pWndRet = (HWND*)p;
    DWORD dwRet = 0;
    CMessageLoop theLoop;
    CDWClientMgt& clt = CDWClientMgt::Instance();

    _Module.AddMessageLoop(&theLoop);

    CDWWebWnd wndWeb;
    if( wndWeb.Create(*pWndRet, wndWeb.rcDefault, L"http://www.baidu.com", WS_CHILD) == NULL)
    {
        ::SetEvent(clt.m_hCreateWebWndEvent);
        return 0;
    }
    *pWndRet = wndWeb;
    ::SetEvent(clt.m_hCreateWebWndEvent);

    clt._AddWebWnd(&wndWeb);

    dwRet = theLoop.Run();

    clt._RemoveWebWnd(&wndWeb);

    _Module.RemoveMessageLoop();
    return dwRet;
}

DWORD WINAPI CDWClientMgt::WaitForQuitThread( LPVOID p )
{
    CDWClientMgt& mgt = CDWClientMgt::Instance();
    
    int nPid = mgt.m_rpcClt.SendRpcMsg( c2s_get_pid, 0, 0 );
    if ( nPid > 0 )
    {
        HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | SYNCHRONIZE, FALSE, nPid );
        if ( hProcess != NULL )
            WaitForSingleObject( hProcess, INFINITE );
    }

    ::PostThreadMessage( m_dwMainThreadId, WM_QUIT, 0, 0 );

    return 0;
}