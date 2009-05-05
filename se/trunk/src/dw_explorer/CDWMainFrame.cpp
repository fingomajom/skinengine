#include "StdAfx.h"
#include "CDWMainFrame.h"


#ifdef __TEST_WEB_WND__
CDWMainFrame::CDWMainFrame(void)
#else
CDWMainFrame::CDWMainFrame(void) :
    m_wndClient(m_wndTableBar)
#endif
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
         WS_POPUPWINDOW | WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN );
}

BOOL CDWMainFrame::PreTranslateMessage(MSG* pMsg)
{
    //if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB)
    //{
    //    return m_wndClient.PreTranslateMessage(pMsg);
    //}
    
    m_wndSuperbar.PreTranslateMessage(pMsg);

    return m_wndClient.PreTranslateMessage(pMsg);
}

LRESULT CDWMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->AddMessageFilter(this);

    m_wndSuperbar.Create( m_hWnd, &rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN );    
    m_wndFavoriteBar.Create( m_hWnd, &rcDefault, NULL, WS_CHILD | WS_VISIBLE );
    m_wndTableBar.Create( m_hWnd, &rcDefault, NULL, WS_CHILD | WS_VISIBLE );

    m_wndClient.Create( m_hWnd, &rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPCHILDREN );
#ifdef __TEST_WEB_WND__
    m_wndClient.OpenURL(L"www.cnbeta.com");
#endif

    m_wndClient.SetFocus();

    CDWEventSvr::Instance().AddCallback(this);
    OnNewURL(NULL);


    return 0L;
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

    rcToolBar.top    += ( 22 );
    rcToolBar.bottom  = rcToolBar.top + nspace;
    
    if ( IsZoomed() )
    {
        rcToolBar.top    += 1;
        rcToolBar.bottom += 1;
    }

    rcToolBar.bottom += 2;
    if ( ::IsWindow(m_wndTableBar) )
        m_wndSuperbar.MoveWindow( &rcToolBar );

    rcToolBar.top    = rcToolBar.bottom; 
    rcToolBar.bottom = rcToolBar.top + nspace + 2;

    if ( ::IsWindow(m_wndFavoriteBar) )
        m_wndFavoriteBar.MoveWindow( &rcToolBar );

    rcToolBar.top = rcToolBar.bottom; 
    rcToolBar.bottom = rcToolBar.top + 28;
    if ( ::IsWindow(m_wndTableBar) )
        m_wndTableBar.MoveWindow( &rcToolBar );

    rcToolBar.top    = rcToolBar.bottom;
    rcToolBar.bottom = rcClient.bottom;

    if ( ::IsWindow(m_wndClient) )
    {
        m_wndClient.MoveWindow( &rcToolBar );
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

void CDWMainFrame::OnNewURL( LPCTSTR pszURL )
{
    CDWProcessMgt& psmgt= CDWProcessMgt::Instance();

    static unsigned short uId = 0;
    static int nIdx = 0;
    uId++;

    nIdx = m_wndTableBar.GetSelectIndex();
    nIdx++;

    ATL::CString strCaption;
    if ( (pszURL == NULL || lstrlenW(pszURL) <= 0 ) || !StrCmpI(pszURL, L"about:blank") )
        strCaption = L"¿Õ°×Ò³";
    else
        strCaption = pszURL;

    m_wndTableBar.InsertTableItem( nIdx, 
        strCaption, uId, uId );

    pszURL =  (pszURL == NULL || lstrlenW(pszURL) <= 0 ) ? L"about:blank" : pszURL;

    CDWEventSvr::Instance().OnMessage( eid_addr_changed, (WPARAM) pszURL, 0 );

#ifndef __TEST_WEB_WND__
    m_wndClient.m_mapUrlWndInfo[(HWND)uId].strURL   = pszURL;
    m_wndClient.m_mapUrlWndInfo[(HWND)uId].strTitle = strCaption;
#endif

    psmgt.CreateWebWnd( m_wndClient, MAKELPARAM(uId, nIdx), pszURL);

    m_wndTableBar.SelectIndex(nIdx);
}


void CDWMainFrame::OnOpenURL( LPCTSTR pszURL )
{
    CDWProcessMgt& psmgt= CDWProcessMgt::Instance();

    int nIdx = m_wndTableBar.GetSelectIndex();

    pszURL =  (pszURL == NULL || lstrlenW(pszURL) <= 0 ) ? L"about:blank" : pszURL;

    CDWEventSvr::Instance().OnMessage( eid_addr_changed, (WPARAM) pszURL, 0 );

    psmgt.WebWndOpenURL( (HWND)m_wndTableBar.GetItemParam(nIdx), pszURL );

}


void CDWMainFrame::OnCloseURL( int nIndex )
{
    CDWProcessMgt& psmgt= CDWProcessMgt::Instance();

    if ( m_wndTableBar.GetItemCount() == 1)
        return ;
    int nSelIndex = m_wndTableBar.GetSelectIndex();

    ATLASSERT( nSelIndex == nIndex );

    HWND hSelWnd = (HWND)m_wndTableBar.GetItemParam(nIndex);
    m_wndTableBar.RemoveTableItem(nSelIndex);
    psmgt.DestryWebWnd( hSelWnd );
    
    if ( nSelIndex >= m_wndTableBar.GetItemCount() )
        nSelIndex = m_wndTableBar.GetItemCount()-1;
    m_wndTableBar.SelectIndex( nSelIndex );
    OnSelectURL(nSelIndex);
}

void CDWMainFrame::OnSelectURL( int nIndex )
{
    HWND hSelWnd = (HWND)m_wndTableBar.GetItemParam(nIndex);
    //ATLASSERT(::IsWindow(hSelWnd));
#ifndef __TEST_WEB_WND__
    m_wndClient.SetFocus();
    m_wndClient.ShowClient( hSelWnd );
#endif
}


LRESULT CDWMainFrame::OnEventMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if ( uMsg == edi_open_url )
    {
        if ( lParam )
        {
#ifndef __TEST_WEB_WND__

            if ( m_wndClient.m_wndClient.IsWindow() && 
                 m_wndClient.m_mapUrlWndInfo[m_wndClient.m_wndClient].strURL == L"about:blank" )
            {
                OnOpenURL( (LPCTSTR) wParam );
            }
            else
#endif
                OnNewURL( (LPCTSTR) wParam );
        }
        else
            OnOpenURL( (LPCTSTR) wParam );
    }

    return 0;
}

LRESULT CDWMainFrame::OnTableBarMsg(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    switch (wParam)
    {
    case TGM_SYS_BTN_CLICK:
        switch ( lParam )
        {
        case CDWTableBar::sys_tbi_new:
            {
                OnNewURL( NULL );
            }
            break;
        case CDWTableBar::sys_tbi_sleft:
            break;
        case CDWTableBar::sys_tbi_rleft:
            break;
        case CDWTableBar::sys_tbi_menu:
            break;
        case CDWTableBar::sys_tbi_prev:
            break;
        }
        break;
    case TGM_ITEM_CLOSE_CLICK:
        OnCloseURL(lParam);
        break;
    case TGM_SELECT_CHANGE:
        OnSelectURL(lParam);
        break;
    }

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

