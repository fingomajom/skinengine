
#pragma once

namespace DWAxHost { class CDWAxHost; };

class CDWHtmlView : 
    public CWindowImpl<CDWHtmlView>,
    public IDispatch
{
    friend class DWAxHost::CDWAxHost;
public:

    CDWHtmlView();

    // Attributes
public:
    ATL::CString GetType() const;
    long GetLeft() const;
    void SetLeft(long nNewValue);
    long GetTop() const;
    void SetTop(long nNewValue);
    long GetHeight() const;
    void SetHeight(long nNewValue);
    long GetWidth() const;
    void SetWidth(long nNewValue);
    void SetVisible(BOOL bNewValue);
    BOOL GetVisible() const;
    ATL::CString GetLocationName() const;
    READYSTATE GetReadyState() const;
    BOOL GetOffline() const;
    void SetOffline(BOOL bNewValue);
    BOOL GetSilent() const;
    void SetSilent(BOOL bNewValue);
    BOOL GetTopLevelContainer() const;
    ATL::CString GetLocationURL() const;
    BOOL GetBusy() const;
    LPDISPATCH GetApplication() const;
    LPDISPATCH GetParentBrowser() const;
    LPDISPATCH GetContainer() const;
    LPDISPATCH GetHtmlDocument() const;
    ATL::CString GetFullName() const;
    int GetToolBar() const;
    void SetToolBar(int nNewValue);
    BOOL GetMenuBar() const;
    void SetMenuBar(BOOL bNewValue);
    BOOL GetFullScreen() const;
    void SetFullScreen(BOOL bNewValue);
    OLECMDF QueryStatusWB(OLECMDID cmdID) const;
    BOOL GetRegisterAsBrowser() const;
    void SetRegisterAsBrowser(BOOL bNewValue);
    BOOL GetRegisterAsDropTarget() const;
    void SetRegisterAsDropTarget(BOOL bNewValue);
    BOOL GetTheaterMode() const;
    void SetTheaterMode(BOOL bNewValue);
    BOOL GetAddressBar() const;
    void SetAddressBar(BOOL bNewValue);
    BOOL GetStatusBar() const;
    void SetStatusBar(BOOL bNewValue);

    IWebBrowser2* GetWebBrowser() const;
    // Operations
public:

    void GoBack();
    void GoForward();
    void GoHome();
    void GoSearch();

    void Navigate2(LPITEMIDLIST pIDL, DWORD dwFlags = 0,
        LPCTSTR lpszTargetFrameName = NULL);
    void Navigate2(LPCTSTR lpszURL, DWORD dwFlags = 0,
        LPCTSTR lpszTargetFrameName = NULL,	LPCTSTR lpszHeaders = NULL,
        LPVOID lpvPostData = NULL, DWORD dwPostDataLen = 0);

    void Refresh();
    void Refresh2(int nLevel);
    void Stop();
    void PutProperty(LPCTSTR lpszProperty, const VARIANT& vtValue);
    void PutProperty(LPCTSTR lpszPropertyName, double dValue);
    void PutProperty(LPCTSTR lpszPropertyName, LPCTSTR lpszValue);
    void PutProperty(LPCTSTR lpszPropertyName, long lValue);
    void PutProperty(LPCTSTR lpszPropertyName, short nValue);
    BOOL GetProperty(LPCTSTR lpszProperty, ATL::CString& strValue);

    CComVariant GetProperty(LPCTSTR lpszProperty);
    void ExecWB(OLECMDID cmdID, OLECMDEXECOPT cmdexecopt, VARIANT* pvaIn,
        VARIANT* pvaOut);

    BOOL GetSource(ATL::CString& strRef);
    BOOL LoadFromResource(LPCTSTR lpszResource);
    BOOL LoadFromResource(UINT nRes);

    HRESULT QueryFormsCommand(DWORD dwCommandID, BOOL* pbSupported,
        BOOL* pbEnabled, BOOL* pbChecked);
    HRESULT ExecFormsCommand(DWORD dwCommandID, VARIANT* pVarIn,
        VARIANT* pVarOut);
    

protected:

    BEGIN_MSG_MAP(CDWHtmlView)
        MESSAGE_HANDLER(WM_CREATE  , OnCreate )
        MESSAGE_HANDLER(WM_DESTROY , OnDestroy )
    END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    //////////////////////////////////////////////////////////////////////////

    STDMETHOD(QueryInterface)(REFIID riid, void** ppv);
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();
    STDMETHOD(GetTypeInfoCount)(UINT* pctinfo) ;
    STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo** ppInfo);
    STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId);
    STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

    //////////////////////////////////////////////////////////////////////////
protected:

    HRESULT _OnNavigateComplete2 (REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr);
    HRESULT _OnBeforeNavigate2   (REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr);
    HRESULT _OnStatusTextChange  (REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr);
    HRESULT _OnStatuesChange     (REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr);
    HRESULT _OnNewWindow2        (REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr);
    HRESULT _OnNewWindow3        (REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr);
    HRESULT _OnDownloadBegin     (REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr);
    HRESULT _OnDownloadComplete  (REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr);
    HRESULT _OnDocumentComplete  (REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr);
    HRESULT _OnProgressChange    (REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr);
    HRESULT _OnWindowClosing     (REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr);
    HRESULT _OnCommandStateChange(REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr);
    HRESULT _OnQuit              (REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr);
    HRESULT _OnNavigateError     (REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr);
    HRESULT _OnTitleChange       (REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr);

    //////////////////////////////////////////////////////////////////////////

    virtual HRESULT OnQueryService( REFGUID rsid, REFIID riid, void** ppvObj);
    virtual HRESULT OnExec(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT *pvaIn, VARIANT *pvaOut);
    virtual HRESULT OnQueryStatus(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD* prgCmds, OLECMDTEXT *pCmdText);
    virtual HRESULT OnShowContextMenu(DWORD dwID, LPPOINT ppt,
        LPUNKNOWN pcmdtReserved, LPDISPATCH pdispReserved);
    virtual HRESULT OnGetExternal(LPDISPATCH *lppDispatch);
    virtual HRESULT OnGetHostInfo(DOCHOSTUIINFO *pInfo);
    virtual HRESULT OnShowUI(DWORD dwID,
        LPOLEINPLACEACTIVEOBJECT pActiveObject,
        LPOLECOMMANDTARGET pCommandTarget, LPOLEINPLACEFRAME pFrame,
        LPOLEINPLACEUIWINDOW pDoc);
    virtual HRESULT OnHideUI();
    virtual HRESULT OnUpdateUI();
    virtual HRESULT OnEnableModeless(BOOL fEnable);
    virtual HRESULT OnDocWindowActivate(BOOL fActivate);
    virtual HRESULT OnFrameWindowActivate(BOOL fActivate);
    virtual HRESULT OnResizeBorder(LPCRECT prcBorder,
        LPOLEINPLACEUIWINDOW pUIWindow, BOOL fFrameWindow);
    virtual HRESULT OnTranslateAccelerator(LPMSG lpMsg,
        const GUID* pguidCmdGroup, DWORD nCmdID);
    virtual HRESULT OnGetOptionKeyPath(LPOLESTR* pchKey, DWORD dwReserved);
    virtual HRESULT OnFilterDataObject(LPDATAOBJECT pDataObject,
        LPDATAOBJECT* ppDataObject);
    virtual HRESULT OnTranslateUrl(DWORD dwTranslate,
        OLECHAR* pchURLIn, OLECHAR** ppchURLOut);
    virtual HRESULT OnGetDropTarget(LPDROPTARGET pDropTarget,
        LPDROPTARGET* ppDropTarget);

    virtual HRESULT OnAmbientProperty( IOleControlSite* pSite, DISPID dispid, VARIANT* pvar);

    // Events
    virtual void OnNavigateComplete2(LPCTSTR strURL);
    virtual void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags,
        LPCTSTR lpszTargetFrameName, CComVariant& baPostedData,
        LPCTSTR lpszHeaders, BOOL* pbCancel);
    virtual void OnStatusTextChange(LPCTSTR lpszText);
    virtual void OnProgressChange(long nProgress, long nProgressMax);
    virtual void OnCommandStateChange(long nCommand, BOOL bEnable);
    virtual void OnDownloadBegin();
    virtual void OnDownloadComplete();
    virtual void OnTitleChange(LPCTSTR lpszText);
    virtual void OnPropertyChange(LPCTSTR lpszProperty);
    virtual void OnNewWindow2(LPDISPATCH* ppDisp, BOOL* bCancel);
    virtual void OnNewWindow3(LPDISPATCH *ppDisp, BOOL* bCancel, DWORD dwFlags, LPCTSTR lpszUrlContext, LPCTSTR lpszUrl);
    virtual void OnDocumentComplete(LPCTSTR lpszURL);
    virtual void OnQuit();
    virtual void OnVisible(BOOL bVisible);
    virtual void OnToolBar(BOOL bToolBar);
    virtual void OnMenuBar(BOOL bMenuBar);
    virtual void OnStatusBar(BOOL bStatusBar);
    virtual void OnFullScreen(BOOL bFullScreen);
    virtual void OnTheaterMode(BOOL bTheaterMode);
    virtual void OnNavigateError(LPCTSTR lpszURL, LPCTSTR lpszFrame, DWORD dwError, BOOL *pbCancel);

    virtual void NavigateComplete2(LPDISPATCH pDisp, VARIANT* URL);
    virtual void BeforeNavigate2(LPDISPATCH pDisp, VARIANT* URL,
        VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData,
        VARIANT* Headers, VARIANT_BOOL* Cancel);
    virtual void DocumentComplete(LPDISPATCH pDisp, VARIANT* URL);
    virtual void NavigateError(LPDISPATCH pDisp, VARIANT* pvURL,
        VARIANT* pvFrame, VARIANT* pvStatusCode, VARIANT_BOOL* pvbCancel);

    //////////////////////////////////////////////////////////////////////////

protected:

    DWORD m_dwCookie;
    DWORD m_dwRef;

    CComPtr<IWebBrowser2> m_spWebBrowser;
};