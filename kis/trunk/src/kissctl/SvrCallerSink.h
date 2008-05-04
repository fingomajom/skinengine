#pragma once

class CSvrCaller;

class CSvrCallerSink :
    public IDispEventSimpleImpl<1, CSvrCallerSink, &__uuidof(IDispatch)>
{
    typedef IDispEventSimpleImpl<1, CSvrCallerSink, &__uuidof(IDispatch)> theBase;

public:
    CSvrCallerSink(void);
    ~CSvrCallerSink(void);

    BEGIN_SINK_MAP(CSvrCallerSink)
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

    STDMETHODIMP GetTypeInfoCount(unsigned int *)
    {	return E_NOTIMPL;	}	

    STDMETHODIMP GetTypeInfo(unsigned int,unsigned long,struct ITypeInfo ** )
    {	return E_NOTIMPL;	}	

    STDMETHODIMP GetIDsOfNames( 
        /* [in] */ REFIID riid,
        /* [size_is][in] */ LPOLESTR *rgszNames,
        /* [in] */ UINT cNames,
        /* [in] */ LCID lcid,
        /* [size_is][out] */ DISPID *rgDispId)
    {	return E_NOTIMPL;	}	

    STDMETHODIMP Invoke(
        /* [in] */ DISPID dispIdMember,
        /* [in] */ REFIID riid,
        /* [in] */ LCID lcid,
        /* [in] */ WORD wFlags,
        /* [out][in] */ DISPPARAMS *pDispParams,
        /* [out] */ VARIANT *pVarResult,
        /* [out] */ EXCEPINFO *pExcepInfo,
        /* [out] */ UINT *puArgErr);


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

public:

    BOOL m_bDisped;

    CSvrCaller* m_pSvrCaller;

};
