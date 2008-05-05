// SvrObject.cpp : Implementation of CSvrObject

#include "stdafx.h"
#include "SvrObject.h"
#include "SvrCallbackMgt.h"
#include "../../publish/kismoduleiddef.h"


CSvrObjectFactory::CSvrObjectFactory()
{
    CSvrObject::instance().QueryInterface(IID_IUnknown, (void**)&m_spObj);
    m_hrCreate = S_OK;
}


CSvrObject& CSvrObject::instance()
{
    static CComObject<CSvrObject> gs_ComSvrObject;

    return gs_ComSvrObject;
}

// CSvrObject

HRESULT STDMETHODCALLTYPE CSvrObject::GetTypeInfoCount(UINT* pctinfo)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CSvrObject::GetTypeInfo(UINT itinfo, LCID lcid, ITypeInfo** pptinfo)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CSvrObject::GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames,
    LCID lcid, DISPID* rgdispid)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CSvrObject::Invoke(DISPID dispidMember, REFIID riid,
    LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult,
    EXCEPINFO* pexcepinfo, UINT* puArgErr)
{
    if (dispidMember != 0xABCDEF ||
        pdispparams == NULL || pdispparams->cArgs != 4 ||
        pvarResult == NULL)
    {
        return E_NOTIMPL;
    }

    if ( pdispparams->rgvarg[3].vt != VT_UI4 ||
         pdispparams->rgvarg[2].vt != VT_UI4 ||
         pdispparams->rgvarg[1].vt != VT_BSTR ||
         pdispparams->rgvarg[0].vt != VT_BSTR )
    {
        return E_FAIL;
    }

    BSTR bstrResult = NULL;

    CallSvrFunc(
        pdispparams->rgvarg[3].ulVal,
        pdispparams->rgvarg[2].ulVal,
        pdispparams->rgvarg[1].bstrVal,
        pdispparams->rgvarg[0].bstrVal,
        &bstrResult);

    if (bstrResult != NULL)
    {
        pvarResult->vt = VT_BSTR;
        pvarResult->bstrVal = bstrResult;
    }

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CSvrObject::CallSvrFunc( 
    /* [in] */ ULONG uDesModuleId,
    /* [in] */ ULONG uSrcCallerId,
    /* [in] */ BSTR bstrFunctionName,
    /* [in] */ BSTR bstrParameter,
    /* [out] */ BSTR *bstrResult)
{
    if ( IS_CALLERID(uDesModuleId) )
    {
        SvrCallbackMgt& callbackMgt = SvrCallbackMgt::Instance();

        return callbackMgt.Fire_NotifyMessage(
            uDesModuleId, 
            uSrcCallerId, 
            bstrFunctionName, 
            bstrParameter, 
            bstrResult);

    }

    return S_OK;
}


HRESULT STDMETHODCALLTYPE CSvrObject::Advise(IUnknown* pUnkSink, DWORD* pdwCookie)
{
    SvrCallbackMgt& callbackMgt = SvrCallbackMgt::Instance();

    HRESULT hResult = CProxy_ISvrObjectEvents<CSvrObject>::Advise(pUnkSink, pdwCookie);
    if (SUCCEEDED(hResult))
        callbackMgt.AddCallback(pUnkSink, *pdwCookie);

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CSvrObject::Unadvise(DWORD dwCookie)
{
    SvrCallbackMgt& callbackMgt = SvrCallbackMgt::Instance();

    HRESULT hResult = CProxy_ISvrObjectEvents<CSvrObject>::Unadvise(dwCookie);
    if (SUCCEEDED(hResult))
        callbackMgt.RemoveCallback(dwCookie);

    return S_OK;
}
