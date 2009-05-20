// htmlViewTest.cpp : main source file for htmlViewTest.exe
//

#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>

#include "resource.h"

#include "htmlViewTestView.h"
#include "aboutdlg.h"
#include "MainFrm.h"
#include "DWAxHost.h"

#include "apihook/detours.h"


template<class Interface>
class CProxyObject : public Interface
{
public:

    CProxyObject( Interface * p ) :
      m_p(p),
          m_dwRef(1)
      {
      }

      virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
          /* [in] */ REFIID riid,
          /* [iid_is][out] */ void **ppvObject)
      {
          return m_p->QueryInterface( riid, ppvObject );
      }

      virtual ULONG STDMETHODCALLTYPE AddRef( void)
      {
          m_dwRef++;
          return m_p->AddRef();
      }

      virtual ULONG STDMETHODCALLTYPE Release( void)
      {
          Interface * p = m_p;

          if ( (--m_dwRef) == 0 )
              delete this;

          return p->Release();
      }


protected:
    Interface* m_p;
    DWORD m_dwRef;
};

#define WM_POPUPFLASH  (WM_USER+1000)
#define WM_ENDFLASH    (WM_USER+1001)

class CTopButWindow : public CWindowImpl< CTopButWindow, CButton , CControlWinTraits>
{
public:
    CTopButWindow()
    {
    }

    BEGIN_MSG_MAP(CTopButWindow)
        MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
    END_MSG_MAP()

    LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        POINT pt;
        RECT  rcWindow;

        GetWindowRect(&rcWindow);
        GetCursorPos(&pt);

        if ( ::PtInRect(&rcWindow, pt) )
        {
            ::SendMessage(m_hWndParent, WM_POPUPFLASH, 0,0);
        }

        return DefWindowProc();
    }

    HWND m_hWndParent;

};

class CFlashAxWindow : public CWindowImpl< CFlashAxWindow, CAxWindow , CFrameWinTraits>
{
public:
    CFlashAxWindow()
    {
    }

    BEGIN_MSG_MAP(CFlashAxWindow)
        MESSAGE_HANDLER(WM_DESTROY  , OnDestroy)
    END_MSG_MAP()

    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return DefWindowProc();
    }

    virtual void OnFinalMessage(HWND /*hWnd*/)
    {
        ::SendMessage(m_hWndParent, WM_ENDFLASH, 0,0);
    }

    HWND m_hWndParent;
};


class CMyShockwaveFlash :
    public CComObjectRootEx<CComSingleThreadModel>,
    public IOleControlImpl<CMyShockwaveFlash>,
    public IOleObjectImpl<CMyShockwaveFlash>,
    public IOleInPlaceActiveObjectImpl<CMyShockwaveFlash>,
    public IViewObjectExImpl<CMyShockwaveFlash>,
    public IOleInPlaceObjectWindowlessImpl<CMyShockwaveFlash>,
    public CComControl<CMyShockwaveFlash>,
    public CComCoClass<CMyShockwaveFlash>
{
    IUnknown* m_p;
    DWORD m_dwRef;

    CTopButWindow  m_wndBtn;
    HWND           m_hWndParent;

    CComObject<CAxHostWindow>*  m_pHostFlash;
    CFlashAxWindow m_wndPopupFlash;

    BOOL bCreated;

public:

    CMyShockwaveFlash( IUnknown * p = 0) :
        m_p(p)
    {
        m_dwRef = 1;
        m_pHostFlash = 0;
        bCreated = FALSE;
    }

    ~CMyShockwaveFlash()
    {
    }

    STDMETHOD(DoVerb)(LONG iVerb, LPMSG pMsg , IOleClientSite* pActiveSite, LONG  lindex ,
        HWND hwndParent, LPCRECT lprcPosRect)
    {
        m_spClientSite = pActiveSite;

        if ( !bCreated )
        {
            bCreated = TRUE;
            m_hWndParent = hwndParent;

            int nWidth  = lprcPosRect->right - lprcPosRect->left;
            int nHeight = lprcPosRect->bottom - lprcPosRect->top;

            CComQIPtr<IShockwaveFlash> spShockwaveFlash(m_p);

            _bstr_t bstrM = spShockwaveFlash->GetMovie();

            VARIANT_BOOL b = VARIANT_TRUE;
            spShockwaveFlash->get_Playing(&b);

            BOOL bGood = 
                StrStrI(bstrM, L"youku.com") != NULL ||
                StrStrI(bstrM, L"56.com") != NULL ||
                StrStrI(bstrM, L"video.sina.com") != NULL;

            if ( m_pHostFlash == NULL && 
                ((nWidth > 280 && nHeight >200 && b == VARIANT_TRUE ) || bGood ) )
            {
                m_pHostFlash = new CComObject<CAxHostWindow>;
                ATLASSERT(m_pHostFlash != NULL);
                if ( m_pHostFlash == NULL )
                    return S_FALSE;

                RECT rcClient = *lprcPosRect;

                Create( hwndParent, &rcClient, NULL, WS_CHILD | WS_VISIBLE );

                m_pHostFlash->AttachControl(m_p, m_hWnd);

                m_wndBtn.m_hWndParent = m_hWnd;
                m_wndBtn.Create( hwndParent, &rcClient, L"Google", 
                    WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE);

            }
        }

        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleObject> spIOleObject(m_p);

            return spIOleObject->DoVerb(iVerb, pMsg, pActiveSite,lindex,hwndParent,lprcPosRect);
        }

        return IOleObjectImpl<CMyShockwaveFlash>::DoVerb( iVerb, pMsg, pActiveSite, lindex, hwndParent, lprcPosRect);
    }

    STDMETHOD(SetClientSite)(IOleClientSite *pClientSite)
    {
        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleObject> spIOleObject(m_p);
            return spIOleObject->SetClientSite(pClientSite);
        }

        return IOleObject_SetClientSite(pClientSite);
    }
    STDMETHOD(GetClientSite)(IOleClientSite **ppClientSite)
    {
        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleObject> spIOleObject(m_p);
            return spIOleObject->GetClientSite(ppClientSite);
        }

        return IOleObject_GetClientSite(ppClientSite);
    }
    STDMETHOD(SetHostNames)(LPCOLESTR szContainerApp , LPCOLESTR szContainerObj)
    {
        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleObject> spIOleObject(m_p);
            return spIOleObject->SetHostNames(szContainerApp,szContainerObj);
        }

        return S_OK;
    }
    STDMETHOD(Close)(DWORD dwSaveOption)
    {
        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleObject> spIOleObject(m_p);
            return spIOleObject->Close(dwSaveOption);
        }

        return IOleObject_Close(dwSaveOption);
    }
    STDMETHOD(SetMoniker)(DWORD dwWhichMoniker , IMoniker* pmk )
    {
        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleObject> spIOleObject(m_p);
            return spIOleObject->SetMoniker(dwWhichMoniker, pmk);
        }

        ATLTRACENOTIMPL(_T("IOleObjectImpl::SetMoniker"));
    }
    STDMETHOD(GetMoniker)(DWORD  dwAssign , DWORD  dwWhichMoniker , IMoniker**  ppmk )
    {
        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleObject> spIOleObject(m_p);
            return spIOleObject->GetMoniker(dwAssign, dwWhichMoniker, ppmk);
        }

        ATLTRACENOTIMPL(_T("IOleObjectImpl::GetMoniker"));
    }
    STDMETHOD(InitFromData)(IDataObject*  pDataObject , BOOL  fCreation , DWORD  dwReserved)
    {
        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleObject> spIOleObject(m_p);
            return spIOleObject->InitFromData(pDataObject, fCreation, dwReserved);
        }

        ATLTRACENOTIMPL(_T("IOleObjectImpl::InitFromData"));
    }
    STDMETHOD(GetClipboardData)(DWORD dwReserved , IDataObject** ppDataObject)
    {
        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleObject> spIOleObject(m_p);
            return spIOleObject->GetClipboardData(dwReserved, ppDataObject);
        }

        ATLTRACENOTIMPL(_T("IOleObjectImpl::GetClipboardData"));
    }

    STDMETHOD(EnumVerbs)(IEnumOLEVERB **ppEnumOleVerb)
    {
        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleObject> spIOleObject(m_p);
            return spIOleObject->EnumVerbs(ppEnumOleVerb);
        }

        if (!ppEnumOleVerb)
            return E_POINTER;
        return OleRegEnumVerbs(GetObjectCLSID(), ppEnumOleVerb);
    }
    STDMETHOD(Update)(void)
    {
        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleObject> spIOleObject(m_p);
            return spIOleObject->Update();
        }

        return S_OK;
    }
    STDMETHOD(IsUpToDate)(void)
    {
        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleObject> spIOleObject(m_p);
            return spIOleObject->IsUpToDate();
        }

        return S_OK;
    }
    STDMETHOD(GetUserClassID)(CLSID *pClsid)
    {
        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleObject> spIOleObject(m_p);
            return spIOleObject->GetUserClassID(pClsid);
        }

        if (!pClsid)
            return E_POINTER;
        *pClsid = GetObjectCLSID();
        return S_OK;
    }
    STDMETHOD(GetUserType)(DWORD dwFormOfType, LPOLESTR *pszUserType)
    {
        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleObject> spIOleObject(m_p);
            return spIOleObject->GetUserType(dwFormOfType,pszUserType);
        }

        return OleRegGetUserType(GetObjectCLSID(), dwFormOfType, pszUserType);
    }
    STDMETHOD(SetExtent)(DWORD dwDrawAspect, SIZEL *psizel)
    {
        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleObject> spIOleObject(m_p);
            return spIOleObject->SetExtent(dwDrawAspect,psizel);
        }

        return IOleObject_SetExtent(dwDrawAspect, psizel);
    }
    STDMETHOD(GetExtent)(DWORD dwDrawAspect, SIZEL *psizel)
    {
        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleObject> spIOleObject(m_p);
            return spIOleObject->GetExtent(dwDrawAspect,psizel);
        }

        if (dwDrawAspect != DVASPECT_CONTENT)
            return E_FAIL;
        if (psizel == NULL)
            return E_POINTER;
        *psizel = m_sizeExtent;
        return S_OK;
    }
    STDMETHOD(Advise)(IAdviseSink *pAdvSink, DWORD *pdwConnection)
    {
        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleObject> spIOleObject(m_p);
            return spIOleObject->Advise(pAdvSink,pdwConnection);
        }

        return IOleObject_Advise(pAdvSink, pdwConnection);
    }
    STDMETHOD(Unadvise)(DWORD dwConnection)
    {
        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleObject> spIOleObject(m_p);
            return spIOleObject->Unadvise(dwConnection);
        }

        HRESULT hRes = E_FAIL;
        if (m_spOleAdviseHolder != NULL)
            hRes = m_spOleAdviseHolder->Unadvise(dwConnection);
        return hRes;
    }
    STDMETHOD(EnumAdvise)(IEnumSTATDATA **ppenumAdvise)
    {
        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleObject> spIOleObject(m_p);
            return spIOleObject->EnumAdvise(ppenumAdvise);
        }

        if (ppenumAdvise == NULL)
            return E_POINTER;
        *ppenumAdvise = NULL;
        HRESULT hRes = E_FAIL;
        if (m_spOleAdviseHolder != NULL)
            hRes = m_spOleAdviseHolder->EnumAdvise(ppenumAdvise);
        return hRes;
    }
    STDMETHOD(GetMiscStatus)(DWORD dwAspect, DWORD *pdwStatus)
    {
        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleObject> spIOleObject(m_p);
            return spIOleObject->GetMiscStatus(dwAspect,pdwStatus);
        }

        return OleRegGetMiscStatus(GetObjectCLSID(), dwAspect, pdwStatus);
    }
    STDMETHOD(SetColorScheme)(LOGPALETTE* pLogpal)
    {
        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleObject> spIOleObject(m_p);
            return spIOleObject->SetColorScheme(pLogpal);
        }

        ATLTRACENOTIMPL(_T("IOleObjectImpl::SetColorScheme"));
    }

    //////////////////////////////////////////////////////////////////////////

    STDMETHOD(SetObjectRects)(LPCRECT prcPos,LPCRECT prcClip)
    {
        if (prcPos == NULL || prcClip == NULL)
            return E_POINTER;

        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleInPlaceObject> spOleInPlaceObject(m_p);

            return spOleInPlaceObject->SetObjectRects(prcPos, prcClip);
        }

        m_rcPos = *prcPos;
        if ( IsWindow() )
        {
            if ( GetStyle() & WS_CHILD )
            {
                RECT rcIXect;
                BOOL b = IntersectRect(&rcIXect, prcPos, prcClip);
                HRGN tempRgn = NULL;
                if (b && !EqualRect(&rcIXect, prcPos))
                {
                    OffsetRect(&rcIXect, -(prcPos->left), -(prcPos->top));
                    tempRgn = CreateRectRgnIndirect(&rcIXect);
                }

                SetWindowRgn( tempRgn, TRUE);

                SIZEL size = {prcPos->right - prcPos->left, prcPos->bottom - prcPos->top};

                SetWindowPos( NULL, prcPos->left,
                    prcPos->top, size.cx, size.cy, 
                    SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
            }
        }

        if ( m_pHostFlash != NULL )
        {
            RECT rcBtn    = *prcPos;
            RECT rcParent = { 0 };

            CWindow wndParent = m_hWndParent;       

            wndParent.GetClientRect(&rcParent);

            if ( rcBtn.left < rcParent.left )
                rcBtn.left = rcParent.left;
            if ( rcBtn.right > rcParent.right )
                rcBtn.right = rcParent.right;

            wndParent.ClientToScreen( &rcBtn );

            rcBtn.top   += 1;
            rcBtn.right -= 1;

            rcBtn.bottom = rcBtn.top   + 20;
            rcBtn.left   = rcBtn.right - 100;

            if ( m_wndBtn.IsWindow() )
                m_wndBtn.MoveWindow( &rcBtn );
        }


        return S_OK;
    }


    STDMETHOD(GetWindow)(HWND* phwnd)
    {
        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleInPlaceObject> spOleInPlaceObject(m_p);

            return spOleInPlaceObject->GetWindow(phwnd);
        }

        HRESULT hRes = E_POINTER;

        if (m_bWasOnceWindowless)
            return E_FAIL;

        if (phwnd != NULL)
        {
            *phwnd = m_hWnd;
            hRes = (*phwnd == NULL) ? E_UNEXPECTED : S_OK;
        }
        return hRes;
    }
    STDMETHOD(ContextSensitiveHelp)(BOOL  fEnterMode )
    {
        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleInPlaceObject> spOleInPlaceObject(m_p);

            return spOleInPlaceObject->ContextSensitiveHelp(fEnterMode);
        }

        ATLTRACENOTIMPL(_T("IOleInPlaceObjectWindowlessImpl::ContextSensitiveHelp"));
    }

    STDMETHOD(InPlaceDeactivate)(void)
    {
        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleInPlaceObject> spOleInPlaceObject(m_p);

            return spOleInPlaceObject->InPlaceDeactivate();
        }

        return IOleInPlaceObject_InPlaceDeactivate();
    }
    STDMETHOD(UIDeactivate)(void)
    {
        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleInPlaceObject> spOleInPlaceObject(m_p);

            return spOleInPlaceObject->UIDeactivate();
        }

        return IOleInPlaceObject_UIDeactivate();
    }
    STDMETHOD(ReactivateAndUndo)(void)
    {
        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleInPlaceObject> spOleInPlaceObject(m_p);

            return spOleInPlaceObject->ReactivateAndUndo();
        }

        ATLTRACENOTIMPL(_T("IOleInPlaceObjectWindowlessImpl::ReactivateAndUndo"));
    }

    // IOleInPlaceObjectWindowless
    //
    STDMETHOD(OnWindowMessage)(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult)
    {
        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleInPlaceObjectWindowless> spOleInPlaceObject(m_p);

            return spOleInPlaceObject->OnWindowMessage(msg,wParam,lParam,plResult);
        }

        _ATL_MSG message(NULL, msg, wParam, lParam);
        const _ATL_MSG* pOldMsg = m_pCurrentMsg;
        m_pCurrentMsg = &message;
        BOOL b = ProcessWindowMessage(m_hWnd, msg, wParam, lParam, *plResult);
        // restore saved value for the current message
        ATLASSERT(m_pCurrentMsg == &message);
        m_pCurrentMsg = pOldMsg;
        return b ? S_OK : S_FALSE;
    }

    STDMETHOD(GetDropTarget)(IDropTarget** ppDropTarget )
    {
        if ( m_pHostFlash == NULL )
        {
            CComQIPtr<IOleInPlaceObjectWindowless> spOleInPlaceObject(m_p);

            return spOleInPlaceObject->GetDropTarget(ppDropTarget);
        }

        ATLTRACENOTIMPL(_T("IOleInPlaceObjectWindowlessImpl::GetDropTarget"));
    }

    virtual HRESULT OnDrawAdvanced(ATL_DRAWINFO& di)
    {
        return S_OK;
    }


    BEGIN_MSG_MAP(thisClass)
        MESSAGE_HANDLER(WM_SETFOCUS     , CComControlBase::OnSetFocus)
        MESSAGE_HANDLER(WM_KILLFOCUS    , CComControlBase::OnKillFocus)
        MESSAGE_HANDLER(WM_MOUSEACTIVATE, CComControlBase::OnMouseActivate)
        MESSAGE_HANDLER(WM_DESTROY      , OnDestroy)

        MESSAGE_HANDLER(WM_POPUPFLASH  , OnPopupFlash )
        MESSAGE_HANDLER(WM_ENDFLASH    , OnEndPopupFlash )

    END_MSG_MAP()

    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        if ( m_wndBtn.IsWindow() )
            m_wndBtn.DestroyWindow();
        return DefWindowProc();
    }

    LRESULT OnPopupFlash(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        if ( m_pHostFlash != NULL )
        {
            if ( !m_wndPopupFlash.IsWindow() )
            {
                RECT rcWindow;
                GetWindowRect(&rcWindow);

                m_wndPopupFlash.m_hWndParent = m_hWnd;
                m_wndPopupFlash.Create(m_hWnd, rcWindow, NULL, 
                    WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0);
                
                CComQIPtr<IShockwaveFlash> spShockwaveFlash(m_p);

                m_pHostFlash->GetControllingUnknown()->AddRef();
                m_pHostFlash->ReleaseAll();

                m_wndPopupFlash.AttachControl(m_p, NULL);
                ::SetForegroundWindow(m_wndPopupFlash);
                m_wndPopupFlash.SetFocus();
                m_wndPopupFlash.SendMessage(WM_MOUSEACTIVATE, 0,0);

            }
        }

        return 0L;
    }

    LRESULT OnEndPopupFlash(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        if ( m_pHostFlash != NULL )
        {
            ATLASSERT(IsWindow());
            CComQIPtr<IShockwaveFlash> spShockwaveFlash(m_p);
            
            m_pHostFlash->AttachControl( m_p, m_hWnd );

            CComQIPtr<IOleObject> spIOleObject(m_p);

            ::SetForegroundWindow(m_hWndParent);
            SetFocus();
            SendMessage(WM_MOUSEACTIVATE, 0,0);

        }
        return 0;
    }

    virtual ULONG STDMETHODCALLTYPE AddRef( void)
    {
        m_dwRef++;
        return m_p->AddRef();
    }

    virtual ULONG STDMETHODCALLTYPE Release( void)
    {
        IUnknown * p = m_p;

        if ( (--m_dwRef) == 0 )
            delete this;

        return p->Release();
    }

    LPUNKNOWN GetUnknown()
    {
        return (LPUNKNOWN)(IViewObjectEx*)this;
    }
    virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void **ppvObject)
    {

        //if (riid == __uuidof(IViewObjectEx)) 
        //{
        //    *ppvObject = (IViewObjectEx*)this;
        //    AddRef();
        //    return S_OK;
        //}
        //else if (riid == __uuidof(IViewObject2)) 
        //{
        //    *ppvObject = (IViewObject2*)this;
        //    AddRef();
        //    return S_OK;
        //}
        //else if (riid == __uuidof(IViewObject)) 
        //{
        //    *ppvObject = (IViewObject*)this;
        //    AddRef();
        //    return S_OK;
        //}
        if (riid == __uuidof(IOleInPlaceObjectWindowless)) 
        {
            *ppvObject = (IOleInPlaceObjectWindowless*)this;
            AddRef();
            return S_OK;
        }
        else if (riid == __uuidof(IOleInPlaceObject)) 
        {
            *ppvObject = (IOleInPlaceObject*)this;
            AddRef();
            return S_OK;
        }
        //else if (riid == __uuidof(IOleInPlaceActiveObject)) 
        //{
        //    *ppvObject = (IOleInPlaceActiveObject*)this;
        //    AddRef();
        //    return S_OK;
        //}
        //else if (riid == __uuidof(IOleControl)) 
        //{
        //    *ppvObject = (IOleControl*)this;
        //    AddRef();
        //    return S_OK;
        //}
        else if (riid == __uuidof(IOleObject)) 
        {
            *ppvObject = (IOleObject*)this;
            AddRef();
            return S_OK;
        }

        return m_p->QueryInterface( riid, ppvObject );
    }

};

class MyClassFactory : public CProxyObject<IClassFactory> 
{
public:
    MyClassFactory( IClassFactory * p ) :
      CProxyObject<IClassFactory>(p)
      {
      }

      HRESULT STDMETHODCALLTYPE CreateInstance( 
          /* [unique][in] */ IUnknown *pUnkOuter,
          /* [in] */ REFIID riid,
          /* [iid_is][out] */ void **ppvObject)
      {
          HRESULT hr = m_p->CreateInstance(pUnkOuter, riid, ppvObject);

          if ( SUCCEEDED(hr) ) //&& riid == __uuidof(IShockwaveFlash) )
              *ppvObject = new CMyShockwaveFlash((IShockwaveFlash*)(*ppvObject));

          return hr;
      }

      HRESULT STDMETHODCALLTYPE LockServer( 
          /* [in] */ BOOL fLock)
      {
          return m_p->LockServer(fLock);
      }
};


static HRESULT (WINAPI * OldCoGetClassObject)(IN REFCLSID rclsid, IN DWORD dwClsContext, IN LPVOID pvReserved,
                                              IN REFIID riid, OUT LPVOID FAR* ppv) = CoGetClassObject;


HRESULT WINAPI  MyCoGetClassObject(IN REFCLSID rclsid, IN DWORD dwClsContext, IN LPVOID pvReserved,
                                   IN REFIID riid, OUT LPVOID FAR* ppv)
{

    HRESULT hResult = OldCoGetClassObject(rclsid, dwClsContext, pvReserved, riid, ppv);

    if ( rclsid == __uuidof(ShockwaveFlash) && riid == __uuidof(IClassFactory) )
    {
        *ppv = new MyClassFactory((IClassFactory*)(*ppv));
    }
    
    return hResult;
}



CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainFrame wndMain;

	if(wndMain.CreateEx() == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

	wndMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach((void**)&OldCoGetClassObject, MyCoGetClassObject);
    DetourTransactionCommit();

	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	AtlAxWinInit();

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
