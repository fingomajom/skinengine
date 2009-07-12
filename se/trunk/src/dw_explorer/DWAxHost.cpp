#include "stdafx.h"
#include "DWHtmlView.h"
#include "DWAxHost.h"


#define DWSTDMETHOD_DEF(method) HRESULT STDMETHODCALLTYPE method

//////////////////////////////////////////////////////////////////////////
// 
// CAxFrameWindow
// 
//////////////////////////////////////////////////////////////////////////

DWAxHost::CAxFrameWindow::CAxFrameWindow()
{
}

void DWAxHost::CAxFrameWindow::FinalRelease()
{
    m_spActiveObject.Release();
    if (m_hWnd)
        DestroyWindow();
}

// IOleWindow
DWSTDMETHOD_DEF(DWAxHost::CAxFrameWindow::GetWindow)(HWND* phwnd)
{
    ATLASSERT(phwnd != NULL);
    if (phwnd == NULL)
        return E_POINTER;

    //if (m_hWnd == NULL)
    //	Create(NULL, NULL, _T("AXWIN Frame Window"), WS_OVERLAPPEDWINDOW, 0, (UINT)NULL);
    *phwnd = NULL;
    return S_OK;
}

DWSTDMETHOD_DEF(DWAxHost::CAxFrameWindow::ContextSensitiveHelp)(BOOL /*fEnterMode*/)
{
    return S_OK;
}

// IOleInPlaceUIWindow
DWSTDMETHOD_DEF(DWAxHost::CAxFrameWindow::GetBorder)(LPRECT /*lprectBorder*/)
{
    return S_OK;
}

DWSTDMETHOD_DEF(DWAxHost::CAxFrameWindow::RequestBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
{
    return INPLACE_E_NOTOOLSPACE;
}

DWSTDMETHOD_DEF(DWAxHost::CAxFrameWindow::SetBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
{
    return S_OK;
}

DWSTDMETHOD_DEF(DWAxHost::CAxFrameWindow::SetActiveObject)(IOleInPlaceActiveObject* pActiveObject, LPCOLESTR /*pszObjName*/)
{
    m_spActiveObject = pActiveObject;
    return S_OK;
}

// IOleInPlaceFrameWindow
DWSTDMETHOD_DEF(DWAxHost::CAxFrameWindow::InsertMenus)(HMENU /*hmenuShared*/, LPOLEMENUGROUPWIDTHS /*lpMenuWidths*/)
{
    return S_OK;
}

DWSTDMETHOD_DEF(DWAxHost::CAxFrameWindow::SetMenu)(HMENU /*hmenuShared*/, HOLEMENU /*holemenu*/, HWND /*hwndActiveObject*/)
{
    return S_OK;
}

DWSTDMETHOD_DEF(DWAxHost::CAxFrameWindow::RemoveMenus)(HMENU /*hmenuShared*/)
{
    return S_OK;
}

DWSTDMETHOD_DEF(DWAxHost::CAxFrameWindow::SetStatusText)(LPCOLESTR /*pszStatusText*/)
{
    return S_OK;
}

DWSTDMETHOD_DEF(DWAxHost::CAxFrameWindow::EnableModeless)(BOOL /*fEnable*/)
{
    return S_OK;
}

DWSTDMETHOD_DEF(DWAxHost::CAxFrameWindow::TranslateAccelerator)(LPMSG /*lpMsg*/, WORD /*wID*/)
{
    return S_FALSE;
}


//////////////////////////////////////////////////////////////////////////
// 
// CAxUIWindow
// 
//////////////////////////////////////////////////////////////////////////


DWAxHost::CAxUIWindow::CAxUIWindow()
{
}

void DWAxHost::CAxUIWindow::FinalRelease()
{
    m_spActiveObject.Release();
    if (m_hWnd)
        DestroyWindow();
}

// IOleWindow
DWSTDMETHOD_DEF(DWAxHost::CAxUIWindow::GetWindow)(HWND* phwnd)
{
    ATLENSURE_RETURN(phwnd);
    //if (m_hWnd == NULL)
    Create(NULL, NULL, _T("AXWIN UI Window"), WS_OVERLAPPEDWINDOW, 0, (UINT)NULL);
    *phwnd = m_hWnd;
    return S_OK;
}

DWSTDMETHOD_DEF(DWAxHost::CAxUIWindow::ContextSensitiveHelp)(BOOL /*fEnterMode*/)
{
    return S_OK;
}

// IOleInPlaceUIWindow
DWSTDMETHOD_DEF(DWAxHost::CAxUIWindow::GetBorder)(LPRECT /*lprectBorder*/)
{
    return S_OK;
}

DWSTDMETHOD_DEF(DWAxHost::CAxUIWindow::RequestBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
{
    return INPLACE_E_NOTOOLSPACE;
}

DWSTDMETHOD_DEF(DWAxHost::CAxUIWindow::SetBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
{
    return S_OK;
}

DWSTDMETHOD_DEF(DWAxHost::CAxUIWindow::SetActiveObject)(IOleInPlaceActiveObject* pActiveObject, LPCOLESTR /*pszObjName*/)
{
    m_spActiveObject = pActiveObject;
    return S_OK;
}


//////////////////////////////////////////////////////////////////////////
// 
// CDWAxHost
// 
//////////////////////////////////////////////////////////////////////////


DWAxHost::CDWAxHost::CDWAxHost()
{
    m_bInPlaceActive = FALSE;
    m_bUIActive = FALSE;
    m_bMDIApp = FALSE;
    m_bWindowless = FALSE;
    m_bCapture = FALSE;
    m_bHaveFocus = FALSE;

    m_bReleaseAll = FALSE;
    m_bSubclassed = FALSE;

    m_dwAdviseSink = 0xCDCDCDCD;
    m_dwDocHostFlags = DOCHOSTUIFLAG_NO3DBORDER;
    m_dwDocHostDoubleClickFlags = DOCHOSTUIDBLCLK_DEFAULT;

    m_hAccel = NULL;
    m_iidSink = IID_NULL;

    m_pHtmlView = NULL;
}

DWAxHost::CDWAxHost::~CDWAxHost()
{
}

void DWAxHost::CDWAxHost::FinalRelease()
{
    ReleaseAll();
}

void DWAxHost::CDWAxHost::OnFinalMessage(HWND /*hWnd*/)
{
    GetControllingUnknown()->Release();
}

CWndClassInfo& DWAxHost::CDWAxHost::GetWndClassInfo()
{
    static CWndClassInfo wc =
    {
        { sizeof(WNDCLASSEX), 0, StartWindowProc,
        0, 0, 0, 0, 0, (HBRUSH)(COLOR_WINDOW + 1), 0, _T(ATLAXWIN_CLASS), 0 },
        NULL, NULL, IDC_ARROW, TRUE, 0, _T("")
    };
    return wc;
}

LRESULT DWAxHost::CDWAxHost::OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
    ATLASSERT(lParam != 0);
    LPMSG lpMsg = (LPMSG)lParam;
    CComQIPtr<IOleInPlaceActiveObject, &__uuidof(IOleInPlaceActiveObject)> spInPlaceActiveObject(m_spUnknown);
    if(spInPlaceActiveObject)
    {
        if(spInPlaceActiveObject->TranslateAccelerator(lpMsg) == S_OK)
            return 1;
    }
    return 0;
}

void DWAxHost::CDWAxHost::ReleaseAll()
{
    if (m_bReleaseAll)
        return;
    m_bReleaseAll = TRUE;

    if (m_spViewObject != NULL)
        m_spViewObject->SetAdvise(DVASPECT_CONTENT, 0, NULL);

    if(m_dwAdviseSink != 0xCDCDCDCD)
    {
        AtlUnadvise(m_spUnknown, m_iidSink, m_dwAdviseSink);
        m_dwAdviseSink = 0xCDCDCDCD;
    }

    if (m_spOleObject)
    {
        m_spOleObject->Unadvise(m_dwOleObject);
        m_spOleObject->Close(OLECLOSE_NOSAVE);
        m_spOleObject->SetClientSite(NULL);
    }

    if (m_spUnknown != NULL)
    {
        CComPtr<IObjectWithSite> spSite;
        m_spUnknown->QueryInterface(__uuidof(IObjectWithSite), (void**)&spSite);
        if (spSite != NULL)
            spSite->SetSite(NULL);
    }

    m_spViewObject.Release();
    m_dwViewObjectType = 0;

    m_spInPlaceObjectWindowless.Release();
    m_spOleObject.Release();
    m_spUnknown.Release();

    m_spInPlaceUIWindow.Release();
    m_spInPlaceFrame.Release();

    m_bInPlaceActive = FALSE;
    m_bWindowless = FALSE;
    m_bInPlaceActive = FALSE;
    m_bUIActive = FALSE;
    m_bCapture = FALSE;
    m_bReleaseAll = FALSE;

    if (m_hAccel != NULL)
    {
        DestroyAcceleratorTable(m_hAccel);
        m_hAccel = NULL;
    }
}


// window message handlers
LRESULT DWAxHost::CDWAxHost::OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    if (m_spViewObject == NULL)
        bHandled = false;

    return 1;
}

LRESULT DWAxHost::CDWAxHost::OnMouseActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    bHandled = FALSE;
    if (m_dwMiscStatus & OLEMISC_NOUIACTIVATE)
    {
        if (m_spOleObject != NULL && !m_bInPlaceActive)
        {
            CComPtr<IOleClientSite> spClientSite;
            GetControllingUnknown()->QueryInterface(__uuidof(IOleClientSite), (void**)&spClientSite);
            if (spClientSite != NULL)
                m_spOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, spClientSite, 0, m_hWnd, &m_rcPos);
        }
    }
    else
    {
        BOOL b;
        OnSetFocus(0, 0, 0, b);
    }
    return 0;
}
LRESULT DWAxHost::CDWAxHost::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    m_bHaveFocus = TRUE;
    bHandled = FALSE;
    return 0;
}
LRESULT DWAxHost::CDWAxHost::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    m_bHaveFocus = FALSE;
    bHandled = FALSE;
    return 0;
}
LRESULT DWAxHost::CDWAxHost::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
    int nWidth = GET_X_LPARAM(lParam);  // width of client area
    int nHeight = GET_Y_LPARAM(lParam); // height of client area

    m_rcPos.right = m_rcPos.left + nWidth;
    m_rcPos.bottom = m_rcPos.top + nHeight;
    m_pxSize.cx = m_rcPos.right - m_rcPos.left;
    m_pxSize.cy = m_rcPos.bottom - m_rcPos.top;
    AtlPixelToHiMetric(&m_pxSize, &m_hmSize);

    if (m_spOleObject)
        m_spOleObject->SetExtent(DVASPECT_CONTENT, &m_hmSize);
    if (m_spInPlaceObjectWindowless)
        m_spInPlaceObjectWindowless->SetObjectRects(&m_rcPos, &m_rcPos);
    if (m_bWindowless)
        InvalidateRect(NULL, TRUE);

    bHandled = FALSE;
    return 0;
}
LRESULT DWAxHost::CDWAxHost::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    GetControllingUnknown()->AddRef();
    DefWindowProc(uMsg, wParam, lParam);
    ReleaseAll();
    bHandled = FALSE;
    return 0;
}
LRESULT DWAxHost::CDWAxHost::OnWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    LRESULT lRes = 0;
    HRESULT hr = S_FALSE;
    if (m_bInPlaceActive && m_bWindowless && m_spInPlaceObjectWindowless)
        hr = m_spInPlaceObjectWindowless->OnWindowMessage(uMsg, wParam, lParam, &lRes);
    if (hr == S_FALSE)
        bHandled = FALSE;
    return lRes;
}
LRESULT DWAxHost::CDWAxHost::OnWindowlessMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    LRESULT lRes = 0;
    if (m_bInPlaceActive && m_bWindowless && m_spInPlaceObjectWindowless)
        m_spInPlaceObjectWindowless->OnWindowMessage(uMsg, wParam, lParam, &lRes);
    bHandled = FALSE;
    return lRes;
}
LRESULT DWAxHost::CDWAxHost::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    if (m_spViewObject == NULL)
        return 0;

    bHandled = FALSE;
    return 0;
}


DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::QueryControl)(REFIID riid, void** ppvObject)
{
    HRESULT hr = E_POINTER;
    if (ppvObject)
    {
        if (m_spUnknown)
            hr = m_spUnknown->QueryInterface(riid, ppvObject);
        else
        {
            *ppvObject = NULL;
            hr = OLE_E_NOCONNECTION;
        }
    }
    return hr;
}

DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::CreateControl)(HWND hWnd)
{
    CComPtr<IUnknown> p;

    ReleaseAll();

    ATLASSERT(m_hWnd == NULL);
    ATLASSERT(::IsWindow(hWnd));
    SubclassWindow(hWnd);

    HRESULT hr = CoCreateInstance(CLSID_WebBrowser, NULL, CLSCTX_INPROC_SERVER, __uuidof(IUnknown), (void**)&m_spUnknown);
    for (int i = 1; i < 10; ++i)
    {
        if (FAILED(hr) || !m_spUnknown)
            hr = CoCreateInstance(CLSID_WebBrowser, NULL, CLSCTX_INPROC_SERVER, __uuidof(IUnknown), (void**)&m_spUnknown);
        else
            break ;
        ATLASSERT(SUCCEEDED(hr));
        ATLASSERT(m_spUnknown);
        Sleep(i * 500) ;
    }

    m_spUnknown->QueryInterface(__uuidof(IOleObject), (void**)&m_spOleObject);
    if (m_spOleObject)
    {
        m_spOleObject->GetMiscStatus(DVASPECT_CONTENT, &m_dwMiscStatus);
        if (m_dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST)
        {
            CComQIPtr<IOleClientSite> spClientSite(GetControllingUnknown());
            m_spOleObject->SetClientSite(spClientSite);
        }

        CComQIPtr<IPersistStreamInit> spPSI(m_spOleObject);
        if (spPSI)
            hr = spPSI->InitNew();

        if (FAILED(hr)) // If the initialization of the control failed...
        {
            // Clean up and return
            if (m_dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST)
                m_spOleObject->SetClientSite(NULL);

            m_dwMiscStatus = 0;
            m_spOleObject.Release();
            m_spUnknown.Release();

            return hr;
        }

        if (0 == (m_dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST))
        {
            CComQIPtr<IOleClientSite> spClientSite(GetControllingUnknown());
            m_spOleObject->SetClientSite(spClientSite);
        }

        m_dwViewObjectType = 0;
        hr = m_spOleObject->QueryInterface(__uuidof(IViewObjectEx), (void**) &m_spViewObject);
        if (FAILED(hr))
        {
            hr = m_spOleObject->QueryInterface(__uuidof(IViewObject2), (void**) &m_spViewObject);
            if (SUCCEEDED(hr))
                m_dwViewObjectType = 3;
        } else
            m_dwViewObjectType = 7;

        if (FAILED(hr))
        {
            hr = m_spOleObject->QueryInterface(__uuidof(IViewObject), (void**) &m_spViewObject);
            if (SUCCEEDED(hr))
                m_dwViewObjectType = 1;
        }
        CComQIPtr<IAdviseSink> spAdviseSink(GetControllingUnknown());
        m_spOleObject->Advise(spAdviseSink, &m_dwOleObject);
        if (m_spViewObject)
            m_spViewObject->SetAdvise(DVASPECT_CONTENT, 0, spAdviseSink);
        m_spOleObject->SetHostNames(OLESTR("AXWIN"), NULL);

        if ((m_dwMiscStatus & OLEMISC_INVISIBLEATRUNTIME) == 0)
        {
            GetClientRect(&m_rcPos);
            m_pxSize.cx = m_rcPos.right - m_rcPos.left;
            m_pxSize.cy = m_rcPos.bottom - m_rcPos.top;
            AtlPixelToHiMetric(&m_pxSize, &m_hmSize);
            m_spOleObject->SetExtent(DVASPECT_CONTENT, &m_hmSize);
            m_spOleObject->GetExtent(DVASPECT_CONTENT, &m_hmSize);
            AtlHiMetricToPixel(&m_hmSize, &m_pxSize);
            m_rcPos.right = m_rcPos.left + m_pxSize.cx;
            m_rcPos.bottom = m_rcPos.top + m_pxSize.cy;

            CComQIPtr<IOleClientSite> spClientSite(GetControllingUnknown());
            hr = m_spOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, spClientSite, 0, m_hWnd, &m_rcPos);
            RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_FRAME);
        }
    }
    CComPtr<IObjectWithSite> spSite;
    m_spUnknown->QueryInterface(__uuidof(IObjectWithSite), (void**)&spSite);
    if (spSite != NULL)
        spSite->SetSite(GetControllingUnknown());

    return hr;
}


DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams,
                  VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
    //HRESULT hr = S_OK;
   
    //CComQIPtr<IOleControl, &__uuidof(IOleControl)> spOleControl(m_spUnknown);
    //if (spOleControl != NULL)
    //    spOleControl->OnAmbientPropertyChange(dispIdMember);
    
    CDWHtmlView *pView = GetView();
    ATLASSERT( pView != NULL );
    if ( pView == NULL )
        return S_OK ;
    
    return pView->OnAmbientProperty( this, dispIdMember, pVarResult );
}

// IObjectWithSite
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::SetSite)(IUnknown* pUnkSite)
{
    HRESULT hr = IObjectWithSiteImpl<CDWAxHost>::SetSite(pUnkSite);

    if (SUCCEEDED(hr) && m_spUnkSite)
    {
        // Look for "outer" IServiceProvider
        hr = m_spUnkSite->QueryInterface(__uuidof(IServiceProvider), (void**)&m_spServices);
        ATLASSERT( SUCCEEDED(hr) && "No ServiceProvider!" );
    }

    if (pUnkSite == NULL)
        m_spServices.Release();

    return hr;
}

// IOleClientSite
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::SaveObject)()
{
    ATLTRACENOTIMPL(_T("IOleClientSite::SaveObject"));
}
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::GetMoniker)(DWORD /*dwAssign*/, DWORD /*dwWhichMoniker*/, IMoniker** /*ppmk*/)
{
    ATLTRACENOTIMPL(_T("IOleClientSite::GetMoniker"));
}
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::GetContainer)(IOleContainer** ppContainer)
{
    ATLTRACE(atlTraceHosting, 2, _T("IOleClientSite::GetContainer\n"));
    ATLASSERT(ppContainer != NULL);

    HRESULT hr = E_POINTER;
    if (ppContainer)
    {
        hr = E_NOTIMPL;
        (*ppContainer) = NULL;
        if (m_spUnkSite)
            hr = m_spUnkSite->QueryInterface(__uuidof(IOleContainer), (void**)ppContainer);
        if (FAILED(hr))
            hr = QueryInterface(__uuidof(IOleContainer), (void**)ppContainer);
    }
    return hr;
}
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::ShowObject)()
{
    ATLTRACE(atlTraceHosting, 2, _T("IOleClientSite::ShowObject\r\n"));

    HDC hdc = CWindowImpl<CDWAxHost>::GetDC();
    if (hdc == NULL)
        return E_FAIL;
    if (m_spViewObject)
        m_spViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, hdc, (RECTL*)&m_rcPos, (RECTL*)&m_rcPos, NULL, NULL); 
    CWindowImpl<CDWAxHost>::ReleaseDC(hdc);
    return S_OK;
}
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::OnShowWindow)(BOOL /*fShow*/)
{
    ATLTRACENOTIMPL(_T("IOleClientSite::OnShowWindow"));
}
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::RequestNewObjectLayout)()
{
    ATLTRACENOTIMPL(_T("IOleClientSite::RequestNewObjectLayout"));
}

// IOleInPlaceSite
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::GetWindow)(HWND* phwnd)
{
    ATLENSURE_RETURN(phwnd);
    *phwnd = m_hWnd;
    return S_OK;
}
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::ContextSensitiveHelp)(BOOL /*fEnterMode*/)
{
    ATLTRACENOTIMPL(_T("IOleInPlaceSite::ContextSensitiveHelp"));
}
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::CanInPlaceActivate)()
{
    return S_OK;
}
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::OnInPlaceActivate)()
{
    // should only be called once the first time control is inplace-activated
    ATLASSUME(m_bInPlaceActive == FALSE);
    ATLASSUME(m_spInPlaceObjectWindowless == NULL);

    m_bInPlaceActive = TRUE;
    OleLockRunning(m_spOleObject, TRUE, FALSE);
    m_bWindowless = FALSE;
    m_spOleObject->QueryInterface(__uuidof(IOleInPlaceObject), (void**) &m_spInPlaceObjectWindowless);
    return S_OK;
}
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::OnUIActivate)()
{
    ATLTRACE(atlTraceHosting, 2, _T("IOleInPlaceSite::OnUIActivate\n"));
    m_bUIActive = TRUE;
    return S_OK;
}
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::GetWindowContext)(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow** ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO pFrameInfo)
{
    if (ppFrame != NULL)
        *ppFrame = NULL;
    if (ppDoc != NULL)
        *ppDoc = NULL;
    if (ppFrame == NULL || ppDoc == NULL || lprcPosRect == NULL || lprcClipRect == NULL)
    {
        ATLASSERT(false);
        return E_POINTER;
    }

    if (!m_spInPlaceFrame)
    {
        CComObject<CAxFrameWindow>* pFrameWindow;
        HRESULT hRet = CComObject<CAxFrameWindow>::CreateInstance(&pFrameWindow);

        if(FAILED(hRet))
        {
            return hRet;
        }
        pFrameWindow->QueryInterface(__uuidof(IOleInPlaceFrame), (void**) &m_spInPlaceFrame);
        ATLASSUME(m_spInPlaceFrame);
    }
    if (!m_spInPlaceUIWindow)
    {
        CComObject<CAxUIWindow>* pUIWindow;
        HRESULT hRet = CComObject<CAxUIWindow>::CreateInstance(&pUIWindow);

        if(FAILED(hRet))
        {
            return hRet;
        }
        pUIWindow->QueryInterface(__uuidof(IOleInPlaceUIWindow), (void**) &m_spInPlaceUIWindow);
        ATLASSUME(m_spInPlaceUIWindow);
    }
    HRESULT hr=S_OK;
    hr=m_spInPlaceFrame.CopyTo(ppFrame);
    if(FAILED(hr))
    {
        return hr;
    }
    hr=m_spInPlaceUIWindow.CopyTo(ppDoc);
    if(FAILED(hr))
    {
        return hr;
    }
    GetClientRect(lprcPosRect);
    GetClientRect(lprcClipRect);

    if (m_hAccel == NULL)
    {
        ACCEL ac = { 0,0,0 };
        m_hAccel = CreateAcceleratorTable(&ac, 1);
    }
    pFrameInfo->cb = sizeof(OLEINPLACEFRAMEINFO);
    pFrameInfo->fMDIApp = m_bMDIApp;
    pFrameInfo->hwndFrame = GetParent();
    pFrameInfo->haccel = m_hAccel;
    pFrameInfo->cAccelEntries = (m_hAccel != NULL) ? 1 : 0;

    return hr;
}
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::Scroll)(SIZE /*scrollExtant*/)
{
    ATLTRACENOTIMPL(_T("IOleInPlaceSite::Scroll"));
}
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::OnUIDeactivate)(BOOL /*fUndoable*/)
{
    ATLTRACE(atlTraceHosting, 2, _T("IOleInPlaceSite::OnUIDeactivate\n"));
    m_bUIActive = FALSE;
    return S_OK;
}
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::OnInPlaceDeactivate)()
{
    m_bInPlaceActive = FALSE;
    m_spInPlaceObjectWindowless.Release();
    return S_OK;
}
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::DiscardUndoState)()
{
    ATLTRACENOTIMPL(_T("IOleInPlaceSite::DiscardUndoState"));
}
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::DeactivateAndUndo)()
{
    ATLTRACENOTIMPL(_T("IOleInPlaceSite::DeactivateAndUndo"));
}
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::OnPosRectChange)(LPCRECT lprcPosRect)
{		
    ATLTRACE2(atlTraceHosting, 0, 	_T("IOleInPlaceSite::OnPosRectChange"));
    if (lprcPosRect==NULL) { return E_POINTER; }

    // Use MoveWindow() to resize the CTuoAxHostWindow.
    // The CTuoAxHostWindow handler for OnSize() will
    // take care of calling IOleInPlaceObject::SetObjectRects().

    // Convert to parent window coordinates for MoveWindow().
    RECT rect = *lprcPosRect;
    ClientToScreen( &rect );
    HWND hWnd = GetParent();

    // Check to make sure it's a non-top-level window.
    if(hWnd != NULL)
    {
        CWindow wndParent(hWnd);
        wndParent.ScreenToClient(&rect);
        wndParent.Detach ();
    }
    // Do the actual move.
    MoveWindow( &rect);

    return S_OK;	
}

// IOleControlSite
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::OnControlInfoChanged)()
{
    return S_OK;
}
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::LockInPlaceActive)(BOOL /*fLock*/)
{
    return S_OK;
}
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::GetExtendedControl)(IDispatch** ppDisp)
{
    if (ppDisp == NULL)
        return E_POINTER;
    return m_spOleObject.QueryInterface(ppDisp);
}
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::TransformCoords)(POINTL* /*pPtlHimetric*/, POINTF* /*pPtfContainer*/, DWORD /*dwFlags*/)
{
    ATLTRACENOTIMPL(_T("CTuoAxHostWindow::TransformCoords"));
}
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::TranslateAccelerator)(LPMSG /*lpMsg*/, DWORD /*grfModifiers*/)
{
    return S_FALSE;
}
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::OnFocus)(BOOL fGotFocus)
{
    m_bHaveFocus = fGotFocus;
    return S_OK;
}
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::ShowPropertyFrame)()
{
    ATLTRACENOTIMPL(_T("CTuoAxHostWindow::ShowPropertyFrame"));
}

// IAdviseSink
void DWAxHost::CDWAxHost::OnDataChange(FORMATETC* /*pFormatetc*/, STGMEDIUM* /*pStgmed*/)
{
}
void DWAxHost::CDWAxHost::OnViewChange(DWORD /*dwAspect*/, LONG /*lindex*/)
{
}
void DWAxHost::CDWAxHost::OnRename(IMoniker* /*pmk*/)
{
}
void DWAxHost::CDWAxHost::OnSave()
{
}
void DWAxHost::CDWAxHost::OnClose()
{
}

// IOleContainer
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::ParseDisplayName)(IBindCtx* /*pbc*/, LPOLESTR /*pszDisplayName*/, ULONG* /*pchEaten*/, IMoniker** /*ppmkOut*/)
{
    ATLTRACENOTIMPL(_T("CTuoAxHostWindow::ParseDisplayName"));
}
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::EnumObjects)(DWORD /*grfFlags*/, IEnumUnknown** ppenum)
{
    if (ppenum == NULL)
        return E_POINTER;
    *ppenum = NULL;
    typedef CComObject<CComEnum<IEnumUnknown, &__uuidof(IEnumUnknown), IUnknown*, _CopyInterface<IUnknown> > > enumunk;
    enumunk* p = NULL;
#pragma warning(push)
#pragma warning(disable: 6014)
    /* prefast noise VSW 489981 */
    ATLTRY(p = new enumunk);
#pragma warning(pop)
    if(p == NULL)
        return E_OUTOFMEMORY;
    IUnknown* pTemp = m_spUnknown;
    // There is always only one object.
    HRESULT hRes = p->Init(reinterpret_cast<IUnknown**>(&pTemp), reinterpret_cast<IUnknown**>(&pTemp + 1), GetControllingUnknown(), AtlFlagCopy);
    if (SUCCEEDED(hRes))
        hRes = p->QueryInterface(__uuidof(IEnumUnknown), (void**)ppenum);
    if (FAILED(hRes))
        delete p;
    return hRes;
}
DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::LockContainer)(BOOL fLock)
{
    m_bLocked = fLock;
    return S_OK;
}
void DWAxHost::CDWAxHost::SubclassWindow(HWND hWnd)
{
    m_bSubclassed = CWindowImpl<CDWAxHost>::SubclassWindow(hWnd);
}

void DWAxHost::CDWAxHost::ReleaseWindow()
{
    if (m_bSubclassed)
    {
        if(UnsubclassWindow(TRUE) != NULL)
            m_bSubclassed = FALSE;
    }
    else
        DestroyWindow();
}

// Reflection
LRESULT DWAxHost::CDWAxHost::ReflectNotifications(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    HWND hWndChild = NULL;

    switch(uMsg)
    {
    case WM_COMMAND:
        if(lParam != NULL)	// not from a menu
            hWndChild = (HWND)lParam;
        break;
    case WM_NOTIFY:
        hWndChild = ((LPNMHDR)lParam)->hwndFrom;
        break;
    case WM_PARENTNOTIFY:
        DefWindowProc();
        switch(LOWORD(wParam))
        {
        case WM_CREATE:
        case WM_DESTROY:
            hWndChild = (HWND)lParam;
            break;
        default:
            hWndChild = GetDlgItem(HIWORD(wParam));
            break;
        }
        break;
    case WM_DRAWITEM:
        {
            DRAWITEMSTRUCT* pdis = ((LPDRAWITEMSTRUCT)lParam);
            if (pdis->CtlType != ODT_MENU)	// not from a menu
                hWndChild = pdis->hwndItem;
            else							// Status bar control sends this message with type set to ODT_MENU
                if (::IsWindow(pdis->hwndItem))
                    hWndChild = pdis->hwndItem;
        }
        break;
    case WM_MEASUREITEM:
        {
            MEASUREITEMSTRUCT* pmis = ((LPMEASUREITEMSTRUCT)lParam);
            if(pmis->CtlType != ODT_MENU)	// not from a menu
                hWndChild = GetDlgItem(pmis->CtlID);
        }
        break;
    case WM_COMPAREITEM:
        // Sent only by combo or list box
        hWndChild =((LPCOMPAREITEMSTRUCT)lParam)->hwndItem;
        break;
    case WM_DELETEITEM:
        // Sent only by combo or list box  
        hWndChild = ((LPDELETEITEMSTRUCT)lParam)->hwndItem; 
        break;
    case WM_VKEYTOITEM:
    case WM_CHARTOITEM:
    case WM_HSCROLL:
    case WM_VSCROLL:
        hWndChild = (HWND)lParam;
        break;
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORMSGBOX:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
        hWndChild = (HWND)lParam;
        break;
    default:
        break;
    }

    if(hWndChild == NULL)
    {
        bHandled = FALSE;
        return 1;
    }

    if (m_bWindowless)
    {
        LRESULT lRes = 0;
        if (m_bInPlaceActive && m_spInPlaceObjectWindowless)
            m_spInPlaceObjectWindowless->OnWindowMessage(OCM__BASE + uMsg, wParam, lParam, &lRes);
        return lRes;
    }

    ATLASSERT(::IsWindow(hWndChild));
    return ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
}

CDWHtmlView* DWAxHost::CDWAxHost::GetView() const
{
    return m_pHtmlView;
}

DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::QueryService)( REFGUID rsid, REFIID riid, void** ppvObj) 
{
    CDWHtmlView *pView = GetView();
    ATLASSERT( pView != NULL );
    if ( pView == NULL )
        return E_NOTIMPL;

    return pView->OnQueryService(rsid, riid, ppvObj);
}


DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::Exec)(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT *pvaIn, VARIANT *pvaOut)
{
    CDWHtmlView *pView = GetView();
    ATLASSERT( pView != NULL );
    if ( pView == NULL )
        return E_NOTIMPL;

    return pView->OnExec(pguidCmdGroup, nCmdID, nCmdexecopt, pvaIn, pvaOut);
}

DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::QueryStatus)(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD* prgCmds, OLECMDTEXT *pCmdText)
{
    CDWHtmlView *pView = GetView();
    ATLASSERT( pView != NULL );
    if ( pView == NULL )
        return E_NOTIMPL;

    return pView->OnQueryStatus(pguidCmdGroup, cCmds, prgCmds, pCmdText);
}


DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::ShowContextMenu)(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved)
{
    CDWHtmlView *pView = GetView();
    ATLASSERT( pView != NULL );
    if ( pView == NULL )
        return E_NOTIMPL ;

    return pView->OnShowContextMenu( dwID, ppt, pcmdtReserved, pdispReserved );
}

DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::GetHostInfo)(DOCHOSTUIINFO *pInfo)
{
    CDWHtmlView *pView = GetView();
    ATLASSERT( pView != NULL );
    if ( pView == NULL )
        return E_NOTIMPL ;

    return pView->OnGetHostInfo( pInfo );
}

DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::ShowUI)(DWORD dwID, 
                  IOleInPlaceActiveObject *pActiveObject, 
                  IOleCommandTarget *pCommandTarget, 
                  IOleInPlaceFrame *pFrame, 
                  IOleInPlaceUIWindow *pDoc)
{
    CDWHtmlView *pView = GetView();
    ATLASSERT( pView != NULL );
    if ( pView == NULL )
        return S_OK ;

    return pView->OnShowUI( dwID, pActiveObject, pCommandTarget, pFrame, pDoc);
}

DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::HideUI)(void)
{
    CDWHtmlView *pView = GetView();
    ATLASSERT( pView != NULL );
    if ( pView == NULL )
        return S_OK ;

    return pView->OnHideUI( );
}

DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::UpdateUI)(void)
{
    CDWHtmlView *pView = GetView();
    ATLASSERT( pView != NULL );
    if ( pView == NULL )
        return S_OK ;

    return pView->OnUpdateUI();
}

DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::EnableModeless)(BOOL fEnable)
{
    CDWHtmlView *pView = GetView();
    ATLASSERT( pView != NULL );
    if ( pView == NULL )
        return E_NOTIMPL ;

    return pView->OnEnableModeless( fEnable );
}

DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::OnDocWindowActivate)(BOOL fActivate) 
{
    CDWHtmlView *pView = GetView();
    ATLASSERT( pView != NULL );
    if ( pView == NULL )
        return E_NOTIMPL ;

    return pView->OnDocWindowActivate( fActivate );
}

DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::OnFrameWindowActivate)(BOOL fActivate)
{
    CDWHtmlView *pView = GetView();
    ATLASSERT( pView != NULL );
    if ( pView == NULL )
        return E_NOTIMPL ;

    return pView->OnFrameWindowActivate( fActivate );
}

DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::ResizeBorder)(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow)
{
    CDWHtmlView *pView = GetView();
    ATLASSERT( pView != NULL );
    if ( pView == NULL )
        return S_OK ;

    return pView->OnResizeBorder( prcBorder, pUIWindow, fRameWindow );
}

DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::TranslateAccelerator)(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID)
{
    CDWHtmlView *pView = GetView();
    ATLASSERT( pView != NULL );
    if ( pView == NULL )
        return S_FALSE ;

    return pView->OnTranslateAccelerator( lpMsg, pguidCmdGroup, nCmdID );
}

DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::GetOptionKeyPath)(LPOLESTR *pchKey, DWORD dw)
{
    CDWHtmlView *pView = GetView();
    ATLASSERT( pView != NULL );
    if ( pView == NULL )
        return E_NOTIMPL ;

    return pView->OnGetOptionKeyPath( pchKey, dw );
}

DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::GetDropTarget)(IDropTarget *pDropTarget, IDropTarget **ppDropTarget)
{
    CDWHtmlView *pView = GetView();
    ATLASSERT( pView != NULL );
    if ( pView == NULL )
        return S_OK ;

    return pView->OnGetDropTarget( pDropTarget, ppDropTarget );
}

DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::GetExternal)(IDispatch **ppDispatch)
{
    CDWHtmlView *pView = GetView();
    ATLASSERT( pView != NULL );
    if ( pView == NULL )
        return E_NOTIMPL ;

    return pView->OnGetExternal( ppDispatch );
}

DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::TranslateUrl)(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut)
{
    CDWHtmlView *pView = GetView();
    ATLASSERT( pView != NULL );
    if ( pView == NULL )
        return S_OK ;

    return pView->OnTranslateUrl( dwTranslate, pchURLIn, ppchURLOut );
}

DWSTDMETHOD_DEF(DWAxHost::CDWAxHost::FilterDataObject)(IDataObject *pDO, IDataObject **ppDORet) 
{
    CDWHtmlView *pView = GetView();
    ATLASSERT( pView != NULL );
    if ( pView == NULL )
        return E_NOTIMPL ;

    return pView->OnFilterDataObject( pDO, ppDORet );
}
