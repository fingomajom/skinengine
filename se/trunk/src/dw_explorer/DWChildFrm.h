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

#include "DWComDef.h"
#include "DWTableBar.h"
#include "DWProcessMgt.h"
#include "DWEventSvr.h"
#include "DWFavIconMgt.h"
#include "DWURLHistoryMgt.h"


class CDWChildFrm : public CWindowImpl<CDWChildFrm>
{
public:

    CDWChildFrm( CDWTableBar& wndTableBar ) :
        m_wndTableBar(wndTableBar)
    {
    }

    CDWTableBar& m_wndTableBar;
    CWindow      m_wndClient;

    ATL::CString m_strURL;
    ATL::CString m_strTitle;

    CIconHandle  m_icon;

    static CDWChildFrm* CreateChildFrm(
        HWND hParent,
        RECT rcClient,
        CDWTableBar& wndTableBar, 
        LPCTSTR pszTitle,
        LPCTSTR pszURL )
    {
        CDWChildFrm* pNewFrm = new CDWChildFrm(wndTableBar);
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
        ATLASSERT( nIdx >= 0 && m_wndClient.IsWindow() );
        if ( nIdx < 0 )
            return;

        ATL::CString strTitle;
        if ( (pszURL == NULL || lstrlenW(pszURL) <= 0 ) || !StrCmpI(pszURL, BLANK_URL) )
            strTitle = L"¿Õ°×Ò³";
        else
            strTitle = pszURL;

        pszURL = (pszURL == NULL || lstrlenW(pszURL) <= 0 ) ? BLANK_URL : pszURL;

        m_strTitle = strTitle;
        m_strURL   = pszURL;

        m_wndTableBar.SetItemCaption( nIdx, strTitle );
        m_wndTableBar.SetItemIcon( nIdx, NULL );

        CDWEventSvr::Instance().OnMessage( eid_addr_changed, (WPARAM) pszURL, 0 );

        psmgt.WebWndOpenURL( m_wndClient, pszURL );

        ::SetForegroundWindow(m_wndClient);
    }


    BEGIN_MSG_MAP(CDWChildFrm)

        MESSAGE_HANDLER(WM_COPYDATA, OnCopyData)

        MESSAGE_HANDLER(WM_CREATE , OnCreate   )
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy  )
        MESSAGE_HANDLER(WM_SIZE   , OnSize     )

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

        if ( m_wndTableBar. GetSelectIndex() == nTabIndex )
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
 
    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        ResizeClient(MAKEWPARAM(0,1));

        return 0L;
    }

    void ResizeClient( WPARAM wParam = MAKEWPARAM(1,1), LPARAM lParam = FALSE )
    {
        RECT rcClient  = { 0 };

        if ( !::IsWindow(m_wndClient) )
            return;

        if ( !GetClientRect(&rcClient) )
        {
            return;
        }

        //m_wndClient.PostMessage( WM_WEBVIEW_MOVESIZE, wParam, lParam);

        SendMessageToWebWnd(WM_WEBVIEW_MOVESIZE, wParam, lParam);

        //::SendMessageTimeout( m_wndClient, 
        //    WM_WEBVIEW_MOVESIZE, 
        //    wParam, lParam, 
        //    SMTO_ABORTIFHUNG, 1, 0);

    }

    void HideClient()
    {
        ShowWindow( SW_HIDE ); 

        if ( ::IsWindow(m_wndClient) )
        {
            SendMessageToWebWnd(WM_WEBVIEW_SHOW);
        }
    }


    void ShowClient()
    {
        ResizeClient();
        ShowWindow( SW_SHOWDEFAULT ); 

        if ( ::IsWindow(m_wndClient) )
        {
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

            m_wndClient.PostMessage(WM_WEBVIEW_SHOW, TRUE);

            if ( !StrCmpI(m_strURL, BLANK_URL) )
            {
                evt.OnMessage( edi_addr_setfocus );
                ::SetForegroundWindow(GetParent());
            }
            else
                ::SetForegroundWindow(m_wndClient);
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