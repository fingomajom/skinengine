#include "StdAfx.h"
#include "SvrCallerSink.h"
#include "SvrCaller.h"

CSvrCallerSink::CSvrCallerSink(void) :
    m_pSvrCaller(NULL),
    m_bDisped(FALSE)
{
}

CSvrCallerSink::~CSvrCallerSink(void)
{
}

STDMETHODIMP CSvrCallerSink::Invoke(
    /* [in] */ DISPID dispIdMember,
    /* [in] */ REFIID riid,
    /* [in] */ LCID lcid,
    /* [in] */ WORD wFlags,
    /* [out][in] */ DISPPARAMS *pDispParams,
    /* [out] */ VARIANT *pVarResult,
    /* [out] */ EXCEPINFO *pExcepInfo,
    /* [out] */ UINT *puArgErr)
{
    if (m_pSvrCaller == NULL)
        return S_OK;

    if (m_pSvrCaller->m_spCallback.p != NULL)
    {
        if (pDispParams == NULL || pDispParams->cArgs != 5)
            return E_FAIL;

        if ( pDispParams->rgvarg[4].vt != VT_UI4 ||
             pDispParams->rgvarg[3].vt != VT_UI4 ||
             pDispParams->rgvarg[2].vt != VT_BSTR ||
             pDispParams->rgvarg[1].vt != VT_BSTR ||
             pDispParams->rgvarg[0].vt != (VT_BSTR | VT_BYREF) )
             return E_FAIL;

        m_pSvrCaller->m_spCallback->NotifyMessage(
            pDispParams->rgvarg[4].ulVal,
            pDispParams->rgvarg[3].ulVal,
            pDispParams->rgvarg[2].bstrVal,
            pDispParams->rgvarg[1].bstrVal,
            (BSTR*)pDispParams->rgvarg[0].byref);
    }

    {
        HRESULT hr = S_OK;

        int cConnections = m_pSvrCaller->m_vec.GetSize();

        for (int iConnection = 0; iConnection < cConnections; iConnection++)
        {
            m_pSvrCaller->Lock();
            CComPtr<IUnknown> punkConnection = m_pSvrCaller->m_vec.GetAt(iConnection);
            m_pSvrCaller->Unlock();

            IDispatch * pConnection = static_cast<IDispatch *>(punkConnection.p);

            if (pConnection)
            {
                hr = pConnection->Invoke(1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, pDispParams, pVarResult, NULL, NULL);
            }
        }

    }

    return S_OK;
}
