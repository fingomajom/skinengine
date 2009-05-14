// quickserachView.h : interface of the CQuickserachView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "QuickSearchMgr.h"

class CQuickserachView : public CWindowImpl<CQuickserachView, CAxWindow>
{
public:
	DECLARE_WND_SUPERCLASS(NULL, CAxWindow::GetWndClassName())

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
		   (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
			return FALSE;

		// give HTML page a chance to translate this message
		BOOL bRet = (BOOL)SendMessage(WM_FORWARDMSG, 0, (LPARAM)pMsg);

        if ( pMsg->message == WM_LBUTTONUP  )
        {
            RECT rcClient;
            GetClientRect(&rcClient);

            ClientToScreen(&rcClient);

            POINT pt;
            GetCursorPos(&pt);

            if ( PtInRect(&rcClient, pt) )
            {
                OnLButtonUp( WM_LBUTTONUP, pMsg->wParam, pMsg->lParam );
            }
        }

        return bRet;
	}

	BEGIN_MSG_MAP(CQuickserachView)
	END_MSG_MAP()

    LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
    {
        CComPtr<IWebBrowser2>   spWebBrowser;
        CComPtr<IDispatch>      spDispatch;
        CComPtr<IHTMLDocument2> spHTMLDocument;

        CAxWindow::QueryControl( &spWebBrowser );

        if ( spWebBrowser.p == NULL )
            return 0L;
        
        spWebBrowser->get_Document( &spDispatch );
        if ( spDispatch.p == NULL )
            return 0L;

        spDispatch->QueryInterface(&spHTMLDocument);
        if ( spHTMLDocument.p == NULL )
            return 0L;

        CComPtr<IHTMLSelectionObject> spHTMLSelectionObject;
        CComPtr<IHTMLTxtRange> spHTMLTxtRange;


        spHTMLDocument->get_selection( &spHTMLSelectionObject );
        if ( spHTMLSelectionObject.p == NULL )
            return 0L;
        
        spHTMLSelectionObject->createRange( (LPDISPATCH*) &spHTMLTxtRange );
        if ( spHTMLTxtRange.p == NULL )
            return 0L;

        CComBSTR bstrSelText;
        spHTMLTxtRange->get_text(&bstrSelText);

        if ( lstrlenW(bstrSelText) > 0 && lstrlenW(bstrSelText) <= 20 )
        {
            CW2AEX<> ascii(bstrSelText, CP_ACP);

            if ( lstrlenA( ascii.m_psz ) < 20 )
            {
                CQuickSearchMgr& mgt = CQuickSearchMgr::Instance();

                POINT pt;
                GetCursorPos(&pt);

                mgt.m_wndQSPopWnd.ShowQSPopWnd(pt, bstrSelText);
            }
        }


        return 1L;
    }


// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
};
