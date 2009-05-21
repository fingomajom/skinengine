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


class CWebViewBgWnd : public CWindowImpl<CWebViewBgWnd>
{
public:

    DECLARE_EMPTY_MSG_MAP()
    DECLARE_WND_CLASS(_T("DWExplorer_WebViewBgWnd"))
};

class CDWWebView : 
    public CDWHtmlEventMgt,
    public CMessageFilter
{
public:

    CWebViewBgWnd m_wndBk;
    CRgn          m_wndRgn;

    
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
        m_wndBk.Create( NULL, rcDefault, NULL, WS_POPUP, WS_EX_TOOLWINDOW );

        HWND hRet = CDWHtmlEventMgt::Create( m_wndBk, 
            rcDefault, 
            NULL,
            WS_POPUP | WS_CLIPCHILDREN, WS_EX_TOOLWINDOW  );

        SetTimer(1001, 200);

        return hRet;
    }

    BOOL DestroyWindow()
    {
        CDWHtmlEventMgt::DestroyWindow();
        return m_wndBk.DestroyWindow();
    }


    BEGIN_MSG_MAP(CDWWebView)

        MESSAGE_HANDLER(WM_COPYDATA         , OnCopyData      )

        MESSAGE_HANDLER(WM_ERASEBKGND       , OnEraseBkGnd    )
        MESSAGE_HANDLER(WM_DESTROY          , OnDestroy       ) 
        MESSAGE_HANDLER(WM_SETFOCUS         , OnSetFocus      )
        MESSAGE_HANDLER(WM_WEBVIEW_OPENURL  , OnWebWndOpenURL )
        MESSAGE_HANDLER(WM_SIZE             , OnSize          )
        MESSAGE_HANDLER(WM_CLOSE            , OnClose         )

        MESSAGE_HANDLER(WM_ACTIVATE         , OnActivate)

        MESSAGE_HANDLER(WM_WEBVIEW_SHOW     , OnShow     )
        MESSAGE_HANDLER(WM_WEBVIEW_MOVESIZE , OnMoveSize )
        MESSAGE_HANDLER(WM_WINDOWPOSCHANGED , OnWndPosChanged )

        MESSAGE_HANDLER(WM_TIMER            , OnTimer )
       
        CHAIN_MSG_MAP(CDWHtmlEventMgt);

    END_MSG_MAP()

        
    LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 1L;
    }

    LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        //LRESULT lRet = DefWindowProc();

        ::SetFocus(GetWebIeWnd());

        return 0L;
    }

    
    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 1L;
    }


    LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        HWND hRoot = ::GetAncestor(m_hNotifyWnd, GA_ROOT);
        
        ::PostMessage( hRoot, WM_CLOSE, 0, 0 );

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

        if ( (wParam == WA_CLICKACTIVE || wParam == WA_ACTIVE) )
        {
            HWND hRoot = ::GetAncestor(m_hNotifyWnd, GA_ROOT);

            //::SetWindowPos(hRoot, HWND_TOP , 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
            //SetWindowPos(hRoot , 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE  | SWP_NOSENDCHANGING);
            m_wndBk.SetWindowPos(m_hWnd , 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOSENDCHANGING | SWP_NOACTIVATE);

            //::SetFocus( GetWebIeWnd() );
        }

        return 0;
    }

    LRESULT OnShow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if ( wParam )
        {            
            CWindow wndClient = m_hNotifyWnd;

            RECT rcClient = { 0 };
            wndClient.GetWindowRect(&rcClient);

            HWND hRoot = ::GetAncestor(m_hNotifyWnd, GA_ROOT);

            SetWindowPos(hRoot, 
                rcClient.left, rcClient.top, 
                rcClient.right-rcClient.left, rcClient.bottom-rcClient.top, 
                SWP_NOSENDCHANGING | SWP_SHOWWINDOW | SWP_NOACTIVATE);
            m_wndBk.SetWindowPos(m_hWnd , 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOSENDCHANGING | SWP_NOACTIVATE);

            ::SetFocus( GetWebIeWnd() );
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

        RECT rcOldWindow = { 0 };
        RECT rcWindow = { 0 };
        wndClient.GetWindowRect(&rcWindow);
        GetWindowRect(&rcOldWindow);

        BOOL bSize = HIWORD(wParam);
        BOOL bMove = LOWORD(wParam);

        UINT nFlags = SWP_NOSENDCHANGING | SWP_SHOWWINDOW | SWP_NOZORDER;
        
        if ( !bSize )
            nFlags |= SWP_NOSIZE;
        else
            nFlags |= (SWP_NOREDRAW | SWP_DEFERERASE);
        if ( !bMove )
            nFlags |= SWP_NOMOVE;

        if ( rcOldWindow.left == rcWindow.left &&
             rcOldWindow.top == rcWindow.top &&
             rcOldWindow.right == rcWindow.right &&
             rcOldWindow.bottom == rcWindow.bottom )
             return Invalidate();

        if ( bSize )
        {
            RECT rcClient = rcWindow;

            rcClient.right  = rcClient.right - rcClient.left;
            rcClient.bottom = rcClient.bottom - rcClient.top;
            rcClient.top = rcClient.left = 0;

            CRgnHandle rgn;
            rgn.CreateRectRgnIndirect(&rcClient);

            SetWindowRgn(rgn, TRUE);

            if (m_wndRgn.m_hRgn != NULL)
                m_wndRgn.DeleteObject();
            m_wndRgn.m_hRgn = rgn;
        }

        LRESULT lRet = SetWindowPos(NULL, 
            rcWindow.left, rcWindow.top, 
            rcWindow.right-rcWindow.left, rcWindow.bottom-rcWindow.top, 
            nFlags);

        return lRet;
    }

    LRESULT OnWndPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if ( (GetStyle() & WS_CHILD) == 0 && ::IsWindowVisible(m_hNotifyWnd) )
        {
            HWND hRoot = ::GetAncestor(m_hNotifyWnd, GA_ROOT);
            
            CWindow wndClient = m_hNotifyWnd;

            RECT rcClient = { 0 };
            wndClient.GetWindowRect(&rcClient);

            //::SetWindowPos(hRoot, HWND_TOP , 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOSENDCHANGING);

            SetWindowPos(hRoot , 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOSENDCHANGING);
            m_wndBk.SetWindowPos(m_hWnd ,
                rcClient.left, rcClient.top, 
                rcClient.right-rcClient.left, rcClient.bottom-rcClient.top, 
                SWP_NOSIZE | SWP_NOSENDCHANGING | SWP_NOACTIVATE);

        }

        return ::DefWindowProc( m_hWnd, uMsg, wParam, lParam );
    }

    LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if ( wParam == 1001 && IsWindow() )
        {
            HWND hRoot = ::GetAncestor(m_hNotifyWnd, GA_ROOT);

            if ( !::IsWindowVisible(m_hNotifyWnd) || ::IsIconic(hRoot) )
            {
                if ( IsWindowVisible() )
                    ShowWindow(SW_HIDE);
            }
            else if ( !IsWindowVisible() || !CheckWebWndZOrder(m_hWnd, hRoot) )
            {
                CWindow wndClient = m_hNotifyWnd;

                RECT rcClient = { 0 };
                wndClient.GetWindowRect(&rcClient);

                SetWindowPos(hRoot, 
                    rcClient.left, rcClient.top, 
                    rcClient.right-rcClient.left, rcClient.bottom-rcClient.top, 
                    SWP_NOSENDCHANGING | SWP_SHOWWINDOW );
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

    bool CheckWebWndZOrder(HWND hAx, HWND hRoot)
    {
        HWND hWnd = hRoot;
        while (hWnd = ::GetWindow(hWnd, GW_HWNDNEXT))
        {
            if (hWnd == hAx)
                return true;
            if (!::IsWindowVisible(hWnd))
                continue;
            if (::GetWindowThreadProcessId(hWnd, NULL) == ::GetCurrentThreadId())
                continue;
            HWND hOwner = ::GetParent(hWnd);
            if (hOwner == hAx)
                continue;
            return false;
        }
        return false;
    }

    bool IsTabedObject(IHTMLElement * pHtmlElem)
    {
        CComQIPtr<IHTMLElement> spHtmlElem = pHtmlElem;
        if (spHtmlElem)
        {
            CComBSTR bstrTagName;
            if (SUCCEEDED(spHtmlElem->get_tagName(&bstrTagName)) && bstrTagName)
            {
                if (StrCmpI(bstrTagName, L"a") == 0 || 
                    StrCmpI(bstrTagName, L"input") == 0 || 
                    StrCmpI(bstrTagName, L"button") == 0 || 
                    StrCmpI(bstrTagName, L"textarea") == 0 || 
                    StrCmpI(bstrTagName, L"select") == 0 || 
                    StrCmpI(bstrTagName, L"object") == 0)
                    return true;
            }
        }
        return false;
    }

    void MoveFocusToIe(bool bShiftDown)
    {
        CComPtr<IDispatch> spDisp;
        if (SUCCEEDED(m_spWebBrowser->get_Document(&spDisp)) && spDisp)
        {
            CComQIPtr<IHTMLDocument2> spDoc2 = spDisp;
            if (spDoc2)
            {
                CComPtr<IHTMLElementCollection> spColl;
                if (SUCCEEDED(spDoc2->get_all(&spColl)) && spColl)
                {
                    long nCnt = 0;
                    spColl->get_length(&nCnt);

                    if (bShiftDown)
                    {
                        for (long i = nCnt - 1; i >= 0; i--)
                        {
                            CComVariant varIdx = i;
                            CComPtr<IDispatch> spTagDisp;
                            if (SUCCEEDED(spColl->item(varIdx, varIdx, &spTagDisp)) && spTagDisp)
                            {
                                CComQIPtr<IHTMLElement> spHtmlElem(spTagDisp);
                                if (IsTabedObject(spHtmlElem))
                                {
                                    CComQIPtr<IHTMLElement2> spHtmlElem2 = spHtmlElem;
                                    spHtmlElem2->focus();
                                    break;
                                }
                            }
                        }
                    }
                    else
                    {
                        for (long i = 0; i < nCnt; i++)
                        {
                            CComVariant varIdx = i;
                            CComPtr<IDispatch> spTagDisp;
                            if (SUCCEEDED(spColl->item(varIdx, varIdx, &spTagDisp)) && spTagDisp)
                            {
                                CComQIPtr<IHTMLElement> spHtmlElem(spTagDisp);
                                if (IsTabedObject(spHtmlElem))
                                {
                                    CComQIPtr<IHTMLElement2> spHtmlElem2 = spHtmlElem;
                                    spHtmlElem2->focus();
                                    break;
                                }
                            }
                        }
                    }

                    spColl.Release();
                }
            }
        }
    }

    HWND GetWebIeWnd()
    {
        HWND hShellEmbedding = ::GetWindow(m_hWnd, GW_CHILD);
        if (hShellEmbedding == NULL)
            return NULL;
        HWND hDocObj = ::GetWindow(hShellEmbedding, GW_CHILD);
        if (hDocObj == NULL)
            return NULL;
        HWND hIEServer = ::FindWindowEx(hDocObj, NULL, CLASS_NAME_IE_SERVER, NULL);
        return hIEServer;
    }


};

