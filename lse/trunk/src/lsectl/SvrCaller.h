// SvrCaller.h : Declaration of the CSvrCaller

#pragma once
#include "resource.h"       // main symbols

#include "SvrCallerSink.h"
#include "..\public\lseiddef.h"
#include "..\public\lsectl.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CSvrCaller

class ATL_NO_VTABLE CSvrCaller :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSvrCaller, &__uuidof(ISvrCaller)>,
	public ISvrCaller
{
    friend class CSvrCallerSink;
public:
    CSvrCaller() :
        m_uCallerId(CALLERID_UNKNOWN)
	{
	}

DECLARE_REGISTRY_RESOURCEID(2)


BEGIN_COM_MAP(CSvrCaller)
	COM_INTERFACE_ENTRY(ISvrCaller)
	COM_INTERFACE_ENTRY(IUnknown)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
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


public:


private:

    HRESULT Relive();

    ULONG m_uCallerId;

    CComPtr<IDispatch>       m_spSvrObject;
    CComPtr<INotifyCallback> m_spCallback;

    CSvrCallerSink m_svrCallerSink;

};

OBJECT_ENTRY_AUTO(__uuidof(SvrCaller), CSvrCaller)
