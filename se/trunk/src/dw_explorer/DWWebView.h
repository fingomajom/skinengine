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

    HWND Create(HWND hWndParent)
    {

        HWND hRet = CDWHtmlEventMgt::Create( hWndParent, 
            rcDefault, 
            NULL,
            WS_CHILD | WS_CLIPCHILDREN );

        //HWND hRet = CDWHtmlEventMgt::Create( NULL, 
        //    rcDefault, 
        //    NULL,
        //    WS_POPUP, WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE);

        SetTimer(1001, 150);

        return hRet;
    }

    BEGIN_MSG_MAP(CDWWebView)

        MESSAGE_HANDLER(WM_COPYDATA         , OnCopyData      )

        MESSAGE_HANDLER(WM_ERASEBKGND       , OnEraseBkGnd    )
        MESSAGE_HANDLER(WM_SETFOCUS         , OnSetFocus      )
        MESSAGE_HANDLER(WM_WEBVIEW_OPENURL  , OnWebWndOpenURL )
        MESSAGE_HANDLER(WM_SIZE             , OnSize          )

        MESSAGE_HANDLER(WM_ACTIVATE         , OnActivate)

        MESSAGE_HANDLER(WM_WEBVIEW_SHOW     , OnShow     )
        MESSAGE_HANDLER(WM_WEBVIEW_MOVESIZE , OnMoveSize )
        MESSAGE_HANDLER(WM_WINDOWPOSCHANGED , OnWndPosChanged )

        MESSAGE_HANDLER(WM_TIMER            , OnTimer )

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

    
    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 1L;
    }

    LRESULT OnEraseBkGnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 1L;
    }

    LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;

        if (!::IsWindowVisible( m_hNotifyWnd ) )
            return 0;

        if ( wParam == WA_CLICKACTIVE || wParam == WA_ACTIVE )
        {
            //HWND hRoot = ::GetAncestor(m_hNotifyWnd, GA_ROOT);

            //::SetWindowPos(hRoot , m_hWnd, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
            //SetWindowPos(HWND_TOP , 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSENDCHANGING);

            //SetFocus();
        }

        return 0;
    }

    LRESULT OnShow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CWindow wndClient = m_hNotifyWnd;

        RECT rcClient = { 0 };
        wndClient.GetWindowRect(&rcClient);

        if ( wParam )
        {
            //::SetForegroundWindow(m_hWnd);
            //::SetForegroundWindow(m_hNotifyWnd);
            
            ShowWindow(SW_SHOWNOACTIVATE);
            //::SetWindowPos(m_hNotifyWnd, m_hWnd , 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
            //SetWindowPos(HWND_TOP, 
            //    rcClient.left, rcClient.top, 
            //    rcClient.right-rcClient.left, rcClient.bottom-rcClient.top, 
            //    SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOSENDCHANGING | SWP_SHOWWINDOW);
        }
        else 
        {
            return ShowWindow(SW_HIDE);
        }

        return 0L;
    }

    LRESULT OnMoveSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CWindow wndClient = m_hNotifyWnd;

        RECT rcClient = { 0 };
        wndClient.GetClientRect(&rcClient);
        return MoveWindow(&rcClient);
        //wndClient.GetWindowRect(&rcClient);

        //HWND hRoot = ::GetAncestor(m_hNotifyWnd, GA_ROOT);

        //return SetWindowPos( hRoot, 
        //    rcClient.left, rcClient.top, 
        //    rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
        //    SWP_SHOWWINDOW);
    }

    LRESULT OnWndPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if ((GetStyle() & WS_CHILD) == 0 && ::IsWindowVisible(m_hNotifyWnd))
        {
            //HWND hRoot = ::GetAncestor(m_hNotifyWnd, GA_ROOT);

            //::SetWindowPos(hRoot , m_hWnd, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
            //::SetWindowPos(m_hWnd, hRoot , 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSENDCHANGING);

            //SetWindowPos(HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSENDCHANGING | SWP_SHOWWINDOW);

        }

        return ::DefWindowProc( m_hWnd, uMsg, wParam, lParam );
    }

    LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if ( wParam == 1001 && IsWindow() )
        {
            if ( !::IsWindowVisible(m_hNotifyWnd) )
                ShowWindow(SW_HIDE);
            else
            {
                ShowWindow(SW_SHOWNORMAL);
                HWND hRoot = ::GetAncestor(m_hNotifyWnd, GA_ROOT);

                //::SetWindowPos(hRoot , m_hWnd, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
                //::SetWindowPos(m_hWnd, hRoot , 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
            }
        }
        return 0;
    }

    LRESULT OnCopyData(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        PCOPYDATASTRUCT pcds = (PCOPYDATASTRUCT)lParam;

        SendMessage( pcds->dwData, (LPARAM)pcds->lpData, 0 );

        return 0;
    }

    
    LRESULT OnWebWndOpenURL(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LPCTSTR pszURL = (LPCTSTR)wParam;

        OpenURL( pszURL );

        return 0;
    }


};

