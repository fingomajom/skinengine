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
    if (m_pSvrCaller == NULL || m_pSvrCaller->m_spCallback.p == NULL)
        return S_OK;

    if ( dispIdMember == dispAPI_CallFunction)
    {
        if (pDispParams == NULL || pDispParams->cArgs != 5)
            return E_FAIL;

        if (  pDispParams->rgvarg[4].vt != VT_UI4 ||
              pDispParams->rgvarg[3].vt != VT_UI4 ||
              pDispParams->rgvarg[2].vt != VT_UI4 ||
            ( pDispParams->rgvarg[1].vt & VT_ARRAY ) )
            return E_FAIL;

        IDataBuffer*  pParameter = NULL;
        IDataBuffer*  ppResult   = NULL;

        pParameter = SafeArray2DataBuffer( pDispParams->rgvarg[1].parray );
        if (pParameter != NULL)
            pParameter->AddRef();

        m_pSvrCaller->m_spCallback->CallbackFunction (
            pDispParams->rgvarg[4].ulVal,
            pDispParams->rgvarg[3].ulVal,
            pDispParams->rgvarg[2].ulVal,
            pParameter,
            &ppResult);

        if (pParameter != NULL)
            pParameter->Release();

        if (ppResult != NULL && pVarResult != NULL)
        {
            pVarResult[0].parray = DataBuffer2SafeArray( ppResult );
            pVarResult[0].vt = pVarResult[0].parray ? VT_UI1 | VT_ARRAY : VT_EMPTY;
        }
    }
    else if ( dispIdMember == dispAPI_Quit )
    {
        m_pSvrCaller->m_spCallback->OnQuit();
    }


    return S_OK;
}
