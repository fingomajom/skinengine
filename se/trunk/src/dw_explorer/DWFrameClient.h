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
        if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
            (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
            return FALSE;

        // give HTML page a chance to translate this message
        //if ( ::IsWindow(m_wndClient) )
        //    return (BOOL)m_wndClient.SendMessage(WM_FORWARDMSG, 0, (LPARAM)pMsg);

        return FALSE;
    }

    BEGIN_MSG_MAP(CDWFrameClient)

        MESSAGE_HANDLER(WM_CREATE , OnCreate )
        MESSAGE_HANDLER(WM_SIZE   , OnSize   )

        MESSAGE_HANDLER(WM_CREATE_WEB_WND   , OnCreateWebWnd )

    END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;

        return 0;
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

            if ( m_wndTableBar.GetSelectIndex() == nTabIndex )
                ShowClient( hCreateWnd );
        }

        if ( ::IsWindow(m_wndClient) )
            m_wndClient.ShowWindow( SW_SHOWDEFAULT );

        ResizeClient();
        return 0;
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

        ClientToScreen(&rcClient);
        
        if ( ::IsWindow(m_wndClient) )
            m_wndClient.MoveWindow(&rcClient);
    }

    void ShowClient( HWND hWndClient )
    {
        if ( ::IsWindow(m_wndClient) )
            m_wndClient.ShowWindow( SW_HIDE );

        m_wndClient = hWndClient;

        ResizeClient();
        if ( ::IsWindow(m_wndClient) )
            m_wndClient.ShowWindow( SW_SHOWDEFAULT );
    }

    CWindow m_wndClient;
};