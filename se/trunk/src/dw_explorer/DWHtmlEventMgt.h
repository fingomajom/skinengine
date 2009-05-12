

#pragma once

#include "DWHtmlView.h"
#include "DWDragDropMgt.h"

HWND CreateWebWnd( HWND hParent, LPCTSTR pszOpenURL, IWebBrowser2** ppOut );


class CDWHtmlEventMgt : public CDWHtmlView
{
public:

    CDWHtmlEventMgt() {}

    ~CDWHtmlEventMgt()
    {
    }

    BEGIN_MSG_MAP(CDWWebView)

        MESSAGE_HANDLER(WM_USER_GET_WEBBROWSER2_CROSS_THREAD , OnGetMarshalWebBrowser2CrossThread)
        MESSAGE_HANDLER(WM_USER_GET_WEBBROWSER2_CROSS_PROCESS, OnGetMarshalWebBrowser2CrossProcess)

        CHAIN_MSG_MAP(CDWHtmlView);

    END_MSG_MAP();

    virtual void OnFinalMessage(HWND /*hWnd*/)
    {
        RevokeDragDrop(m_hWnd);
    }


    LRESULT OnGetMarshalWebBrowser2CrossThread(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        __try
        {
            IStream* pStream = NULL ;

            HRESULT hr = CoMarshalInterThreadInterfaceInStream(IID_IWebBrowser2, m_spWebBrowser, &pStream) ;
            if (FAILED(hr) || !pStream)
            {
                return NULL ;
            }
            return (LRESULT)pStream ;
        }
        __except (1)
        {
            ATLASSERT(FALSE) ;
            return NULL ;
        }
    }

    LRESULT OnGetMarshalWebBrowser2CrossProcess(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        DWORD dwRet = 0;

        ULONG ulSize = 0;
        if (SUCCEEDED(::CoGetMarshalSizeMax (&ulSize, IID_IWebBrowser2, m_spWebBrowser, MSHCTX_LOCAL, NULL, MSHLFLAGS_NORMAL)))
        {
            HGLOBAL hGlobal = ::GlobalAlloc(GHND, (DWORD)ulSize);
            if (hGlobal)
            {
                IStream* pStream = NULL ;
                if (SUCCEEDED(::CreateStreamOnHGlobal (hGlobal, FALSE, &pStream)))
                {
                    if (SUCCEEDED(CoMarshalInterface(pStream, IID_IWebBrowser2, m_spWebBrowser, MSHCTX_LOCAL, NULL, MSHLFLAGS_NORMAL)))
                    {
                        COPYDATASTRUCT cds = { WM_USER_MULTI_PROCESS_GET_STREAM, ulSize, ::GlobalLock(hGlobal) };
                        ::SendMessage((HWND)wParam, WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);
                    }
                }
            }
        }

        return dwRet;
    }

    virtual HRESULT OnTranslateUrl(DWORD dwTranslate, OLECHAR* pchURLIn, OLECHAR** ppchURLOut)
    {
        return S_OK;
    }

    virtual HRESULT OnGetDropTarget(LPDROPTARGET pDropTarget, LPDROPTARGET* ppDropTarget)
    {
        RegisterDragDrop(m_hWnd, &m_DragDropMgt);
        
        m_DragDropMgt.m_pHtmlView    = this;
        m_DragDropMgt.m_spDropTarget = pDropTarget;
        *ppDropTarget = &m_DragDropMgt;

        return S_OK;
    }

    virtual void OnNavigateComplete2(LPCTSTR strURL)
    {
    }
    virtual void OnBeforeNavigate2(
        LPCTSTR lpszURL, 
        DWORD nFlags,
        LPCTSTR lpszTargetFrameName, 
        CComVariant& baPostedData,
        LPCTSTR lpszHeaders, 
        BOOL* pbCancel)
    {
    }

    virtual void OnStatusTextChange(LPCTSTR lpszText)
    {
        ::SendCopyData(m_hNotifyWnd, 
            WM_WEBVIEW_SETSTATUS, 
            lpszText,
            (lstrlenW(lpszText) + 1) * sizeof(TCHAR) );
    }

    virtual void OnProgressChange(long nProgress, long nProgressMax)
    {
        int a = 0;
    }
    virtual void OnCommandStateChange(long nCommand, BOOL bEnable)
    {
        int a = 0;
    }
    virtual void OnDownloadBegin()
    {
        int a = 0;
    }
    virtual void OnDownloadComplete()
    {
        int a = 0;
    }

    virtual void OnTitleChange(LPCTSTR lpszText)
    {
        if ( StrCmpI(m_strTitle, lpszText) )
        {
            m_strTitle = lpszText;

            ::SendCopyData(m_hNotifyWnd, 
                WM_WEBVIEW_SETTITLE, 
                lpszText,
                (lstrlenW(lpszText) + 1) * sizeof(TCHAR) );
        }
    }

    virtual void OnPropertyChange(LPCTSTR lpszProperty)
    {
    }
    virtual void OnNewWindow2(LPDISPATCH* ppDisp, BOOL* bCancel)
    {
        HWND hWndChildFrm = (HWND)::SendMessage( m_hNotifyWnd, WM_WEBVIEW_CREATE, 0, 0 ); 
        if ( hWndChildFrm == NULL || !::IsWindow(hWndChildFrm) )
        {
            *bCancel = TRUE;
            return;
        }

        HWND hWnd = (HWND)::SendMessage( hWndChildFrm, WM_WEBVIEW_CREATE, TRUE, 0 );
        while ( hWnd == NULL )
        {
            Sleep(30);
            hWnd = (HWND)::SendMessage( hWndChildFrm, WM_WEBVIEW_CREATE, TRUE, 0 );
        }

        DWORD dwPID = 0 ;
        GetWindowThreadProcessId(hWnd, &dwPID);

        ATLASSERT ( dwPID == GetCurrentProcessId() );
        if ( dwPID == GetCurrentProcessId() )
        {
            IStream* pStream = (IStream*)::SendMessage(hWnd, WM_USER_GET_WEBBROWSER2_CROSS_THREAD, 0, 0) ;
            ATLASSERT(pStream);
            if (!pStream)
            {
                *bCancel = VARIANT_TRUE;
                return;
            }
            CComPtr<IWebBrowser2> spWebBrowser2;
            if (SUCCEEDED(CoGetInterfaceAndReleaseStream(pStream, IID_IWebBrowser2, (void**)&spWebBrowser2)) && spWebBrowser2)
            {
                spWebBrowser2->get_Application(ppDisp);
            }

            ::SetFocus(hWnd);
        }
    }

    BOOL IsToppestWindowOfBrowser2(IDispatch* pDispatch)
    {
        if (!pDispatch || !m_spWebBrowser)
        {
            return FALSE ;
        }
        IUnknown*  pUnk;
        LPDISPATCH lpWBDisp;

        if (FAILED(m_spWebBrowser->QueryInterface(IID_IUnknown, (void**)&pUnk)) || !pUnk)
        {
            return FALSE ;
        }

        if (FAILED(pUnk->QueryInterface(IID_IDispatch, (void**)&lpWBDisp)) || !lpWBDisp)
        {
            return FALSE ;
        }

        if (pDispatch == lpWBDisp)
        {
            return TRUE ;
        }

        return FALSE ;
    }

    BOOL IsToppestWindowOfBrowser(IDispatch* pDispatch)
    {
        if (!pDispatch)
        {
            return FALSE ;
        }

        HRESULT hr = S_OK ;
        CComPtr<IWebBrowser2> spWebBrowser2 ;
        hr = pDispatch->QueryInterface(IID_IWebBrowser2, (void**)&spWebBrowser2) ;
        if (FAILED(hr) || !spWebBrowser2)
        {
            return FALSE ;
        }
        CComPtr<IDispatch> spDispDoc ;
        hr = spWebBrowser2->get_Document(&spDispDoc) ;
        if (FAILED(hr) || !spDispDoc)
        {
            return FALSE ;
        }
        CComPtr<IHTMLDocument2> spHTMLDocument2 ;
        hr = spDispDoc->QueryInterface(IID_IHTMLDocument2, (void**)&spHTMLDocument2) ;
        if (FAILED(hr) || !spHTMLDocument2)
        {
            return FALSE ;
        }
        CComPtr<IHTMLWindow2> spHTMLWindow2 ;
        hr = spHTMLDocument2->get_parentWindow(&spHTMLWindow2) ;
        if (FAILED(hr) || !spHTMLWindow2)
        {
            return FALSE ;
        }

        CComPtr<IHTMLWindow2> spTopWindow2 ;
        hr = spHTMLWindow2->get_top(&spTopWindow2) ;

        if (FAILED(hr) || !spHTMLWindow2)
        {
            return FALSE ;
        }

        //CComPtr<IObjectIdentity> spIdentity ;
        //hr = spHTMLWindow2->QueryInterface(IID_IObjectIdentity, (void**)&spIdentity) ;
        //if (FAILED(hr) || !spHTMLWindow2)
        //{
        //    return FALSE ;
        //}
        //hr = spIdentity->IsEqualObject(spTopWindow2) ;
        //if (S_OK == hr)
        //{
        //    return TRUE ;
        //}

        return TRUE ;
    }

    virtual void NavigateComplete2(LPDISPATCH pDisp, VARIANT* URL)
    {
        if ( !IsToppestWindowOfBrowser(pDisp) ||
             !IsToppestWindowOfBrowser2(pDisp) )
        {
             return;
        }

        ATL::CString strURL(*URL);

        if ( !StrCmpNI(L"javascript:", strURL, 11) ||
             !StrCmpNI(L"vbscript:"  , strURL, 9)  ||
             !StrCmpNI(L"mailto:"    , strURL, 7)  ||
             !StrCmpNI(L"about:"     , strURL, 6)  ||
             !StrCmpI( m_strURL, strURL ) )
        {
            return;
        }

        m_strURL = strURL;

        ::SendCopyData(m_hNotifyWnd, 
            WM_WEBVIEW_SETURL, 
            (LPCTSTR)m_strURL,
            (lstrlenW(m_strURL) + 1) * sizeof(TCHAR) );
    }

    virtual void OnQuit()
    {
    }
    virtual void OnTheaterMode(BOOL bTheaterMode)
    {
    }
    virtual void OnNavigateError(LPCTSTR lpszURL, LPCTSTR lpszFrame, DWORD dwError, BOOL *pbCancel)
    {
    }


public:

    CDWDragDropMgt m_DragDropMgt;

    HWND m_hNotifyWnd;

    ATL::CString m_strURL;
    ATL::CString m_strTitle;

};