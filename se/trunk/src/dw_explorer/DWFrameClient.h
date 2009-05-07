/********************************************************************
	created:	2009/05/03
	created:	3:5:2009   15:00
	filename: 	d:\lidengwang\se\trunk\src\dw_explorer\DWFrameClient.h
	file path:	d:\lidengwang\se\trunk\src\dw_explorer
	file base:	DWFrameClient
	file ext:	h
	author:		lidengwang@s-zone.com
	
	purpose:	
*********************************************************************/

#pragma once

#include "DWComDef.h"
#include "DWTableBar.h"
#include "DWProcessMgt.h"
#include "DWEventSvr.h"
#include "DWFavIconMgt.h"


struct URL_WND_INFO {
    ATL::CString strURL;
    ATL::CString strTitle;
};

class CDWFrameClient : public CWindowImpl<CDWFrameClient>
{
public:

    CDWFrameClient( CDWTableBar& wndTableBar ) :
        m_wndTableBar(wndTableBar)
    {
    }

    CDWTableBar& m_wndTableBar;

    BOOL PreTranslateMessage(MSG* pMsg)
    {
        if ( pMsg->message == WM_WINDOWPOSCHANGED ||
             pMsg->message == WM_WINDOWPOSCHANGING )
        {
             ResizeClient();
        }
        return FALSE;
    }

    BEGIN_MSG_MAP(CDWFrameClient)

        MESSAGE_HANDLER(WM_CREATE , OnCreate )
        MESSAGE_HANDLER(WM_SIZE   , OnSize   )

        MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus )

        MESSAGE_HANDLER(WM_ERASEBKGND       , OnEraseBkGnd     )
        MESSAGE_HANDLER(WM_CREATE_WEB_WND   , OnCreateWebWnd   )
        MESSAGE_HANDLER(WM_FAV_ICON_REFLASH , OnFavIconReflash )

        
        MESSAGE_HANDLER(WM_WEBWND_INFO_CHANGED , OnWebWndInfoChanged )
        
    END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;

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

        if ( lParam != 0 )  // 事件创建 进程内发来
        {
            unsigned short uID  = LOWORD(lParam);
            int nIdx = HIWORD(lParam);

            int nTabIndex = m_wndTableBar.FindID(uID);
            if ( nTabIndex < 0 )
            {
                ATLASSERT(0);
            }
            else
            {
                m_mapUrlWndInfo[hCreateWnd].strURL   = m_mapUrlWndInfo[(HWND)uID].strURL ;
                m_mapUrlWndInfo[hCreateWnd].strTitle = m_mapUrlWndInfo[(HWND)uID].strTitle ;
                m_mapUrlWndInfo.RemoveKey((HWND)uID);

                HICON hIcon = CDWFavIconMgt::Instance().GetFavIcon( m_mapUrlWndInfo[hCreateWnd].strURL
                    , m_hWnd, wParam );
                if ( hIcon != NULL )
                    m_wndTableBar.SetItemIcon( nTabIndex, hIcon );

                if( m_wndTableBar.SetItemParam( nTabIndex, wParam ) != uID )
                    ATLASSERT( 0 && L"id changed no.." );

                if ( m_wndTableBar.GetSelectIndex() == nTabIndex )
                {
                    ShowClient( hCreateWnd );

                    if ( m_mapUrlWndInfo[hCreateWnd].strURL == BLANK_URL )
                    {
                        CDWEventSvr::Instance().OnMessage( edi_addr_setfocus, 0, 0 );
                    }
                }
            }
        }
        else // 单击创建 从进程外发来
        {
            CDWProcessMgt& psmgt= CDWProcessMgt::Instance();

            psmgt.AddWnd2Process(hCreateWnd);

            m_mapUrlWndInfo[hCreateWnd].strURL   = BLANK_URL;
            m_mapUrlWndInfo[hCreateWnd].strTitle = L"正在打开";            

            int nIdx = m_wndTableBar.GetSelectIndex();
            nIdx++;

            m_wndTableBar.InsertTableItem( nIdx, 
                m_mapUrlWndInfo[(HWND)wParam].strTitle, 0, wParam );

            CDWEventSvr::Instance().OnMessage( eid_addr_changed, 
                (WPARAM)(LPCTSTR)m_mapUrlWndInfo[hCreateWnd].strURL, 0 );

            m_wndTableBar.SelectIndex(nIdx);
            
            psmgt.GetWebWndInfo(m_hWnd, hCreateWnd);
            ShowClient(hCreateWnd);
        }

        return 0;
    }

    LRESULT OnFavIconReflash(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        int nTabIndex = m_wndTableBar.FindParam(lParam);
        ATLASSERT(nTabIndex >= 0);

        if ( nTabIndex >= 0 && wParam != NULL )
        {
            m_wndTableBar.SetItemIcon( nTabIndex, (HICON)wParam );

            if ( nTabIndex == m_wndTableBar.GetSelectIndex() )
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
        else
        {
            IDataBuffer* piBuffer = (IDataBuffer*)lParam;
            piBuffer->AddRef();
            WCHAR* pszBuf = (WCHAR*)piBuffer->GetDataBuffer();

            ATL::CString strTitle = pszBuf+1;
            ATL::CString strURL   = pszBuf + pszBuf[0];

            m_mapUrlWndInfo[(HWND)wParam].strURL   = strURL;
            m_mapUrlWndInfo[(HWND)wParam].strTitle = strTitle;

            if ( strTitle.GetLength() > 0 || strURL.GetLength() > 0 )
            {
                int nTabIndex = m_wndTableBar.FindParam(wParam);
                ATLASSERT(nTabIndex >= 0);

                if ( strURL.GetLength() > 0 && nTabIndex == m_wndTableBar.GetSelectIndex() )
                {
                    CDWEventSvr::Instance().OnMessage( eid_addr_changed, 
                        (WPARAM)(LPCTSTR)strURL, 
                        (WPARAM)(LPCTSTR)strTitle);
                    if ( ::IsWindow(m_wndClient) )
                        m_wndClient.SetFocus();
                }                

                if ( strTitle.GetLength() <= 0 )
                    strTitle = strURL;
                if ( nTabIndex >= 0 && strTitle.CollateNoCase(BLANK_URL))
                {
                    m_wndTableBar.SetItemCaption( nTabIndex, strTitle );
                }

                if ( strURL.GetLength() > 0 && strURL.CollateNoCase(BLANK_URL) )
                {
                    HICON hIcon = CDWFavIconMgt::Instance().GetFavIcon( strURL, m_hWnd, wParam );
                    if ( hIcon != NULL )
                    {
                        m_wndTableBar.SetItemIcon( nTabIndex, hIcon );

                        if ( nTabIndex == m_wndTableBar.GetSelectIndex() )
                            CDWEventSvr::Instance().OnMessage( edi_spr_icon_changed, 
                            (WPARAM)hIcon, 0 );

                    }
                }
            }

            piBuffer->Release();
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

    void ShowClient( HWND hWndClient )
    {
        if ( ::IsWindow(m_wndClient) )
            m_wndClient.ShowWindow( SW_HIDE );

        m_wndClient = hWndClient;

        ResizeClient();

        if ( ::IsWindow(m_wndClient) )
        {
            CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

            m_wndClient.ShowWindow( SW_SHOWDEFAULT );           
            m_wndClient.SetFocus();

            HICON hIcon = m_wndTableBar.GetItemIcon( m_wndTableBar.FindParam((WPARAM)hWndClient) );
            if ( hIcon == NULL )
                hIcon = skin.iconNull;
            CDWEventSvr::Instance().OnMessage( edi_spr_icon_changed, 
                (WPARAM)hIcon, 0 );


            ATL::CAtlMap<HWND, URL_WND_INFO>::CPair* pFind = 
                m_mapUrlWndInfo.Lookup( m_wndClient );

            if ( pFind != NULL )
            {
                ATL::CString strTitle = pFind->m_value.strTitle;
                if ( strTitle.IsEmpty() )
                    strTitle = pFind->m_value.strURL;

                CDWEventSvr::Instance().OnMessage( 
                    eid_addr_changed, 
                    (WPARAM)(LPCTSTR)pFind->m_value.strURL, 
                    (WPARAM)(LPCTSTR)pFind->m_value.strTitle);

                if ( pFind->m_value.strURL == BLANK_URL )
                    CDWEventSvr::Instance().OnMessage( edi_addr_setfocus, 0, 0 );
            }
        }
    }

    CWindow m_wndClient;

    ATL::CAtlMap<HWND, URL_WND_INFO> m_mapUrlWndInfo;
};