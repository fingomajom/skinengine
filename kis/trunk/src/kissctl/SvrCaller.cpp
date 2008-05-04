// SvrCaller.cpp : Implementation of CSvrCaller

#include "stdafx.h"
#include "SvrCaller.h"
#include <shlguid.h>
#include "../kissvr/kissvr.h"

// CSvrCaller


HRESULT CSvrCaller::FinalConstruct()
{  
    m_svrCallerSink.m_pSvrCaller = this;

    return S_OK;
}

void CSvrCaller::FinalRelease()
{

}


STDMETHODIMP CSvrCaller::Initialize(ULONG uCallerId, BSTR bstrJsCallbackMethod)
{
    if (m_uCallerId != CALLERID_UNKNOWN)
        return S_OK;

    m_uCallerId = uCallerId;
    m_bstrJsCallBackMethod = bstrJsCallbackMethod;

    CComPtr<IClassFactory> spFactory;

    HRESULT hr = CoGetClassObject( 
        __uuidof(SvrObject), CLSCTX_LOCAL_SERVER, 0, 
        IID_IClassFactory, (void**)&spFactory );

    if (FAILED(hr))
        goto Exit0;

    hr = spFactory->CreateInstance(NULL, 
        __uuidof(IDispatch), (void**)&m_spSvrObject );

    if (FAILED(hr))
        goto Exit0;

    if (bstrJsCallbackMethod != NULL && 
        SysStringLen(bstrJsCallbackMethod) > 0)
    {
        hr = ParaseJSFun();
        if (FAILED(hr))
            goto Exit0;
    }


    if (m_spCallback.p != NULL || m_jsCallbackId != DISPID_UNKNOWN )
        m_svrCallerSink.DispEventUnadvise(m_spSvrObject);

    return S_OK;

Exit0:

    Uninitialize();

    return E_FAIL;
}

STDMETHODIMP CSvrCaller::Uninitialize(void)
{
    m_uCallerId = CALLERID_UNKNOWN;

    if (m_spSvrObject.p != NULL)
    {
        m_svrCallerSink.DispEventUnadvise(m_spSvrObject);

        m_spSvrObject.Detach()->Release();
    }

    if (m_spScriptDisp.p != NULL)
    {
        m_spScriptDisp.Detach()->Release();
    }

    return S_OK;
}

STDMETHODIMP CSvrCaller::CallSvrFunc( 
    /* [in] */ ULONG uDesModuleId,
    /* [in] */ BSTR bstrFunctionName,
    /* [in] */ BSTR bstrParameter,
    /* [retval][out] */ BSTR *bstrResult)
{
    if ( m_spSvrObject.p != NULL )
    {
        CComVariant avarParams[4];
        CComVariant varResult;

        avarParams[3] = uDesModuleId;
        avarParams[3].vt = VT_UI4;
        avarParams[2] = m_uCallerId;
        avarParams[2].vt = VT_UI4;
        avarParams[1] = bstrFunctionName;
        avarParams[1].vt = VT_BSTR;
        avarParams[0] = bstrParameter;
        avarParams[0].vt = VT_BSTR;

        DISPPARAMS params = { avarParams, NULL, 4, 0 };
        
        HRESULT hr = m_spSvrObject->Invoke(1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
        
        if (SUCCEEDED(hr) && bstrResult != NULL && varResult.vt == VT_BSTR)
        {
            *bstrResult = varResult.bstrVal;
            
            varResult.vt = VT_EMPTY;
        }

        return hr;
    }

    return E_FAIL;
}

STDMETHODIMP CSvrCaller::RegisterCallback(IUnknown* piCallback)
{
    if (piCallback != NULL)
    {
        HRESULT hr = piCallback->QueryInterface(__uuidof(INotifyCallback),
            (void**)&m_spCallback);

        if (SUCCEEDED(hr) && m_spSvrObject.p != NULL)
            m_svrCallerSink.DispEventAdvise(m_spSvrObject);
    }
    else
    {
        if (m_spCallback.p != NULL)
            m_spCallback.Detach()->Release();

        if (m_spSvrObject.p != NULL)
        {
            if (m_jsCallbackId == DISPID_UNKNOWN)
                m_svrCallerSink.DispEventUnadvise(m_spSvrObject);
        }
    }


    return S_OK;
}

HRESULT CSvrCaller::ParaseJSFun()
{	
    HRESULT hr = E_FAIL;

    if (m_spUnkSite.p == NULL)
        return hr;

    CComPtr<IUnknown> spUnkSite;
    //得到脚本对象以及域名
    CComQIPtr<IServiceProvider> spSvrProvider(m_spUnkSite);		
    if (spSvrProvider == NULL)
        return hr;

    CComPtr<IWebBrowser2> spWebBrowser;
    hr = spSvrProvider->QueryService(SID_SWebBrowserApp, IID_IWebBrowser2, (void**)&spWebBrowser);
    if (FAILED(hr))
        return hr;


    CComPtr<IDispatch> spDocDisp;
    hr = spWebBrowser->get_Document(&spDocDisp);
    if (FAILED(hr))
        return hr;

    CComQIPtr<IHTMLDocument2> spHTML(spDocDisp);
    if (m_spScriptDisp != NULL)
        m_spScriptDisp.Release();

    hr = spHTML->get_Script(&m_spScriptDisp);
    if (FAILED(hr))
        return hr;

    hr = m_spScriptDisp->GetIDsOfNames(IID_NULL, 
        &m_bstrJsCallBackMethod, 1, 
        LOCALE_SYSTEM_DEFAULT, &m_jsCallbackId);
    if (FAILED(hr))
        return hr;

    hr = spHTML->get_domain(&m_bstrDomain);
    if (FAILED(hr))
        return hr;

    return hr;
}
