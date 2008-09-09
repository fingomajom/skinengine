// SvrCaller.cpp : Implementation of CSvrCaller

#include "stdafx.h"
#include "SvrCaller.h"
#include "..\public\lsesvr.h"

// CSvrCaller

HRESULT STDMETHODCALLTYPE CSvrCaller::Initialize( 
    /* [in] */ ULONG     uCallerId,
    /* [in] */ IUnknown *piCallback)
{

    CComPtr<IClassFactory> spFactory;

    m_uCallerId = uCallerId;
    if (m_uCallerId == CALLERID_UNKNOWN)
        return E_FAIL;
    
    if (piCallback != NULL)
    {
        piCallback->QueryInterface(__uuidof(INotifyCallback),
            (void**)&m_spCallback);
        
        if (m_spCallback.p != NULL)
            m_svrCallerSink.DispEventUnadvise(m_spSvrObject);
    }    

    HRESULT hr = CoGetClassObject( 
        __uuidof(SvrObject), CLSCTX_LOCAL_SERVER, 0, 
        IID_IClassFactory, (void**)&spFactory );

    if (FAILED(hr))
        goto Exit0;

    hr = spFactory->CreateInstance(NULL, 
        __uuidof(IDispatch), (void**)&m_spSvrObject );

    if (FAILED(hr))
        goto Exit0;

    return S_OK;

Exit0:

    Uninitialize();

    return E_FAIL;
}

HRESULT STDMETHODCALLTYPE CSvrCaller::Uninitialize( void )
{
    m_uCallerId = CALLERID_UNKNOWN;

    if (m_spSvrObject.p != NULL)
    {
        m_svrCallerSink.DispEventUnadvise(m_spSvrObject);

        m_spSvrObject.Detach()->Release();
    }

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CSvrCaller::CallSvrFunc( 
    /* [in]  */ ULONG uTargetId,
    /* [in]  */ ULONG uFunctionId,
    /* [in]  */ IDataBuffer*  pParameter,
    /* [out] */ IDataBuffer** ppResult)
{
    HRESULT hr = E_FAIL;

    ATLASSERT( m_spSvrObject.p != NULL && m_uCallerId != CALLERID_UNKNOWN );
    if ( m_spSvrObject.p == NULL || m_uCallerId == CALLERID_UNKNOWN )
        return E_FAIL;


    CComVariant avarParams[5];
    CComVariant varResult;

    avarParams[4] = uTargetId;
    avarParams[4].vt = VT_UI4;
    avarParams[3] = m_uCallerId;
    avarParams[3].vt = VT_UI4;
    avarParams[2] = uFunctionId;
    avarParams[2].vt = VT_UI4;

    avarParams[1].vt = VT_EMPTY;

    if (pParameter != NULL)
    {
        avarParams[1].parray = DataBuffer2SafeArray( pParameter );
        avarParams[1].vt = avarParams[0].parray ? VT_UI1 | VT_ARRAY : VT_EMPTY;
    }

    DISPPARAMS params = { avarParams, NULL, 5, 0 };

    hr = m_spSvrObject->Invoke(dispAPI_CallFunction, 
        IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, 
        &params, &varResult, NULL, NULL);

    if (SUCCEEDED(hr) && ppResult != NULL && (varResult.vt & VT_ARRAY) )
    {
        *ppResult = SafeArray2DataBuffer( varResult.parray );
    }

    return hr;
}
