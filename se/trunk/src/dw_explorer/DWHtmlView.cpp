#include "stdafx.h"
#include "DWHtmlView.h"
#include "DWAxHost.h"
#include <exdispid.h>


CDWHtmlView::CDWHtmlView() : 
    m_dwRef(0),
    m_dwCookie(0)
{
}


LRESULT CDWHtmlView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    CComObject<DWAxHost::CDWAxHost>* pAxHost = new CComObject<DWAxHost::CDWAxHost>;
    ATLASSERT(pAxHost != NULL);
    if ( pAxHost == NULL )
        return -1;

    HRESULT hr = pAxHost->CreateControl(m_hWnd);
    ATLASSERT( SUCCEEDED(hr) );
    pAxHost->m_pHtmlView = this;

    pAxHost->QueryControl( IID_IWebBrowser2, (void**)&m_spWebBrowser );

    SetRegisterAsBrowser(TRUE);
    SetRegisterAsDropTarget(TRUE);
    //SetSilent(TRUE);

    hr = AtlAdvise(m_spWebBrowser, (IUnknown*)this, DIID_DWebBrowserEvents2, &m_dwCookie) ;
    ATLASSERT( SUCCEEDED(hr) );
    AddRef();

    return 0L;
}


LRESULT CDWHtmlView::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if ( m_spWebBrowser || m_dwCookie )
    {
        AtlUnadvise(m_spWebBrowser, DIID_DWebBrowserEvents2, m_dwCookie) ;
    }

    return DefWindowProc();
}


void CDWHtmlView::SetRegisterAsBrowser(BOOL bNewValue)
{
    ATLASSERT(m_spWebBrowser != NULL); 
    m_spWebBrowser->put_RegisterAsBrowser((short) (bNewValue ? VARIANT_TRUE : VARIANT_FALSE)); 
}

void CDWHtmlView::SetRegisterAsDropTarget(BOOL bNewValue)
{
    ATLASSERT(m_spWebBrowser != NULL); 
    m_spWebBrowser->put_RegisterAsDropTarget((short) (bNewValue ? VARIANT_TRUE : VARIANT_FALSE)); 
}

void CDWHtmlView::SetTheaterMode(BOOL bNewValue)
{ 
    ATLASSERT(m_spWebBrowser != NULL); 
    m_spWebBrowser->put_TheaterMode((short) (bNewValue ? VARIANT_TRUE : VARIANT_FALSE)); 
}

void CDWHtmlView::SetVisible(BOOL bNewValue)
{ 
    ATLASSERT(m_spWebBrowser != NULL); 
    m_spWebBrowser->put_Visible((short) (bNewValue ? VARIANT_TRUE : VARIANT_FALSE));
}

void CDWHtmlView::SetMenuBar(BOOL bNewValue)
{ 
    ATLASSERT(m_spWebBrowser != NULL); 
    m_spWebBrowser->put_MenuBar((short) (bNewValue ? VARIANT_TRUE : VARIANT_FALSE));
}

void CDWHtmlView::SetToolBar(int nNewValue)
{ 
    ATLASSERT(m_spWebBrowser != NULL); 
    m_spWebBrowser->put_ToolBar(nNewValue); 
}

void CDWHtmlView::SetOffline(BOOL bNewValue)
{ 
    ATLASSERT(m_spWebBrowser != NULL); 
    m_spWebBrowser->put_Offline((short) (bNewValue ? VARIANT_TRUE : VARIANT_FALSE)); 
}

void CDWHtmlView::SetSilent(BOOL bNewValue)
{ 
    ATLASSERT(m_spWebBrowser != NULL); 
    m_spWebBrowser->put_Silent((short) (bNewValue ? VARIANT_TRUE : VARIANT_FALSE)); 
}

void CDWHtmlView::GoBack()
{ 
    ATLASSERT(m_spWebBrowser != NULL); 
    m_spWebBrowser->GoBack(); }

void CDWHtmlView::GoForward()
{ ATLASSERT(m_spWebBrowser != NULL); m_spWebBrowser->GoForward(); 
}

void CDWHtmlView::GoHome()
{ 
    ATLASSERT(m_spWebBrowser != NULL); 
    m_spWebBrowser->GoHome(); 
}

void CDWHtmlView::GoSearch()
{ 
    ATLASSERT(m_spWebBrowser != NULL);
    m_spWebBrowser->GoSearch();
}

void CDWHtmlView::Refresh()
{ 
    ATLASSERT(m_spWebBrowser != NULL); 
    m_spWebBrowser->Refresh(); 
}

void CDWHtmlView::Refresh2(int nLevel)
{ 
    ATLASSERT(m_spWebBrowser != NULL); 
    m_spWebBrowser->Refresh2(&CComVariant((long)nLevel)); 
}

void CDWHtmlView::Stop()
{ 
    ATLASSERT(m_spWebBrowser != NULL); 
    m_spWebBrowser->Stop(); 
}

void CDWHtmlView::SetFullScreen(BOOL bNewValue)
{ 
    ATLASSERT(m_spWebBrowser != NULL); 
    m_spWebBrowser->put_FullScreen((short) (bNewValue ? VARIANT_TRUE : VARIANT_FALSE)); 
}

void CDWHtmlView::SetAddressBar(BOOL bNewValue)
{ 
    ATLASSERT(m_spWebBrowser != NULL); 
    m_spWebBrowser->put_AddressBar((short) (bNewValue ? VARIANT_TRUE : VARIANT_FALSE)); 
}

void CDWHtmlView::SetHeight(long nNewValue)
{ 
    ATLASSERT(m_spWebBrowser != NULL); 
    m_spWebBrowser->put_Height(nNewValue); 
}

void CDWHtmlView::SetWidth(long nNewValue)
{
    ATLASSERT(m_spWebBrowser != NULL); 
    m_spWebBrowser->put_Width(nNewValue); 
}

void CDWHtmlView::PutProperty(LPCTSTR lpszPropertyName, long lValue)
{ 
    ATLASSERT(m_spWebBrowser != NULL); 
    ATLASSERT(lpszPropertyName != NULL); 
    PutProperty(lpszPropertyName, CComVariant(lValue));
}

void CDWHtmlView::PutProperty(LPCTSTR lpszPropertyName, short nValue)
{ 
    ATLASSERT(m_spWebBrowser != NULL); 
    ATLASSERT(lpszPropertyName != NULL); 
    PutProperty(lpszPropertyName, CComVariant(nValue)); 
}

void CDWHtmlView::PutProperty(LPCTSTR lpszPropertyName, LPCTSTR lpszValue)
{ 
    ATLASSERT(m_spWebBrowser != NULL); 
    ATLASSERT(lpszPropertyName != NULL); 
    PutProperty(lpszPropertyName, CComVariant(lpszValue)); 
}

void CDWHtmlView::PutProperty(LPCTSTR lpszPropertyName, double dValue)
{ 
    ATLASSERT(m_spWebBrowser != NULL); 
    ATLASSERT(lpszPropertyName != NULL); 
    PutProperty(lpszPropertyName, CComVariant(dValue)); 
}

void CDWHtmlView::SetTop(long nNewValue)
{ 
    ATLASSERT(m_spWebBrowser != NULL); 
    m_spWebBrowser->put_Top(nNewValue); 
}

void CDWHtmlView::SetLeft(long nNewValue)
{ 
    ATLASSERT(m_spWebBrowser != NULL); m_spWebBrowser->put_Left(nNewValue); 
}

void CDWHtmlView::SetStatusBar(BOOL bNewValue)
{ 
    ATLASSERT(m_spWebBrowser != NULL); 
    m_spWebBrowser->put_StatusBar((short) (bNewValue ? VARIANT_TRUE : VARIANT_FALSE)); 
}

IWebBrowser2* CDWHtmlView::GetWebBrowser() const
{
    return m_spWebBrowser;
}


ATL::CString CDWHtmlView::GetType() const
{
    ATLASSERT(m_spWebBrowser != NULL);

    CComBSTR bstr;
    HRESULT hr = m_spWebBrowser->get_Type(&bstr);
    if(FAILED(hr))
    {
        ATLASSERT(FALSE);
        return ATL::CString();
    }

    ATL::CString retVal(bstr);
    return retVal;
}

long CDWHtmlView::GetLeft() const
{
    ATLASSERT(m_spWebBrowser != NULL);

    long result;
    HRESULT hr = m_spWebBrowser->get_Left(&result);
    if(FAILED(hr))
        return -1;

    return result;
}


long CDWHtmlView::GetTop() const
{
    ATLASSERT(m_spWebBrowser != NULL);
    long result;
    HRESULT hr = m_spWebBrowser->get_Top(&result);
    if(FAILED(hr))
        return -1;

    return result;
}

int CDWHtmlView::GetToolBar() const
{
    ATLASSERT(m_spWebBrowser != NULL);
    int result;
    HRESULT hr = m_spWebBrowser->get_ToolBar(&result);
    if(FAILED(hr))
        return -1;

    return result;
}

long CDWHtmlView::GetHeight() const
{
    ATLASSERT(m_spWebBrowser != NULL);
    long result;
    HRESULT hr = m_spWebBrowser->get_Height(&result);
    if(FAILED(hr))
        return -1;

    return result;
}

long CDWHtmlView::GetWidth() const
{
    ATLASSERT(m_spWebBrowser != NULL);
    long result;
    HRESULT hr = m_spWebBrowser->get_Width(&result);
    if(FAILED(hr))
        return -1;

    return result;
}

BOOL CDWHtmlView::GetVisible() const
{
    ATLASSERT(m_spWebBrowser != NULL);

    VARIANT_BOOL result;
    HRESULT hr = m_spWebBrowser->get_Visible(&result);
    if(FAILED(hr))
    {
        ATLASSERT(FALSE);
        return FALSE;
    }

    return result;
}

ATL::CString CDWHtmlView::GetLocationName() const
{
    ATLASSERT(m_spWebBrowser != NULL);

    CComBSTR bstr;
    HRESULT hr = m_spWebBrowser->get_LocationName(&bstr);
    if(FAILED(hr))
    {
        ATLASSERT(FALSE);
        return ATL::CString();
    }

    ATL::CString retVal(bstr);
    return retVal;
}

ATL::CString CDWHtmlView::GetLocationURL() const
{
    ATLASSERT(m_spWebBrowser != NULL);

    CComBSTR bstr;
    HRESULT hr = m_spWebBrowser->get_LocationURL(&bstr);
    if(FAILED(hr))
    {
        ATLASSERT(FALSE);
        return ATL::CString();
    }

    ATL::CString retVal(bstr);
    return retVal;
}

BOOL CDWHtmlView::GetBusy() const
{
    ATLASSERT(m_spWebBrowser != NULL);

    VARIANT_BOOL result;
    HRESULT hr = m_spWebBrowser->get_Busy(&result);
    if(FAILED(hr))
    {
        ATLASSERT(FALSE);
        return FALSE;
    }

    return result;
}

READYSTATE CDWHtmlView::GetReadyState() const
{
    ATLASSERT(m_spWebBrowser != NULL);

    READYSTATE result;
    HRESULT hr = m_spWebBrowser->get_ReadyState(&result);
    if(FAILED(hr))
        return result;

    return result;
}

BOOL CDWHtmlView::GetOffline() const
{
    ATLASSERT(m_spWebBrowser != NULL);

    VARIANT_BOOL result;
    HRESULT hr = m_spWebBrowser->get_Offline(&result);
    if(FAILED(hr))
    {
        ATLASSERT(FALSE);
        return FALSE;
    }

    return result;
}

BOOL CDWHtmlView::GetSilent() const
{
    ATLASSERT(m_spWebBrowser != NULL);

    VARIANT_BOOL result;
    HRESULT hr = m_spWebBrowser->get_Silent(&result);
    if(FAILED(hr))
    {
        ATLASSERT(FALSE);
        return FALSE;
    }

    return result;
}

LPDISPATCH CDWHtmlView::GetApplication() const
{
    ATLASSERT(m_spWebBrowser != NULL);

    LPDISPATCH result;
    HRESULT hr = m_spWebBrowser->get_Application(&result);
    if(FAILED(hr))
    {
        ATLASSERT(FALSE);
        return NULL;
    }

    return result;
}


LPDISPATCH CDWHtmlView::GetParentBrowser() const
{
    ATLASSERT(m_spWebBrowser != NULL);

    LPDISPATCH result;
    HRESULT hr = m_spWebBrowser->get_Parent(&result);
    if(FAILED(hr))
    {
        ATLASSERT(FALSE);
        return NULL;
    }

    return result;
}

LPDISPATCH CDWHtmlView::GetContainer() const
{
    ATLASSERT(m_spWebBrowser != NULL);

    LPDISPATCH result;
    HRESULT hr = m_spWebBrowser->get_Container(&result);
    if(FAILED(hr))
    {
        ATLASSERT(FALSE);
        return NULL;
    }

    return result;
}

LPDISPATCH CDWHtmlView::GetHtmlDocument() const
{
    ATLASSERT(m_spWebBrowser != NULL);

    LPDISPATCH result;
    HRESULT hr = m_spWebBrowser->get_Document(&result);
    if(FAILED(hr))
    {
        ATLASSERT(FALSE);
        return NULL;
    }

    return result;
}

BOOL CDWHtmlView::GetTopLevelContainer() const
{
    ATLASSERT(m_spWebBrowser != NULL);

    VARIANT_BOOL result;
    m_spWebBrowser->get_TopLevelContainer(&result);
    return result;
}

BOOL CDWHtmlView::GetMenuBar() const
{
    ATLASSERT(m_spWebBrowser != NULL);

    VARIANT_BOOL result;
    HRESULT hr = m_spWebBrowser->get_MenuBar(&result);
    if(FAILED(hr))
    {
        ATLASSERT(FALSE);
        return FALSE;
    }

    return result;
}

BOOL CDWHtmlView::GetFullScreen() const
{
    ATLASSERT(m_spWebBrowser != NULL);

    VARIANT_BOOL result;
    HRESULT hr = m_spWebBrowser->get_FullScreen(&result);
    if(FAILED(hr))
    {
        ATLASSERT(FALSE);
        return FALSE;
    }

    return result;
}

BOOL CDWHtmlView::GetStatusBar() const
{
    ATLASSERT(m_spWebBrowser != NULL);

    VARIANT_BOOL result;
    HRESULT hr = m_spWebBrowser->get_StatusBar(&result);
    if(FAILED(hr))
    {
        ATLASSERT(FALSE);
        return FALSE;
    }

    return result;
}

OLECMDF CDWHtmlView::QueryStatusWB(OLECMDID cmdID) const
{
    ATLASSERT(m_spWebBrowser != NULL);

    OLECMDF result;
    HRESULT hr = m_spWebBrowser->QueryStatusWB(cmdID, &result);
    if(FAILED(hr))
        return result;

    return result;
}

void CDWHtmlView::ExecWB(OLECMDID cmdID, OLECMDEXECOPT cmdexecopt,
                       VARIANT* pvaIn, VARIANT* pvaOut)
{
    ATLASSERT(m_spWebBrowser != NULL);

    m_spWebBrowser->ExecWB(cmdID, cmdexecopt, pvaIn, pvaOut);
}

HRESULT CDWHtmlView::ExecFormsCommand(DWORD dwCommandID,
                                    VARIANT* pVarIn, VARIANT* pVarOut)
{
    ATLASSERT(m_spWebBrowser != NULL);
    if ( m_spWebBrowser == NULL )
        return E_FAIL;

    HRESULT hr = E_FAIL;
    CComPtr<IDispatch> spDispDocument;
    m_spWebBrowser->get_Document(&spDispDocument);
    CComQIPtr<IHTMLDocument2> spDoc = spDispDocument;
    if (spDoc != NULL)
    {
        CComQIPtr<IOleCommandTarget> spCmdTarget = spDoc;
        if (spCmdTarget != NULL)
            hr = spCmdTarget->Exec(&CMDSETID_Forms3, dwCommandID,
            OLECMDEXECOPT_DONTPROMPTUSER, pVarIn, pVarOut);
    }

    return hr;
}

HRESULT CDWHtmlView::QueryFormsCommand(DWORD dwCommandID,
                                     BOOL* pbSupported, BOOL* pbEnabled, BOOL* pbChecked)
{
    ATLASSERT(m_spWebBrowser != NULL);
    if ( m_spWebBrowser == NULL )
        return E_FAIL;
    HRESULT hr = E_FAIL;

    CComPtr<IDispatch> spDispDocument;
    m_spWebBrowser->get_Document(&spDispDocument);
    CComQIPtr<IHTMLDocument2> spDoc = spDispDocument;	
    if (spDoc != NULL)
    {
        CComQIPtr<IOleCommandTarget> spCmdTarget = spDoc;
        if (spCmdTarget != NULL)
        {
            OLECMD cmdInfo;
            cmdInfo.cmdID = dwCommandID;
            cmdInfo.cmdf = 0;

            hr = spCmdTarget->QueryStatus(&CMDSETID_Forms3, 1, &cmdInfo, NULL);

            if (SUCCEEDED(hr))
            {
                if (pbSupported != NULL)
                    *pbSupported = (cmdInfo.cmdf & OLECMDF_SUPPORTED) ? TRUE : FALSE;
                if (pbEnabled != NULL)
                    *pbEnabled = (cmdInfo.cmdf & OLECMDF_ENABLED) ? TRUE : FALSE;
                if (pbChecked != NULL)
                    *pbChecked = (cmdInfo.cmdf & OLECMDF_LATCHED) ? TRUE : FALSE;
            }
        }
    }

    return hr;
}

BOOL CDWHtmlView::GetRegisterAsBrowser() const
{
    ATLASSERT(m_spWebBrowser != NULL);

    VARIANT_BOOL result;
    HRESULT hr = m_spWebBrowser->get_RegisterAsBrowser(&result);
    if(FAILED(hr))
    {
        ATLASSERT(FALSE);
        return FALSE;
    }

    return result;
}

BOOL CDWHtmlView::GetRegisterAsDropTarget() const
{
    ATLASSERT(m_spWebBrowser != NULL);

    VARIANT_BOOL result;
    HRESULT hr = m_spWebBrowser->get_RegisterAsDropTarget(&result);
    if(FAILED(hr))
    {
        ATLASSERT(FALSE);
        return FALSE;
    }

    return result;
}

BOOL CDWHtmlView::GetTheaterMode() const
{
    ATLASSERT(m_spWebBrowser != NULL);

    VARIANT_BOOL result;
    HRESULT hr = m_spWebBrowser->get_TheaterMode(&result);
    if(FAILED(hr))
    {
        ATLASSERT(FALSE);
        return FALSE;
    }

    return result;
}

BOOL CDWHtmlView::GetAddressBar() const
{
    ATLASSERT(m_spWebBrowser != NULL);

    VARIANT_BOOL result;
    HRESULT hr = m_spWebBrowser->get_AddressBar(&result);
    if(FAILED(hr))
    {
        ATLASSERT(FALSE);
        return FALSE;
    }

    return result;
}

/////////////////////////////////////////////////////////////////////////////
// CDWHtmlView operations

BOOL CDWHtmlView::GetSource(ATL::CString& refString)
{
    ATLASSERT(m_spWebBrowser != NULL);
    if ( m_spWebBrowser == NULL )
        return E_FAIL;

    BOOL bRetVal = FALSE;

    CComPtr<IDispatch> spDisp; 
    m_spWebBrowser->get_Document(&spDisp);	
    if (spDisp != NULL)
    {
        HGLOBAL hMemory;
        hMemory = GlobalAlloc(GMEM_MOVEABLE, 0);
        if (hMemory != NULL)
        {
            CComQIPtr<IPersistStreamInit> spPersistStream = spDisp;
            if (spPersistStream != NULL)
            {
                CComPtr<IStream> spStream;
                if (SUCCEEDED(CreateStreamOnHGlobal(hMemory, TRUE, &spStream)))
                {
                    spPersistStream->Save(spStream, FALSE);

                    LPCSTR pstr = static_cast<LPCSTR>(GlobalLock(hMemory));
                    if (pstr != NULL)
                    {
                        // Stream is always ANSI, but ATL::CString
                        // assignment operator will convert implicitly.

                        bRetVal = TRUE;
                        //__try
                        //{						
                            refString = pstr;
                        //}
                        //__except(1)
                        //{
                            bRetVal = FALSE;
                        //}

                        if(bRetVal == FALSE)
                            GlobalFree(hMemory);
                        else
                            GlobalUnlock(hMemory);
                    }
                    else
                    {
                        GlobalFree(hMemory);
                    }
                }
                else
                {
                    GlobalFree(hMemory);
                }
            }
            else
            {
                GlobalFree(hMemory);
            }
        }
    }

    return bRetVal;
}

BOOL CDWHtmlView::LoadFromResource(LPCTSTR lpszResource)
{
    HINSTANCE hInstance = _Module.GetResourceInstance();
    ATLASSERT(hInstance != NULL);

    ATL::CString strResourceURL;
    BOOL bRetVal = TRUE;
    LPTSTR lpszModule = new TCHAR[_MAX_PATH];

    if (GetModuleFileName(hInstance, lpszModule, _MAX_PATH))
    {
        strResourceURL.Format(_T("res://%s/%s"), lpszModule, lpszResource);
        Navigate2(strResourceURL);
    }
    else
        bRetVal = FALSE;

    delete [] lpszModule;
    return bRetVal;
}

BOOL CDWHtmlView::LoadFromResource(UINT nRes)
{
    HINSTANCE hInstance = _Module.GetResourceInstance();
    ATLASSERT(hInstance != NULL);

    ATL::CString strResourceURL;
    BOOL bRetVal = TRUE;
    LPTSTR lpszModule = new TCHAR[_MAX_PATH];

    if (GetModuleFileName(hInstance, lpszModule, _MAX_PATH))
    {
        strResourceURL.Format(_T("res://%s/%d"), lpszModule, nRes);
        Navigate2(strResourceURL);
    }
    else
        bRetVal = FALSE;

    delete [] lpszModule;
    return bRetVal;
}

void CDWHtmlView::Navigate2(LPITEMIDLIST pIDL, DWORD dwFlags /* = 0 */,
                          LPCTSTR lpszTargetFrameName /* = NULL */)
{
    ATLASSERT(m_spWebBrowser != NULL);

    CComVariant vPIDL((LPCTSTR)pIDL);
    CComVariant empty;

    m_spWebBrowser->Navigate2(
        &vPIDL,
        &CComVariant((long) dwFlags),
        &CComVariant(lpszTargetFrameName),
        &empty, &empty);
}

void CDWHtmlView::Navigate2(LPCTSTR lpszURL, DWORD dwFlags /* = 0 */,
                          LPCTSTR lpszTargetFrameName /* = NULL */,
                          LPCTSTR lpszHeaders /* = NULL */,
                          LPVOID lpvPostData /* = NULL */, DWORD dwPostDataLen /* = 0 */)
{
    ATLASSERT(m_spWebBrowser != NULL);

    CComVariant vPostData;
    if (lpvPostData != NULL)
    {
        if (dwPostDataLen == 0)
            dwPostDataLen = lstrlen((LPCTSTR) lpvPostData);


        SAFEARRAYBOUND rgsabound[1] = { 0 };

        rgsabound[0].cElements = dwPostDataLen;
        rgsabound[0].lLbound   = 0;

        SAFEARRAY * psf = SafeArrayCreate( VT_UI1, 1, rgsabound);

        if ( psf != NULL )
        {
            memcpy( psf->pvData, lpvPostData, dwPostDataLen);
            vPostData = psf;
        }
    }

    CComVariant vURL(lpszURL);
    CComVariant vHeaders(lpszHeaders);
    CComVariant vTargetFrameName(lpszTargetFrameName);
    CComVariant vFlags((long) dwFlags );

    m_spWebBrowser->Navigate2(&vURL,
        &vFlags, &vTargetFrameName, &vPostData, &vHeaders);
}


void CDWHtmlView::PutProperty(LPCTSTR lpszProperty, const VARIANT& vtValue)
{
    ATLASSERT(m_spWebBrowser != NULL);

    ATL::CString strProp(lpszProperty);
    BSTR bstrProp = strProp.AllocSysString();
    m_spWebBrowser->PutProperty(bstrProp, vtValue);
    ::SysFreeString(bstrProp);
}

BOOL CDWHtmlView::GetProperty(LPCTSTR lpszProperty, ATL::CString& strValue)
{
    ATLASSERT(m_spWebBrowser != NULL);

    ATL::CString strProperty(lpszProperty);
    BSTR bstrProperty = strProperty.AllocSysString();

    BOOL bResult = FALSE;
    VARIANT vReturn;
    vReturn.vt = VT_BSTR;
    vReturn.bstrVal = NULL;
    HRESULT hr = m_spWebBrowser->GetProperty(bstrProperty, &vReturn);

    if (SUCCEEDED(hr))
    {
        strValue = ATL::CString(vReturn.bstrVal);
        bResult = TRUE;
    }

    ::SysFreeString(bstrProperty);
    return bResult;
}

CComVariant CDWHtmlView::GetProperty(LPCTSTR lpszProperty)
{
    CComVariant result;

    //static BYTE parms[] = VTS_BSTR;

    //m_wndBrowser.InvokeHelper(0x12f, DISPATCH_METHOD,
    //    VT_VARIANT, (void*)&result, parms, lpszProperty);

    return result;
}

ATL::CString CDWHtmlView::GetFullName() const
{
    ATLASSERT(m_spWebBrowser != NULL);

    CComBSTR bstr;
    HRESULT hr = m_spWebBrowser->get_FullName(&bstr);
    if(FAILED(hr))
    {
        ATLASSERT(FALSE);
        return ATL::CString();
    }

    ATL::CString retVal(bstr);
    return retVal;
}



ULONG CDWHtmlView::AddRef()
{
    return ++m_dwRef ;
}

ULONG CDWHtmlView::Release()
{
    if(--m_dwRef == 0)
    {
        delete this ;
        return 0;
    }

    return m_dwRef;
}

HRESULT CDWHtmlView::QueryInterface(const IID &riid, void **ppv)
{
    if (!ppv)
    {
        return E_POINTER ;
    }

    if (IID_IUnknown == riid)
    {
        AddRef();
        *ppv = (IUnknown*)this ;
        return S_OK ;
    }
    else if (IID_IDispatch == riid)
    {
        AddRef();
        *ppv = (IDispatch*)this ;
        return S_OK ;
    }

    return E_NOTIMPL ;
}

HRESULT CDWHtmlView::GetTypeInfoCount(UINT* pctinfo)
{
    return E_NOTIMPL ;
}

HRESULT CDWHtmlView::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppInfo)
{
    return E_NOTIMPL ;
}

HRESULT CDWHtmlView::GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId)
{
    return E_NOTIMPL ;
}

HRESULT CDWHtmlView::Invoke(DISPID dispIdMember, const IID &riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
    switch(dispIdMember)
    {
    case DISPID_TITLECHANGE :
        return _OnTitleChange(riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr) ;

    case DISPID_STATUSTEXTCHANGE :
        return _OnStatusTextChange(riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr) ;

    case DISPID_NEWWINDOW2 :
        return _OnNewWindow2(riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr) ;

    case DISPID_NEWWINDOW3:
        return _OnNewWindow3(riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);

    case DISPID_BEFORENAVIGATE2 :
        return _OnBeforeNavigate2(riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr) ;

    case DISPID_NAVIGATECOMPLETE2 :
        return _OnNavigateComplete2(riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr) ;

    case DISPID_DOCUMENTCOMPLETE :
        return _OnDocumentComplete(riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr) ;

    case DISPID_PROGRESSCHANGE :
        return _OnProgressChange(riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr) ;

    case DISPID_WINDOWCLOSING :
        return _OnWindowClosing(riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr) ;

    case DISPID_COMMANDSTATECHANGE :
        return _OnCommandStateChange(riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr) ;

    case DISPID_NAVIGATEERROR :
        return _OnNavigateError(riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr) ;

    case DISPID_DOWNLOADBEGIN:
        return _OnDownloadBegin(riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr) ;
    case DISPID_DOWNLOADCOMPLETE:
        return _OnDownloadComplete(riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr) ;
    }

    return E_NOTIMPL ;
}

HRESULT CDWHtmlView::_OnNavigateComplete2 (REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
    NavigateComplete2( 
        pDispParams->rgvarg[1].pdispVal, 
        &pDispParams->rgvarg[0]);

    return S_OK;
}
HRESULT CDWHtmlView::_OnBeforeNavigate2(REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{   
    BeforeNavigate2( 
        pDispParams->rgvarg[6].pdispVal,
        &pDispParams->rgvarg[5],
        &pDispParams->rgvarg[4],
        &pDispParams->rgvarg[3],
        &pDispParams->rgvarg[2],
        &pDispParams->rgvarg[1],
        pDispParams->rgvarg[0].pboolVal);

    return S_OK;
}
HRESULT CDWHtmlView::_OnTitleChange(REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
    ATL::CString strTitle(pDispParams->rgvarg[0]);

    OnTitleChange( strTitle );

    return S_OK;
}
HRESULT CDWHtmlView::_OnStatusTextChange(REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
    ATL::CString strStatus(pDispParams->rgvarg[0]);

    OnStatusTextChange( strStatus );

    return S_OK;
}
HRESULT CDWHtmlView::_OnNewWindow2(REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
    BOOL bCancel = FALSE;

    OnNewWindow2(pDispParams->rgvarg[1].ppdispVal , &bCancel);

    if (bCancel)
        *pDispParams->rgvarg[0].pboolVal = VARIANT_TRUE;
    else
        *pDispParams->rgvarg[0].pboolVal = VARIANT_FALSE;

    return S_OK;
}
HRESULT CDWHtmlView::_OnNewWindow3(REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
    return S_OK;
}

HRESULT CDWHtmlView::_OnDownloadBegin(REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
    OnDownloadBegin();

    return S_OK;
}

HRESULT CDWHtmlView::_OnDownloadComplete(REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
    OnDownloadComplete();

    return S_OK;
}
HRESULT CDWHtmlView::_OnDocumentComplete(REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
    DocumentComplete( 
        pDispParams->rgvarg[1].pdispVal, 
        &pDispParams->rgvarg[0]);

    return S_OK;
}
HRESULT CDWHtmlView::_OnProgressChange(REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
    OnProgressChange(
        pDispParams->rgvarg[1].lVal, 
        pDispParams->rgvarg[0].lVal);

    return S_OK;
}

HRESULT CDWHtmlView::_OnWindowClosing(REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
    return S_OK;
}
HRESULT CDWHtmlView::_OnCommandStateChange(REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
    OnCommandStateChange(
        pDispParams->rgvarg[1].lVal, 
        pDispParams->rgvarg[0].boolVal != VARIANT_FALSE);
    return S_OK;
}
HRESULT CDWHtmlView::_OnQuit(REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
    OnQuit();

    return S_OK;
}
HRESULT CDWHtmlView::_OnNavigateError(REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
    NavigateError( 
        pDispParams->rgvarg[4].pdispVal,
        &pDispParams->rgvarg[3],
        &pDispParams->rgvarg[2],
        &pDispParams->rgvarg[1],
        pDispParams->rgvarg[0].pboolVal);
    
    return S_OK;
}



//////////////////////////////////////////////////////////////////////////


HRESULT CDWHtmlView::OnGetExternal(LPDISPATCH*)
{
    return S_FALSE;
}

HRESULT CDWHtmlView::OnShowContextMenu(DWORD, LPPOINT, LPUNKNOWN, LPDISPATCH)
{
    return S_FALSE;
}

HRESULT CDWHtmlView::OnGetHostInfo(DOCHOSTUIINFO*)
{
    return S_OK;
}


HRESULT CDWHtmlView::OnShowUI(DWORD, LPOLEINPLACEACTIVEOBJECT,
                            LPOLECOMMANDTARGET, LPOLEINPLACEFRAME, LPOLEINPLACEUIWINDOW)
{
    return S_FALSE;
}


HRESULT CDWHtmlView::OnHideUI()
{
    return S_OK;
}


HRESULT CDWHtmlView::OnUpdateUI()
{
    return S_OK;
}


HRESULT CDWHtmlView::OnEnableModeless(BOOL)
{
    return S_OK;
}


HRESULT CDWHtmlView::OnDocWindowActivate(BOOL)
{
    return S_OK;
}


HRESULT CDWHtmlView::OnFrameWindowActivate(BOOL)
{
    return S_OK;
}


HRESULT CDWHtmlView::OnResizeBorder(LPCRECT, LPOLEINPLACEUIWINDOW, BOOL)
{
    return S_OK;
}


HRESULT CDWHtmlView::OnTranslateAccelerator(LPMSG, const GUID*, DWORD)
{
    return S_FALSE;
}


HRESULT CDWHtmlView::OnGetOptionKeyPath(LPOLESTR*, DWORD)	
{
    return S_FALSE;
}

HRESULT CDWHtmlView::OnGetDropTarget(LPDROPTARGET, LPDROPTARGET*)
{
    return S_FALSE;
}

HRESULT CDWHtmlView::OnTranslateUrl(DWORD, OLECHAR*, OLECHAR**)
{
    return S_FALSE;
}


HRESULT CDWHtmlView::OnFilterDataObject(LPDATAOBJECT pDataObject,
                                      LPDATAOBJECT* ppDataObject)
{
    return S_FALSE;
}

void CDWHtmlView::OnProgressChange(long lProgress, long lProgressMax)
{
}

void CDWHtmlView::OnCommandStateChange(long lCommand, BOOL bEnable)
{
}

void CDWHtmlView::OnDownloadBegin()
{
}

void CDWHtmlView::OnDownloadComplete()
{
}

void CDWHtmlView::OnTitleChange(LPCTSTR lpszText)
{
}

void CDWHtmlView::OnPropertyChange(LPCTSTR lpszProperty)
{
}

void CDWHtmlView::OnNewWindow2(LPDISPATCH* ppDisp, BOOL* bCancel)
{
    *bCancel = FALSE;
}

void CDWHtmlView::OnDocumentComplete(LPCTSTR lpszURL)
{
}

void CDWHtmlView::OnQuit()
{
}

void CDWHtmlView::OnVisible(BOOL bVisible)
{
}

void CDWHtmlView::OnToolBar(BOOL bToolBar)
{
}

void CDWHtmlView::OnMenuBar(BOOL bMenuBar)
{
}

void CDWHtmlView::OnStatusBar(BOOL bStatusBar)
{
}

void CDWHtmlView::OnFullScreen(BOOL bFullScreen)
{
}

void CDWHtmlView::OnTheaterMode(BOOL bTheaterMode)
{
}

void CDWHtmlView::OnNavigateComplete2(LPCTSTR lpszURL)
{
}

void CDWHtmlView::OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags,
    LPCTSTR lpszTargetFrameName, CComVariant& baPostData,
    LPCTSTR lpszHeaders, BOOL* bCancel)
{
    *bCancel = FALSE;
}

void CDWHtmlView::OnStatusTextChange(LPCTSTR pszText)
{
}

void CDWHtmlView::OnNavigateError(LPCTSTR lpszURL, LPCTSTR lpszFrame, DWORD dwError, BOOL *pbCancel)
{
    *pbCancel = FALSE;
}

void CDWHtmlView::NavigateComplete2(LPDISPATCH /* pDisp */, VARIANT* URL)
{
    ATLASSERT(V_VT(URL) & VT_BSTR);

    ATL::CString str(*URL);
    OnNavigateComplete2(str);
}

void CDWHtmlView::BeforeNavigate2(
    LPDISPATCH /* pDisp */, 
    VARIANT* URL,
    VARIANT* Flags, 
    VARIANT* TargetFrameName,
    VARIANT* PostData, 
    VARIANT* Headers, 
    VARIANT_BOOL* Cancel) 
{
    ATLASSERT(V_VT(URL) & VT_BSTR);
    ATLASSERT(V_VT(TargetFrameName) & VT_BSTR);
    ATLASSERT(V_VT(PostData) & (VT_VARIANT | VT_BYREF));
    ATLASSERT(V_VT(Headers) & VT_BSTR);
    ATLASSERT(Cancel != NULL);

    CComVariant varPostData = *PostData;

    CString strTargetFrameName(V_BSTR(TargetFrameName));
    CString strURL(V_BSTR(URL));
    CString strHeaders(V_BSTR(Headers));
    DWORD nFlags = V_I4(Flags);


    BOOL bCancel = FALSE;

    OnBeforeNavigate2(strURL, nFlags, strTargetFrameName,
        varPostData, strHeaders, &bCancel);

    if (bCancel)
        *Cancel = VARIANT_TRUE;
    else
        *Cancel = VARIANT_FALSE;
}

void CDWHtmlView::DocumentComplete(LPDISPATCH pDisp, VARIANT* URL)
{
    ATLASSERT(V_VT(URL) & VT_BSTR);

    ATL::CString str( *URL );
    OnDocumentComplete(str);
}

void CDWHtmlView::NavigateError(
    LPDISPATCH pDisp, 
    VARIANT* pvURL,
    VARIANT* pvFrame, 
    VARIANT* pvStatusCode, 
    VARIANT_BOOL* pvbCancel)
{
    ATLASSERT(pvURL != NULL);
    ATLASSERT(pvFrame != NULL);
    ATLASSERT(pvStatusCode != NULL);
    ATLASSERT(pvbCancel != NULL);
    ATLASSERT(V_VT(pvURL) & VT_BSTR);
    ATLASSERT(V_VT(pvFrame) & VT_BSTR);

    CString strURL(*pvURL);
    CString strFrame(*pvFrame);
    DWORD dwError = V_I4(pvStatusCode);

    BOOL bCancel = FALSE;

    OnNavigateError(strURL, strFrame, dwError, &bCancel);

    if (pvbCancel)
        *pvbCancel = bCancel ? VARIANT_TRUE : VARIANT_FALSE;
}
