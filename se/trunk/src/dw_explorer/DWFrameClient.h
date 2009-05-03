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

        MESSAGE_HANDLER(WM_ERASEBKGND       , OnEraseBkGnd )
        MESSAGE_HANDLER(WM_CREATE_WEB_WND   , OnCreateWebWnd )


        MESSAGE_HANDLER(WM_WEBWND_INFO_CHANGED , OnWebWndInfoChanged )
        
    END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;

        return 0;
    }

    LRESULT OnEraseBkGnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 1 ;
    }


    LRESULT OnCreateWebWnd(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        HWND hCreateWnd = (HWND)wParam;

        ATLASSERT(::IsWindow(hCreateWnd));
        if ( !::IsWindow(hCreateWnd) )
            return 1L;

        unsigned short uID  = LOWORD(lParam);
        int nIdx = HIWORD(lParam);

        int nTabIndex = m_wndTableBar.FindID(uID);
        if ( nTabIndex < 0 )
        {
            ATLASSERT(0);
        }
        else
        {
            ATLASSERT( m_wndTableBar.SetItemParam( nTabIndex, wParam ) == uID );

            //m_wndTableBar.SetItemCaption( nTabIndex, L"¿Õ°×Ò³" );

            if ( m_wndTableBar.GetSelectIndex() == nTabIndex )
                ShowClient( hCreateWnd );
        }

        //if ( ::IsWindow(m_wndClient) )
        //    m_wndClient.ShowWindow( SW_SHOWDEFAULT );

        //ResizeClient();
        return 0;
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
            if ( strTitle.IsEmpty() )
                strTitle = pszBuf + pszBuf[0];

            if ( strTitle.GetLength() )
            {
                int nTabIndex = m_wndTableBar.FindParam(wParam);
                ATLASSERT(nTabIndex >= 0);
                if ( nTabIndex >= 0 && strTitle.CollateNoCase(L"about:blank"))
                {
                    m_wndTableBar.SetItemCaption( nTabIndex, strTitle );
                }

                strTitle = pszBuf + pszBuf[0];
                if ( strTitle.GetLength() > 0 && nTabIndex == m_wndTableBar.GetSelectIndex() )
                {
                    CDWEventSvr::Instance().OnMessage( eid_addr_changed, (WPARAM)(LPCTSTR)strTitle, 0 );
                }
            }

            piBuffer->Release();
        }

        return 0L;
    }


    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;
        
        ResizeClient();

        return 0L;
    }

    void ResizeClient()
    {
        RECT rcClient = { 0 };
        GetClientRect(&rcClient);

        //ClientToScreen(&rcClient);
        
        if ( ::IsWindow(m_wndClient) )
            m_wndClient.MoveWindow(&rcClient);
    }

    void ShowClient( HWND hWndClient )
    {
        //if ( ::IsWindow(m_wndClient) )
        //    m_wndClient.PostMessage(WM_SHOWWINDOW, 10, 101);

        //m_wndClient = hWndClient;

        //ResizeClient();
        //if ( ::IsWindow(m_wndClient) )
        //    m_wndClient.PostMessage(WM_SHOWWINDOW, 10, 100);

        if ( ::IsWindow(m_wndClient) )
            m_wndClient.ShowWindow( SW_HIDE );

        m_wndClient = hWndClient;

        ResizeClient();
        if ( ::IsWindow(m_wndClient) )
            m_wndClient.ShowWindow( SW_SHOWDEFAULT );

    }

    CWindow m_wndClient;
};