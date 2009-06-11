#include "StdAfx.h"
#include "CDWMainFrame.h"
#include "DWConfigDB.h"


#define IDS_FRM_TITLE L"%s - 视频提取测试工具"


CDWMainFrame::CDWMainFrame(void)
{
    m_pNowChildFrm = NULL;
}

CDWMainFrame::~CDWMainFrame(void)
{
}


HWND CDWMainFrame::CreateEx()
{
    CDWBaseConfig cfg;

    RECT rcWindow   = { 0 };
    BOOL bMaxed     = FALSE;
    cfg.get_Frm_Default_Rect( rcWindow, bMaxed );

    RECT rcWorkArea = { 0 };
    SystemParametersInfo( SPI_GETWORKAREA, 0, &rcWorkArea, 0 );

    if ( bMaxed || ( rcWindow.right - rcWindow.left ) < 400 || ( rcWindow.bottom - rcWindow.top ) < 300)
    {
        rcWindow = rcWorkArea;

        rcWindow.left   = ( rcWindow.right - rcWindow.left ) / 8;
        rcWindow.right  = rcWindow.left * 7;
        rcWindow.top    = ( rcWindow.bottom - rcWindow.top ) / 8;
        rcWindow.bottom = rcWindow.top * 7;
    }
    else
    {
        if ( rcWindow.left < rcWorkArea.left )
            rcWindow.left = rcWorkArea.left;
        if ( rcWindow.top < rcWorkArea.top )
            rcWindow.top = rcWorkArea.top;
        if ( rcWindow.right > rcWorkArea.right )
            rcWindow.right = rcWorkArea.right;
        if ( rcWindow.bottom > rcWorkArea.bottom )
            rcWindow.bottom = rcWorkArea.bottom;
    }

    ATL::CString strTitle;
    strTitle.Format(IDS_FRM_TITLE, L"空白页");
    HWND hRet = Create( NULL, &rcWindow, strTitle, 
        WS_POPUP | WS_SYSMENU | WS_SIZEBOX | 
         WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN );

    if ( bMaxed )
    {
        ::SetForegroundWindow(m_hWnd);
        BringWindowToTop();
        PostMessage( WM_SYSCOMMAND, SC_MAXIMIZE );
    }

    return hRet;
}

BOOL CDWMainFrame::PreTranslateMessage(MSG* pMsg)
{
    if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB )
    {
        HWND hFWnd = GetFocus();

        if ( m_wndSuperbar.m_address_edit == hFWnd )
            m_wndSuperbar.m_search_edit.SetFocus();
        else if ( m_wndSuperbar.m_search_edit == hFWnd )
        {
            if ( m_pNowChildFrm != NULL )
            {
                m_pNowChildFrm->SetFocus();
            }
        }
    }
    
#ifndef __TEST_WEB_WND__

    if ( m_wndSuperbar.PreTranslateMessage(pMsg) )
        return TRUE;

    return FALSE;

#else

    //return m_wndClient.PreTranslateMessage(pMsg);


    // give HTML page a chance to translate this message
    return (BOOL)m_wndClient.SendMessage(WM_FORWARDMSG, 0, (LPARAM)pMsg);

#endif

}

LRESULT CDWMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->AddMessageFilter(this);


    HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
        IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
    SetIcon(hIcon, TRUE);
    HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
        IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
    SetIcon(hIconSmall, FALSE);


    m_wndSuperbar.Create   ( m_hWnd, &rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN );    
    m_wndFavoriteBar.Create( m_hWnd, &rcDefault, NULL, WS_CHILD | WS_VISIBLE );
    m_wndTableBar.Create   ( m_hWnd, &rcDefault, NULL, WS_CHILD | WS_VISIBLE );
    m_wndStatusBar.Create  ( m_hWnd, &rcDefault, NULL, WS_CHILD | WS_VISIBLE );

    m_wndChildFrmBkgnd.m_wndParent = m_hWnd;
    m_wndChildFrmBkgnd.Create( NULL, &rcDefault, NULL, WS_POPUP, WS_EX_TOOLWINDOW );

    CDWEventSvr::Instance().AddCallback(this);

#ifdef __TEST_WEB_WND__
    m_wndClient.Create( m_hWnd, &rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPCHILDREN );
    m_wndClient.OpenURL(L"http://www.sogou.com");
#else
    //OnNewURL(NULL);
#endif

    return 0L;
}


LRESULT CDWMainFrame::OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
    ::PostQuitMessage(1);

    CDWBaseConfig cfg;

    RECT rcWindow   = { 0 };
    BOOL bMaxed     = IsZoomed();

    GetWindowRect(&rcWindow);
    cfg.set_Frm_Default_Rect( rcWindow, bMaxed );

    m_wndChildFrmBkgnd.DestroyWindow();

    bHandled = FALSE;

    return 1;
}

LRESULT CDWMainFrame::OnWndPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if ( m_pNowChildFrm != NULL && m_pNowChildFrm->IsWindow() )
    {
        m_pNowChildFrm->ResizeClient(MAKEWPARAM(1,0),TRUE);

        if ( m_wndChildFrmBkgnd.IsWindow() && m_wndChildFrmBkgnd.IsWindowVisible() )
        {
            RECT rcWindow;
            m_pNowChildFrm->GetWindowRect(&rcWindow);

            m_wndChildFrmBkgnd.SetWindowPos(
                NULL, rcWindow.left, rcWindow.top,
                rcWindow.right-rcWindow.left, 
                rcWindow.bottom-rcWindow.top,
                SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    }

    return DefWindowProc();
}

LRESULT CDWMainFrame::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;

    DWORD dwStyle = GetStyle();
    if ((dwStyle & WS_MINIMIZE) == WS_MINIMIZE)
        return 1L;

    
    RECT rcClient  = { 0 };
    RECT rcToolBar = { 0 };
    RECT rcStatusBar = { 0 };
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
    if ( m_wndSuperbar.IsWindow() )
        m_wndSuperbar.MoveWindow( &rcToolBar );

    rcToolBar.top    = rcToolBar.bottom; 
    rcToolBar.bottom = rcToolBar.top + nspace + 2;

    if ( m_wndFavoriteBar.IsWindow() )
        m_wndFavoriteBar.MoveWindow( &rcToolBar );

    rcToolBar.top = rcToolBar.bottom; 
    rcToolBar.bottom = rcToolBar.top + 28;
    if ( m_wndTableBar.IsWindow() )
        m_wndTableBar.MoveWindow( &rcToolBar );

    rcStatusBar = rcClient;
    rcStatusBar.top = rcStatusBar.bottom - 20;

    if ( m_wndStatusBar.IsWindow() )
    {
        m_wndStatusBar.MoveWindow( &rcStatusBar );
    }

    rcToolBar.top    = rcToolBar.bottom;
    rcToolBar.bottom = rcStatusBar.top;

    if ( m_pNowChildFrm != NULL && m_pNowChildFrm->IsWindow() )
    {
        m_pNowChildFrm->MoveWindow( &rcToolBar );
    }

#ifdef __TEST_WEB_WND__

    if ( m_wndClient.IsWindow() )
        m_wndClient.MoveWindow( &rcToolBar );
    
#endif

    return 0L;
}

void CDWMainFrame::GetChildFrmRect(RECT& rcChildFrm)
{
    GetClientRect(&rcChildFrm);
    
    if ( IsZoomed() )
    {
        rcChildFrm.top += 1;
    }

    rcChildFrm.top += 94;
    rcChildFrm.bottom -= 20;
}


LRESULT CDWMainFrame::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    if ( m_pNowChildFrm != NULL && m_pNowChildFrm->IsWindow() )
    {
        //m_pNowChildFrm->SetFocus();
    }

    return 0L;
}

LRESULT CDWMainFrame::OnActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    WORD fActive    = LOWORD(wParam); 
    BOOL fMinimized = (BOOL)HIWORD(wParam);

    if ( fActive != WA_INACTIVE && !fMinimized )
    {
        if ( m_pNowChildFrm != NULL && m_pNowChildFrm->IsWindow() )
        {
            //m_pNowChildFrm->ShowClient();
        }
    }

    return DefWindowProc();
}

CDWChildFrm* CDWMainFrame::OnNewURL( LPCTSTR pszURL, BOOL bActive  )
{
    int nIdx = m_wndTableBar.GetSelectIndex();

    ATL::CString strTitle;
    if ( (pszURL == NULL || lstrlenW(pszURL) <= 0 ) || !StrCmpI(pszURL, BLANK_URL) )
        strTitle = L"空白页";
    else
        strTitle = pszURL;

    pszURL =  (pszURL == NULL || lstrlenW(pszURL) <= 0 ) ? BLANK_URL : pszURL;

    RECT rcChildFrm = { 0 };
    GetChildFrmRect(rcChildFrm);

    CDWChildFrm* pNewFrm = CDWChildFrm::CreateChildFrm( 
        m_hWnd, 
        rcChildFrm,
        m_wndTableBar,
        m_wndChildFrmBkgnd,
        strTitle, 
        pszURL);
    if ( pNewFrm == NULL )
        return NULL;

    m_wndTableBar.InsertTableItem( ++nIdx, strTitle, 0, (LPARAM)pNewFrm );

    m_listChildFrm.AddTail(pNewFrm);

    if ( bActive )
    {
        m_wndTableBar.SelectIndex( nIdx );
        //OnSelectURL(nIdx);
    }
    CDWEventSvr::Instance().OnMessage( eid_addr_changed, (WPARAM) pszURL, 0 );
    
    return pNewFrm;
}


void CDWMainFrame::OnOpenURL( LPCTSTR pszURL )
{
    ATLASSERT( m_pNowChildFrm != NULL );
    if ( m_pNowChildFrm != NULL )
        m_pNowChildFrm->OnOpenURL( pszURL );
}


void CDWMainFrame::OnCloseURL( int nIndex )
{
    if ( m_wndTableBar.GetItemCount() == 1) // 最后一页 不关闭 
    {   
        CDWSkinUIMgt* pskin = CDWSkinUIMgt::InstancePtr();
        if ( pskin == NULL )
            return;

        OnOpenURL(NULL);

        ::SetForegroundWindow(m_hWnd);
        m_wndSuperbar.m_address_edit.SetSel(0,-1);
        m_wndSuperbar.m_address_edit.SetFocus();

        CDWEventSvr::Instance().OnMessage( 
            edi_spr_icon_changed, (WPARAM)pskin->iconNull.m_hIcon, 0 );

        return ;
    }

    int nSelIndex = m_wndTableBar.GetSelectIndex();

    ATLASSERT( nSelIndex == nIndex );

    CDWChildFrm* pCloseFrm = (CDWChildFrm*)m_wndTableBar.GetItemParam(nIndex);   
    m_wndTableBar.RemoveTableItem(nSelIndex);

    ATLASSERT( m_pNowChildFrm == pCloseFrm );

    POSITION pos = m_listChildFrm.Find(pCloseFrm);
    ATLASSERT( pos != NULL );
    if ( pos != NULL )
        m_listChildFrm.RemoveAt(pos);

    m_pNowChildFrm = NULL;
    
    if ( --nSelIndex < 0 )
        nSelIndex = 0;
    
    m_wndTableBar.SelectIndex( nSelIndex );
    OnSelectURL(nSelIndex);

    pCloseFrm->DestroyWindow();

}

void CDWMainFrame::OnSelectURL( int nIndex )
{
    CDWChildFrm* pNextFrm = (CDWChildFrm*)m_wndTableBar.GetItemParam(nIndex);
    ATLASSERT( pNextFrm != NULL );
    ATLASSERT( m_listChildFrm.Find(pNextFrm) != NULL );
    if ( pNextFrm == NULL )
        return;

#ifndef __TEST_WEB_WND__
    

    if ( pNextFrm != m_pNowChildFrm )
    {
        RECT rcChildFrm;
        GetChildFrmRect(rcChildFrm);

        LPARAM lParam = 0;
        if ( m_pNowChildFrm != NULL && m_pNowChildFrm->m_wndClient.IsWindow() )
            lParam = (LPARAM)m_pNowChildFrm->m_wndClient.m_hWnd;

        pNextFrm->MoveWindow(&rcChildFrm, FALSE);
        pNextFrm->SetFocus();
        pNextFrm->ShowClient(lParam);

        if ( m_pNowChildFrm != NULL )
        {
            ATLASSERT( m_listChildFrm.Find(m_pNowChildFrm) != NULL );

            m_pNowChildFrm->HideClient();
        }

        m_pNowChildFrm = pNextFrm;
    }

#endif
}


LRESULT CDWMainFrame::OnEventMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if ( uMsg == edi_open_url )
    {
        if ( lParam )
        {
#ifndef __TEST_WEB_WND__

            if ( m_pNowChildFrm != NULL && m_pNowChildFrm->IsWindow() &&
                 m_pNowChildFrm->m_strURL == BLANK_URL )
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
    else if ( uMsg == eid_addr_changed )
    {
        LPCTSTR pszTitle = (LPCTSTR)lParam;
        if ( lstrlenW(pszTitle) > 0 && StrCmp(pszTitle, L"空白页") )
        {
            ATL::CString strNewTitle;
            strNewTitle.Format(IDS_FRM_TITLE, pszTitle);
            SetWindowText(strNewTitle);
        }

        m_sys_title.Invalidate();
    }
    else if ( uMsg == edi_spr_icon_changed )
    {
        //SetIcon( (HICON) wParam, FALSE );
        //m_sys_title.Invalidate();
    }
    else if ( uMsg == edi_skin_changed )
    {
        RedrawWindow(NULL, NULL, RDW_FRAME | RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
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

LRESULT CDWMainFrame::OnWebViewCreate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    CDWChildFrm* pNew = OnNewURL((LPCTSTR)wParam, lParam);

    ATLASSERT( pNew != NULL  && pNew->IsWindow() );
    
    if ( pNew != NULL )
        return (LRESULT)pNew->m_hWnd;

    return NULL;
}

LRESULT CDWMainFrame::OnWebViewOpenSearch(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    m_wndSuperbar.OpenSerach((LPCTSTR)wParam);
    
    return 1L;
}


LRESULT CDWMainFrame::OnReflashURL(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    if ( m_pNowChildFrm != NULL && m_pNowChildFrm->IsWindow() )
    {
        m_pNowChildFrm->OnOpenURL( m_pNowChildFrm->m_strURL );
    }

    return 1L;
}

LRESULT CDWMainFrame::OnKeydown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if ( wParam == VK_F5 && m_wndChildFrmBkgnd.IsWindowVisible() )
    {
        PostMessage(WM_REFLASH_URL);
    }

    return DefWindowProc();
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

