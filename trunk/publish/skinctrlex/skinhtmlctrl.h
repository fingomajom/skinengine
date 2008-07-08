/********************************************************************
* CreatedOn: 2008-6-3   10:41
* FileName: skinhtmlctrl.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include <skinwin.h>
#include <exdispid.h>
#include <mshtmdid.h>
#include <mshtmcid.h>
#include <comutil.h>
#include <atlstr.h>

#pragma comment(lib, "comsupp.lib")

namespace KSGUI{
    
//  
//  KSGUI::SkinWindowCreator::Instance().AddSkinCreator( 
//      KSGUI::skinxmlhtmlctrl::GetSkinWndClassName(),
//      KSGUI::CSkinHtmlCtrl::SkinCreate_Static);

class CDHtmlEventCallback
{
public:

    virtual void OnClicked( LPCTSTR pszItemId ) = 0;

};


#define SKIN_HTMLCTRL_SINK_MAP_ID  0x21


static _ATL_FUNC_INFO DocumentCompleteFuncInfo = { CC_STDCALL, VT_EMPTY, 2 , VT_DISPATCH, VT_VARIANT | VT_BYREF };


class CStreamOnCString : public IStream
{
public:

    CStreamOnCString(LPCTSTR szData);

    STDMETHOD(QueryInterface)(REFIID iid, void **ppUnk);
    ULONG STDMETHODCALLTYPE AddRef ( void );
    ULONG STDMETHODCALLTYPE Release( void );

    STDMETHOD(Read)(void *pv, ULONG cb, ULONG *pcbRead);
    STDMETHOD(Write)(const void *pv, ULONG cb, ULONG *pcbWritten);
    STDMETHOD(Seek)(LARGE_INTEGER , DWORD , ULARGE_INTEGER *);
    STDMETHOD(SetSize)(ULARGE_INTEGER );
    STDMETHOD(CopyTo)(IStream *, ULARGE_INTEGER , ULARGE_INTEGER *,
          ULARGE_INTEGER *);
    STDMETHOD(Commit)(DWORD );
    STDMETHOD(Revert)( void);
    STDMETHOD(LockRegion)(ULARGE_INTEGER , ULARGE_INTEGER , DWORD );
    STDMETHOD(UnlockRegion)(ULARGE_INTEGER , ULARGE_INTEGER , DWORD );
    STDMETHOD(Stat)(STATSTG *, DWORD );
    STDMETHOD(Clone)(IStream **);

protected:

    ATL::CString m_strStream;
    ATL::CStringA m_strAnsi;
    UINT m_current_index;
};


class CDHtmlEventSink : 
    public IDispEventSimpleImpl<1, CDHtmlEventSink, &__uuidof(HTMLDocumentEvents)>
{
public:
    CDHtmlEventSink( ) :
        pCallback(NULL)
    {
    }

      virtual HRESULT __stdcall QueryInterface( 
          /* [in] */ REFIID riid,
          /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
      {
          *ppvObject = this;
          return S_OK;
      }

      virtual ULONG __stdcall AddRef( void)
      {
          return 1L;
      }

      virtual ULONG __stdcall Release( void)
      {
          return 0L;
      }


      HRESULT CDHtmlEventSink::GetDHtmlEventEntry(IHTMLElement **ppsrcElement);

      virtual /* [local] */ HRESULT __stdcall Invoke( 
          /* [in] */ DISPID dispIdMember,
          /* [in] */ REFIID riid,
          /* [in] */ LCID lcid,
          /* [in] */ WORD wFlags,
          /* [out][in] */ DISPPARAMS *pDispParams,
          /* [out] */ VARIANT *pVarResult,
          /* [out] */ EXCEPINFO *pExcepInfo,
          /* [out] */ UINT *puArgErr);

      BEGIN_SINK_MAP(CDHtmlEventSink)
      END_SINK_MAP()

      CComPtr<IHTMLDocument2> spHTMLDocument;
    
      CDHtmlEventCallback* pCallback;
private:
};



class CSkinHtmlCtrl : 
    public SkinWindowImpl<CSkinHtmlCtrl, CAxWindowT<SkinWindow>, CControlWinTraits>,
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CSkinHtmlCtrl>,
    public IDispEventSimpleImpl<SKIN_HTMLCTRL_SINK_MAP_ID, CSkinHtmlCtrl, &DIID_DWebBrowserEvents2>
{
protected:
    CComPtr<IWebBrowser2> m_spWebBrowser;
    KSGUI::CString        m_strGoUrl;
    KSGUI::CString        m_strHtml;

    BOOL m_bAdvised;
    BOOL m_bLoadHtml;

    CDHtmlEventSink m_sink;

public:

    CSkinHtmlCtrl(void) :
        m_bAdvised(FALSE),
        m_bLoadHtml(FALSE)
    {
    }
    ~CSkinHtmlCtrl(void)
    {
    }

    virtual HWND Create(HWND hWndParent, 
        _U_RECT rect      = NULL, 
        LPCTSTR pszGoUrl  = NULL, 
        DWORD   dwStyle   = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 
        DWORD   dwStyleEx = WS_EX_TRANSPARENT,
        DWORD   dwId      = 0)
    {
        if (pszGoUrl != NULL)
            m_strGoUrl = pszGoUrl;

        return SkinWindowImpl<CSkinHtmlCtrl, CAxWindowT<SkinWindow>, CControlWinTraits>::
            Create(hWndParent, rect, NULL, dwStyle, dwStyleEx, dwId);
    }

    BOOL Navigate( LPCTSTR pszUrl )
    {
        HRESULT hr = E_FAIL;

        if ( pszUrl == NULL || _tcslen(pszUrl) <= 0 )
            return FALSE;

        if (!_CreateWebBrowser())
            return FALSE;

        hr = m_spWebBrowser->Navigate( _bstr_t(pszUrl) , NULL, NULL, NULL, NULL);

        return SUCCEEDED(hr);
    }

    BOOL LoadHtml( LPCTSTR pszHtml )
    {
        HRESULT hr = E_FAIL;

        if ( pszHtml == NULL || _tcslen(pszHtml) <= 0 )
            return FALSE;

        if (!_CreateWebBrowser())
            return FALSE;

		m_strHtml   = pszHtml;

		if (m_sink.spHTMLDocument != NULL)
		{
			m_bLoadHtml = TRUE;
			OnDocumentComplete( NULL, NULL );
		}
		else
		{
			m_bLoadHtml = TRUE;
			hr = m_spWebBrowser->Navigate(_T("about:blank"), NULL, NULL, NULL, NULL);
		}


        return SUCCEEDED(hr);
    }

    void SetCallback( CDHtmlEventCallback* piCallback )
    {
        m_sink.pCallback = piCallback;
    }

    BEGIN_MSG_MAP(CSkinHtmlCtrl)
        MESSAGE_HANDLER(WM_CREATE , OnCreate)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
    END_MSG_MAP()

    BOOL _CreateWebBrowser()
    {
        HRESULT hr = E_FAIL;

        if ( m_spWebBrowser == NULL )
        {
            WCHAR wszCLSID[50] = { 0 };

            StringFromGUID2(CLSID_WebBrowser, wszCLSID, sizeof(wszCLSID) / sizeof(wszCLSID[0])); 

            hr = CreateControlEx( wszCLSID );
            if (FAILED(hr))
                return FALSE;

            hr = QueryControl( &m_spWebBrowser);
            if (FAILED(hr))
                return FALSE;

            DispEventAdvise( m_spWebBrowser );
            m_bAdvised = TRUE;

            CComPtr<IAxWinAmbientDispatch> spWinAmbientDispatch;

            CComPtr<IAxWinHostWindow> spWinHost;
            hr = QueryHost(&spWinAmbientDispatch);

            if (SUCCEEDED(hr))
                spWinAmbientDispatch->put_AllowContextMenu(FALSE);
        }
        
        return TRUE;
    }


    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        if ( m_strGoUrl.GetLength() > 0 )
            Navigate( m_strGoUrl );

        return DefWindowProc();
    }

    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        if (m_bAdvised)
            DispEventUnadvise(m_spWebBrowser);

        if (m_sink.spHTMLDocument != NULL)
        {
            m_sink.DispEventUnadvise(m_sink.spHTMLDocument);
        }

        m_bAdvised = FALSE;

        return DefWindowProc();
    }

    BEGIN_SINK_MAP(CSkinHtmlCtrl)
        SINK_ENTRY_INFO(SKIN_HTMLCTRL_SINK_MAP_ID, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE, OnDocumentComplete, &DocumentCompleteFuncInfo)
    END_SINK_MAP()


    void _stdcall OnDocumentComplete(LPDISPATCH pDisp, VARIANT* URL)
    {
		if (m_sink.spHTMLDocument == NULL)
		{
            CComPtr<IDispatch> spDispatch;

            m_spWebBrowser->get_Document(&spDispatch);
			if (spDispatch)
				spDispatch->QueryInterface(&m_sink.spHTMLDocument);

            m_sink.DispEventAdvise(m_sink.spHTMLDocument);

		}

		if (m_bLoadHtml)
        {
            CStreamOnCString stream(m_strHtml);
            CComQIPtr<IPersistStreamInit> spPSI;

            if (m_sink.spHTMLDocument)
            {
                spPSI = m_sink.spHTMLDocument;
                if (spPSI)
                    spPSI->Load(static_cast<IStream*>(&stream));
            }
        }

        m_bLoadHtml = FALSE;
    }

public:

    HWND SkinCreate( 
        const SkinXmlElement& xmlElement,
        HWND hWndParent, _U_MENUorID MenuOrID = 0U ) throw()
    {

        skinxmlwin xmlWin(xmlElement);

        RECT rcClient = rcDefault;
        KSGUI::CString strCaption;

        DWORD dwStyle   = 0;
        DWORD dwExStyle = 0;
        DWORD dwId      = 0;

        GetRealRect(hWndParent, xmlElement, rcClient);

        xmlWin.GetCaption(strCaption);

        xmlWin.GetStyle(dwStyle);
        xmlWin.GetExStyle(dwExStyle);

        xmlWin.GetItemId(dwId);

        HWND hWndResult = Create( hWndParent, rcClient, strCaption, dwStyle, dwExStyle, dwId);

        return hWndResult;
    }


    static SkinWindow* SkinCreate_Static(
        const SkinXmlElement& xmlElement,
        HWND hWndParent, _U_MENUorID MenuOrID = 0U )
    {
        CSkinHtmlCtrl * pSkinWindow = new CSkinHtmlCtrl;

        if (pSkinWindow == NULL)
            return pSkinWindow;


        if (pSkinWindow->SkinCreate(xmlElement, 
            hWndParent, 
            MenuOrID) == NULL)
        {
            delete pSkinWindow;

            pSkinWindow = NULL;
        }

        return pSkinWindow;
    }

protected:

    
};



inline CStreamOnCString::CStreamOnCString(LPCTSTR szData):
        m_strStream(szData),
        m_current_index(0)
{
}

inline HRESULT(CStreamOnCString::QueryInterface)(REFIID iid, void **ppUnk)
    {
        if (ppUnk == NULL) 
        { 
            return E_POINTER; 
        }
        if (::InlineIsEqualGUID(iid, IID_IUnknown) ||
            ::InlineIsEqualGUID(iid, IID_IStream) ||
            ::InlineIsEqualGUID(iid, IID_ISequentialStream))
        {
            *ppUnk = (void*)this;
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }

inline ULONG STDMETHODCALLTYPE CStreamOnCString::AddRef( void) 
    {
        return (ULONG)1;
    }

inline ULONG STDMETHODCALLTYPE CStreamOnCString::Release( void) 
    {
        return (ULONG)1;
    }

inline HRESULT(CStreamOnCString::Read)(void *pv, ULONG cb, ULONG *pcbRead)
{
    if (pcbRead)
        *pcbRead = 0;
    if (m_strAnsi.GetLength() == 0)
        m_strAnsi = m_strStream;

    if (!pv)
        return E_POINTER;

    unsigned int length = m_strAnsi.GetLength();
    char *pStream = m_strAnsi.GetBuffer();
    if (!pStream)
        return E_UNEXPECTED;

    UINT_PTR uSum = (UINT_PTR)pStream + length;
    if (uSum < static_cast<UINT_PTR>(length))
        return E_UNEXPECTED;
    char *pEnd = reinterpret_cast<char*>(uSum);

    uSum = (UINT_PTR)pStream + m_current_index;
    if (uSum < m_current_index)
        return E_UNEXPECTED;
    char *pStart = reinterpret_cast<char*>(uSum);

    if (pStart >= pEnd)
        return S_FALSE; // no more data to read

    int bytes_left = (int)(pEnd-pStart);
    int bytes_to_copy = (int)__min(bytes_left, (int)cb);
    if (bytes_to_copy <= 0)
    {
        // reset members so this stream can be used again
        m_current_index = 0;
        m_strAnsi.Empty();
        return S_FALSE;
    }

    ::ATL::Checked::memcpy_s(pv, cb, (void*)uSum, bytes_to_copy);
    if (pcbRead)
        *pcbRead = (ULONG)bytes_to_copy;
    m_current_index += bytes_to_copy;
    m_strAnsi.ReleaseBuffer(0);
    return S_OK;
}

inline HRESULT(CStreamOnCString::Write)(const void *pv, ULONG cb, ULONG *pcbWritten)
{
    if (pcbWritten)
        *pcbWritten = 0;
    try
    {
        int flags = IS_TEXT_UNICODE_UNICODE_MASK;
        if (0 != IsTextUnicode((LPVOID)pv, cb, &flags))
        {
            // compiling UNICODE and got a UNICODE buffer
            UINT nCharsToSkip = flags & IS_TEXT_UNICODE_SIGNATURE ? 1 : 0;
            m_strStream.Append((wchar_t*)( ((wchar_t*)pv)+nCharsToSkip), (cb/sizeof(wchar_t))-nCharsToSkip);
            if (pcbWritten)
                *pcbWritten = cb-nCharsToSkip*sizeof(wchar_t);
        }
        else
        {
            // compiling UNICODE and got an ansi buffer
            // convert ansi buffer to UNICODE buffer
            CStringW strTemp((LPCSTR)pv, cb);
            m_strStream.Append(strTemp);
            if (pcbWritten)
                *pcbWritten = cb;

        }
    }
    catch (...)
    {
        return E_OUTOFMEMORY;
    }
    return S_OK;
}

inline HRESULT(CStreamOnCString::Seek)(LARGE_INTEGER , DWORD , ULARGE_INTEGER *)
{
    return E_NOTIMPL;
}

inline HRESULT(CStreamOnCString::SetSize)(ULARGE_INTEGER )
{
    return E_NOTIMPL;
}

inline HRESULT(CStreamOnCString::CopyTo)(IStream *, ULARGE_INTEGER , ULARGE_INTEGER *,
    ULARGE_INTEGER *)
{
    return E_NOTIMPL;
}

inline HRESULT(CStreamOnCString::Commit)(DWORD )
{
    return E_NOTIMPL;
}

inline HRESULT(CStreamOnCString::Revert)( void)
{
    return E_NOTIMPL;
}

inline HRESULT(CStreamOnCString::LockRegion)(ULARGE_INTEGER , ULARGE_INTEGER , DWORD )
{
    return E_NOTIMPL;
}

inline HRESULT(CStreamOnCString::UnlockRegion)(ULARGE_INTEGER , ULARGE_INTEGER ,
    DWORD )
{
    return E_NOTIMPL;
}

inline HRESULT(CStreamOnCString::Stat)(STATSTG *, DWORD )
{
    return E_NOTIMPL;
}

inline HRESULT(CStreamOnCString::Clone)(IStream **)
{
    return E_NOTIMPL;
}

inline HRESULT CDHtmlEventSink::GetDHtmlEventEntry(IHTMLElement **ppsrcElement)
{
    HRESULT hr = DISP_E_MEMBERNOTFOUND;

    CComPtr<IHTMLWindow2> sphtmlWnd;
    CComPtr<IHTMLEventObj> sphtmlEvent;
    CComPtr<IHTMLElement> sphtmlElement;
    CComBSTR bstrName;
    CComBSTR bstrClass;
    CComBSTR bstrTagName;

    if(ppsrcElement == NULL)
        return E_POINTER;

    *ppsrcElement = NULL;

    // get the element that generated the event
    spHTMLDocument->get_parentWindow(&sphtmlWnd);
    if ((sphtmlWnd==NULL) || FAILED(sphtmlWnd->get_event(&sphtmlEvent)) || (sphtmlEvent==NULL))
    {
        hr = DISP_E_MEMBERNOTFOUND;
        goto Error;
    }
    sphtmlEvent->get_srcElement(&sphtmlElement);
    *ppsrcElement = sphtmlElement;
    if (sphtmlElement)
        sphtmlElement.p->AddRef();

    return S_OK;

Error:
    if (*ppsrcElement)
    {
        (*ppsrcElement)->Release();
        *ppsrcElement = NULL;
    }

    return E_POINTER;
}

inline HRESULT __stdcall CDHtmlEventSink::Invoke( 
    /* [in] */ DISPID dispIdMember,
    /* [in] */ REFIID riid,
    /* [in] */ LCID lcid,
    /* [in] */ WORD wFlags,
    /* [out][in] */ DISPPARAMS *pDispParams,
    /* [out] */ VARIANT *pVarResult,
    /* [out] */ EXCEPINFO *pExcepInfo,
    /* [out] */ UINT *puArgErr)
{
    switch(dispIdMember)
    {
    case DISPID_HTMLELEMENTEVENTS_ONCLICK:
        {
            IHTMLElement *ppsrcElement = NULL;

            GetDHtmlEventEntry(&ppsrcElement);

            if (ppsrcElement != NULL)
            {
                CComBSTR bstrName;
                CComBSTR bstrClass;
                CComBSTR bstrTagName;
                CComBSTR bstrHtml;

                ppsrcElement->get_id(&bstrName);
                ppsrcElement->get_className(&bstrClass);
                ppsrcElement->get_tagName(&bstrTagName);
                ppsrcElement->get_innerHTML(&bstrHtml);

                if ( bstrName != NULL && _tcslen(bstrName) > 0 && pCallback != NULL )
                {
                    pCallback->OnClicked( bstrName );
                }

                ppsrcElement->Release();
            }
        }

        break;
    }

    return TRUE;
}


}; // namespace KSGUI
