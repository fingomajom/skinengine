#include "StdAfx.h"
#include "DWClientMgt.h"
#include "DWWebWnd.h"
#include "CookieMgt.h"

DWORD CDWClientMgt::m_dwMainThreadId = NULL;

int OnCreateWebWnd(    
    /* [in]  */ IDataBuffer*  pParameter,
    /* [out] */ IDataBuffer** ppResult );

int OnDestroyWebWnd(    
    /* [in]  */ IDataBuffer*  pParameter,
    /* [out] */ IDataBuffer** ppResult );

int OnWebWndInfo(    
    /* [in]  */ IDataBuffer*  pParameter,
    /* [out] */ IDataBuffer** ppResult );

int OnWebWndOpenURL(    
    /* [in]  */ IDataBuffer*  pParameter,
    /* [out] */ IDataBuffer** ppResult );


int DWCltReceiveRpcMsg( 
    /* [in]  */ int nMsgId,
    /* [in]  */ IDataBuffer*  pParameter,
    /* [out] */ IDataBuffer** ppResult)
{
    if ( nMsgId == s2c_create_webwnd )
    {
        return OnCreateWebWnd(pParameter, ppResult);
    }
    else if ( nMsgId == s2c_destroy_webwnd )
    {
        return OnDestroyWebWnd(pParameter, ppResult);
    }
    else if ( nMsgId == s2c_quit )
    {
        ::PostThreadMessage(CDWClientMgt::m_dwMainThreadId, WM_QUIT, 0, 0);
    }
    else if ( nMsgId == s2c_webwnd_info )
    {
        return OnWebWndInfo(pParameter, ppResult);
    }
    else if ( nMsgId == s2c_webwnd_openurl )
    {
        return OnWebWndOpenURL(pParameter, ppResult);
    }

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

    if ( pParameter->GetBufferSize() < 4 )
        return -2;

    HWND hParent = cltmgt.CreateWebWnd( 
        (HWND)(*((ULONG*)pParameter->GetDataBuffer())),
        (WCHAR*)pParameter->GetDataBuffer()+2);

    static LSEDataBufferImpl<ULONG> BufResult;
    BufResult = (ULONG)hParent;

    *ppResult = BufResult.GetDataBuffer();

    return 0;
}

int OnDestroyWebWnd(    
    /* [in]  */ IDataBuffer*  pParameter,
    /* [out] */ IDataBuffer** ppResult )
{
    CDWClientMgt& cltmgt = CDWClientMgt::Instance();

    ATLASSERT(pParameter != NULL);

    if ( pParameter == NULL )
        return -1;

    if ( pParameter->GetBufferSize() != 4 )
        return -2;

    HWND hWnd = (HWND)(*((ULONG*)pParameter->GetDataBuffer()));
    
    cltmgt.DestroyWebWnd(hWnd);

    return 0;
}

int OnWebWndInfo(    
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

    HWND hWnd = (HWND)(*((ULONG*)pParameter->GetDataBuffer()));
    
    ATL::CString strTitle;
    ATL::CString strURL;

    cltmgt.m_cs.Lock();

    for ( POSITION pos = cltmgt.m_listWebWnd.GetHeadPosition(); pos != NULL; )
    {
        CDWWebWnd* pWebWnd = cltmgt.m_listWebWnd.GetNext(pos);
        if ( pWebWnd != NULL && pWebWnd->m_hWnd == hWnd )
        {
            strTitle = pWebWnd->m_strTitle;
            strURL   = pWebWnd->m_strURL;
            break;
        }
    }

    cltmgt.m_cs.Unlock();
    
    int nStrLen = strTitle.GetLength() + strURL.GetLength();

    *ppResult = LSECreateDataBuffer( (nStrLen + 3) * sizeof(WCHAR) );
    if ( *ppResult == NULL )
        return -1;

    WCHAR* pszBufer = (WCHAR*)(*ppResult)->GetDataBuffer();
    pszBufer[0] = strTitle.GetLength() + 2;
    StrCpy( pszBufer + 1, strTitle);
    StrCpy( pszBufer + pszBufer[0], strURL);

    return 0;
}

int OnWebWndOpenURL(    
    /* [in]  */ IDataBuffer*  pParameter,
    /* [out] */ IDataBuffer** ppResult )
{
    CDWClientMgt& cltmgt = CDWClientMgt::Instance();

    ATLASSERT(pParameter != NULL);

    if ( pParameter == NULL )
        return -1;

    if ( pParameter->GetBufferSize() < 4 )
        return -2;

    HWND hWnd = (HWND)(*((ULONG*)pParameter->GetDataBuffer()));

    cltmgt.m_cs.Lock();

    for ( POSITION pos = cltmgt.m_listWebWnd.GetHeadPosition(); pos != NULL; )
    {
        CDWWebWnd* pWebWnd = cltmgt.m_listWebWnd.GetNext(pos);
        if ( pWebWnd != NULL && pWebWnd->m_hWnd == hWnd )
        {
            pWebWnd->OpenURL( (WCHAR*)pParameter->GetDataBuffer()+2 );

            break;
        }
    }

    cltmgt.m_cs.Unlock();

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

    CDWMessageLoop theLoop;
    _Module.AddMessageLoop(&theLoop);
    CDWClientMgt& cltmgt   = CDWClientMgt::Instance();
    CCookieMgt&   ckemgt   = CCookieMgt::Instance();
    //CDWCrashMgt&  crashmgt = CDWCrashMgt::Instance();

    cltmgt.m_rpcSvr.SetReceiveFunc(DWCltReceiveRpcMsg);
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
    delete &ckemgt;
    //delete &crashmgt;

    return nRet;
}

HWND CreateWebWnd( HWND hParent, LPCTSTR pszOpenURL, IWebBrowser2** ppOut )
{
    return CDWClientMgt::Instance().CreateWebWnd( hParent, pszOpenURL, ppOut );
}


HWND CDWClientMgt::CreateWebWnd( HWND hParent, LPCTSTR pszOpenURL, IWebBrowser2** ppOut )
{
    HWND   hResult    = hParent;    
    DWORD  dwThreadId = 0;
    HANDLE hThread    = NULL;
    
    m_hCreateWebWndEvent = CreateEvent( NULL, FALSE, FALSE, NULL);
    if ( m_hCreateWebWndEvent == NULL )
        return NULL;

    m_CreateWebWndCS.Lock();
    m_strOpenURL = pszOpenURL;
    hThread = CreateThread( NULL, 0, WebWndMsgLoopThread, (LPVOID)&hResult, 0, &dwThreadId);
    if ( hThread != NULL )
    {
        WaitForSingleObject(m_hCreateWebWndEvent, INFINITE);
        CloseHandle( hThread );
    }
    if ( ppOut != NULL )
        *ppOut = m_piWebBrowser;
    m_CreateWebWndCS.Unlock();
    CloseHandle(m_hCreateWebWndEvent);
    
    if ( hResult == hParent )
        hResult = NULL;

    return hResult;
}

BOOL CDWClientMgt::DestroyWebWnd( HWND hWnd )
{
    ATLASSERT(::IsWindow(hWnd));
    if ( ::IsWindow(hWnd) )
    {
        DWORD dwThreadId = GetWindowThreadProcessId( hWnd, NULL );
        return ::PostThreadMessage(dwThreadId, WM_QUIT, 0, 0);
    }
    
    return FALSE;
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
    CDWMessageLoop theLoop;
    CDWClientMgt& clt = CDWClientMgt::Instance();

    HRESULT hRes = ::OleInitialize(NULL);
    ATLASSERT(SUCCEEDED(hRes));

    _Module.AddMessageLoop(&theLoop);
    
    ATL::CString strURL = clt.m_strOpenURL;

    CDWWebWnd wndWeb;

    wndWeb.m_hNotifyWnd = *pWndRet;
    if ( wndWeb.Create(*pWndRet, &wndWeb.rcDefault, NULL) == NULL )
    {
        clt.m_piWebBrowser = NULL;
        ::SetEvent(clt.m_hCreateWebWndEvent);
        return 0;
    }
    clt.m_piWebBrowser = wndWeb.m_spWebBrowser.p;
    *pWndRet = wndWeb;
    ::SetEvent(clt.m_hCreateWebWndEvent);

    if ( strURL.GetLength() > 0 )
        wndWeb.OpenURL(strURL);

    clt._AddWebWnd(&wndWeb);

    theLoop.AddMessageFilter(&wndWeb);
    dwRet = theLoop.Run();

    clt._RemoveWebWnd(&wndWeb);

    if ( ::IsWindow(wndWeb) )
        wndWeb.DestroyWindow();

    _Module.RemoveMessageLoop();

    OleUninitialize();
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