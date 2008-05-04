// SvrCaller.cpp : Implementation of CSvrCaller

#include "stdafx.h"
#include "SvrCaller.h"
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


STDMETHODIMP CSvrCaller::Advise(IUnknown* pUnkSink, DWORD* pdwCookie)
{
    HRESULT hr = CProxy_ISvrCallerEvents<CSvrCaller>::Advise( pUnkSink, pdwCookie );

    if (SUCCEEDED(hr) && m_spSvrObject.p != NULL)
        m_svrCallerSink.DispEventAdvise(m_spSvrObject);

    return hr;
}

STDMETHODIMP CSvrCaller::Unadvise(DWORD dwCookie)
{
    HRESULT hr = CProxy_ISvrCallerEvents<CSvrCaller>::Unadvise( dwCookie );

    if (SUCCEEDED(hr) && m_spSvrObject.p != NULL && m_spCallback.p == NULL )
    {
        Lock();
        int nsize = m_vec.GetSize();
        Unlock();

        if (nsize == 0)
            m_svrCallerSink.DispEventUnadvise(m_spSvrObject);
    }

    return hr;
}


STDMETHODIMP CSvrCaller::Initialize(ULONG uCallerId)
{
    if (m_uCallerId != -1)
        return S_OK;

    m_uCallerId = uCallerId;

    CComPtr<IClassFactory> spFactory;

    HRESULT hr = CoGetClassObject( 
        __uuidof(SvrObject), CLSCTX_LOCAL_SERVER, 0, 
        IID_IClassFactory, (void**)&spFactory );

    if (FAILED(hr))
        return hr;

    hr = spFactory->CreateInstance(NULL, 
        __uuidof(IDispatch), (void**)&m_spSvrObject );

    if (FAILED(hr))
        return hr;

    Lock();
    int nsize = m_vec.GetSize();
    Unlock();

    if (m_spCallback.p != NULL || nsize > 0)
        m_svrCallerSink.DispEventUnadvise(m_spSvrObject);

    return S_OK;
}

STDMETHODIMP CSvrCaller::Uninitialize(void)
{
    m_uCallerId = -1;

    if (m_spSvrObject.p != NULL)
    {
        m_svrCallerSink.DispEventUnadvise(m_spSvrObject);

        m_spSvrObject.Detach()->Release();
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
        CComVariant avarParams[5];
        CComVariant varResult;

        avarParams[4] = uDesModuleId;
        avarParams[4].vt = VT_UI4;
        avarParams[3] = m_uCallerId;
        avarParams[3].vt = VT_UI4;
        avarParams[2] = bstrFunctionName;
        avarParams[2].vt = VT_BSTR;
        avarParams[1] = bstrParameter;
        avarParams[1].vt = VT_BSTR;
        avarParams[0].byref = bstrResult;
        avarParams[0].vt = VT_BSTR|VT_BYREF;

        DISPPARAMS params = { avarParams, NULL, 5, 0 };
        
        HRESULT hr = m_spSvrObject->Invoke(1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);

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
            Lock();
            int nsize = m_vec.GetSize();
            Unlock();

            if (nsize == 0)
                m_svrCallerSink.DispEventUnadvise(m_spSvrObject);
        }
    }


    return S_OK;
}
