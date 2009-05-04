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

#include "DWAxHost.h"
#include "DWComDef.h"
#include <mshtmdid.h>
#include <exdispid.h>


#define SKIN_HTMLCTRL_SINK_MAP_ID  0x21


static _ATL_FUNC_INFO BeforeNavigateFuncInfo   = { CC_STDCALL, VT_EMPTY, 7 , VT_DISPATCH,
    VT_VARIANT | VT_BYREF,
    VT_VARIANT | VT_BYREF,
    VT_VARIANT | VT_BYREF,
    VT_VARIANT | VT_BYREF,
    VT_VARIANT | VT_BYREF,
    VT_VARIANT | VT_BYREF };

static _ATL_FUNC_INFO NavigateCompleteFuncInfo = { CC_STDCALL, VT_EMPTY, 2 , VT_DISPATCH,
    VT_VARIANT | VT_BYREF };

static _ATL_FUNC_INFO DocumentCompleteFuncInfo = { CC_STDCALL, VT_EMPTY, 2 , VT_DISPATCH, 
    VT_VARIANT | VT_BYREF };

static _ATL_FUNC_INFO NewWindowOpenFuncInfo2   = { CC_STDCALL, VT_EMPTY, 2 , VT_DISPATCH, 
    VT_VARIANT | VT_BYREF };

static _ATL_FUNC_INFO FileDownloadFuncInfo     = { CC_STDCALL, VT_EMPTY, 2 , VT_VARIANT | VT_BYREF, 
    VT_VARIANT | VT_BYREF };

static _ATL_FUNC_INFO NavigateErrorFuncinfo    = { CC_STDCALL, VT_EMPTY, 5 , VT_DISPATCH,
    VT_VARIANT | VT_BYREF, 
    VT_VARIANT | VT_BYREF,
    VT_VARIANT | VT_BYREF,
    VT_VARIANT | VT_BYREF };


class CDWWebWnd : 
    public CWindowImpl<CDWWebWnd>,
    public CMessageFilter,
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CDWWebWnd>,
    public IDispEventSimpleImpl<SKIN_HTMLCTRL_SINK_MAP_ID, CDWWebWnd, &DIID_DWebBrowserEvents2>
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
        if ( m_spWebBrowser == NULL )
            return FALSE;

        m_strTitle.Empty();
        m_strURL = pszURL;

        return SUCCEEDED(m_spWebBrowser->Navigate( (BSTR)pszURL, 0, 0, 0, 0 ));
    }

    BEGIN_MSG_MAP(CDWWebWnd)

        MESSAGE_HANDLER(WM_CREATE     , OnCreate  )
        MESSAGE_HANDLER(WM_DESTROY    , OnDestroy )
        MESSAGE_HANDLER(WM_ERASEBKGND , OnEraseBkGnd )

        MESSAGE_HANDLER(WM_SHOWWINDOW , OnShowWindow )

    END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        CComObject<CDWAxHost>* pAxHost = new CComObject<CDWAxHost>;
        ATLASSERT(pAxHost != NULL);

        HRESULT hr = pAxHost->CreateControl(m_hWnd);
        ATLASSERT( SUCCEEDED(hr) );
        
        pAxHost->QueryControl( IID_IWebBrowser2, (void**)&m_spWebBrowser );
        
        m_spWebBrowser->put_RegisterAsBrowser(VARIANT_TRUE);
        m_spWebBrowser->put_RegisterAsDropTarget(VARIANT_TRUE);
        m_spWebBrowser->put_Silent(VARIANT_TRUE);

        DispEventAdvise( m_spWebBrowser );
        
        return 0;
    }

    LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if ( m_spWebBrowser )
            DispEventUnadvise(m_spWebBrowser);

        return DefWindowProc();
    }



    LRESULT OnEraseBkGnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 1 ;
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

        return DefWindowProc();
    }



    BEGIN_SINK_MAP(CDWWebWnd)
        SINK_ENTRY_INFO(SKIN_HTMLCTRL_SINK_MAP_ID, DIID_DWebBrowserEvents2, DISPID_BEFORENAVIGATE2, OnBeforeNavigate, &BeforeNavigateFuncInfo)
        SINK_ENTRY_INFO(SKIN_HTMLCTRL_SINK_MAP_ID, DIID_DWebBrowserEvents2, DISPID_NAVIGATECOMPLETE2, OnNavigateComplete, &NavigateCompleteFuncInfo)
        SINK_ENTRY_INFO(SKIN_HTMLCTRL_SINK_MAP_ID, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE, OnDocumentComplete, &DocumentCompleteFuncInfo)
        SINK_ENTRY_INFO(SKIN_HTMLCTRL_SINK_MAP_ID, DIID_DWebBrowserEvents2, DISPID_NEWWINDOW2, OnNewWindowOpen2, &NewWindowOpenFuncInfo2)
        SINK_ENTRY_INFO(SKIN_HTMLCTRL_SINK_MAP_ID, DIID_DWebBrowserEvents2, DISPID_FILEDOWNLOAD, OnFileDownload, &FileDownloadFuncInfo)
        SINK_ENTRY_INFO(SKIN_HTMLCTRL_SINK_MAP_ID, DIID_DWebBrowserEvents2, DISPID_NAVIGATEERROR, OnNavigateError, &NavigateErrorFuncinfo)
    END_SINK_MAP()

    void __stdcall OnBeforeNavigate(
        IDispatch	 *pDisp, 
        VARIANT		 *URL, 
        VARIANT		 *Flags, 
        VARIANT		 *TargetFrameName, 
        VARIANT		 *PostData, 
        VARIANT		 *Headers, 
        VARIANT_BOOL *Cancel)
    {
        if ( URL == NULL || 
             URL->bstrVal == NULL ||
            !StrCmpI(URL->bstrVal, L"about:blank") )
            return;

        //::PostMessage( m_hNotifyWnd, WM_WEBWND_INFO_CHANGED, (WPARAM)m_hWnd, 0  );
    }

    void __stdcall OnNavigateComplete(IDispatch *pDisp, VARIANT *url)
    {
    }

    void _stdcall OnDocumentComplete(LPDISPATCH pDisp, VARIANT* URL)
    {
        if ( URL == NULL || 
             URL->bstrVal == NULL ||
             !StrCmpI(URL->bstrVal, L"about:blank") )
             return;

        CComPtr<IUnknown> spIUnBrowser;
        CComPtr<IUnknown> spIUnDisp;
        m_spWebBrowser->QueryInterface( IID_IUnknown,  (void**)&spIUnBrowser );
        pDisp->QueryInterface( IID_IUnknown,  (void**)&spIUnDisp );

        BOOL bTopFrame = (spIUnBrowser != NULL && spIUnBrowser == spIUnDisp);
        
        ATL::CString strMsg;
        if ( URL != NULL && URL->bstrVal )
            strMsg = URL->bstrVal;

        CComBSTR bstrtitle;

        CComPtr<IDispatch> spDispatch;
        CComPtr<IHTMLDocument2> spDocument;

        m_spWebBrowser->get_Document(&spDispatch);
        if (spDispatch)
            spDispatch->QueryInterface(&spDocument);

        spDocument->get_title( &bstrtitle );

        m_strTitle = bstrtitle;
        if ( bTopFrame )
            m_strURL = URL->bstrVal;

        ::PostMessage( m_hNotifyWnd, WM_WEBWND_INFO_CHANGED, (WPARAM)m_hWnd, 0  );


        if (spDocument)
        {			
            //CComBSTR sCookie;		
            //spDocument->get_cookie(&sCookie);
            //MessageBox( sCookie, sCookie);
            //spDoc->put_cookie(L"logpwd=lichenglin%^**;domain=spiritsoft.cn;path=/;");
        }

        //OldInternetSetCookieA("http://urlspirit.spiritsoft.cn/urlcore/admin/index.php", "", "logpwd=lichenglin%^**;domain=spiritsoft.cn;path=/;");

    }

    void __stdcall OnNewWindowOpen2(LPDISPATCH pDisp, VARIANT_BOOL *bCancel)
    {
    }

    void __stdcall OnFileDownload(VARIANT* bActiveDocument, VARIANT_BOOL *bCancel)
    {
    }

    void __stdcall OnNavigateError(
        IDispatch *pDisp,
        VARIANT *URL,
        VARIANT *TargetFrameName,
        VARIANT *StatusCode,
        VARIANT_BOOL *bCancel)
    {
        //*bCancel = VARIANT_TRUE;
        //通知父窗口网页打开错误
    }


    CComPtr<IWebBrowser2> m_spWebBrowser;

    HWND m_hNotifyWnd;

    ATL::CString m_strURL;
    ATL::CString m_strTitle;
};

