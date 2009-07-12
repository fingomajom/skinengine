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

#include "DWHtmlView.h"


#pragma pack(push,_ATL_PACKING)

namespace DWAxHost
{

class ATL_NO_VTABLE CAxFrameWindow : 
	public CComObjectRootEx<CComObjectThreadModel>,
	public CWindowImpl<CAxFrameWindow>,
	public IOleInPlaceFrame
{
public:

	CAxFrameWindow();


	DECLARE_POLY_AGGREGATABLE(CAxFrameWindow)

	BEGIN_COM_MAP(CAxFrameWindow)
		COM_INTERFACE_ENTRY(IOleInPlaceFrame)
		COM_INTERFACE_ENTRY(IOleInPlaceUIWindow)
		COM_INTERFACE_ENTRY(IOleWindow)
	END_COM_MAP()

	DECLARE_EMPTY_MSG_MAP()

protected:
    void FinalRelease();
protected:

	// IOleWindow
	STDMETHOD(GetWindow)(HWND* phwnd);
	STDMETHOD(ContextSensitiveHelp)(BOOL /*fEnterMode*/);
	// IOleInPlaceUIWindow
	STDMETHOD(GetBorder)(LPRECT /*lprectBorder*/);
	STDMETHOD(RequestBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/);
	STDMETHOD(SetBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/);
	STDMETHOD(SetActiveObject)(IOleInPlaceActiveObject* pActiveObject, LPCOLESTR /*pszObjName*/);	
    // IOleInPlaceFrameWindow
	STDMETHOD(InsertMenus)(HMENU /*hmenuShared*/, LPOLEMENUGROUPWIDTHS /*lpMenuWidths*/);
	STDMETHOD(SetMenu)(HMENU /*hmenuShared*/, HOLEMENU /*holemenu*/, HWND /*hwndActiveObject*/);
	STDMETHOD(RemoveMenus)(HMENU /*hmenuShared*/);
	STDMETHOD(SetStatusText)(LPCOLESTR /*pszStatusText*/);
	STDMETHOD(EnableModeless)(BOOL /*fEnable*/);
	STDMETHOD(TranslateAccelerator)(LPMSG /*lpMsg*/, WORD /*wID*/);

	CComPtr<IOleInPlaceActiveObject> m_spActiveObject;
};


class ATL_NO_VTABLE CAxUIWindow : 
	public CComObjectRootEx<CComObjectThreadModel>,
	public CWindowImpl<CAxUIWindow>,
	public IOleInPlaceUIWindow
{
public:

	CAxUIWindow();


	DECLARE_POLY_AGGREGATABLE(CAxUIWindow)

	BEGIN_COM_MAP(CAxUIWindow)
		COM_INTERFACE_ENTRY(IOleInPlaceUIWindow)
		COM_INTERFACE_ENTRY(IOleWindow)
	END_COM_MAP()

	DECLARE_EMPTY_MSG_MAP()

protected:

    void FinalRelease();


protected:

	// IOleWindow
	STDMETHOD(GetWindow)(HWND* phwnd);
	STDMETHOD(ContextSensitiveHelp)(BOOL /*fEnterMode*/);
	STDMETHOD(GetBorder)(LPRECT /*lprectBorder*/);
	STDMETHOD(RequestBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/);
	STDMETHOD(SetBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/);
	STDMETHOD(SetActiveObject)(IOleInPlaceActiveObject* pActiveObject, LPCOLESTR /*pszObjName*/);

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
	public IDispatchImpl<IDispatch, &__uuidof(IDispatch), &CAtlModule::m_libid, 0xFFFF, 0xFFFF>
{
public:
	// ctor/dtor
	CDWAxHost();
	virtual ~CDWAxHost();

	DECLARE_NO_REGISTRY()
	DECLARE_POLY_AGGREGATABLE(CDWAxHost)
	DECLARE_GET_CONTROLLING_UNKNOWN()

	BEGIN_COM_MAP(CDWAxHost)
		COM_INTERFACE_ENTRY(IOleClientSite)
		COM_INTERFACE_ENTRY(IOleInPlaceSite)
		COM_INTERFACE_ENTRY(IOleWindow)
		COM_INTERFACE_ENTRY(IOleControlSite)
		COM_INTERFACE_ENTRY(IOleContainer)
		COM_INTERFACE_ENTRY(IObjectWithSite)
		COM_INTERFACE_ENTRY(IServiceProvider)
        COM_INTERFACE_ENTRY(IAdviseSink)
        COM_INTERFACE_ENTRY(IOleCommandTarget)
        COM_INTERFACE_ENTRY(IDocHostUIHandler)
        COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	static CWndClassInfo& GetWndClassInfo();

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
		if (TRUE)
		{
			bHandled = TRUE;
			lResult = ReflectNotifications(uMsg, wParam, lParam, bHandled);
			if(bHandled)
				return TRUE;
		}
		MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
	END_MSG_MAP()


    void FinalRelease();
    void ReleaseAll();
    virtual void OnFinalMessage(HWND /*hWnd*/);

    void SubclassWindow(HWND hWnd);
    void ReleaseWindow();
    // Reflection
    LRESULT ReflectNotifications(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// window message handlers
    LRESULT OnForwardMsg     (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnMouseActivate  (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnSetFocus       (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnKillFocus      (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnSize           (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnDestroy        (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnWindowMessage  (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnWindowlessMouseMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnPaint          (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

	STDMETHOD(QueryControl)(REFIID riid, void** ppvObject);
	STDMETHOD(CreateControl)(HWND hWnd);

	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams,
		VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

	// IObjectWithSite
	STDMETHOD(SetSite)(IUnknown* pUnkSite);

	// IOleClientSite
	STDMETHOD(SaveObject)();
	STDMETHOD(GetMoniker)(DWORD /*dwAssign*/, DWORD /*dwWhichMoniker*/, IMoniker** /*ppmk*/);
	STDMETHOD(GetContainer)(IOleContainer** ppContainer);
	STDMETHOD(ShowObject)();
	STDMETHOD(OnShowWindow)(BOOL /*fShow*/);
	STDMETHOD(RequestNewObjectLayout)();
	// IOleInPlaceSite
	STDMETHOD(GetWindow)(HWND* phwnd);
	STDMETHOD(ContextSensitiveHelp)(BOOL /*fEnterMode*/);
	STDMETHOD(CanInPlaceActivate)();
	STDMETHOD(OnInPlaceActivate)();
	STDMETHOD(OnUIActivate)();
	STDMETHOD(GetWindowContext)(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow** ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO pFrameInfo);
	STDMETHOD(Scroll)(SIZE /*scrollExtant*/);
	STDMETHOD(OnUIDeactivate)(BOOL /*fUndoable*/);
	STDMETHOD(OnInPlaceDeactivate)();
	STDMETHOD(DiscardUndoState)();
	STDMETHOD(DeactivateAndUndo)();
	STDMETHOD(OnPosRectChange)(LPCRECT lprcPosRect);
	// IOleControlSite
	STDMETHOD(OnControlInfoChanged)();
	STDMETHOD(LockInPlaceActive)(BOOL /*fLock*/);
	STDMETHOD(GetExtendedControl)(IDispatch** ppDisp);
	STDMETHOD(TransformCoords)(POINTL* /*pPtlHimetric*/, POINTF* /*pPtfContainer*/, DWORD /*dwFlags*/);
	STDMETHOD(TranslateAccelerator)(LPMSG /*lpMsg*/, DWORD /*grfModifiers*/);
	STDMETHOD(OnFocus)(BOOL fGotFocus);
	STDMETHOD(ShowPropertyFrame)();
	// IAdviseSink
	STDMETHOD_(void, OnDataChange)(FORMATETC* /*pFormatetc*/, STGMEDIUM* /*pStgmed*/);
	STDMETHOD_(void, OnViewChange)(DWORD /*dwAspect*/, LONG /*lindex*/);
	STDMETHOD_(void, OnRename)(IMoniker* /*pmk*/);
	STDMETHOD_(void, OnSave)();
	STDMETHOD_(void, OnClose)();

	// IOleContainer
	STDMETHOD(ParseDisplayName)(IBindCtx* /*pbc*/, LPOLESTR /*pszDisplayName*/, ULONG* /*pchEaten*/, IMoniker** /*ppmkOut*/);
	STDMETHOD(EnumObjects)(DWORD /*grfFlags*/, IEnumUnknown** ppenum);
	STDMETHOD(LockContainer)(BOOL fLock);
	// pointers
	CComPtr<IUnknown>            m_spUnknown;
	CComPtr<IOleObject>          m_spOleObject;
	CComPtr<IOleInPlaceFrame>    m_spInPlaceFrame;
	CComPtr<IOleInPlaceUIWindow> m_spInPlaceUIWindow;
	CComPtr<IViewObjectEx>       m_spViewObject;
	CComPtr<IOleInPlaceObjectWindowless> m_spInPlaceObjectWindowless;
	CComPtr<IDispatch>           m_spExternalDispatch;

	IID   m_iidSink;
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
	RECT  m_rcPos;

	// Accelerator table
	HACCEL m_hAccel;

	// Ambient property storage
	CComPtr<IServiceProvider>  m_spServices;
	DWORD    m_dwDocHostFlags;
	DWORD    m_dwDocHostDoubleClickFlags;
	CComBSTR m_bstrOptionKeyPath;
    BOOL     m_bSubclassed;

    CDWHtmlView* m_pHtmlView;
    CDWHtmlView* GetView() const;

	STDMETHOD(QueryService)( REFGUID rsid, REFIID riid, void** ppvObj);
    STDMETHOD(Exec)(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT *pvaIn, VARIANT *pvaOut);
    STDMETHOD(QueryStatus)(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD* prgCmds, OLECMDTEXT *pCmdText);
    STDMETHOD(ShowContextMenu)(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved);
    STDMETHOD(GetHostInfo)(DOCHOSTUIINFO *pInfo);
    STDMETHOD(ShowUI)(DWORD dwID, 
        IOleInPlaceActiveObject *pActiveObject, 
        IOleCommandTarget *pCommandTarget, 
        IOleInPlaceFrame *pFrame, 
        IOleInPlaceUIWindow *pDoc);

    STDMETHOD(HideUI)(void);
    STDMETHOD(UpdateUI)(void);
    STDMETHOD(EnableModeless)(BOOL fEnable);
    STDMETHOD(OnDocWindowActivate)(BOOL fActivate) ;
    STDMETHOD(OnFrameWindowActivate)(BOOL fActivate);
    STDMETHOD(ResizeBorder)(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow);
    STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID);
    STDMETHOD(GetOptionKeyPath)(LPOLESTR *pchKey, DWORD dw);
    STDMETHOD(GetDropTarget)(IDropTarget *pDropTarget, IDropTarget **ppDropTarget);
    STDMETHOD(GetExternal)(IDispatch **ppDispatch);
    STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut);
    STDMETHOD(FilterDataObject)(IDataObject *pDO, IDataObject **ppDORet);

};


}

#pragma pack(pop)
