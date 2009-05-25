/********************************************************************
	created:	2009/05/08
	created:	8:5:2009   16:04
	filename: 	e:\lidengwang\gcode\se\trunk\src\dw_explorer\DWChildFrm.h
	file path:	e:\lidengwang\gcode\se\trunk\src\dw_explorer
	file base:	DWChildFrm.h
	author:		lidengwang@sohu-rd.com
	
	purpose:	
*********************************************************************/

#pragma once

#include <atlctrlx.h>

#include "DWComDef.h"
#include "DWTableBar.h"
#include "DWProcessMgt.h"
#include "DWEventSvr.h"
#include "DWFavIconMgt.h"
#include "DWURLHistoryMgt.h"




class CDWChildFrmBkgnd : public CWindowImpl<CDWChildFrmBkgnd>
{
public:

    CHyperLink m_link;

    CBrush  m_bkBrush;
    CWindow m_wndParent;


    BEGIN_MSG_MAP(CDWChildFrm)

        MESSAGE_HANDLER( WM_CREATE     , OnCreate     )
        MESSAGE_HANDLER( WM_DESTROY    , OnDestroy    )
        MESSAGE_HANDLER( WM_ERASEBKGND , OnEraseBkGnd )
        MESSAGE_HANDLER( WM_SIZE       , OnSize       )
        MESSAGE_HANDLER( WM_KEYDOWN    , OnKeydown    )

        MESSAGE_HANDLER( WM_CTLCOLORBTN   , OnCtlColor )
        MESSAGE_HANDLER( WM_CTLCOLORSTATIC, OnCtlColor )

        COMMAND_CODE_HANDLER( BN_CLICKED, OnReflash )

    END_MSG_MAP()
    DECLARE_WND_CLASS(_T("DWExplorer_DWChildFrmBkgnd"))

    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        m_link.SetHyperLinkExtendedStyle(
            HLINK_UNDERLINED | HLINK_COMMANDBUTTON);        
        m_link.Create(m_hWnd, 
            rcDefault,
            L"立即刷新", 
            WS_CHILD | WS_VISIBLE, 0,
            10);

        m_link.SetLinkFont(AtlGetDefaultGuiFont());

        return DefWindowProc();
    }

    LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if ( m_bkBrush.m_hBrush != NULL )
            m_bkBrush.DeleteObject();

        return DefWindowProc();
    }

    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        RECT rcClient;
        GetClientRect(&rcClient);

        if ( m_link.IsWindow() )
        {
            rcClient.left   = (rcClient.left + rcClient.right) / 2 - 25;
            rcClient.right  = rcClient.left + 50;
            rcClient.top    = (rcClient.top + rcClient.bottom) / 2;
            rcClient.bottom = rcClient.top + 20;
            
            m_link.MoveWindow(&rcClient);
            m_link.CalcLabelRect();

        }

        return DefWindowProc();
    }

    LRESULT OnEraseBkGnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        RECT rcClient;
        GetClientRect(&rcClient);

        CMemoryDC dc((HDC)wParam, rcClient);

        LRESULT lRet = DefWindowProc(uMsg, (WPARAM)dc.m_hDC, lParam);

        CFont font, font1;
        font.CreatePointFont ( 198, L"Tahoma" );
        font1.CreatePointFont( 120, L"Tahoma" );


        HFONT hOldFont = dc.SelectFont(font);

        rcClient.top = (rcClient.top + rcClient.bottom) / 2 - 100;
        rcClient.bottom = rcClient.top + 50;
        dc.SetTextColor(RGB(255, 100, 100));
        dc.DrawText(L"非常报歉，当前页面出错并崩溃。", -1, &rcClient, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        dc.SelectFont(font1);

        rcClient.top = rcClient.bottom;
        rcClient.bottom = rcClient.top + 30;
        dc.SetTextColor(RGB(50, 50, 50));
        dc.DrawText(L"你可以安全的关闭或刷新当前页面，并不会影响其它页面。", -1, &rcClient, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        dc.SelectFont(hOldFont);

        return lRet;
    }  

    LRESULT OnCtlColor(UINT /* uMsg */, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
    {
        CDCHandle dc = (HDC)wParam;
        dc.SetBkMode( TRANSPARENT );

        if ( m_bkBrush.m_hBrush == NULL )
            m_bkBrush.CreateSolidBrush( GetSysColor(COLOR_WINDOW) );

        return (LRESULT)m_bkBrush.m_hBrush;
    }

    LRESULT OnKeydown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if ( wParam == VK_F5 )
        {
            m_wndParent.SendMessage(WM_REFLASH_URL);
        }

        return DefWindowProc();
    }

    LRESULT OnReflash(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        m_wndParent.SendMessage(WM_REFLASH_URL);

        return DefWindowProc();
    }

   
};



class CDWChildFrm : public CWindowImpl<CDWChildFrm>
{
public:

    CDWChildFrm( CDWTableBar& wndTableBar, CDWChildFrmBkgnd& wndChildFrmBkgnd ) :
        m_wndTableBar(wndTableBar),
        m_wndChildFrmBkgnd(wndChildFrmBkgnd)
    {
    }

    CDWTableBar&      m_wndTableBar;
    CDWChildFrmBkgnd& m_wndChildFrmBkgnd;

    CWindow      m_wndClient;

    ATL::CString m_strURL;
    ATL::CString m_strTitle;

    CIconHandle  m_icon;

    static CDWChildFrm* CreateChildFrm(
        HWND hParent,
        RECT rcClient,
        CDWTableBar& wndTableBar, 
        CDWChildFrmBkgnd& wndChildFrmBkgnd,
        LPCTSTR pszTitle,
        LPCTSTR pszURL )
    {
        CDWChildFrm* pNewFrm = new CDWChildFrm(wndTableBar, wndChildFrmBkgnd);
        ATLASSERT( pNewFrm != NULL );
        if ( pNewFrm == NULL )
            return NULL;

        pNewFrm->m_strTitle = pszTitle;
        pNewFrm->m_strURL   = pszURL;

        HWND hRet = pNewFrm->Create( hParent, &rcClient, NULL,  
            WS_CHILD | WS_CLIPCHILDREN);
        if ( hRet == NULL )
        {
            delete pNewFrm;
            pNewFrm = NULL;
        }

        return pNewFrm;
    }

    void OnOpenURL( LPCTSTR pszURL )
    {
        CDWProcessMgt& psmgt= CDWProcessMgt::Instance();

        int nIdx = m_wndTableBar.FindParam((LPARAM)this);
        ATLASSERT( nIdx >= 0 );
        if ( nIdx < 0 )
            return;

        ATL::CString strTitle;
        if ( (pszURL == NULL || lstrlenW(pszURL) <= 0 ) || !StrCmpI(pszURL, BLANK_URL) )
            strTitle = L"空白页";
        else
            strTitle = pszURL;

        pszURL = (pszURL == NULL || lstrlenW(pszURL) <= 0 ) ? BLANK_URL : pszURL;

        m_strTitle = strTitle;
        m_strURL   = pszURL;

        m_wndTableBar.SetItemCaption( nIdx, strTitle );
        m_wndTableBar.SetItemIcon( nIdx, NULL );

        CDWEventSvr::Instance().OnMessage( eid_addr_changed, (WPARAM) pszURL, 0 );

        if ( m_wndClient.IsWindow() )
            psmgt.WebWndOpenURL( m_wndClient, pszURL );
        else
            psmgt.CreateWebWnd( m_hWnd, 0, m_strURL);


        ::SetForegroundWindow(m_wndClient);
    }


    BEGIN_MSG_MAP(CDWChildFrm)

        MESSAGE_HANDLER(WM_COPYDATA, OnCopyData)

        MESSAGE_HANDLER(WM_CREATE , OnCreate   )
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy  )
        MESSAGE_HANDLER(WM_SIZE   , OnSize     )
        MESSAGE_HANDLER(WM_TIMER  , OnTimer    )

        MESSAGE_HANDLER(WM_SETFOCUS   , OnSetFocus )
        MESSAGE_HANDLER(WM_ERASEBKGND , OnEraseBkGnd )


        MESSAGE_HANDLER(WM_WEBVIEW_SETTITLE  , OnSetTitle  )
        MESSAGE_HANDLER(WM_WEBVIEW_SETURL    , OnSetUrl )
        MESSAGE_HANDLER(WM_WEBVIEW_SETSTATUS , OnSetStatus  )


        MESSAGE_HANDLER(WM_CREATE_WEB_WND   , OnCreateWebWnd   )
        MESSAGE_HANDLER(WM_FAV_ICON_REFLASH , OnFavIconReflash )
        
        MESSAGE_HANDLER(WM_WEBVIEW_CREATE     , OnWebViewCreate)
        MESSAGE_HANDLER(WM_WEBVIEW_GET        , OnWebViewGet)
        MESSAGE_HANDLER(WM_WEBVIEW_OPENSEARCH , OnWebViewOpenSearch)
        
    END_MSG_MAP()


    virtual void OnFinalMessage(HWND /*hWnd*/)
    {
        delete this;
    }

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {   
        CDWProcessMgt& psmgt= CDWProcessMgt::Instance();

        psmgt.CreateWebWnd( m_hWnd, 0, m_strURL);

        return 0;
    }

    LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled)
    {
        if ( m_wndClient.IsWindow() )
        {
            CDWProcessMgt& psmgt= CDWProcessMgt::Instance();

            psmgt.DestryWebWnd( m_wndClient );
        }

        return 1;
    }

    LRESULT OnWebViewCreate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return ::SendMessage(GetParent(), WM_WEBVIEW_CREATE, wParam, lParam);
    }

    LRESULT OnWebViewGet(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return (LRESULT)m_wndClient.m_hWnd;
    }

    LRESULT OnWebViewOpenSearch(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return ::SendMessage(GetParent(), WM_WEBVIEW_OPENSEARCH, wParam, lParam);
    }

    

    LRESULT OnCopyData(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        PCOPYDATASTRUCT pcds = (PCOPYDATASTRUCT)lParam;

        SendMessage( pcds->dwData, (LPARAM)pcds->lpData, 0 );

        return 0;
    }


    LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        if ( m_wndClient.IsWindow() )
        {
            ::SetForegroundWindow(m_wndClient);
        }

        return 0L;
    }

    LRESULT OnSetStatus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LPCTSTR pszStatus = (LPCTSTR)wParam;

        if ( !IsWindowVisible() )
            return 0L;

        CDWEventSvr::Instance().OnMessage( edi_status_bar, 
            wParam);

        return 0;
    }

    LRESULT OnSetTitle(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LPCTSTR pszTitle = (LPCTSTR)wParam;

        if ( StrCmpI( pszTitle, m_strTitle ) )
        {
            m_strTitle = pszTitle;

            int nTabIndex = m_wndTableBar.FindParam( (LPARAM)this );
            ATLASSERT( nTabIndex >= 0 );
            if ( nTabIndex < 0 )
                return 0L;
            
            m_wndTableBar.SetItemCaption(nTabIndex, pszTitle);

            CDWURLHistoryMgt::Instance().AddURLHistory(
                m_strTitle,
                m_strURL);

        }

        return 0;
    }

    LRESULT OnSetUrl(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LPCTSTR pszUrl = (LPCTSTR)wParam;

        if ( 1 || StrCmpI( pszUrl, m_strURL ) )
        {
            m_strURL = pszUrl;

            int nTabIndex = m_wndTableBar.FindParam( (LPARAM)this );
            ATLASSERT( nTabIndex >= 0 );
            if ( nTabIndex < 0 )
                return 0L;

            m_icon = CDWFavIconMgt::Instance().GetFavIcon( m_strURL, m_hWnd, 0 );
            if ( m_icon != NULL )
                m_wndTableBar.SetItemIcon( nTabIndex, m_icon );
            
            if ( IsWindowVisible() )
            {
                CDWEventSvr::Instance().OnMessage( eid_addr_changed, 
                    (WPARAM)(LPCTSTR)m_strURL, 
                    (WPARAM)(LPCTSTR)m_strTitle);

                CDWEventSvr::Instance().OnMessage( edi_spr_icon_changed, 
                    (WPARAM)m_icon.m_hIcon, 0 );
            }

            CDWURLHistoryMgt::Instance().AddURLHistory(
                m_strTitle,
                m_strURL);
        }

        return 0;
    }


    LRESULT OnEraseBkGnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if ( m_wndClient.IsWindow() )
            return 1L;

        return DefWindowProc();
    }


    LRESULT OnCreateWebWnd(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        HWND hCreateWnd = (HWND)wParam;

        ATLASSERT(::IsWindow(hCreateWnd));
        if ( !::IsWindow(hCreateWnd) )
            return 1L;

        m_wndClient = hCreateWnd;

        int nTabIndex = m_wndTableBar.FindParam( (LPARAM)this );
        ATLASSERT( nTabIndex >= 0 );
        if ( nTabIndex < 0 )
            return 0L;

        HICON hIcon = CDWFavIconMgt::Instance().GetFavIcon( m_strURL, m_hWnd, 0 );
        if ( hIcon != NULL )
            m_wndTableBar.SetItemIcon( nTabIndex, hIcon );

        if ( m_wndTableBar.GetSelectIndex() == nTabIndex )
            SendMessage( GetParent(), WM_TABLE_BAR_MSG, TGM_SELECT_CHANGE, nTabIndex );

        if ( IsWindowVisible() )
            ShowClient();

        return 0;
    }

    LRESULT OnFavIconReflash(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        int nTabIndex = m_wndTableBar.FindParam( (LPARAM)this );
        ATLASSERT( nTabIndex >= 0 );

        m_icon.m_hIcon = (HICON)wParam;

        if ( nTabIndex >= 0 && m_icon.m_hIcon != NULL )
        {
            m_wndTableBar.SetItemIcon( nTabIndex, (HICON)wParam );

            if ( IsWindowVisible() )
                CDWEventSvr::Instance().OnMessage( edi_spr_icon_changed, 
                wParam, 0 );

        }

        return 0L;
    }

    LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if ( wParam == 1001 )
        {
            if ( !m_wndClient.IsWindow() && !m_wndChildFrmBkgnd.IsWindowVisible() )
                ShowClient();
        }

        return DefWindowProc();
    }

 
    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        ResizeClient(MAKEWPARAM(0,1));

        return 0L;
    }

    void ResizeClient( WPARAM wParam = MAKEWPARAM(1,1), LPARAM lParam = FALSE )
    {
        RECT rcClient  = { 0 };

        if ( !::IsWindow(m_wndClient) )
        {
            if ( m_wndChildFrmBkgnd.IsWindow() && m_wndChildFrmBkgnd.IsWindowVisible() )
            {
                RECT rcWindow;
                GetWindowRect(&rcWindow);

                m_wndChildFrmBkgnd.SetWindowPos(
                    GetParent(), rcWindow.left, rcWindow.top,
                    rcWindow.right-rcWindow.left, 
                    rcWindow.bottom-rcWindow.top,
                    SWP_NOMOVE | SWP_NOACTIVATE);
            }

            return;
        }

        if ( !GetClientRect(&rcClient) )
        {
            return;
        }

        SendMessageToWebWnd(WM_WEBVIEW_MOVESIZE, wParam, lParam);
    }

    void HideClient()
    {
        ShowWindow( SW_HIDE ); 

        if ( ::IsWindow(m_wndClient) )
        {
            SendMessageToWebWnd(WM_WEBVIEW_SHOW);
        }

        KillTimer(1001);
    }

    void ShowClient( LPARAM lParam = 0 )
    {
        ResizeClient();
        ShowWindow( SW_SHOWDEFAULT ); 

        if ( ::IsWindow(m_wndClient) )
        {
            if ( m_wndChildFrmBkgnd.IsWindow() )
                m_wndChildFrmBkgnd.ShowWindow(SW_HIDE);

            CDWSkinUIMgt* pskin = CDWSkinUIMgt::InstancePtr();
            if ( pskin == NULL )
                return;

            CDWEventSvr& evt = CDWEventSvr::Instance();
            evt.OnMessage( eid_addr_changed, 
                (WPARAM)(LPCTSTR)m_strURL, 
                (WPARAM)(LPCTSTR)m_strTitle);

            CIconHandle icon = m_icon.m_hIcon;
            if ( icon.m_hIcon == NULL )
                icon = pskin->iconNull;

            evt.OnMessage( edi_spr_icon_changed, 
                (WPARAM)icon.m_hIcon, 0 );

            SendMessageToWebWnd(WM_WEBVIEW_SHOW, TRUE, lParam);

            if ( !StrCmpI(m_strURL, BLANK_URL) )
            {
                evt.OnMessage( edi_addr_setfocus );
                ::SetForegroundWindow(GetParent());
            }
            else
                ::SetForegroundWindow(m_wndClient);

            SetTimer(1001, 500);
        }
        else
        {
            if ( m_wndChildFrmBkgnd.IsWindow() )
            {
                RECT rcWindow;
                GetWindowRect(&rcWindow);
                m_wndChildFrmBkgnd.SetWindowPos(
                    GetParent(), rcWindow.left, rcWindow.top,
                    rcWindow.right-rcWindow.left, 
                    rcWindow.bottom-rcWindow.top,
                    SWP_SHOWWINDOW );
            }
        }
    }

    LRESULT SendMessageToWebWnd( UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0)
    {
        return ::SendMessageTimeout( m_wndClient, 
            uMsg, 
            wParam, lParam, 
            SMTO_ABORTIFHUNG, 100, 0);
    }

    DECLARE_WND_CLASS(_T("DWExplorer_DWChildFrm"));

};