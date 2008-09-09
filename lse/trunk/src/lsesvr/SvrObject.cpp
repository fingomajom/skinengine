// SvrObject.cpp : Implementation of CSvrObject

#include "stdafx.h"
#include "SvrObject.h"
#include "..\public\IDataBuffer.h"
#include "ModuleMgt.h"
// CSvrObject


extern HRESULT StopService();

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
    CModuleMgt& moduleMgt = CModuleMgt::Instance();
    
    if (dispidMember == dispAPI_CallFunction)
    {
        return moduleMgt.Invoke( pdispparams, pvarResult );
    }
    else if (dispidMember == dispAPI_Quit)
    {
        StopService();
    }

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CSvrObject::CallSvrFunc( 
    /* [in]  */ ULONG uTargetId,
    /* [in]  */ ULONG uCallerId,
    /* [in]  */ ULONG uFunctionId,
    /* [in]  */ IDataBuffer*  pParameter,
    /* [out] */ IDataBuffer** ppResult)
{
    CModuleMgt& moduleMgt = CModuleMgt::Instance();

    return moduleMgt.CallSvrFunc( 
        uTargetId, 
        uCallerId, 
        uFunctionId, 
        pParameter, 
        ppResult );

    return S_OK;
}


HRESULT STDMETHODCALLTYPE CSvrObject::Advise(IUnknown* pUnkSink, DWORD* pdwCookie)
{
    CModuleMgt& moduleMgt = CModuleMgt::Instance();

    HRESULT hResult = CProxy_ISvrObjectEvents<CSvrObject>::Advise(pUnkSink, pdwCookie);
    if (SUCCEEDED(hResult))
        moduleMgt.AddCallerCallback(pUnkSink, *pdwCookie);

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CSvrObject::Unadvise(DWORD dwCookie)
{
    CModuleMgt& moduleMgt = CModuleMgt::Instance();

    HRESULT hResult = CProxy_ISvrObjectEvents<CSvrObject>::Unadvise(dwCookie);
    if (SUCCEEDED(hResult))
        moduleMgt.RemoveCallerCallback(dwCookie);

    return S_OK;
}
