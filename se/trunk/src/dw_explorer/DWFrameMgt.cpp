#include "StdAfx.h"
#include "DWFrameMgt.h"
#include "CDWMainFrame.h"
#include "DWProcessMgt.h"
#include "DWEventSvr.h"
#include "DWFavIconMgt.h"

DWORD CDWFrameMgt::m_dwMainThreadId = NULL;

HANDLE  g_hMutex = NULL;
LPCTSTR g_pszMutexName =  L"Global\\{96D77347-C566-4749-A1B9-C5B6CCE35106}";


int DWSvrReceiveRpcMsg( 
    /* [in]  */ int nMsgId,
    /* [in]  */ IDataBuffer*  pParameter,
    /* [out] */ IDataBuffer** ppResult)
{
    if ( nMsgId == c2s_get_pid )
        return GetCurrentProcessId();

    return 0;
}


CDWFrameMgt::CDWFrameMgt(void)
{
}

CDWFrameMgt::~CDWFrameMgt(void)
{
}


int CDWFrameMgt::RunMainMsgLoop( LPTSTR lpstrCmdLine )
{
    int nRet = 0;

    CDWMessageLoop theLoop;

    _Module.AddMessageLoop(&theLoop);

    CDWFrameMgt& frmmgt = CDWFrameMgt::Instance();

    CDWProcessMgt::Instance();
    CDWFavIconMgt::Instance();

    g_hMutex = OpenMutex( MUTEX_MODIFY_STATE, TRUE, g_pszMutexName );
    if ( g_hMutex != NULL )
    {
        ReleaseMutex (g_hMutex);
        return 0;
    }
    g_hMutex = CreateMutex(NULL, TRUE, g_pszMutexName );

    frmmgt.m_rpcSvr.SetReceiveFunc(DWSvrReceiveRpcMsg);
    
    if ( !frmmgt.m_rpcSvr.InitRpcServer(Rpc_Svr_EPoint_Name) )
        return 0;

    frmmgt.m_dwMainThreadId = GetCurrentThreadId();
    frmmgt.CreateNewFrame( 0 );
    
    nRet = theLoop.Run();

    frmmgt.m_rpcSvr.UninitRpcServer();
    ReleaseMutex ( g_hMutex );
    _Module.RemoveMessageLoop();

    
    CDWFrameMgt::DeleteInstance();
    CDWFavIconMgt::DeleteInstance();
    CDWProcessMgt::DeleteInstance();
    CDWEventSvr ::DeleteInstance();
    CDWSearchMgt::DeleteInstance();
    CDWSkinUIMgt::DeleteInstance();
    CDWSkinUIMgt::DeleteInstance();
    CDWSearchMgt::DeleteInstance();

    return nRet;
}

BOOL CDWFrameMgt::CreateNewFrame( LPVOID p )
{
    BOOL   bResult    = FALSE;    
    DWORD  dwThreadId = 0;
    HANDLE hThread    = NULL;


    hThread = CreateThread( NULL, 0, FrameMsgLoopThread, p, 0, &dwThreadId);
    if ( hThread != NULL )
    {
        CloseHandle( hThread );
        bResult = TRUE;
    }

    return bResult;
}

void CDWFrameMgt::_AddFrame( CDWMainFrame* pf )
{
    m_cs.Lock();
    m_listFrame.AddTail( pf );
    m_cs.Unlock();
}
void CDWFrameMgt::_RemoveFrame( CDWMainFrame* pf )
{
    BOOL bAllQuit = FALSE;

    m_cs.Lock();

    POSITION pos = m_listFrame.Find( pf );
    if ( pos )
        m_listFrame.RemoveAt( pos );

    bAllQuit = (m_listFrame.GetCount() == 0);

    m_cs.Unlock();

    if ( bAllQuit )
        PostThreadMessage(m_dwMainThreadId, WM_QUIT, 0, 0);
}

DWORD WINAPI CDWFrameMgt::FrameMsgLoopThread( LPVOID p )
{
    DWORD dwRet = 0;
    CDWMessageLoop theLoop;
    CDWFrameMgt& mgt = CDWFrameMgt::Instance();

    HRESULT hRes = ::OleInitialize(NULL);
    ATLASSERT(SUCCEEDED(hRes));

    AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	

    _Module.AddMessageLoop(&theLoop);
    
    CDWMainFrame wndMain;
    if(wndMain.CreateEx() == NULL)
    {
        ATLTRACE(_T("Main window creation failed!\n"));
        return 0;
    }
    wndMain.ShowWindow(SW_SHOWNORMAL);
    
    mgt._AddFrame(&wndMain);
    
    dwRet = theLoop.Run();

    mgt._RemoveFrame(&wndMain);

    _Module.RemoveMessageLoop();
    
    OleUninitialize();

    return dwRet;
}
