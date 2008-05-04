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

STDMETHODIMP CSvrCallerSink::GetTypeInfoCount(UINT* pctinfo)
{	
    if (pctinfo != NULL && m_pSvrCaller != NULL)
        *pctinfo = m_pSvrCaller->m_uCallerId;

    return S_OK;
}	

STDMETHODIMP CSvrCallerSink::GetTypeInfo(unsigned int,unsigned long,struct ITypeInfo ** )
{	
    return E_NOTIMPL;	
}	

STDMETHODIMP CSvrCallerSink::GetIDsOfNames( 
                           /* [in] */ REFIID riid,
                           /* [size_is][in] */ LPOLESTR *rgszNames,
                           /* [in] */ UINT cNames,
                           /* [in] */ LCID lcid,
                           /* [size_is][out] */ DISPID *rgDispId)
{	
    if (rgDispId != NULL && m_pSvrCaller != NULL)
        *rgDispId = m_pSvrCaller->m_uCallerId;

    return E_NOTIMPL;	
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
        if (pDispParams == NULL || pDispParams->cArgs != 4)
            return E_FAIL;

        if ( pDispParams->rgvarg[4].vt != VT_UI4 ||
             pDispParams->rgvarg[3].vt != VT_UI4 ||
             pDispParams->rgvarg[2].vt != VT_BSTR ||
             pDispParams->rgvarg[1].vt != VT_BSTR )
             return E_FAIL;

        BSTR bstrResult = NULL;

        m_pSvrCaller->m_spCallback->NotifyMessage(
            pDispParams->rgvarg[4].ulVal,
            pDispParams->rgvarg[3].ulVal,
            pDispParams->rgvarg[2].bstrVal,
            pDispParams->rgvarg[1].bstrVal,
            &bstrResult);

        if (bstrResult != NULL)
        {
            pVarResult->vt = VT_BSTR;
            pVarResult->bstrVal = bstrResult;
        }
    }

    {
        HRESULT hr = S_OK;

        if (m_pSvrCaller->m_jsCallbackId != DISPID_UNKNOWN &&
            m_pSvrCaller->m_spScriptDisp.p != NULL)
        {
            hr = m_pSvrCaller->m_spScriptDisp->Invoke(
                m_pSvrCaller->m_jsCallbackId, 
                IID_NULL, 
                LOCALE_USER_DEFAULT, 
                DISPATCH_METHOD, 
                pDispParams, 
                pVarResult, 
                NULL, 
                NULL);
        }

    }

    return S_OK;
}
