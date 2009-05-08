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

        HWND hRet = pNewFrm->Create( hParent, &rcClient, NULL,  
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPCHILDREN, 0);
        if ( hRet == NULL )
        {
            delete pNewFrm;
            pNewFrm = NULL;
        }

        pNewFrm->m_strTitle = pszTitle;
        pNewFrm->m_strURL   = pszURL;

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
            strTitle = L"�հ�ҳ";
        else
            strTitle = pszURL;

        pszURL = (pszURL == NULL || lstrlenW(pszURL) <= 0 ) ? BLANK_URL : pszURL;

        m_strTitle = strTitle;
        m_strURL   = pszURL;

        m_wndTableBar.SetItemCaption( nIdx, strTitle );
        m_wndTableBar.SetItemIcon( nIdx, NULL );

        CDWEventSvr::Instance().OnMessage( eid_addr_changed, (WPARAM) pszURL, 0 );

        psmgt.WebWndOpenURL( m_hWnd, pszURL );
    }


    BEGIN_MSG_MAP(CDWFrameClient)

        MESSAGE_HANDLER(WM_COPYDATA, OnCopyData)

        MESSAGE_HANDLER(WM_CREATE , OnCreate )
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy  )
        MESSAGE_HANDLER(WM_SIZE   , OnSize   )

        MESSAGE_HANDLER(WM_SETFOCUS   , OnSetFocus )
        MESSAGE_HANDLER(WM_ERASEBKGND , OnEraseBkGnd )


        MESSAGE_HANDLER(WM_WEBVIEW_SETTITLE  , OnSetTitle  )
        MESSAGE_HANDLER(WM_WEBVIEW_SETURL    , OnSetUrl )
        MESSAGE_HANDLER(WM_WEBVIEW_SETSTATUS , OnSetStatus  )


        MESSAGE_HANDLER(WM_CREATE_WEB_WND   , OnCreateWebWnd   )
        MESSAGE_HANDLER(WM_FAV_ICON_REFLASH , OnFavIconReflash )

        MESSAGE_HANDLER(WM_WEBWND_INFO_CHANGED , OnWebWndInfoChanged )

    END_MSG_MAP()

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
            m_wndClient.SetFocus();
        }

        LRESULT lResult = DefWindowProc();

        return lResult;
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
        }

        return 0;
    }

    LRESULT OnSetUrl(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LPCTSTR pszUrl = (LPCTSTR)wParam;

        if ( StrCmpI( pszUrl, m_strURL ) )
        {
            m_strURL = pszUrl;
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

        if ( IsWindowVisible() )
            ShowClient();

        return 0;
    }

    LRESULT OnFavIconReflash(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        int nTabIndex = m_wndTableBar.FindParam( (LPARAM)this );
        ATLASSERT( nTabIndex >= 0 );

        if ( nTabIndex >= 0 && wParam != NULL )
        {
            m_wndTableBar.SetItemIcon( nTabIndex, (HICON)wParam );

            if ( IsWindowVisible() )
                CDWEventSvr::Instance().OnMessage( edi_spr_icon_changed, 
                wParam, 0 );

        }

        return 0L;
    }

    LRESULT OnWebWndInfoChanged(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if ( lParam == NULL )
        {
            CDWProcessMgt& psmgt= CDWProcessMgt::Instance();

            psmgt.GetWebWndInfo( m_hWnd, (HWND)wParam);
        }

        return 0L;
    }


    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;

        ResizeClient(FALSE);

        return 0L;
    }

    void ResizeClient( BOOL bRePaint = TRUE )
    {
        RECT rcClient  = { 0 };
        RECT rcCClient = { 0 };

        if ( !::IsWindow(m_wndClient) )
            return;

        if ( !GetClientRect(&rcClient) ||
            !m_wndClient.GetClientRect(&rcCClient) ||
            (rcClient.right == 0 && rcClient.bottom == 0 ) )
        {
            return;
        }

        if ( rcCClient.bottom == rcClient.bottom &&
            rcCClient.right  == rcClient.right  )
            return;

        if ( !bRePaint )
            m_wndClient.SetRedraw(FALSE);

        m_wndClient.MoveWindow(&rcClient);

        if ( !bRePaint )
            m_wndClient.SetRedraw(TRUE);
    }

    void HideClient()
    {
        ShowWindow( SW_HIDE ); 

        if ( ::IsWindow(m_wndClient) )
        {
            m_wndClient.ShowWindow( SW_HIDE );           
        }
    }


    void ShowClient()
    {
        ResizeClient();
        ShowWindow( SW_SHOWDEFAULT ); 

        if ( ::IsWindow(m_wndClient) )
        {
            m_wndClient.ShowWindow( SW_SHOWDEFAULT );           
            m_wndClient.SetFocus();
        }
    }

};