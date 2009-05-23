#include "stdafx.h"
#include "DWExtractVideo.h"
#include "resource.h"
#include "DWFrameUI.h"
#include "DWAxHost.h"
#include "detours/detours.h"
#import  "c:\\windows\\system32\\macromed\\flash\\Flash10b.ocx"

#include <atlctl.h>

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


class IPopupFlashEvent
{
public:
    virtual void OnPopupFlash() = 0;
    virtual void OnEndPopupFlash() = 0;
};

class CTopButWindow : public CWindowImpl< CTopButWindow, CButton , CControlWinTraits>
{
public:
    CTopButWindow()
    {
    }

    BEGIN_MSG_MAP(CTopButWindow)
        MESSAGE_HANDLER(WM_CREATE   , OnCreate   )
        MESSAGE_HANDLER(WM_DESTROY  , OnDestroy  )
        MESSAGE_HANDLER(WM_TIMER    , OnTimer    )
        MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
    END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        SetTimer( 1001, 500 );
        return DefWindowProc();
    }

    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {       
        return DefWindowProc();
    }


    LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        POINT pt;
        RECT  rcWindow = { 0 };
        RECT  rcBtnWindow = { 0 };

        ::GetWindowRect(m_hWndParent, &rcWindow);
        GetWindowRect(&rcBtnWindow);
        ::GetCursorPos(&pt);

        POINT pt1 = { rcBtnWindow.left, rcBtnWindow.top };
        POINT pt2 = { rcBtnWindow.right, rcBtnWindow.bottom };;

        if ( PtInRect(&rcWindow, pt) && PtInRect(&m_rcFlash, pt) && !m_bPopupFlash)
        {
            RECT rcBtn = m_rcFlash;

            if ( rcBtn.top < rcWindow.top )
                rcBtn.top = rcWindow.top;
            if ( rcBtn.left < rcWindow.left )
                rcBtn.left = rcWindow.left;
            if ( rcBtn.right > rcWindow.right )
                rcBtn.right = rcWindow.right;
            if ( rcBtn.bottom > rcWindow.bottom )
                rcBtn.bottom = rcWindow.bottom;

            rcBtn.top   += 1;
            rcBtn.right -= 1;

            rcBtn.bottom = rcBtn.top   + 20;
            rcBtn.left   = rcBtn.right - 80;

            MoveWindow(&rcBtn);
            ShowWindow(SW_SHOWNOACTIVATE);
        }
        else
        {
            ShowWindow(SW_HIDE);
        }

        return DefWindowProc();
    }

    LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        POINT pt;
        RECT  rcWindow;

        GetWindowRect(&rcWindow);
        GetCursorPos(&pt);

        if ( ::PtInRect(&rcWindow, pt) )
        {
            m_pflashEvent->OnPopupFlash();
        }

        return DefWindowProc();
    }

    BOOL m_bPopupFlash;
    HWND m_hWndParent;
    RECT m_rcFlash;
    IPopupFlashEvent* m_pflashEvent;

};

class CFlashHostWindow : public DWAxHost::CDWAxHost
{
public:

    ~CFlashHostWindow()
    {
        int a = 0;
    }

    HRESULT ActivateAx(IUnknown* pUnkControl, bool bInited, IStream* pStream)
    {
        if (pUnkControl == NULL)
            return S_OK;

        m_spUnknown = pUnkControl;

        HRESULT hr = S_OK;
        pUnkControl->QueryInterface(__uuidof(IOleObject), (void**)&m_spOleObject);
        if (m_spOleObject)
        {
            CComQIPtr<IOleClientSite> spClientSite(GetControllingUnknown());
            m_spOleObject->SetClientSite(spClientSite);

            GetClientRect(&m_rcPos);
            m_pxSize.cx = m_rcPos.right - m_rcPos.left;
            m_pxSize.cy = m_rcPos.bottom - m_rcPos.top;
            AtlPixelToHiMetric(&m_pxSize, &m_hmSize);
            m_spOleObject->SetExtent(DVASPECT_CONTENT, &m_hmSize);
            m_spOleObject->GetExtent(DVASPECT_CONTENT, &m_hmSize);
            AtlHiMetricToPixel(&m_hmSize, &m_pxSize);
            m_rcPos.right = m_rcPos.left + m_pxSize.cx;
            m_rcPos.bottom = m_rcPos.top + m_pxSize.cy;

            CComQIPtr<IAdviseSink> spAdviseSink(GetControllingUnknown());
            m_spOleObject->Advise(spAdviseSink, &m_dwOleObject);

            hr = m_spOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, spClientSite, 0, m_hWnd, &m_rcPos);
            RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_FRAME);
        }


        return hr;
    }

    STDMETHOD(AttachControl)(IUnknown* pUnkControl, HWND hWnd)
    {
        HRESULT hr = S_FALSE;

        ReleaseAll();

        bool bReleaseWindowOnFailure = false; // Used to keep track of whether we subclass the window

        if ((m_hWnd != NULL) && (m_hWnd != hWnd)) // Don't release the window if it's the same as the one we already subclass/own
        {
            RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_FRAME);
            ReleaseWindow();
        }

        if (::IsWindow(hWnd))
        {
            if (m_hWnd != hWnd) // Don't need to subclass the window if we already own it
            {
                SubclassWindow(hWnd);
                bReleaseWindowOnFailure = true;
            }

            hr = ActivateAx(pUnkControl, true, NULL);

            if (FAILED(hr))
            {
                ReleaseAll();

                if (m_hWnd != NULL)
                {
                    RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_FRAME);
                    if (bReleaseWindowOnFailure) // We subclassed the window in an attempt to create this control, so we unsubclass on failure
                        ReleaseWindow();
                }
            }
        }
        return hr;
    }

};

class CMyAxWindow : public CWindowImpl< CMyAxWindow, CWindow >
{
public:

    CMyAxWindow()
    {
        m_pHostAx = NULL;
    }


    ~CMyAxWindow()
    {
    }

    BEGIN_MSG_MAP(CMyAxWindow)
        MESSAGE_HANDLER(WM_CREATE   , OnCreate   )
        MESSAGE_HANDLER(WM_DESTROY  , OnDestroy )
    END_MSG_MAP()

    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {       
        return 0L;
    }

    virtual void OnFinalMessage(HWND /*hWnd*/)
    {
        if ( m_wndParent.IsWindow() )
            m_wndParent.DestroyWindow();

        m_pHostAx = NULL;
    }


    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return DefWindowProc();
    }

    HRESULT AttachControl(IUnknown* pUnkControl)
    {
        ATLASSERT(m_pHostAx == NULL);
        m_pHostAx = new CComObject<CFlashHostWindow>;
        ATLASSERT(m_pHostAx != NULL);

        return m_pHostAx->AttachControl(pUnkControl, m_hWnd);
    }

    CWindow m_wndParent;

    CComObject<CFlashHostWindow>* m_pHostAx;
};

class CAudioAxWindow : 
    public CWindowImpl< CAudioAxWindow, CWindow , CFrameWinTraits>,
    public CDWFrameUIImpl<CAudioAxWindow>
{
public:
    CAudioAxWindow()
    {
        m_pflashEvent = NULL;
        m_hWndParent  = NULL;
    }

    ~CAudioAxWindow()
    {
        m_pflashEvent = NULL;
        if ( IsWindow() )
            DestroyWindow();
    }

    BEGIN_MSG_MAP(CAudioAxWindow)
        CHAIN_MSG_MAP(CDWFrameUIImpl<CAudioAxWindow>)
        MESSAGE_HANDLER(WM_CREATE   , OnCreate   )
        MESSAGE_HANDLER(WM_DESTROY  , OnDestroy  )
        MESSAGE_HANDLER(WM_SIZE     , OnSize     )
    END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        m_wndAxFlash.m_wndParent = m_hWnd;
        m_wndAxFlash.Create( m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE );

        return DefWindowProc();
    }

    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        m_wndAxFlash.m_wndParent.m_hWnd = NULL;

        return DefWindowProc();
    }

    LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        RECT rcClient;
        GetClientRect(&rcClient);

        rcClient.top += 22;

        if ( m_wndAxFlash.IsWindow() )
            m_wndAxFlash.MoveWindow(&rcClient, FALSE);

        return DefWindowProc();
    }

    void GetChildFrmRect(RECT& rcChildFrm)
    {
        memset(&rcChildFrm, 0, sizeof(rcChildFrm));
    }


    virtual void OnFinalMessage(HWND /*hWnd*/)
    {
        if ( m_pflashEvent != NULL )
            m_pflashEvent->OnEndPopupFlash();
    }

    CMyAxWindow m_wndAxFlash;

    HWND m_hWndParent;
    IPopupFlashEvent* m_pflashEvent;

};


class CDWExtractVideoProxy :
    public CComObjectRootEx<CComSingleThreadModel>,
    public IOleControlImpl<CDWExtractVideoProxy>,
    public IOleObjectImpl<CDWExtractVideoProxy>,
    public IOleInPlaceActiveObjectImpl<CDWExtractVideoProxy>,
    public IViewObjectExImpl<CDWExtractVideoProxy>,
    public IOleInPlaceObjectWindowlessImpl<CDWExtractVideoProxy>,
    public CComControl<CDWExtractVideoProxy>,
    public CComCoClass<CDWExtractVideoProxy>,
    public IPopupFlashEvent
{
    IUnknown* m_p;
    DWORD m_dwRef;

    CTopButWindow  m_wndBtn;
    HWND           m_hWndParent;

    CAudioAxWindow m_wndPopupFlash;

    RECT m_rcWndFlash;
    CComPtr<IOleClientSite> m_spOleClientSite;

public:

    CDWExtractVideoProxy( IUnknown * p = 0) :
      m_p(p)
      {
          m_dwRef = 1;
      }

      ~CDWExtractVideoProxy()
      {
          if ( m_wndBtn.IsWindow() )
              m_wndBtn.DestroyWindow();
      }

      STDMETHOD(DoVerb)(LONG iVerb, LPMSG pMsg , IOleClientSite* pActiveSite, LONG  lindex ,
          HWND hwndParent, LPCRECT lprcPosRect)
      {
          m_hWndParent = hwndParent;

          //CComQIPtr<ShockwaveFlashObjects::IShockwaveFlash> spShockwaveFlash(m_p);
          //if ( spShockwaveFlash.p != NULL )
          //{
          //    _bstr_t bstr(L"always");
          //    spShockwaveFlash->PutAllowScriptAccess(bstr);
          //    spShockwaveFlash->DisableLocalSecurity();
          //}

          CComQIPtr<IOleObject> spIOleObject(m_p);
          return spIOleObject->DoVerb(iVerb, pMsg, pActiveSite,lindex,hwndParent,lprcPosRect);

      }

      STDMETHOD(SetClientSite)(IOleClientSite *pClientSite)
      {
          CComQIPtr<IOleObject> spIOleObject(m_p);
          return spIOleObject->SetClientSite(pClientSite);
      }
      STDMETHOD(GetClientSite)(IOleClientSite **ppClientSite)
      {
          CComQIPtr<IOleObject> spIOleObject(m_p);
          return spIOleObject->GetClientSite(ppClientSite);
      }
      STDMETHOD(SetHostNames)(LPCOLESTR szContainerApp , LPCOLESTR szContainerObj)
      {
          CComQIPtr<IOleObject> spIOleObject(m_p);
          return spIOleObject->SetHostNames(szContainerApp,szContainerObj);
      }
      STDMETHOD(Close)(DWORD dwSaveOption)
      {
          CComQIPtr<IOleObject> spIOleObject(m_p);
          return spIOleObject->Close(dwSaveOption);
      }
      STDMETHOD(SetMoniker)(DWORD dwWhichMoniker , IMoniker* pmk )
      {
          CComQIPtr<IOleObject> spIOleObject(m_p);
          return spIOleObject->SetMoniker(dwWhichMoniker, pmk);
      }
      STDMETHOD(GetMoniker)(DWORD  dwAssign , DWORD  dwWhichMoniker , IMoniker**  ppmk )
      {
          CComQIPtr<IOleObject> spIOleObject(m_p);
          return spIOleObject->GetMoniker(dwAssign, dwWhichMoniker, ppmk);
      }
      STDMETHOD(InitFromData)(IDataObject*  pDataObject , BOOL  fCreation , DWORD  dwReserved)
      {
          CComQIPtr<IOleObject> spIOleObject(m_p);
          return spIOleObject->InitFromData(pDataObject, fCreation, dwReserved);
      }
      STDMETHOD(GetClipboardData)(DWORD dwReserved , IDataObject** ppDataObject)
      {
          CComQIPtr<IOleObject> spIOleObject(m_p);
          return spIOleObject->GetClipboardData(dwReserved, ppDataObject);
      }

      STDMETHOD(EnumVerbs)(IEnumOLEVERB **ppEnumOleVerb)
      {
          CComQIPtr<IOleObject> spIOleObject(m_p);
          return spIOleObject->EnumVerbs(ppEnumOleVerb);
      }
      STDMETHOD(Update)(void)
      {
          CComQIPtr<IOleObject> spIOleObject(m_p);
          return spIOleObject->Update();
      }
      STDMETHOD(IsUpToDate)(void)
      {
          CComQIPtr<IOleObject> spIOleObject(m_p);
          return spIOleObject->IsUpToDate();
      }
      STDMETHOD(GetUserClassID)(CLSID *pClsid)
      {
          CComQIPtr<IOleObject> spIOleObject(m_p);
          return spIOleObject->GetUserClassID(pClsid);
      }
      STDMETHOD(GetUserType)(DWORD dwFormOfType, LPOLESTR *pszUserType)
      {
          CComQIPtr<IOleObject> spIOleObject(m_p);
          return spIOleObject->GetUserType(dwFormOfType,pszUserType);
      }
      STDMETHOD(SetExtent)(DWORD dwDrawAspect, SIZEL *psizel)
      {
          CComQIPtr<IOleObject> spIOleObject(m_p);
          return spIOleObject->SetExtent(dwDrawAspect,psizel);
      }
      STDMETHOD(GetExtent)(DWORD dwDrawAspect, SIZEL *psizel)
      {
          CComQIPtr<IOleObject> spIOleObject(m_p);
          return spIOleObject->GetExtent(dwDrawAspect,psizel);
      }
      STDMETHOD(Advise)(IAdviseSink *pAdvSink, DWORD *pdwConnection)
      {
          CComQIPtr<IOleObject> spIOleObject(m_p);
          return spIOleObject->Advise(pAdvSink,pdwConnection);
      }
      STDMETHOD(Unadvise)(DWORD dwConnection)
      {
          CComQIPtr<IOleObject> spIOleObject(m_p);
          return spIOleObject->Unadvise(dwConnection);
      }
      STDMETHOD(EnumAdvise)(IEnumSTATDATA **ppenumAdvise)
      {
          CComQIPtr<IOleObject> spIOleObject(m_p);
          return spIOleObject->EnumAdvise(ppenumAdvise);
      }
      STDMETHOD(GetMiscStatus)(DWORD dwAspect, DWORD *pdwStatus)
      {
          CComQIPtr<IOleObject> spIOleObject(m_p);
          return spIOleObject->GetMiscStatus(dwAspect,pdwStatus);
      }
      STDMETHOD(SetColorScheme)(LOGPALETTE* pLogpal)
      {
          CComQIPtr<IOleObject> spIOleObject(m_p);
          return spIOleObject->SetColorScheme(pLogpal);
      }

      //////////////////////////////////////////////////////////////////////////

      STDMETHOD(SetObjectRects)(LPCRECT prcPos,LPCRECT prcClip)
      {
          if (prcPos == NULL || prcClip == NULL)
              return E_POINTER;

          memcpy(&m_rcWndFlash, prcPos, sizeof(m_rcWndFlash));

          while ( TRUE && ::IsWindow(m_hWndParent))
          {
              RECT rcBtn    = *prcPos;
              RECT rcParent = { 0 };
              RECT rcFlash  = *prcPos;

              CWindow wndParent = m_hWndParent;
              wndParent.ClientToScreen(&rcFlash);
              m_wndBtn.m_rcFlash = rcFlash;

              int nWidth  = rcBtn.right - rcBtn.left;
              int nHeight = rcBtn.bottom - rcBtn.top;

              if ( nWidth < 100 || nHeight < 80 )
              {
                  if ( m_wndBtn.IsWindow() )
                      m_wndBtn.ShowWindow(SW_HIDE);
              }
              else
              {
                  if ( !m_wndBtn.IsWindow() && !FilterAudio() )
                  {
                      m_wndBtn.m_bPopupFlash = FALSE;
                      m_wndBtn.m_hWndParent  = m_hWndParent;
                      m_wndBtn.m_pflashEvent = this;

                      m_wndBtn.Create( m_hWndParent, &rcDefault, L"ÌáÈ¡²¥·Å", 
                          WS_POPUP , WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE);
                      m_wndBtn.SetFont(::AtlGetDefaultGuiFont());

                  }
              }

              break;
          }

          CComQIPtr<IOleInPlaceObject> spOleInPlaceObject(m_p);

          return spOleInPlaceObject->SetObjectRects(prcPos, prcClip);
      }

      BOOL FilterAudio()
      {
          CComQIPtr<ShockwaveFlashObjects::IShockwaveFlash> spShockwaveFlash(m_p);
          if ( spShockwaveFlash.p != NULL )
          {
              VARIANT_BOOL bPlaying = VARIANT_FALSE;
              CComBSTR bstrMovie;
              spShockwaveFlash->get_Movie(&bstrMovie);
              spShockwaveFlash->raw_IsPlaying(&bPlaying);

              if ( bPlaying == VARIANT_FALSE && 
                  StrStrI(bstrMovie, L"s2.bai.itc.cn") != NULL)
              {
                  return TRUE;
              }
          }

          return FALSE;
      }

      BOOL FixPlayingFlash()
      {
          CComQIPtr<ShockwaveFlashObjects::IShockwaveFlash> spShockwaveFlash(m_p);
          if ( spShockwaveFlash.p != NULL )
          {
              spShockwaveFlash->put_Loop(VARIANT_TRUE);
          }
          return TRUE;
      }

      ATL::CString GetAudioTitle()
      {
          ATL::CString strRet;

          CComQIPtr<ShockwaveFlashObjects::IShockwaveFlash> spShockwaveFlash(m_p);
          if ( spShockwaveFlash.p != NULL )
          {
              CComBSTR bstrRet;
              spShockwaveFlash->get_Movie(&bstrRet);
              strRet = bstrRet;
          }

          return strRet;
      }

      STDMETHOD(GetWindow)(HWND* phwnd)
      {
          CComQIPtr<IOleInPlaceObject> spOleInPlaceObject(m_p);
          return spOleInPlaceObject->GetWindow(phwnd);
      }
      STDMETHOD(ContextSensitiveHelp)(BOOL  fEnterMode )
      {
          CComQIPtr<IOleInPlaceObject> spOleInPlaceObject(m_p);
          return spOleInPlaceObject->ContextSensitiveHelp(fEnterMode);
      }

      STDMETHOD(InPlaceDeactivate)(void)
      {
          CComQIPtr<IOleInPlaceObject> spOleInPlaceObject(m_p);
          return spOleInPlaceObject->InPlaceDeactivate();
      }
      STDMETHOD(UIDeactivate)(void)
      {
          CComQIPtr<IOleInPlaceObject> spOleInPlaceObject(m_p);
          return spOleInPlaceObject->UIDeactivate();
      }
      STDMETHOD(ReactivateAndUndo)(void)
      {
          CComQIPtr<IOleInPlaceObject> spOleInPlaceObject(m_p);
          return spOleInPlaceObject->ReactivateAndUndo();
      }

      // IOleInPlaceObjectWindowless
      //
      STDMETHOD(OnWindowMessage)(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult)
      {
          CComQIPtr<IOleInPlaceObjectWindowless> spOleInPlaceObject(m_p);
          return spOleInPlaceObject->OnWindowMessage(msg,wParam,lParam,plResult);
      }

      STDMETHOD(GetDropTarget)(IDropTarget** ppDropTarget )
      {
          CComQIPtr<IOleInPlaceObjectWindowless> spOleInPlaceObject(m_p);
          return spOleInPlaceObject->GetDropTarget(ppDropTarget);
      }

      void ReleaseWebFlash()
      {
          ATLASSERT(m_spOleClientSite == NULL);

          CComQIPtr<IViewObject>      spViewObject(m_p);
          CComQIPtr<IOleObject>       spOleObject(m_p);

          if (spOleObject)
          {
              spOleObject->GetClientSite(&m_spOleClientSite);

              //m_spOleClientSite->OnShowWindow(FALSE);
              //CComQIPtr<IOleInPlaceObject> spOleInPlaceObject(m_p);
              //spOleInPlaceObject->InPlaceDeactivate();
              //spOleInPlaceObject->UIDeactivate();
              //spOleObject->DoVerb(OLEIVERB_DISCARDUNDOSTATE, NULL, NULL, 0, NULL, NULL);

              spOleObject->Close(OLECLOSE_NOSAVE);
              spOleObject->SetClientSite(NULL);
          }

          m_wndBtn.m_bPopupFlash = TRUE;
      }
        

      HRESULT ActivateAxToWeb()
      {
          HRESULT hr = S_OK;

          if( m_spOleClientSite == NULL )
              return E_FAIL;

          CComQIPtr<IViewObject>      spViewObject(m_p);
          CComQIPtr<IOleObject>       spOleObject(m_p);

          if (spOleObject)
          {
              spOleObject->SetClientSite(m_spOleClientSite);

              SIZE xSize, mSize;
              RECT rcPos = m_rcWndFlash;
              xSize.cx = rcPos.right - rcPos.left;
              xSize.cy = rcPos.bottom - rcPos.top;
              AtlPixelToHiMetric(&xSize, &mSize);
              spOleObject->SetExtent(DVASPECT_CONTENT, &mSize);
              spOleObject->GetExtent(DVASPECT_CONTENT, &mSize);
              AtlHiMetricToPixel(&mSize, &xSize);
              m_rcPos.right = m_rcPos.left + xSize.cx;
              m_rcPos.bottom = m_rcPos.top + xSize.cy;

              hr = spOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, m_spOleClientSite, 0, m_hWnd, &rcPos);
          }

          m_spOleClientSite.Detach();
          m_wndBtn.m_bPopupFlash = FALSE;
          FixPlayingFlash();
          return hr;
      }

      virtual void OnPopupFlash()
      {
          if ( !m_wndPopupFlash.IsWindow() )
          {
              RECT rcWndFlash = m_rcWndFlash;

              ::ClientToScreen(m_hWndParent, (LPPOINT)&rcWndFlash);
              ::ClientToScreen(m_hWndParent, ((LPPOINT)&rcWndFlash)+1);

              rcWndFlash.right  += 10;
              rcWndFlash.bottom += 28;

              m_wndPopupFlash.m_hWndParent  = m_hWnd;
              m_wndPopupFlash.m_pflashEvent = this;

              m_wndPopupFlash.Create(m_hWnd, &rcWndFlash, GetAudioTitle(), 
                  WS_POPUP | WS_SIZEBOX | WS_CLIPCHILDREN |
                  WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE, 
                  WS_EX_TOPMOST);

              ReleaseWebFlash();

              ::SetForegroundWindow(m_wndPopupFlash);
              m_wndPopupFlash.m_wndAxFlash.AttachControl(m_p);
              m_wndPopupFlash.SetFocus();
              FixPlayingFlash();
          }
      }

      virtual void OnEndPopupFlash()
      {
          ::SetForegroundWindow(m_hWndParent);
          ActivateAxToWeb();
          ::SetFocus(m_hWndParent);
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
              *ppvObject = new CDWExtractVideoProxy((IUnknown*)(*ppvObject));

          return hr;
      }

      HRESULT STDMETHODCALLTYPE LockServer( 
          /* [in] */ BOOL fLock)
      {
          return m_p->LockServer(fLock);
      }
};


struct __declspec(uuid("d27cdb6e-ae6d-11cf-96b8-444553540000"))
    ShockwaveFlash;


MIDL_INTERFACE("F3E70CEA-956E-49CC-B444-73AFE593AD7F")
IXPPlayer : public IUnknown
{
};

MIDL_INTERFACE("C728DAB8-FDF5-4CD7-89DD-879D25794C77")
IKooPlayer : public IUnknown
{
};

MIDL_INTERFACE("6BF52A52-394A-11d3-B153-00C04F79FAA6")
IWindowsMediaPlayer : public IUnknown
{
};



static HRESULT (WINAPI * OldCoGetClassObject)(IN REFCLSID rclsid, IN DWORD dwClsContext, IN LPVOID pvReserved,
                                              IN REFIID riid, OUT LPVOID FAR* ppv) = CoGetClassObject;


HRESULT WINAPI  MyCoGetClassObject(IN REFCLSID rclsid, IN DWORD dwClsContext, IN LPVOID pvReserved,
                                   IN REFIID riid, OUT LPVOID FAR* ppv)
{

    HRESULT hResult = OldCoGetClassObject(rclsid, dwClsContext, pvReserved, riid, ppv);

    if ( ( rclsid == __uuidof(ShockwaveFlash) || 
        rclsid == __uuidof(IXPPlayer) ||
        rclsid == __uuidof(IKooPlayer) 
        /*rclsid == __uuidof(IWindowsMediaPlayer)*/ ) && 
        riid == __uuidof(IClassFactory) )
    {
        *ppv = new MyClassFactory((IClassFactory*)(*ppv));
    }

    return hResult;
}


void InitExtractVideo()
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach((void**)&OldCoGetClassObject, MyCoGetClassObject);
    DetourTransactionCommit();
}

void UninitExtractVideo()
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach((void**)&OldCoGetClassObject, MyCoGetClassObject);
    DetourTransactionCommit();
}