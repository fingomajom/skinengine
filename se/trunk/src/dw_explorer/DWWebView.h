/********************************************************************
	created:	2009/05/02
	created:	2:5:2009   12:30
	filename: 	d:\lidengwang\se\trunk\src\dw_explorer\DWWebWnd.h
	file path:	d:\lidengwang\se\trunk\src\dw_explorer
	file base:	DWWebWnd
	file ext:	h
	author:		lidengwang@s-zone.com
	
	purpose:	
*********************************************************************/

#pragma once

#include "DWComDef.h"
#include <mshtmdid.h>
#include "DWHtmlEventMgt.h"


class CDWWebView : 
    public CDWHtmlEventMgt,
    public CMessageFilter
{
public:
    
    BOOL PreTranslateMessage(MSG* pMsg)
    {
        if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
            (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
            return FALSE;

        return (BOOL)SendMessage(WM_FORWARDMSG, 0, (LPARAM)pMsg);
    }

    BOOL OpenURL( LPCTSTR pszURL )
    {
        m_strTitle.Empty();
        m_strURL.Empty();

        Navigate2( pszURL );

        return TRUE;
    }

    BEGIN_MSG_MAP(CDWWebView)

        MESSAGE_HANDLER(WM_ERASEBKGND , OnEraseBkGnd )
        MESSAGE_HANDLER(WM_SETFOCUS   , OnSetFocus )

        MESSAGE_HANDLER(WM_SHOWWINDOW , OnShowWindow )

        MESSAGE_HANDLER(WM_USER_GET_WEBBROWSER2_CROSS_THREAD , OnGetMarshalWebBrowser2CrossThread)
        MESSAGE_HANDLER(WM_USER_GET_WEBBROWSER2_CROSS_PROCESS, OnGetMarshalWebBrowser2CrossProcess)
        
        CHAIN_MSG_MAP(CDWHtmlView);

    END_MSG_MAP()

    LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        HWND hShellEmbedding = ::GetWindow(m_hWnd, GW_CHILD);
        if (hShellEmbedding == NULL)
            return 0L;
        HWND hDocObj = ::GetWindow(hShellEmbedding, GW_CHILD);
        if (hDocObj == NULL)
            return 0L;
        HWND hIEServer = ::FindWindowEx(hDocObj, NULL, CLASS_NAME_IE_SERVER, NULL);

        ::SetFocus(hIEServer);

        return 0L;
    }

    LRESULT OnEraseBkGnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 1;
    }

    LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if ( wParam == 10 && lParam == 100 )
        {
            ShowWindow(SW_SHOWDEFAULT);
            return SetForegroundWindow(m_hWnd);
        }
        else if ( wParam == 10 && lParam == 101)
            return ShowWindow(SW_HIDE);

        return ::DefWindowProc( m_hWnd, uMsg, wParam, lParam );
    }

};

