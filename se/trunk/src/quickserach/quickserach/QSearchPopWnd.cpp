#include "stdafx.h"
#include "QSearchPopWnd.h"
#include "QuickSearchMgr.h"
#include "RichEditCtrlEx.h"

#include "Markup.h"
#include <atlctrlx.h>


class CQSToolbar : public IWindowLayer
{
public:

    enum{
        client_width  = 220,
        client_height = 30
    };

    virtual BOOL Create( HWND hWndParent )
    {
        m_hWndParent = hWndParent;

        m_kw_edit.Create ( m_hWndParent, CWindow::rcDefault, NULL, 
            WS_CHILD | WS_BORDER | WS_TABSTOP, 0, IDC_KEYWORD_EDIT);
        
        
        m_qs_btn.Create  ( m_hWndParent, CWindow::rcDefault, L"G", WS_CHILD | BS_DEFPUSHBUTTON | WS_TABSTOP, 0, IDC_QSEARCH_BTN);
        m_cpy_btn.Create ( m_hWndParent, CWindow::rcDefault, L"C", WS_CHILD | WS_TABSTOP, 0, IDC_COPY_BTN);
        m_set_btn.Create ( m_hWndParent, CWindow::rcDefault, L"S", WS_CHILD | WS_TABSTOP, 0, IDC_SETTING_BTN);
        m_doct_btn.Create( m_hWndParent, CWindow::rcDefault, L"-", WS_CHILD | WS_TABSTOP, 0, IDC_DOCT_BTN);

        if ( ::IsWindow(m_kw_edit) )  m_kw_edit.SetFont( AtlGetDefaultGuiFont() );
        if ( ::IsWindow(m_qs_btn) )   m_qs_btn.SetFont( AtlGetDefaultGuiFont() );
        if ( ::IsWindow(m_cpy_btn) )  m_cpy_btn.SetFont( AtlGetDefaultGuiFont() );
        if ( ::IsWindow(m_set_btn) )  m_set_btn.SetFont( AtlGetDefaultGuiFont() );
        if ( ::IsWindow(m_doct_btn) )  m_doct_btn.SetFont( AtlGetDefaultGuiFont() );


        return TRUE;
    }

    virtual BOOL Destroy()
    {
        if ( ::IsWindow(m_kw_edit) )  m_kw_edit.DestroyWindow();
        if ( ::IsWindow(m_qs_btn) )   m_qs_btn.DestroyWindow();
        if ( ::IsWindow(m_cpy_btn) )  m_cpy_btn.DestroyWindow();
        if ( ::IsWindow(m_set_btn) )  m_set_btn.DestroyWindow();
        if ( ::IsWindow(m_doct_btn) )  m_doct_btn.DestroyWindow();

        return TRUE;
    }

    virtual BOOL ShowWindow( BOOL  bShow, HWND hWndParent )
    {
        if ( bShow && !::IsWindow(m_kw_edit) )
            Create( hWndParent );

        if ( ::IsWindow(m_kw_edit) )   m_kw_edit.ShowWindow( bShow ? SW_SHOWNOACTIVATE : SW_HIDE );
        if ( ::IsWindow(m_qs_btn) )    m_qs_btn.ShowWindow( bShow ? SW_SHOWNOACTIVATE : SW_HIDE );
        if ( ::IsWindow(m_cpy_btn) )   m_cpy_btn.ShowWindow( bShow ? SW_SHOWNOACTIVATE : SW_HIDE );
        if ( ::IsWindow(m_set_btn) )   m_set_btn.ShowWindow( bShow ? SW_SHOWNOACTIVATE : SW_HIDE );
        if ( ::IsWindow(m_doct_btn) )  m_doct_btn.ShowWindow( bShow ? SW_SHOWNOACTIVATE : SW_HIDE );

        return TRUE;
    }
    virtual BOOL MoveWindow( RECT rcClient, HWND hWndParent )
    {
        if ( !::IsWindow(m_kw_edit) )
            Create( hWndParent );
        
        RECT rcBtn = rcClient;
        const int nspace = 5;

        rcBtn.top    = rcClient.top + nspace ; 
        rcBtn.bottom = rcClient.bottom - nspace;
        rcBtn.right -= nspace;
        rcBtn.left  = rcBtn.right - ( client_height - nspace * 2 );

        m_doct_btn.MoveWindow(&rcBtn);

        rcBtn.right -= (client_height - nspace);
        rcBtn.left  -= (client_height - nspace);
        m_set_btn.MoveWindow(&rcBtn);
        rcBtn.right -= (client_height - nspace);
        rcBtn.left  -= (client_height - nspace);
        m_cpy_btn.MoveWindow(&rcBtn);
        rcBtn.right -= (client_height - nspace);
        rcBtn.left  -= (client_height - nspace);
        m_qs_btn.MoveWindow(&rcBtn);

        rcBtn.right  = rcBtn.left;
        rcBtn.left   = rcClient.left + nspace;
        m_kw_edit.MoveWindow(&rcBtn);

        return TRUE;
    }

    virtual void OnPaint( HDC hDC, RECT rcClient )
    {
        CDCHandle dc = hDC;

        rcClient.top = rcClient.bottom - 1;

        dc.Draw3dRect( &rcClient, RGB(173, 186, 200), RGB(173, 186, 200));
    }


    virtual BOOL IsWindowVisible()
    {
        if ( !::IsWindow(m_kw_edit) )
            return FALSE;
        return m_kw_edit.IsWindowVisible();
    }

    virtual BOOL GetClientSize( SIZE& sizeClient )
    {
        sizeClient.cx = client_width;
        sizeClient.cy = client_height;

        return TRUE;
    }

    BEGIN_MSG_MAP(CQSToolbar)
    END_MSG_MAP();


    CEdit   m_kw_edit;
    CButton m_qs_btn;
    CButton m_cpy_btn;
    CButton m_set_btn;
    CButton m_doct_btn;

    HWND    m_hWndParent;
};

class CQSReaultView : public IWindowLayer
{
public:

    enum{
        client_width  = 220,
        client_height = 60,

        client_max_width  = 500,
        client_max_height = 300,

        space_width   = 15,
        space_height  = 10
    };

    virtual BOOL Create( HWND hWndParent )
    {
        m_hWndParent = hWndParent;
        
        //m_ret_edit.SetNoFocus(TRUE);
        m_ret_edit.Create ( m_hWndParent, CWindow::rcDefault, NULL, 
            WS_CHILD  | ES_MULTILINE | ES_READONLY | ES_AUTOHSCROLL ,
            WS_EX_TRANSPARENT, IDC_RESULT_EDIT_BTN);
        if ( ::IsWindow(m_ret_edit) )  m_ret_edit.SetFont( AtlGetDefaultGuiFont() );

        m_ret_edit.AppendText( L"北京天气",  RICHEDIT_TEXT_FONT_BOLD );
        m_ret_edit.AppendEndOfLine();
        m_ret_edit.AppendEndOfLine();
        m_ret_edit.AppendText( L"  时间：2009年04月25日",  0 );
        m_ret_edit.AppendEndOfLine();
        m_ret_edit.AppendText( L"  风况：北风3-4级",  0 );

        return TRUE;
    }

    virtual BOOL Destroy()
    {
        if ( ::IsWindow(m_ret_edit) )  m_ret_edit.DestroyWindow();

        return TRUE;
    }

    virtual BOOL ShowWindow( BOOL  bShow, HWND hWndParent )
    {
        if ( bShow && !::IsWindow(m_ret_edit) )
            Create( hWndParent );

        if ( ::IsWindow(m_ret_edit) )   m_ret_edit.ShowWindow( bShow ? SW_SHOWNOACTIVATE : SW_HIDE );

        return TRUE;
    }
    virtual BOOL MoveWindow( RECT rcClient, HWND hWndParent )
    {
        if ( !::IsWindow(m_ret_edit) )
            Create( hWndParent );

        ::InflateRect(&rcClient, -space_width, -space_height);

        m_ret_edit.MoveWindow(&rcClient);

        return TRUE;
    }

    virtual void OnPaint( HDC dc, RECT rcClient )
    {
    }

    virtual BOOL IsWindowVisible()
    {
        if ( !::IsWindow(m_ret_edit) )
            return FALSE;
        return m_ret_edit.IsWindowVisible();
    }

    virtual BOOL GetClientSize( SIZE& sizeClient )
    {
        sizeClient.cx = client_width;
        sizeClient.cy = client_height;

        if ( ::IsWindow(m_ret_edit) )
        {
            TCHAR szBuffer[1025] = { 0 };

            int nLine = m_ret_edit.GetLineCount();
            sizeClient.cy = nLine * 16 + space_height * 2;
            if (sizeClient.cy < client_height)
                sizeClient.cy = client_height;

            CClientDC dc(m_ret_edit);

            HFONT hOldFont = dc.SelectFont( ::AtlGetDefaultGuiFont() );

            for ( int i = 0; i < nLine; i++)
            {
                m_ret_edit.GetLine( i, szBuffer, 1024 );
                if ( wcslen(szBuffer) <= 0 || szBuffer[0] == '\r' )
                    continue;

                SIZE sizeText = { 0 };
                dc.GetTextExtent( szBuffer, wcslen(szBuffer), &sizeText);
                if ( ( sizeText.cx + space_width*2 ) > sizeClient.cx )
                    sizeClient.cx = sizeText.cx + space_width*2 + 5;
            }

            dc.SelectFont(hOldFont);
        }

        if ( sizeClient.cx > client_max_width )
            sizeClient.cx = client_max_width;
        if ( sizeClient.cy > client_max_height )
            sizeClient.cy = client_max_height;

        return TRUE;
    }

    BEGIN_MSG_MAP(CQSToolbar)
    END_MSG_MAP();

    CRichEditCtrlEx m_ret_edit;

    HWND    m_hWndParent;
};


class CSogouTitleView : public IWindowLayer
{
public:

    CSogouTitleView() :
        m_bShow(FALSE),
        m_hWndParent(NULL)
    {
    }

    enum{
        client_width  = 220,
        client_height = 20,

        space_width   = 10,
        space_height  = 10
    };

    virtual BOOL Create( HWND hWndParent )
    {
        m_bShow = FALSE;
        m_hWndParent = hWndParent;

        m_strTitle = L"搜狗快搜";
        m_clrText  = RGB(0, 153, 204);

        m_font.Attach( AtlCreateBoldFont(AtlGetDefaultGuiFont()) );
        ATLASSERT(m_font.m_hFont != NULL);

        return TRUE;
    }

    virtual BOOL Destroy()
    {
        return TRUE;
    }

    virtual BOOL ShowWindow( BOOL  bShow, HWND hWndParent )
    {
        if ( m_hWndParent == NULL )
            Create( hWndParent );

        m_bShow = bShow;
        return TRUE;
    }
    virtual BOOL MoveWindow( RECT rcClient, HWND hWndParent )
    {
        if ( m_hWndParent == NULL )
            Create( hWndParent );

        return TRUE;
    }

    virtual void OnPaint( HDC hDC, RECT rcClient )
    {
        CDCHandle dc = hDC;
        
        ::InflateRect(&rcClient, -space_width, 0);
        rcClient.top += 4;

        HFONT hOldFont = dc.SelectFont( m_font );
        dc.SetBkMode(TRANSPARENT);
        dc.SetTextColor(m_clrText);

        dc.DrawText(m_strTitle, m_strTitle.GetLength(), 
            &rcClient, DT_LEFT | DT_VCENTER | DT_SINGLELINE);        

        SIZE sizeText = { 0 };

        dc.GetTextExtent(m_strTitle, -1, &sizeText);
        
        rcClient.left += ( sizeText.cx + 5 );

        rcClient.top = rcClient.bottom = ( rcClient.top + rcClient.bottom ) / 2;

        dc.Draw3dRect( &rcClient, RGB(230, 235, 239), m_clrText);

        dc.SelectFont(hOldFont);
    }

    virtual BOOL IsWindowVisible()
    {
        return m_bShow;
    }

    virtual BOOL GetClientSize( SIZE& sizeClient )
    {
        sizeClient.cx = client_width;
        sizeClient.cy = client_height;

        return TRUE;
    }

    BEGIN_MSG_MAP(CQSToolbar)
    END_MSG_MAP();


    CFont m_font;
    BOOL  m_bShow;
    HWND  m_hWndParent;
    COLORREF m_clrText;
    ATL::CString m_strTitle;
};


class CSettingView : public IWindowLayer
{
public:

    enum{
        client_width  = 220,
        client_height = 60,

        space_width   = 20,
        space_height  = 5,
    };

    CSettingView( CComPtr<IQuickSearchService>& spQSService ) :
        m_spQSService(spQSService)
    {
    }

    virtual BOOL Create( HWND hWndParent )
    {
        m_hWndParent = hWndParent;
        
        m_switch_cbox.Create  ( m_hWndParent, CWindow::rcDefault, 
            L"开启划词快捷搜索", WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP, WS_EX_TRANSPARENT, IDC_SETTING_CBOX);
        if ( ::IsWindow(m_switch_cbox) )  m_switch_cbox.SetFont( AtlGetDefaultGuiFont() );

        m_ret_btn.SetHyperLinkExtendedStyle(HLINK_UNDERLINED | HLINK_COMMANDBUTTON);        
        m_ret_btn.Create  ( m_hWndParent, CWindow::rcDefault, 
            L"返回", WS_CHILD | WS_TABSTOP, 0, IDC_RETURN_BTN );
        if ( ::IsWindow(m_ret_btn) )  m_ret_btn.SetFont( AtlGetDefaultGuiFont() );

        int nCount = 0;

        m_spQSService->GetQSearchProviderCount( &nCount );
        for ( int idx = 0; idx < nCount; idx ++ )
        {
            QSPINFO info = { 0 };
            m_spQSService->GetQSearchProviderInfo( idx, &info );
            
            ATL::CString strBtnText;
            strBtnText = L"开启";
            strBtnText += info.pszName;

            CButton btn;
            btn.Create ( m_hWndParent, CWindow::rcDefault, 
                strBtnText, WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP, 0, IDC_SETTING_CBOX + idx + 1);
            if ( ::IsWindow(btn) )  btn.SetFont( AtlGetDefaultGuiFont() );

            m_vt_cbox.Add( btn );
        }

        return TRUE;
    }

    virtual BOOL Destroy()
    {
        if ( ::IsWindow(m_switch_cbox) )  m_switch_cbox.DestroyWindow();
        if ( ::IsWindow(m_ret_btn) )  m_ret_btn.DestroyWindow();

        for ( int idx = 0; idx < m_vt_cbox.GetSize(); idx++ )
        {
            if ( ::IsWindow(m_vt_cbox[idx]) )
                ::DestroyWindow( m_vt_cbox[idx] );
        }

        return TRUE;
    }

    virtual BOOL ShowWindow( BOOL  bShow, HWND hWndParent )
    {
        if ( bShow && !::IsWindow(m_switch_cbox) )
            Create( hWndParent );

        if ( ::IsWindow(m_switch_cbox) )  m_switch_cbox.ShowWindow( bShow ? SW_SHOWNOACTIVATE : SW_HIDE );
        if ( ::IsWindow(m_ret_btn) )  m_ret_btn.ShowWindow( bShow ? SW_SHOW : SW_HIDE );

        for ( int idx = 0; idx < m_vt_cbox.GetSize(); idx++ )
        {
            if ( ::IsWindow(m_vt_cbox[idx]) )
                ::ShowWindow( m_vt_cbox[idx], bShow ? SW_SHOWNOACTIVATE : SW_HIDE );
        }

        return TRUE;
    }

    virtual BOOL MoveWindow( RECT rcClient, HWND hWndParent )
    {
        if ( !::IsWindow(m_switch_cbox) )
            Create( hWndParent );

        ::InflateRect(&rcClient, -space_width, -space_height );
        rcClient.bottom = rcClient.top + space_width;
        m_switch_cbox.MoveWindow(&rcClient);
        
        rcClient.left += space_width;
        for ( int idx = 0; idx < m_vt_cbox.GetSize(); idx++ )
        {
            rcClient.top += space_width;
            rcClient.bottom += space_width;
            
            CButton btn = m_vt_cbox[idx];
            btn.MoveWindow(&rcClient);
        }

        rcClient.top    += space_width;
        rcClient.bottom += space_width;
        
        rcClient.left = ( rcClient.left + rcClient.right - 60 ) / 2;

        m_ret_btn.MoveWindow(&rcClient);
        m_ret_btn.CalcLabelRect();

        return TRUE;
    }

    virtual void OnPaint( HDC dc, RECT rcClient )
    {
    }

    virtual BOOL IsWindowVisible()
    {
        if ( !::IsWindow(m_switch_cbox) )
            return FALSE;
        return m_switch_cbox.IsWindowVisible();
    }

    virtual BOOL GetClientSize( SIZE& sizeClient )
    {
        sizeClient.cx = client_width;
        sizeClient.cy = (m_vt_cbox.GetSize() + 2) * space_width + space_height * 2;

        return TRUE;
    }

    BEGIN_MSG_MAP(CSettingView)
        COMMAND_RANGE_CODE_HANDLER( IDC_SETTING_CBOX, IDC_SETTING_CBOX + 9  , BN_CLICKED, OnSettubgCBBtnClicked )
    END_MSG_MAP();

    LRESULT OnSettubgCBBtnClicked(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        if ( wID == IDC_SETTING_CBOX )
        {
            BOOL bCheck = m_switch_cbox.GetCheck();
            for ( int idx = 0; idx < m_vt_cbox.GetSize(); idx++ )
            {
                CButton btn = m_vt_cbox[idx];
                btn.EnableWindow( bCheck );
                if ( !bCheck )
                    btn.SetCheck( FALSE );
            }

        }

        return 1L;
    }


    CButton    m_switch_cbox;
    CHyperLink m_ret_btn;
    ATL::CSimpleArray<HWND> m_vt_cbox;

    HWND    m_hWndParent;

    CComPtr<IQuickSearchService>& m_spQSService;
};


class CSettingToolbarView : public IWindowLayer
{
public:

    enum{
        client_width  = 220,
        client_height = 60,

        space_width   = 15,
        space_height  = 10
    };

    CSettingToolbarView( CComPtr<IQuickSearchService>& spQSService ) :
        m_spQSService(spQSService)
    {
    }

    virtual BOOL Create( HWND hWndParent )
    {
        m_hWndParent = hWndParent;

        return TRUE;
    }

    virtual BOOL Destroy()
    {

        return TRUE;
    }

    virtual BOOL ShowWindow( BOOL  bShow, HWND hWndParent )
    {

        return TRUE;
    }
    virtual BOOL MoveWindow( RECT rcClient, HWND hWndParent )
    {

        return TRUE;
    }

    virtual void OnPaint( HDC dc, RECT rcClient )
    {
    }

    virtual BOOL IsWindowVisible()
    {
        if ( !::IsWindow(m_set_btn) )
            return FALSE;
        return m_set_btn.IsWindowVisible();
    }

    virtual BOOL GetClientSize( SIZE& sizeClient )
    {

        return TRUE;
    }

    BEGIN_MSG_MAP(CQSToolbar)
    END_MSG_MAP();

    CButton m_set_btn;
    ATL::CSimpleArray<HWND> m_vt_btn;

    HWND    m_hWndParent;

    CComPtr<IQuickSearchService>& m_spQSService;
};


CQSearchPopWnd::CQSearchPopWnd()
{
    m_clrBorder = RGB(173, 186, 200);
    m_clrWindow = RGB(240, 245, 249);
    m_byteTran  = 100;
    m_bMouseIn  = FALSE;
    m_bDoct     = FALSE;
}


BOOL CQSearchPopWnd::ShowQSPopWnd( POINT pt, LPCTSTR pszQSText )
{
    ShowWindow(SW_SHOWDEFAULT);
    ShowChildWindow(em_cid_qstoolbar   , TRUE);
    ShowChildWindow(em_cid_sogoutitle  , TRUE);
    ShowChildWindow(em_cid_qsresultview, TRUE);
    ShowChildWindow(em_cid_settingview , FALSE);
    ShowChildWindow(em_cid_settoolbar  , FALSE);
    m_byteTran = 180;
    m_bMouseIn = FALSE;
    SetTransparency( m_byteTran );
    ShowAutoSizeWnd();

    return TRUE;
}

BOOL CQSearchPopWnd::ShowQSPopWnd_ForQSBar( POINT pt )
{
    ShowWindow(SW_SHOWNOACTIVATE);

    ShowChildWindow(em_cid_qstoolbar   , FALSE);
    ShowChildWindow(em_cid_sogoutitle  , TRUE);
    ShowChildWindow(em_cid_qsresultview, TRUE);
    ShowChildWindow(em_cid_settingview , FALSE);
    ShowChildWindow(em_cid_settoolbar  , FALSE);
    m_byteTran = 255;
    m_bMouseIn = FALSE;
    SetTransparency( m_byteTran );
    ShowAutoSizeWnd();

    ((CSogouTitleView*)m_vtChildWindow[em_cid_sogoutitle])->m_strTitle = L"搜狗快搜";

    RECT rcWindow = { 0 };
    GetWindowRect(&rcWindow);

    pt.x = pt.x - (rcWindow.right - rcWindow.left);

    SetWindowPos( NULL, pt.x, pt.y, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOOWNERZORDER );

    return TRUE;
}

void CQSearchPopWnd::ShowChildWindow( int id, BOOL bShow )
{
    if ( id >= 0 && id < (int)m_vtChildWindow.GetCount())
    {
        IWindowLayer* piChildWnd = m_vtChildWindow[id];
        if ( piChildWnd != NULL )
            piChildWnd->ShowWindow( bShow, m_hWnd );
    }
}

void CQSearchPopWnd::SetTransparency( BYTE byteTran )
{
    if ( ::IsWindow(m_hWnd) )
        SetLayeredWindowAttributes( m_hWnd, 0, byteTran, LWA_ALPHA);
}

void CQSearchPopWnd::ShowAutoSizeWnd()
{
    SIZE  sizeWindow = { 0 };
    RECT  rcClient   = { 0 };

    for ( size_t idx = 0; idx < m_vtChildWindow.GetCount(); idx++ )
    {
        IWindowLayer* piChildWnd = m_vtChildWindow[idx];
        if ( piChildWnd != NULL && piChildWnd->IsWindowVisible() )
        {
            SIZE sizeChild = { 0 };
            if ( piChildWnd->GetClientSize( sizeChild ) )
            {
                sizeWindow.cy += sizeChild.cy;
                if ( sizeWindow.cx < sizeChild.cx )
                    sizeWindow.cx = sizeChild.cx;
            }
        }
    }
    
    SetWindowPos( NULL, 0, 0, sizeWindow.cx, sizeWindow.cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);

    rcClient.right = sizeWindow.cx;
    for ( size_t idx = 0; idx < m_vtChildWindow.GetCount(); idx++ )
    {
        IWindowLayer* piChildWnd = m_vtChildWindow[idx];
        if ( piChildWnd != NULL && piChildWnd->IsWindowVisible() )
        {
            SIZE sizeChild = { 0 };
            if ( piChildWnd->GetClientSize( sizeChild ) )
            {
                rcClient.bottom += sizeChild.cy;
                piChildWnd->MoveWindow( rcClient, m_hWnd );
                rcClient.top = rcClient.bottom;
            }
        }
    }
}

LRESULT CQSearchPopWnd::OnInitDialog(UINT /* uMsg */, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    m_vtChildWindow.SetCount( em_cid_count ); 
    ATLASSERT( m_vtChildWindow.GetCount() == em_cid_count );

    m_vtChildWindow[em_cid_qstoolbar] = new CQSToolbar;
    m_vtChildWindow[em_cid_sogoutitle] = new CSogouTitleView;
    m_vtChildWindow[em_cid_qsresultview] = new CQSReaultView;
    m_vtChildWindow[em_cid_settingview] = new CSettingView(m_spQSService);
    m_vtChildWindow[em_cid_settoolbar] = new CSettingToolbarView(m_spQSService);


    SetFont( ::AtlGetDefaultGuiFont() );

    SetWindowLong( GWL_EXSTYLE, GetWindowLong( GWL_EXSTYLE ) | WS_EX_LAYERED );
    
    m_vtChildWindow[em_cid_qstoolbar]->Create( m_hWnd );
    m_tooltip.Create( m_hWnd );

    m_tooltip.AddTool(GetDlgItem(IDC_QSEARCH_BTN), L"快搜" );
    m_tooltip.AddTool(GetDlgItem(IDC_COPY_BTN)   , L"复制快搜结果到剪贴板" );
    m_tooltip.AddTool(GetDlgItem(IDC_SETTING_BTN), L"设置" );
    m_tooltip.AddTool(GetDlgItem(IDC_DOCT_BTN)   , L"固定面板" );

    m_tooltip.Activate(TRUE);

    return DefWindowProc();
}

LRESULT CQSearchPopWnd::OnPaint(UINT /* uMsg */, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    CPaintDC dc(m_hWnd);

    RECT rcClient = { 0 };
    GetClientRect(&rcClient);

    CPen   pen;     pen.CreatePen( PS_SOLID, 1, m_clrBorder );
    CBrush brush;   brush.CreateSolidBrush( m_clrWindow );

    HPEN   hOldPen   = dc.SelectPen(pen);
    HBRUSH hOldBrush = dc.SelectBrush(brush);

    dc.Rectangle( &rcClient );

    dc.SelectPen(hOldPen);
    dc.SelectBrush(hOldBrush);


    rcClient.bottom = 0;
    for ( size_t idx = 0; idx < m_vtChildWindow.GetCount(); idx++ )
    {
        IWindowLayer* piChildWnd = m_vtChildWindow[idx];
        if ( piChildWnd != NULL && piChildWnd->IsWindowVisible() )
        {
            SIZE sizeChild = { 0 };
            if ( piChildWnd->GetClientSize( sizeChild ) )
            {
                rcClient.bottom += sizeChild.cy;
                piChildWnd->OnPaint( dc, rcClient );
                rcClient.top += rcClient.bottom;
            }
        }
    }

    return 1L;
}

LRESULT CQSearchPopWnd::OnLButtonDown(UINT /* uMsg */, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    int xPos = GET_X_LPARAM(lParam); 
    int yPos = GET_Y_LPARAM(lParam); 

    if ( m_vtChildWindow[em_cid_qstoolbar]->IsWindowVisible() && yPos < CQSToolbar::client_height )
    {
        SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, lParam);
    }

    return DefWindowProc();
}

HHOOK g_hMouseMsgHook = NULL;
HWND  g_hWnd = NULL;

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN || wParam == WM_MBUTTONDOWN
            || wParam == WM_NCLBUTTONDOWN || wParam == WM_NCRBUTTONDOWN || wParam == WM_NCMBUTTONDOWN)
        {
            MOUSEHOOKSTRUCT *pms = (MOUSEHOOKSTRUCT*)lParam;
            RECT rcWindow = { 0 };
            GetWindowRect(g_hWnd, &rcWindow);
            
            if ( !::PtInRect(&rcWindow, pms->pt) )
                ::ShowWindow(g_hWnd, SW_HIDE);
        }
    }

    return ::CallNextHookEx( g_hMouseMsgHook, nCode, wParam, lParam);
}

LRESULT CQSearchPopWnd::OnShowWindow(UINT /* uMsg */, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    LRESULT lResult = DefWindowProc();

    if ( wParam )
    {
        g_hWnd = m_hWnd;
        if ( g_hMouseMsgHook == NULL )
            g_hMouseMsgHook = ::SetWindowsHookEx(WH_MOUSE, (HOOKPROC)MouseProc, NULL, GetCurrentThreadId());
    }
    else
    {
        if ( g_hMouseMsgHook != NULL )
        {
            UnhookWindowsHookEx(g_hMouseMsgHook);
            g_hMouseMsgHook = NULL;
            g_hWnd = NULL;
        }
    }

    return lResult;
}

LRESULT CQSearchPopWnd::OnMouseMove(UINT /* uMsg */, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    int xPos = GET_X_LPARAM(lParam); 
    int yPos = GET_Y_LPARAM(lParam); 

    if ( m_vtChildWindow[em_cid_qstoolbar]->IsWindowVisible() && yPos < CQSToolbar::client_height )
    {
        SetCursor( ::LoadCursor(NULL, IDC_SIZEALL) );
        ::SetClassLong( m_hWnd, GCL_HCURSOR, (LONG)::LoadCursor(NULL, IDC_SIZEALL) );
    }
    else
    {
        SetCursor( ::LoadCursor(NULL, IDC_ARROW) );
        ::SetClassLong( m_hWnd, GCL_HCURSOR, (LONG)::LoadCursor(NULL, IDC_ARROW) );
    }
        
    return NULL;
}

LRESULT CQSearchPopWnd::OnKillFocus(UINT /* uMsg */, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{

    return DefWindowProc();
}

LRESULT CQSearchPopWnd::OnCtlColor(UINT /* uMsg */, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    CDCHandle dc = (HDC)wParam;
    dc.SetBkMode( TRANSPARENT );

    static CBrush s_bkBrush;
    if ( s_bkBrush.m_hBrush == NULL )
        s_bkBrush.CreateSolidBrush( m_clrWindow );
    
    return (LRESULT)s_bkBrush.m_hBrush;
}

BOOL CQSearchPopWnd::IsDialogMessage(LPMSG lpMsg)
{
    m_tooltip.RelayEvent(lpMsg);

    BOOL bResult = ::IsDialogMessage(m_hWnd, lpMsg);

    if ( lpMsg->message == WM_KEYDOWN && lpMsg->wParam == VK_RETURN )
    {
        if ( GetDlgItem(IDC_KEYWORD_EDIT) == GetFocus() )
        { 
            WPARAM wParam = MAKEWPARAM( IDC_QSEARCH_BTN, BN_CLICKED );
            PostMessage( WM_COMMAND, wParam, 0 );
        }
    }
    else if ( lpMsg->message == WM_MOUSEMOVE )
    {
        if ( !m_bMouseIn && ::IsWindow(m_hWnd) && IsWindowVisible() )
        {
            RECT  rcWindow = { 0 };
            POINT pt       = { 0 };
            
            GetCursorPos ( &pt );
            GetWindowRect( &rcWindow );

            if ( ::PtInRect(&rcWindow, pt) )
            {
                SetTransparency( 255 );
                m_bMouseIn = TRUE;
                SetTimer( 1001, 200 );
            }
        }
    }
    else if ( lpMsg->hwnd == m_hWnd && lpMsg->message == WM_MOUSELEAVE )
    {
        if ( m_bMouseIn )
        {
            SetTransparency( m_byteTran );
        }
        m_bMouseIn = FALSE;

    }
    else if ( lpMsg->hwnd == m_hWnd && lpMsg->message == WM_TIMER && lpMsg->wParam == 1001 )
    {
        RECT  rcWindow = { 0 };
        RECT  rcShow   = { 0 };
        POINT pt       = { 0 };

        GetCursorPos ( &pt );
        GetWindowRect( &rcWindow );

        rcShow = rcWindow;
        ::InflateRect(&rcShow, 12, 12);
        if ( !::PtInRect(&rcShow, pt) )
        {
            KillTimer(1001);
            PostMessage(WM_MOUSELEAVE);
            if ( !m_bDoct )
                ShowWindow( SW_HIDE );
        }
        else if ( m_bMouseIn && !::PtInRect(&rcWindow, pt) )
        {
            PostMessage(WM_MOUSELEAVE);
        }
    }
    //else if ( lpMsg->message == WM_KILLFOCUS && ( lpMsg->hwnd == m_hWnd || ::GetParent(lpMsg->hwnd) == m_hWnd))
    //{
    //    if ( GetFocus() == m_hWnd ||
    //        ::GetParent(GetFocus()) == m_hWnd )
    //        return bResult;

    //    if ( !m_bDoct )
    //        ShowWindow( SW_HIDE );
    //}
    
    return bResult;
}


void CQSearchPopWnd::ClearResult()
{
    CQSReaultView* pResultView = static_cast<CQSReaultView*>(m_vtChildWindow[em_cid_qsresultview]);
    if ( pResultView == NULL )
        return;

    CRichEditCtrlEx& edit = pResultView->m_ret_edit;
    edit.ClearAll();
}

LRESULT CQSearchPopWnd::OnQSearchBtnClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    TCHAR szKeyword[MAX_PATH] = { 0 };
    GetDlgItemText(IDC_KEYWORD_EDIT, szKeyword, MAX_PATH);

    if ( wcslen(szKeyword) <= 0 )
        return 0L;

    QSearchKeyword(szKeyword);

    CQSReaultView* pResultView = static_cast<CQSReaultView*>(m_vtChildWindow[em_cid_qsresultview]);
    if ( pResultView == NULL )
        return FALSE;

    CRichEditCtrlEx& edit = pResultView->m_ret_edit;
    edit.ClearAll();

    return 1L;
}

LRESULT CQSearchPopWnd::OnCopyBtnClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CQSReaultView* pResultView = static_cast<CQSReaultView*>(m_vtChildWindow[em_cid_qsresultview]);
    if ( pResultView == NULL )
        return FALSE;

    CRichEditCtrlEx& edit = pResultView->m_ret_edit;
   
    edit.SetSel(0,-1);
    edit.Copy();
    edit.SetSel(0,0);

    m_tooltip.Activate(TRUE);


    return 1L;
}
LRESULT CQSearchPopWnd::OnSettingBtnClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    if ( m_vtChildWindow[em_cid_settingview]->IsWindowVisible() )
        return 1L;

    m_strLastTitle = ((CSogouTitleView*)m_vtChildWindow[em_cid_sogoutitle])->m_strTitle;
    for ( size_t idx = 0; idx < m_vtChildWindow.GetCount(); idx++)
    {
        m_bChildShow[idx] = FALSE;
        if ( m_vtChildWindow[idx] != NULL )
            m_bChildShow[idx] = m_vtChildWindow[idx]->IsWindowVisible();
    }

    ((CSogouTitleView*)m_vtChildWindow[em_cid_sogoutitle])->m_strTitle = L"设置";
    ShowChildWindow(em_cid_qstoolbar, TRUE);
    ShowChildWindow(em_cid_sogoutitle, TRUE);
    ShowChildWindow(em_cid_qsresultview, FALSE);
    ShowChildWindow(em_cid_settingview, TRUE);
    ShowChildWindow(em_cid_settoolbar, FALSE);
    ShowAutoSizeWnd();
    ShowWindow(SW_SHOWDEFAULT);

    return 1L;
}
LRESULT CQSearchPopWnd::OnDoctBtnClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    m_bDoct = m_bDoct ? FALSE : TRUE;
    SetDlgItemText(IDC_DOCT_BTN, m_bDoct ? L"|" : L"-");

    return 1L;
}

LRESULT CQSearchPopWnd::OnSettubgCBBtnClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
    bHandled = FALSE;
    return 1L;
}
LRESULT CQSearchPopWnd::OnReturnBtnClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    for ( size_t idx = 0; idx < m_vtChildWindow.GetCount(); idx++)
    {
        ShowChildWindow(idx, m_bChildShow[idx]);
    }
    ((CSogouTitleView*)m_vtChildWindow[em_cid_sogoutitle])->m_strTitle = m_strLastTitle;
    ShowAutoSizeWnd();
    ShowWindow(SW_SHOWDEFAULT);

    return 1L;
}


HRESULT STDMETHODCALLTYPE CQSearchPopWnd::OnQSearchResult( 
    /* [in ] */ LPCWSTR   pszResult,
    /* [in ] */ LPQSPINFO pQSInfo )
{
    CQSReaultView* pResultView = static_cast<CQSReaultView*>(m_vtChildWindow[em_cid_qsresultview]);
    if ( pResultView == NULL )
        return FALSE;

    CRichEditCtrlEx& edit = pResultView->m_ret_edit;
    edit.ClearAll();

    if ( pQSInfo->nPType == PTYPE_SOGOU )
    {
        CMarkup markup;

        edit.AppendText( pQSInfo->pszName, RICHEDIT_TEXT_FONT_BOLD, RGB( 128, 0, 128 ) );
        edit.AppendEndOfLine();
        
        ATL::CString strRet = pszResult;
        int nBPos = 0;
        int nEPos = 0;
        nBPos = strRet.Find(L"<p>IP：");
        if ( nBPos >= 0 )
        {
            nEPos = strRet.Find(L"</p>", nBPos);

            ATL::CString strText = strRet.Mid( nBPos + 3, nEPos - nBPos - 3);

            edit.AppendText( strText , RICHEDIT_TEXT_FONT_BOLD, RGB( 0, 128, 0 ) );
            edit.AppendEndOfLine();

            nBPos = strRet.Find(L"<p>", nEPos);
            nEPos = strRet.Find(L"</p>", nBPos);

            strText = strRet.Mid( nBPos + 3, nEPos - nBPos - 3);

            edit.AppendText( strText , 0 );
        }
    }
    else if (pQSInfo->nPType == PTYPE_DICT ) 
    {
        CMarkup markup;

        edit.AppendText( pQSInfo->pszName, RICHEDIT_TEXT_FONT_BOLD, RGB( 128, 0, 128 ) );
        edit.AppendEndOfLine();
        
        while ( markup.SetDoc( pszResult ) )
        {
            if ( !markup.IsWellFormed() ) 
                break;

            if (!markup.FindElem(_T("dict")) || !markup.IntoElem())
                break;

            if (!markup.FindElem(_T("key")))
                break;

            edit.AppendText( markup.GetData().c_str() , RICHEDIT_TEXT_FONT_BOLD, RGB( 0, 128, 0 ) );
            edit.AppendEndOfLine();

            markup.ResetMainPos();
            if (!markup.FindElem(_T("def")))
                break;

            edit.AppendText( markup.GetData().c_str(), 0 );
            
            break;
        }
    }

    ShowAutoSizeWnd();
    Invalidate();

    return TRUE; 
}

BOOL CQSearchPopWnd::QSearchKeyword( LPCTSTR pszKeyword )
{
    CQuickSearchMgr& mgt = CQuickSearchMgr::Instance();

    ATL::CString strKeyword = pszKeyword;

    if ( strKeyword.GetLength() <= 0 )
        return FALSE;
    
    return mgt.QSearchKeyword( pszKeyword );
}
