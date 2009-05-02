#include "StdAfx.h"
#include "CDWMainFrame.h"
#include "DWProcessMgt.h"

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

    m_wndSuperbar.Create( m_hWnd, &rcDefault, NULL, WS_CHILD | WS_VISIBLE );
    m_wndFavoriteBar.Create( m_hWnd, &rcDefault, NULL, WS_CHILD | WS_VISIBLE );
    m_wndTableBar.Create( m_hWnd, &rcDefault, NULL, WS_CHILD | WS_VISIBLE );

    CDWProcessMgt::Instance().CreateWebWnd(m_hWnd, &m_wndAx.m_hWnd);

    return 0;
}


LRESULT CDWMainFrame::OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
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

    rcToolBar.top    += (22 );
    rcToolBar.bottom  = rcToolBar.top + nspace;
    
    if ( IsZoomed() )
    {
        rcToolBar.top    += GetSystemMetrics(SM_CYSIZEFRAME);
        rcToolBar.bottom += GetSystemMetrics(SM_CYSIZEFRAME);
    }

    if ( ::IsWindow(m_wndTableBar) )
        m_wndSuperbar.MoveWindow( &rcToolBar );
    rcToolBar.top += nspace; rcToolBar.bottom += nspace;
    if ( ::IsWindow(m_wndFavoriteBar) )
        m_wndFavoriteBar.MoveWindow( &rcToolBar );
    rcToolBar.top += nspace; rcToolBar.bottom += nspace;
    if ( ::IsWindow(m_wndTableBar) )
        m_wndTableBar.MoveWindow( &rcToolBar );

    rcToolBar.top    = rcToolBar.bottom + 1;
    rcToolBar.bottom = rcClient.bottom+1;

    if ( ::IsWindow(m_wndAx) )
    {
        m_wndAx.ShowWindow( SW_SHOWDEFAULT );
        m_wndAx.MoveWindow( &rcToolBar );
    }


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

