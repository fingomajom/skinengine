// SvrCaller.h : Declaration of the CSvrCaller

#pragma once
#include "resource.h"       // main symbols

#include "..\public\lseiddef.h"
#include "..\public\lsectl.h"
#include "..\public\lsecaller.h"

class CSvrCaller :
	public ILSECaller
{
public:
    CSvrCaller() :
        m_dwRef(0)  
	{
	}


    ULONG STDMETHODCALLTYPE AddRef(void) 
    { 
        return (++m_dwRef); 
    }

    ULONG STDMETHODCALLTYPE Release( void) 
    { 
        if (--m_dwRef == 0)
        {
            delete this;
        }

        return (m_dwRef);
    } 

    HRESULT STDMETHODCALLTYPE QueryInterface( 
        /* [in] */ REFIID riid, 
        /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject) 
    {

        if ((riid) == __uuidof(ILSECaller)) 
        { 
            *ppvObject = static_cast<ILSECaller*>(this); 
            AddRef();
            return S_OK;
        }
        else if ((riid) == IID_IUnknown)
        {
            *ppvObject = static_cast<IUnknown*>(this); 
            AddRef();
            return S_OK;
        }
        return E_FAIL; 
    }

    virtual HRESULT STDMETHODCALLTYPE Initialize( 
        /* [in] */ ULONG     uCallerId,
        /* [in] */ IUnknown *piCallback);
    virtual HRESULT STDMETHODCALLTYPE Uninitialize( void );
    virtual HRESULT STDMETHODCALLTYPE CallSvrFunc( 
        /* [in]  */ ULONG uTargetId,
        /* [in]  */ ULONG uFunctionId,
        /* [in]  */ IDataBuffer*  pParameter,
        /* [out] */ IDataBuffer** ppResult);


private:

    UINT m_dwRef;
    
    CLSECaller m_lseCaller;
};

//OBJECT_ENTRY_AUTO(__uuidof(SvrCaller), CSvrCaller)
