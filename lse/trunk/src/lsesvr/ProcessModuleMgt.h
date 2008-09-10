/********************************************************************
* CreatedOn: 2008-9-8   23:09
* FileName: ProcessModuleMgt.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include "..\public\lsesvr_m.h"
#include "ModuleInfo.h"


class CProcessModuleMgt : 
    public CComObjectRootEx<CComSingleThreadModel>,
    public IDispEventSimpleImpl<1, CProcessModuleMgt, &__uuidof(IDispatch)>,
    public ISvrCallbackObject
{
    typedef IDispEventSimpleImpl<1, CProcessModuleMgt, &__uuidof(IDispatch)> theBase;

public:
    CProcessModuleMgt(void);
    ~CProcessModuleMgt(void);


    HRESULT ProcessModule( LPCTSTR pszDllFile );


    BEGIN_SINK_MAP(CProcessModuleMgt)
    END_SINK_MAP()


    STDMETHODIMP QueryInterface(const struct _GUID &iid,void ** ppv)
    {
        *ppv = this;
        return S_OK;
    }

    ULONG __stdcall AddRef(void)
    {	return 1;	}

    ULONG __stdcall Release(void)
    {	return 0;	}	

    STDMETHODIMP GetTypeInfoCount(UINT *);

    STDMETHODIMP GetTypeInfo(unsigned int,unsigned long,struct ITypeInfo ** );

    STDMETHODIMP GetIDsOfNames( 
        /* [in] */ REFIID riid,
        /* [size_is][in] */ LPOLESTR *rgszNames,
        /* [in] */ UINT cNames,
        /* [in] */ LCID lcid,
        /* [size_is][out] */ DISPID *rgDispId);

    STDMETHODIMP Invoke(
        /* [in] */ DISPID dispIdMember,
        /* [in] */ REFIID riid,
        /* [in] */ LCID lcid,
        /* [in] */ WORD wFlags,
        /* [out][in] */ DISPPARAMS *pDispParams,
        /* [out] */ VARIANT *pVarResult,
        /* [out] */ EXCEPINFO *pExcepInfo,
        /* [out] */ UINT *puArgErr);

    HRESULT STDMETHODCALLTYPE CallSvrFunc( 
        /* [in]  */ ULONG uTargetId,
        /* [in]  */ ULONG uCallerId,
        /* [in]  */ ULONG uFunctionId,
        /* [in]  */ IDataBuffer*  pParameter,
        /* [out] */ IDataBuffer** ppResult);


    HRESULT DispEventAdvise(IUnknown* pUnk)
    {
        if ( m_bDisped )
            DispEventUnadvise( pUnk );

        HRESULT hr = theBase::DispEventAdvise(pUnk);

        if ( SUCCEEDED(hr) )
            m_bDisped = TRUE;

        return hr;
    }

    HRESULT DispEventUnadvise(IUnknown* pUnk)
    {
        if ( !m_bDisped )
            return S_OK;

        HRESULT hr = theBase::DispEventUnadvise(pUnk);

        if ( SUCCEEDED(hr) )
            m_bDisped = FALSE;

        return hr;
    }

private:

    HRESULT Relive();


    BOOL m_bDisped;

    HANDLE m_hQuitEvent;
    
    CModuleInfo m_ModuleInfo;
    CComPtr<IDispatch> m_spSvrObject;

};
