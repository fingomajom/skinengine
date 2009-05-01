#include "StdAfx.h"
#include "CDWMainFrame.h"


CDWMainFrame::CDWMainFrame(void)
{
}

CDWMainFrame::~CDWMainFrame(void)
{
}


HWND CDWMainFrame::CreateEx()
{
    RECT rcClient = { 0 };
    SystemParametersInfo( SPI_GETWORKAREA, 0, &rcClient, 0 );

    rcClient.left   = ( rcClient.right - rcClient.left ) / 8;
    rcClient.right  = rcClient.left * 7;
    rcClient.top    = ( rcClient.bottom - rcClient.top ) / 8;
    rcClient.bottom = rcClient.top * 7;

    return Create( NULL, &rcClient, _T("DW_Explorer"), 
         WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN );
}

BOOL CDWMainFrame::PreTranslateMessage(MSG* pMsg)
{
    return FALSE;
}

LRESULT CDWMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->AddMessageFilter(this);

    m_wndAx.Create( m_hWnd, &rcDefault, L"http://www.baidu.com", WS_CHILD | WS_VISIBLE );

    m_wndSuperbar.Create( m_hWnd, &rcDefault, NULL, WS_CHILD | WS_VISIBLE );
    m_wndFavoriteBar.Create( m_hWnd, &rcDefault, NULL, WS_CHILD | WS_VISIBLE );
    m_wndTableBar.Create( m_hWnd, &rcDefault, NULL, WS_CHILD | WS_VISIBLE );

    STARTUPINFO			stInfo		= { 0 };
    PROCESS_INFORMATION psInfo		= { 0 };
    stInfo.cb = sizeof(STARTUPINFO);
    
    TCHAR szModuleFile[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, szModuleFile, MAX_PATH);

    ATL::CString strCmdLine;
    strCmdLine.Format(L"%ld", (UINT)m_hWnd);

    _tcscat_s( szModuleFile, L" " );
    _tcscat_s( szModuleFile, strCmdLine );

    BOOL bCreate = CreateProcess(
        NULL, szModuleFile, 
        NULL, NULL, TRUE, 
        BELOW_NORMAL_PRIORITY_CLASS, 
        NULL, NULL, &stInfo, &psInfo);

    ::CloseHandle(psInfo.hProcess);
    ::CloseHandle(psInfo.hThread);

    return 0;
}


LRESULT CDWMainFrame::OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
    if((GetStyle() & (WS_CHILD)) == 0)
        ::PostQuitMessage(1);

    bHandled = FALSE;

    return 1;
}

LRESULT CDWMainFrame::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;
    
    RECT rcClient  = { 0 };
    RECT rcToolBar = { 0 };
    GetClientRect(&rcClient);

    rcToolBar = rcClient;
    int nspace = 20;

    rcToolBar.top    += 30;
    rcToolBar.bottom  = rcToolBar.top + nspace;
    
    m_wndSuperbar.MoveWindow( &rcToolBar );
    rcToolBar.top += nspace; rcToolBar.bottom += nspace;
    m_wndFavoriteBar.MoveWindow( &rcToolBar );
    rcToolBar.top += nspace; rcToolBar.bottom += nspace;
    m_wndTableBar.MoveWindow( &rcToolBar );

    rcToolBar.top    = rcToolBar.bottom + 1;
    rcToolBar.bottom = rcClient.bottom;
    //m_wndAx.MoveWindow( &rcToolBar );


    return 0L;
}


LRESULT CDWMainFrame::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
    if ( !IsZoomed() && GET_Y_LPARAM(lParam) < 20 )
    {
        PostMessage( WM_NCLBUTTONDOWN, HTCAPTION, lParam );
    }

    bHandled = FALSE;

    return 1L;
}


LRESULT CDWMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    PostMessage(WM_CLOSE);
    return 0;
}

LRESULT CDWMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    return 0;
}

LRESULT CDWMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    return 0;
}

