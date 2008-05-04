// SvrCaller.h : Declaration of the CSvrCaller

#pragma once
#include "resource.h"       // main symbols

#include "kissctl.h"
#include "_ISvrCallerEvents_CP.h"
#include "SvrCallerSink.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CSvrCaller

class ATL_NO_VTABLE CSvrCaller :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSvrCaller, &CLSID_SvrCaller>,
	public IConnectionPointContainerImpl<CSvrCaller>,
	public CProxy_ISvrCallerEvents<CSvrCaller>,
	public IDispatchImpl<ISvrCaller, &IID_ISvrCaller, &LIBID_kissctlLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
    CSvrCaller() :
        m_uCallerId(-1)
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SVRCALLER)


BEGIN_COM_MAP(CSvrCaller)
	COM_INTERFACE_ENTRY(ISvrCaller)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CSvrCaller)
	CONNECTION_POINT_ENTRY(__uuidof(_ISvrCallerEvents))
END_CONNECTION_POINT_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void    FinalRelease();

    //

    STDMETHOD(Advise)(IUnknown* pUnkSink, DWORD* pdwCookie);
    STDMETHOD(Unadvise)(DWORD dwCookie);

public:

    STDMETHOD(Initialize)(ULONG uCallerId);
    STDMETHOD(Uninitialize)(void);
    STDMETHOD(CallSvrFunc)( 
        /* [in] */ ULONG uDesModuleId,
        /* [in] */ BSTR bstrFunctionName,
        /* [in] */ BSTR bstrParameter,
        /* [retval][out] */ BSTR *bstrResult);
    STDMETHOD(RegisterCallback)(IUnknown* piCallback);

public:

    ULONG m_uCallerId;

    CComPtr<IDispatch>       m_spSvrObject;
    CComPtr<INotifyCallback> m_spCallback;


    CSvrCallerSink m_svrCallerSink;
};

OBJECT_ENTRY_AUTO(__uuidof(SvrCaller), CSvrCaller)
