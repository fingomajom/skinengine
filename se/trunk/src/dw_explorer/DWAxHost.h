#pragma once

#ifndef _ATL_NO_PRAGMA_WARNINGS
#pragma warning (push)
#pragma warning(disable: 4702) // unreachable code
#pragma warning(disable: 4127) // conditional expression constant
#pragma warning(disable: 4571) //catch(...) blocks compiled with /EHs do NOT catch or re-throw Structured Exceptions
#endif //!_ATL_NO_PRAGMA_WARNINGS

#include <urlmon.h>
#include <mshtml.h>
#include <mshtmhst.h>
#include <exdisp.h>

#include <atlcom.h>


#pragma pack(push,_ATL_PACKING)

namespace DWAxHost
{

class ATL_NO_VTABLE CAxFrameWindow : 
	public CComObjectRootEx<CComObjectThreadModel>,
	public CWindowImpl<CAxFrameWindow>,
	public IOleInPlaceFrame
{
public:
	CAxFrameWindow()
	{
	}
	void FinalRelease()
	{
		m_spActiveObject.Release();
		if (m_hWnd)
			DestroyWindow();
	}

	DECLARE_POLY_AGGREGATABLE(CAxFrameWindow)

	BEGIN_COM_MAP(CAxFrameWindow)
		COM_INTERFACE_ENTRY(IOleInPlaceFrame)
		COM_INTERFACE_ENTRY(IOleInPlaceUIWindow)
		COM_INTERFACE_ENTRY(IOleWindow)
	END_COM_MAP()

	DECLARE_EMPTY_MSG_MAP()

	// IOleWindow
	STDMETHOD(GetWindow)(HWND* phwnd)
	{
		ATLASSERT(phwnd != NULL);
		if (phwnd == NULL)
			return E_POINTER;

		//if (m_hWnd == NULL)
		//	Create(NULL, NULL, _T("AXWIN Frame Window"), WS_OVERLAPPEDWINDOW, 0, (UINT)NULL);
		*phwnd = NULL;
		return S_OK;
	}
	STDMETHOD(ContextSensitiveHelp)(BOOL /*fEnterMode*/)
	{
		return S_OK;
	}

	// IOleInPlaceUIWindow
	STDMETHOD(GetBorder)(LPRECT /*lprectBorder*/)
	{
		return S_OK;
	}

	STDMETHOD(RequestBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
	{
		return INPLACE_E_NOTOOLSPACE;
	}

	STDMETHOD(SetBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
	{
		return S_OK;
	}

	STDMETHOD(SetActiveObject)(IOleInPlaceActiveObject* pActiveObject, LPCOLESTR /*pszObjName*/)
	{
		m_spActiveObject = pActiveObject;
		return S_OK;
	}

	// IOleInPlaceFrameWindow
	STDMETHOD(InsertMenus)(HMENU /*hmenuShared*/, LPOLEMENUGROUPWIDTHS /*lpMenuWidths*/)
	{
		return S_OK;
	}

	STDMETHOD(SetMenu)(HMENU /*hmenuShared*/, HOLEMENU /*holemenu*/, HWND /*hwndActiveObject*/)
	{
		return S_OK;
	}

	STDMETHOD(RemoveMenus)(HMENU /*hmenuShared*/)
	{
		return S_OK;
	}

	STDMETHOD(SetStatusText)(LPCOLESTR /*pszStatusText*/)
	{
		return S_OK;
	}

	STDMETHOD(EnableModeless)(BOOL /*fEnable*/)
	{
		return S_OK;
	}

	STDMETHOD(TranslateAccelerator)(LPMSG /*lpMsg*/, WORD /*wID*/)
	{
		return S_FALSE;
	}

	CComPtr<IOleInPlaceActiveObject> m_spActiveObject;
};


class ATL_NO_VTABLE CAxUIWindow : 
	public CComObjectRootEx<CComObjectThreadModel>,
	public CWindowImpl<CAxUIWindow>,
	public IOleInPlaceUIWindow
{
public:
	CAxUIWindow()
	{
	}

	void FinalRelease()
	{
		m_spActiveObject.Release();
		if (m_hWnd)
			DestroyWindow();
	}

	DECLARE_POLY_AGGREGATABLE(CAxUIWindow)

	BEGIN_COM_MAP(CAxUIWindow)
		COM_INTERFACE_ENTRY(IOleInPlaceUIWindow)
		COM_INTERFACE_ENTRY(IOleWindow)
	END_COM_MAP()

	DECLARE_EMPTY_MSG_MAP()

	// IOleWindow
	STDMETHOD(GetWindow)(HWND* phwnd)
	{
		ATLENSURE_RETURN(phwnd);
		if (m_hWnd == NULL)
			Create(NULL, NULL, _T("AXWIN UI Window"), WS_OVERLAPPEDWINDOW, 0, (UINT)NULL);
		*phwnd = m_hWnd;
		return S_OK;
	}

	STDMETHOD(ContextSensitiveHelp)(BOOL /*fEnterMode*/)
	{
		return S_OK;
	}

	// IOleInPlaceUIWindow
	STDMETHOD(GetBorder)(LPRECT /*lprectBorder*/)
	{
		return S_OK;
	}

	STDMETHOD(RequestBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
	{
		return INPLACE_E_NOTOOLSPACE;
	}

	STDMETHOD(SetBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
	{
		return S_OK;
	}

	STDMETHOD(SetActiveObject)(IOleInPlaceActiveObject* pActiveObject, LPCOLESTR /*pszObjName*/)
	{
		m_spActiveObject = pActiveObject;
		return S_OK;
	}

	CComPtr<IOleInPlaceActiveObject> m_spActiveObject;
};



class ATL_NO_VTABLE CDWAxHost : 
	public CComCoClass<CDWAxHost , &CLSID_NULL>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CWindowImpl<CDWAxHost>,
	public IOleInPlaceSite,
	public IOleClientSite,
	public IOleControlSite,
	public IOleContainer,
    public IOleCommandTarget,
    public IDocHostUIHandler,
	public IObjectWithSiteImpl<CDWAxHost>,
	public IServiceProvider,
	public IAdviseSink,
	public IDispatchImpl<IAxWinAmbientDispatchEx, &__uuidof(IAxWinAmbientDispatchEx), &CAtlModule::m_libid, 0xFFFF, 0xFFFF>
{
public:
	// ctor/dtor
	CDWAxHost()
	{
		m_bInPlaceActive = FALSE;
		m_bUIActive = FALSE;
		m_bMDIApp = FALSE;
		m_bWindowless = FALSE;
		m_bCapture = FALSE;
		m_bHaveFocus = FALSE;

		// Initialize ambient properties
		m_bCanWindowlessActivate = TRUE;
		m_bUserMode = TRUE;
		m_bDisplayAsDefault = FALSE;
		m_clrForeground = GetSysColor(COLOR_WINDOWTEXT);
		m_lcidLocaleID = LOCALE_USER_DEFAULT;
		m_bMessageReflect = true;

		m_bReleaseAll = FALSE;

		m_bSubclassed = FALSE;

		m_dwAdviseSink = 0xCDCDCDCD;
		m_dwDocHostFlags = DOCHOSTUIFLAG_NO3DBORDER;
		m_dwDocHostDoubleClickFlags = DOCHOSTUIDBLCLK_DEFAULT;
		m_bAllowContextMenu = true;
		m_bAllowShowUI = false;

		m_hAccel = NULL;
		m_iidSink = IID_NULL;
	}

	virtual ~CDWAxHost()
	{
	}
	void FinalRelease()
	{
		ReleaseAll();
	}

	virtual void OnFinalMessage(HWND /*hWnd*/)
	{
		GetControllingUnknown()->Release();
	}

	DECLARE_NO_REGISTRY()
	DECLARE_POLY_AGGREGATABLE(CDWAxHost)
	DECLARE_GET_CONTROLLING_UNKNOWN()

	BEGIN_COM_MAP(CDWAxHost)
		COM_INTERFACE_ENTRY2(IDispatch, IAxWinAmbientDispatchEx)
		COM_INTERFACE_ENTRY(IOleClientSite)
		COM_INTERFACE_ENTRY(IOleInPlaceSite)
		COM_INTERFACE_ENTRY(IOleWindow)
		COM_INTERFACE_ENTRY(IOleControlSite)
		COM_INTERFACE_ENTRY(IOleContainer)
		COM_INTERFACE_ENTRY(IObjectWithSite)
		COM_INTERFACE_ENTRY(IServiceProvider)
		COM_INTERFACE_ENTRY(IAxWinAmbientDispatchEx)
		COM_INTERFACE_ENTRY(IAxWinAmbientDispatch)
        COM_INTERFACE_ENTRY(IAdviseSink)
        COM_INTERFACE_ENTRY(IOleCommandTarget)
        COM_INTERFACE_ENTRY(IDocHostUIHandler)
	END_COM_MAP()

	static CWndClassInfo& GetWndClassInfo()
	{
		static CWndClassInfo wc =
		{
			{ sizeof(WNDCLASSEX), 0, StartWindowProc,
			0, 0, 0, 0, 0, (HBRUSH)(COLOR_WINDOW + 1), 0, _T(ATLAXWIN_CLASS), 0 },
			NULL, NULL, IDC_ARROW, TRUE, 0, _T("")
		};
		return wc;
	}

	BEGIN_MSG_MAP(CDWAxHost)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		if (m_bWindowless && uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST)
		{
			// Mouse messages handled when a windowless control has captured the cursor
			// or if the cursor is over the control
			DWORD dwHitResult = m_bCapture ? HITRESULT_HIT : HITRESULT_OUTSIDE;
			if (dwHitResult == HITRESULT_OUTSIDE && m_spViewObject != NULL)
			{
				POINT ptMouse = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				m_spViewObject->QueryHitPoint(DVASPECT_CONTENT, &m_rcPos, ptMouse, 0, &dwHitResult);
			}
			if (dwHitResult == HITRESULT_HIT)
			{
				MESSAGE_HANDLER(WM_MOUSEMOVE, OnWindowlessMouseMessage)				
				MESSAGE_HANDLER(WM_LBUTTONUP, OnWindowlessMouseMessage)
				MESSAGE_HANDLER(WM_RBUTTONUP, OnWindowlessMouseMessage)
				MESSAGE_HANDLER(WM_MBUTTONUP, OnWindowlessMouseMessage)
				MESSAGE_HANDLER(WM_LBUTTONDOWN, OnWindowlessMouseMessage)
				MESSAGE_HANDLER(WM_RBUTTONDOWN, OnWindowlessMouseMessage)
				MESSAGE_HANDLER(WM_MBUTTONDOWN, OnWindowlessMouseMessage)
				MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnWindowlessMouseMessage)
				MESSAGE_HANDLER(WM_RBUTTONDBLCLK, OnWindowlessMouseMessage)
				MESSAGE_HANDLER(WM_MBUTTONDBLCLK, OnWindowlessMouseMessage)
			}
		}
		if (m_bWindowless & m_bHaveFocus)
		{
			// Keyboard messages handled only when a windowless control has the focus
			MESSAGE_HANDLER(WM_KEYDOWN, OnWindowMessage)
			MESSAGE_HANDLER(WM_KEYUP, OnWindowMessage)
			MESSAGE_HANDLER(WM_CHAR, OnWindowMessage)
			MESSAGE_HANDLER(WM_DEADCHAR, OnWindowMessage)
			MESSAGE_HANDLER(WM_SYSKEYDOWN, OnWindowMessage)
			MESSAGE_HANDLER(WM_SYSKEYUP, OnWindowMessage)
			MESSAGE_HANDLER(WM_SYSDEADCHAR, OnWindowMessage)
			MESSAGE_HANDLER(WM_HELP, OnWindowMessage)
			MESSAGE_HANDLER(WM_CANCELMODE, OnWindowMessage)
			MESSAGE_HANDLER(WM_IME_CHAR, OnWindowMessage)
			MESSAGE_HANDLER(WM_MBUTTONDBLCLK, OnWindowMessage)
			MESSAGE_RANGE_HANDLER(WM_IME_SETCONTEXT, WM_IME_KEYUP, OnWindowMessage)
		}
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		if (m_bMessageReflect)
		{
			bHandled = TRUE;
			lResult = ReflectNotifications(uMsg, wParam, lParam, bHandled);
			if(bHandled)
				return TRUE;
		}
		MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
	END_MSG_MAP()

	LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
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

	void ReleaseAll()
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
	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (m_spViewObject == NULL)
			bHandled = false;

		return 1;
	}

	LRESULT OnMouseActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
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
	LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		m_bHaveFocus = TRUE;
		bHandled = FALSE;
		return 0;
	}
	LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		m_bHaveFocus = FALSE;
		bHandled = FALSE;
		return 0;
	}
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
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
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		GetControllingUnknown()->AddRef();
		DefWindowProc(uMsg, wParam, lParam);
		ReleaseAll();
		bHandled = FALSE;
		return 0;
	}
	LRESULT OnWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = 0;
		HRESULT hr = S_FALSE;
		if (m_bInPlaceActive && m_bWindowless && m_spInPlaceObjectWindowless)
			hr = m_spInPlaceObjectWindowless->OnWindowMessage(uMsg, wParam, lParam, &lRes);
		if (hr == S_FALSE)
			bHandled = FALSE;
		return lRes;
	}
	LRESULT OnWindowlessMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = 0;
		if (m_bInPlaceActive && m_bWindowless && m_spInPlaceObjectWindowless)
			m_spInPlaceObjectWindowless->OnWindowMessage(uMsg, wParam, lParam, &lRes);
		bHandled = FALSE;
		return lRes;
	}
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (m_spViewObject == NULL)
			return 0;

//		m_spViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, hdcCompatible, (RECTL*)&m_rcPos, (RECTL*)&m_rcPos, NULL, NULL); 

		bHandled = FALSE;
		return 0;
	}


	STDMETHOD(QueryControl)(REFIID riid, void** ppvObject)
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

	STDMETHOD(CreateControl)(HWND hWnd)
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


	HRESULT FireAmbientPropertyChange(DISPID dispChanged)
	{
		HRESULT hr = S_OK;
		CComQIPtr<IOleControl, &__uuidof(IOleControl)> spOleControl(m_spUnknown);
		if (spOleControl != NULL)
			hr = spOleControl->OnAmbientPropertyChange(dispChanged);
		return hr;
	}

	// IAxWinAmbientDispatch

	CComPtr<IDispatch> m_spAmbientDispatch;

	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams,
		VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
	{
		HRESULT hr = IDispatchImpl<IAxWinAmbientDispatchEx, &__uuidof(IAxWinAmbientDispatchEx), &CAtlModule::m_libid, 0xFFFF, 0xFFFF>::Invoke
			(dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
		if ((hr == DISP_E_MEMBERNOTFOUND || hr == TYPE_E_ELEMENTNOTFOUND) && m_spAmbientDispatch != NULL)
		{
			hr = m_spAmbientDispatch->Invoke(dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
			if (SUCCEEDED(hr) && (wFlags & DISPATCH_PROPERTYPUT) != 0)
			{
				hr = FireAmbientPropertyChange(dispIdMember);
			}
		}
		return hr;
	}

	STDMETHOD(put_AllowWindowlessActivation)(VARIANT_BOOL bAllowWindowless)
	{
		m_bCanWindowlessActivate = bAllowWindowless;
		return S_OK;
	}
	STDMETHOD(get_AllowWindowlessActivation)(VARIANT_BOOL* pbAllowWindowless)
	{
		ATLASSERT(pbAllowWindowless != NULL);
		if (pbAllowWindowless == NULL)
			return E_POINTER;

		*pbAllowWindowless = m_bCanWindowlessActivate ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
		return S_OK;
	}
	STDMETHOD(put_BackColor)(OLE_COLOR clrBackground)
	{
		FireAmbientPropertyChange(DISPID_AMBIENT_BACKCOLOR);
		InvalidateRect(0, FALSE);
		return S_OK;
	}
	STDMETHOD(get_BackColor)(OLE_COLOR* pclrBackground)
	{
		ATLASSERT(pclrBackground != NULL);
		if (pclrBackground == NULL)
			return E_POINTER;

		*pclrBackground = 0xffffff;
		return S_OK;
	}
	STDMETHOD(put_ForeColor)(OLE_COLOR clrForeground)
	{
		m_clrForeground = clrForeground;
		FireAmbientPropertyChange(DISPID_AMBIENT_FORECOLOR);
		return S_OK;
	}
	STDMETHOD(get_ForeColor)(OLE_COLOR* pclrForeground)
	{
		ATLASSERT(pclrForeground != NULL);
		if (pclrForeground == NULL)
			return E_POINTER;

		*pclrForeground = m_clrForeground;
		return S_OK;
	}
	STDMETHOD(put_LocaleID)(LCID lcidLocaleID)
	{
		m_lcidLocaleID = lcidLocaleID;
		FireAmbientPropertyChange(DISPID_AMBIENT_LOCALEID);
		return S_OK;
	}
	STDMETHOD(get_LocaleID)(LCID* plcidLocaleID)
	{
		ATLASSERT(plcidLocaleID != NULL);
		if (plcidLocaleID == NULL)
			return E_POINTER;

		*plcidLocaleID = m_lcidLocaleID;
		return S_OK;
	}
	STDMETHOD(put_UserMode)(VARIANT_BOOL bUserMode)
	{
		m_bUserMode = bUserMode;
		FireAmbientPropertyChange(DISPID_AMBIENT_USERMODE);
		return S_OK;
	}
	STDMETHOD(get_UserMode)(VARIANT_BOOL* pbUserMode)
	{
		ATLASSERT(pbUserMode != NULL);
		if (pbUserMode == NULL)
			return E_POINTER;

		*pbUserMode = m_bUserMode ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
		return S_OK;
	}
	STDMETHOD(put_DisplayAsDefault)(VARIANT_BOOL bDisplayAsDefault)
	{
		m_bDisplayAsDefault = bDisplayAsDefault;
		FireAmbientPropertyChange(DISPID_AMBIENT_DISPLAYASDEFAULT);
		return S_OK;
	}
	STDMETHOD(get_DisplayAsDefault)(VARIANT_BOOL* pbDisplayAsDefault)
	{
		ATLASSERT(pbDisplayAsDefault != NULL);
		if (pbDisplayAsDefault == NULL)
			return E_POINTER;

		*pbDisplayAsDefault = m_bDisplayAsDefault ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
		return S_OK;
	}
	STDMETHOD(put_Font)(IFontDisp* pFont)
	{
		m_spFont = pFont;
		FireAmbientPropertyChange(DISPID_AMBIENT_FONT);
		return S_OK;
	}
	STDMETHOD(get_Font)(IFontDisp** pFont)
	{
		ATLASSERT(pFont != NULL);
		if (pFont == NULL)
			return E_POINTER;
		*pFont = NULL;

		if (m_spFont == NULL)
		{
			USES_CONVERSION_EX;
			HFONT hSystemFont = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
			if (hSystemFont == NULL)
				hSystemFont = (HFONT) GetStockObject(SYSTEM_FONT);
			if (hSystemFont == NULL)
				return AtlHresultFromLastError();
			LOGFONT logfont;
			GetObject(hSystemFont, sizeof(logfont), &logfont);
			FONTDESC fd;
			fd.cbSizeofstruct = sizeof(FONTDESC);
			fd.lpstrName = T2OLE_EX_DEF(logfont.lfFaceName);
			fd.sWeight = (short)logfont.lfWeight;
			fd.sCharset = logfont.lfCharSet;
			fd.fItalic = logfont.lfItalic;
			fd.fUnderline = logfont.lfUnderline;
			fd.fStrikethrough = logfont.lfStrikeOut;

			long lfHeight = logfont.lfHeight;
			if (lfHeight < 0)
				lfHeight = -lfHeight;

			int ppi;
			HDC hdc;
			if (m_hWnd)
			{
				hdc = ::GetDC(m_hWnd);
				if (hdc == NULL)
					return AtlHresultFromLastError();
				ppi = GetDeviceCaps(hdc, LOGPIXELSY);
				::ReleaseDC(m_hWnd, hdc);
			}
			else
			{
				hdc = ::GetDC(GetDesktopWindow());
				if (hdc == NULL)
					return AtlHresultFromLastError();
				ppi = GetDeviceCaps(hdc, LOGPIXELSY);
				::ReleaseDC(GetDesktopWindow(), hdc);
			}
			fd.cySize.Lo = lfHeight * 720000 / ppi;
			fd.cySize.Hi = 0;

			OleCreateFontIndirect(&fd, __uuidof(IFontDisp), (void**) &m_spFont);
		}

		return m_spFont.CopyTo(pFont);
	}
	STDMETHOD(put_MessageReflect)(VARIANT_BOOL bMessageReflect)
	{
		m_bMessageReflect = bMessageReflect;
		FireAmbientPropertyChange(DISPID_AMBIENT_MESSAGEREFLECT);
		return S_OK;
	}
	STDMETHOD(get_MessageReflect)(VARIANT_BOOL* pbMessageReflect)
	{

		ATLASSERT(pbMessageReflect != NULL);
		if (pbMessageReflect == NULL)
			return E_POINTER;

		*pbMessageReflect = m_bMessageReflect ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
		return S_OK;
	}
	STDMETHOD(get_ShowGrabHandles)(VARIANT_BOOL* pbShowGrabHandles)
	{
		*pbShowGrabHandles = ATL_VARIANT_FALSE;
		return S_OK;
	}
	STDMETHOD(get_ShowHatching)(VARIANT_BOOL* pbShowHatching)
	{
		ATLASSERT(pbShowHatching != NULL);
		if (pbShowHatching == NULL)
			return E_POINTER;

		*pbShowHatching = ATL_VARIANT_FALSE;
		return S_OK;
	}
	STDMETHOD(put_DocHostFlags)(DWORD dwDocHostFlags)
	{
		m_dwDocHostFlags = dwDocHostFlags;
		FireAmbientPropertyChange(DISPID_UNKNOWN);
		return S_OK;
	}
	STDMETHOD(get_DocHostFlags)(DWORD* pdwDocHostFlags)
	{
		ATLASSERT(pdwDocHostFlags != NULL);
		if (pdwDocHostFlags == NULL)
			return E_POINTER;

		*pdwDocHostFlags = m_dwDocHostFlags;
		return S_OK;
	}
	STDMETHOD(put_DocHostDoubleClickFlags)(DWORD dwDocHostDoubleClickFlags)
	{
		m_dwDocHostDoubleClickFlags = dwDocHostDoubleClickFlags;
		return S_OK;
	}
	STDMETHOD(get_DocHostDoubleClickFlags)(DWORD* pdwDocHostDoubleClickFlags)
	{
		ATLASSERT(pdwDocHostDoubleClickFlags != NULL);
		if (pdwDocHostDoubleClickFlags == NULL)
			return E_POINTER;

		*pdwDocHostDoubleClickFlags = m_dwDocHostDoubleClickFlags;
		return S_OK;
	}
	STDMETHOD(put_AllowContextMenu)(VARIANT_BOOL bAllowContextMenu)
	{
		m_bAllowContextMenu = bAllowContextMenu;
		return S_OK;
	}
	STDMETHOD(get_AllowContextMenu)(VARIANT_BOOL* pbAllowContextMenu)
	{
		ATLASSERT(pbAllowContextMenu != NULL);
		if (pbAllowContextMenu == NULL)
			return E_POINTER;

		*pbAllowContextMenu = m_bAllowContextMenu ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
		return S_OK;
	}
	STDMETHOD(put_AllowShowUI)(VARIANT_BOOL bAllowShowUI)
	{
		m_bAllowShowUI = bAllowShowUI;
		return S_OK;
	}
	STDMETHOD(get_AllowShowUI)(VARIANT_BOOL* pbAllowShowUI)
	{
		ATLASSERT(pbAllowShowUI != NULL);
		if (pbAllowShowUI == NULL)
			return E_POINTER;

		*pbAllowShowUI = m_bAllowShowUI ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
		return S_OK;
	}
	STDMETHOD(put_OptionKeyPath)(BSTR bstrOptionKeyPath) throw()
	{
		m_bstrOptionKeyPath = bstrOptionKeyPath;
		return S_OK;
	}
	STDMETHOD(get_OptionKeyPath)(BSTR* pbstrOptionKeyPath)
	{
		ATLASSERT(pbstrOptionKeyPath != NULL);
		if (pbstrOptionKeyPath == NULL)
			return E_POINTER;

		*pbstrOptionKeyPath = SysAllocString(m_bstrOptionKeyPath);
		return S_OK;
	}

	STDMETHOD(SetAmbientDispatch)(IDispatch* pDispatch)
	{
		m_spAmbientDispatch = pDispatch;
		return S_OK;
	}

	// IObjectWithSite
	STDMETHOD(SetSite)(IUnknown* pUnkSite)
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
	STDMETHOD(SaveObject)()
	{
		ATLTRACENOTIMPL(_T("IOleClientSite::SaveObject"));
	}
	STDMETHOD(GetMoniker)(DWORD /*dwAssign*/, DWORD /*dwWhichMoniker*/, IMoniker** /*ppmk*/)
	{
		ATLTRACENOTIMPL(_T("IOleClientSite::GetMoniker"));
	}
	STDMETHOD(GetContainer)(IOleContainer** ppContainer)
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
	STDMETHOD(ShowObject)()
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
	STDMETHOD(OnShowWindow)(BOOL /*fShow*/)
	{
		ATLTRACENOTIMPL(_T("IOleClientSite::OnShowWindow"));
	}
	STDMETHOD(RequestNewObjectLayout)()
	{
		ATLTRACENOTIMPL(_T("IOleClientSite::RequestNewObjectLayout"));
	}

	// IOleInPlaceSite
	STDMETHOD(GetWindow)(HWND* phwnd)
	{
		ATLENSURE_RETURN(phwnd);
		*phwnd = m_hWnd;
		return S_OK;
	}
	STDMETHOD(ContextSensitiveHelp)(BOOL /*fEnterMode*/)
	{
		ATLTRACENOTIMPL(_T("IOleInPlaceSite::ContextSensitiveHelp"));
	}
	STDMETHOD(CanInPlaceActivate)()
	{
		return S_OK;
	}
	STDMETHOD(OnInPlaceActivate)()
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
	STDMETHOD(OnUIActivate)()
	{
		ATLTRACE(atlTraceHosting, 2, _T("IOleInPlaceSite::OnUIActivate\n"));
		m_bUIActive = TRUE;
		return S_OK;
	}
	STDMETHOD(GetWindowContext)(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow** ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO pFrameInfo)
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
	STDMETHOD(Scroll)(SIZE /*scrollExtant*/)
	{
		ATLTRACENOTIMPL(_T("IOleInPlaceSite::Scroll"));
	}
	STDMETHOD(OnUIDeactivate)(BOOL /*fUndoable*/)
	{
		ATLTRACE(atlTraceHosting, 2, _T("IOleInPlaceSite::OnUIDeactivate\n"));
		m_bUIActive = FALSE;
		return S_OK;
	}
	STDMETHOD(OnInPlaceDeactivate)()
	{
		m_bInPlaceActive = FALSE;
		m_spInPlaceObjectWindowless.Release();
		return S_OK;
	}
	STDMETHOD(DiscardUndoState)()
	{
		ATLTRACENOTIMPL(_T("IOleInPlaceSite::DiscardUndoState"));
	}
	STDMETHOD(DeactivateAndUndo)()
	{
		ATLTRACENOTIMPL(_T("IOleInPlaceSite::DeactivateAndUndo"));
	}
	STDMETHOD(OnPosRectChange)(LPCRECT lprcPosRect)
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
	STDMETHOD(OnControlInfoChanged)()
	{
		return S_OK;
	}
	STDMETHOD(LockInPlaceActive)(BOOL /*fLock*/)
	{
		return S_OK;
	}
	STDMETHOD(GetExtendedControl)(IDispatch** ppDisp)
	{
		if (ppDisp == NULL)
			return E_POINTER;
		return m_spOleObject.QueryInterface(ppDisp);
	}
	STDMETHOD(TransformCoords)(POINTL* /*pPtlHimetric*/, POINTF* /*pPtfContainer*/, DWORD /*dwFlags*/)
	{
		ATLTRACENOTIMPL(_T("CTuoAxHostWindow::TransformCoords"));
	}
	STDMETHOD(TranslateAccelerator)(LPMSG /*lpMsg*/, DWORD /*grfModifiers*/)
	{
		return S_FALSE;
	}
	STDMETHOD(OnFocus)(BOOL fGotFocus)
	{
		m_bHaveFocus = fGotFocus;
		return S_OK;
	}
	STDMETHOD(ShowPropertyFrame)()
	{
		ATLTRACENOTIMPL(_T("CTuoAxHostWindow::ShowPropertyFrame"));
	}

	// IAdviseSink
	STDMETHOD_(void, OnDataChange)(FORMATETC* /*pFormatetc*/, STGMEDIUM* /*pStgmed*/)
	{
	}
	STDMETHOD_(void, OnViewChange)(DWORD /*dwAspect*/, LONG /*lindex*/)
	{
	}
	STDMETHOD_(void, OnRename)(IMoniker* /*pmk*/)
	{
	}
	STDMETHOD_(void, OnSave)()
	{
	}
	STDMETHOD_(void, OnClose)()
	{
	}

	// IOleContainer
	STDMETHOD(ParseDisplayName)(IBindCtx* /*pbc*/, LPOLESTR /*pszDisplayName*/, ULONG* /*pchEaten*/, IMoniker** /*ppmkOut*/)
	{
		ATLTRACENOTIMPL(_T("CTuoAxHostWindow::ParseDisplayName"));
	}
	STDMETHOD(EnumObjects)(DWORD /*grfFlags*/, IEnumUnknown** ppenum)
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
	STDMETHOD(LockContainer)(BOOL fLock)
	{
		m_bLocked = fLock;
		return S_OK;
	}

	// pointers
	CComPtr<IUnknown> m_spUnknown;
	CComPtr<IOleObject> m_spOleObject;
	CComPtr<IOleInPlaceFrame> m_spInPlaceFrame;
	CComPtr<IOleInPlaceUIWindow> m_spInPlaceUIWindow;
	CComPtr<IViewObjectEx> m_spViewObject;
	CComPtr<IOleInPlaceObjectWindowless> m_spInPlaceObjectWindowless;
	CComPtr<IDispatch> m_spExternalDispatch;
	IID m_iidSink;
	DWORD m_dwViewObjectType;
	DWORD m_dwAdviseSink;

	// state
	unsigned long m_bInPlaceActive:1;
	unsigned long m_bUIActive:1;
	unsigned long m_bMDIApp:1;
	unsigned long m_bWindowless:1;
	unsigned long m_bCapture:1;
	unsigned long m_bHaveFocus:1;
	unsigned long m_bReleaseAll:1;
	unsigned long m_bLocked:1;

	DWORD m_dwOleObject;
	DWORD m_dwMiscStatus;
	SIZEL m_hmSize;
	SIZEL m_pxSize;
	RECT m_rcPos;

	// Accelerator table
	HACCEL m_hAccel;

	// Ambient property storage
	unsigned long m_bCanWindowlessActivate:1;
	unsigned long m_bUserMode:1;
	unsigned long m_bDisplayAsDefault:1;
	unsigned long m_bMessageReflect:1;
	unsigned long m_bSubclassed:1;
	unsigned long m_bAllowContextMenu:1;
	unsigned long m_bAllowShowUI:1;
	OLE_COLOR m_clrForeground;
	LCID m_lcidLocaleID;
	CComPtr<IFontDisp> m_spFont;
	CComPtr<IServiceProvider>  m_spServices;
	DWORD m_dwDocHostFlags;
	DWORD m_dwDocHostDoubleClickFlags;
	CComBSTR m_bstrOptionKeyPath;

	void SubclassWindow(HWND hWnd)
	{
		m_bSubclassed = CWindowImpl<CDWAxHost>::SubclassWindow(hWnd);
	}

	void ReleaseWindow()
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
	LRESULT ReflectNotifications(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

	STDMETHOD(QueryService)( REFGUID rsid, REFIID riid, void** ppvObj) 
	{
		ATLASSERT(ppvObj != NULL);
		if (ppvObj == NULL)
			return E_POINTER;
		*ppvObj = NULL;

		HRESULT hr = E_NOINTERFACE;
		// Try for service on this object

		// No services currently

		// If that failed try to find the service on the outer object
		if (FAILED(hr) && m_spServices)
			hr = m_spServices->QueryService(rsid, riid, ppvObj);

		return hr;
	}


    STDMETHOD(Exec)(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT *pvaIn, VARIANT *pvaOut)
    {
        HRESULT hr = OLECMDERR_E_UNKNOWNGROUP ;

        if (pguidCmdGroup && IsEqualGUID(*pguidCmdGroup, CGID_DocHostCommandHandler))
        {
            switch (nCmdID) 
            {
                //���νű�����ĶԻ���
            case OLECMDID_SHOWSCRIPTERROR:
                {
                    (*pvaOut).vt = VT_BOOL ;
                    (*pvaOut).boolVal = VARIANT_TRUE ;
                    return S_OK ;
                }
                break ;
            default:
                hr = OLECMDERR_E_NOTSUPPORTED;
                break;
            }
        }

        return hr ;
    }

    STDMETHOD(QueryStatus)(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD* prgCmds, OLECMDTEXT *pCmdText)
    {
        return E_NOTIMPL ;
    }


    STDMETHOD(ShowContextMenu)(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved)
    {
        return E_NOTIMPL;
    }

    STDMETHOD(GetHostInfo)(DOCHOSTUIINFO *pInfo)
    {
        pInfo->cbSize = sizeof(DOCHOSTUIINFO) ;
        pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT ;
        pInfo->dwFlags |= DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_THEME | DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE;
        return S_OK ;
    }

    STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc)
    {
        return S_OK ;
    }

    STDMETHOD(HideUI)(void)
    {
        return S_OK ;
    }

    STDMETHOD(UpdateUI)(void)
    {
        return S_OK ;
    }

    STDMETHOD(EnableModeless)(BOOL fEnable)
    {
        return E_NOTIMPL ;
    }

    STDMETHOD(OnDocWindowActivate)(BOOL fActivate) 
    {
        return E_NOTIMPL ;
    }

    STDMETHOD(OnFrameWindowActivate)(BOOL fActivate)
    {
        return E_NOTIMPL ;
    }

    STDMETHOD(ResizeBorder)(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow)
    {
        return S_OK ;
    }

    STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID)
    {
        return S_FALSE ;
    }

    STDMETHOD(GetOptionKeyPath)(LPOLESTR *pchKey, DWORD dw)
    {
        return E_NOTIMPL ;
    }

    STDMETHOD(GetDropTarget)(IDropTarget *pDropTarget, IDropTarget **ppDropTarget)
    {
        //CDragDropManager *pDDM = new CDragDropManager(m_pAxControl);
        //RegisterDragDrop(m_pAxControl->m_hWnd, (IDropTarget*)pDDM);
        //pDDM->SetOriginalDropTarget(pDropTarget);
        //*ppDropTarget = (IDropTarget*)pDDM;
        //m_pDragDropManagers.push_back(pDDM);

        return S_OK;
    }

    STDMETHOD(GetExternal)(IDispatch **ppDispatch)
    {

        return E_NOTIMPL ;
    }

    STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut)
    {
        //m_bstrUrl = pchURLIn ;
        return S_OK;
    }

    STDMETHOD(FilterDataObject)(IDataObject *pDO, IDataObject **ppDORet) 
    {
        return E_NOTIMPL ;
    }

};


}

#pragma pack(pop)
