#include "StdAfx.h"
#include "SvrCallbackMgt.h"
#include "../../publish/kismoduleiddef.h"

SvrCallbackMgt::SvrCallbackMgt(void)
{
}

SvrCallbackMgt::~SvrCallbackMgt(void)
{
}

SvrCallbackMgt& SvrCallbackMgt::Instance()
{
    static SvrCallbackMgt gs_SvrCallbackMgt_Instance;

    return gs_SvrCallbackMgt_Instance;
}

HRESULT SvrCallbackMgt::AddCallback(IUnknown* pUnkSink, DWORD dwCookie)
{
    HRESULT hResult = E_FAIL;

    CComPtr<IDispatch> spDispatch;


    hResult = pUnkSink->QueryInterface( __uuidof(IDispatch) , 
        (void**)&spDispatch );

    if (FAILED(hResult))
        return hResult;

    Callback_Info info = { 0 , dwCookie, spDispatch };

    hResult = info.piDispatch->GetTypeInfoCount( (UINT*)&info.uCallerId );
    if (FAILED(hResult))
        return hResult;
    
    spDispatch.Detach();

    m_cs.Lock();
    
    m_callbacks.Add(info);
    
    m_cs.Unlock();

    return hResult;
}

HRESULT SvrCallbackMgt::RemoveCallback( DWORD dwCookie )
{
    HRESULT hResult = S_OK;

    m_cs.Lock();
    
    for ( int idx = 0; idx < m_callbacks.GetSize(); idx++ )
    {
        if (m_callbacks[idx].dwCookie == dwCookie)
        {
            m_callbacks.RemoveAt(idx);
            break;
        }
    }

    m_cs.Unlock();

    return hResult;
}

HRESULT SvrCallbackMgt::Fire_NotifyMessage( ULONG uDesCallerId,  ULONG uSrcModuleId,  BSTR bstrFunctionName,  BSTR bstrParameter,  BSTR * bstrResult)
{
    HRESULT hr = S_OK;

    int ncallbacks = m_callbacks.GetSize();


    CComVariant avarParams[4];

    avarParams[3] = uDesCallerId;
    avarParams[3].vt = VT_UI4;
    avarParams[2] = uSrcModuleId;
    avarParams[2].vt = VT_UI4;
    avarParams[1] = bstrFunctionName;
    avarParams[1].vt = VT_BSTR;
    avarParams[0] = bstrParameter;
    avarParams[0].vt = VT_BSTR;

    DISPPARAMS params = { avarParams, NULL, 4, 0 };


    for (int idx = 0; idx < ncallbacks; idx++)
    {
        m_cs.Lock();

        Callback_Info info = m_callbacks[idx];
        CComPtr<IDispatch> spDispatch = info.piDispatch;

        m_cs.Unlock();

        if ( info.uCallerId != CALLERID_FULL &&
             uDesCallerId   != CALLERID_FULL &&
             uDesCallerId   != info.uCallerId)
        {
             continue;
        }             

        if (spDispatch.p)
        {
            CComVariant varResult;

            hr = spDispatch->Invoke(1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);

            if (SUCCEEDED(hr) && bstrResult != NULL && varResult.vt == VT_BSTR)
            {
                *bstrResult = varResult.bstrVal;

                varResult.vt = VT_EMPTY;

                break;
            }

        }
    }

    return hr;
}