// SvrObject.h : Declaration of the CSvrObject

#pragma once
#include "resource.h"       // main symbols

#include "kissvr.h"
#include "_ISvrObjectEvents_CP.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

class CSvrObject;

class CSvrObjectFactory :
    public CComClassFactorySingleton<CSvrObject>
{
public:
    
    CSvrObjectFactory();
};


// CSvrObject

class ATL_NO_VTABLE CSvrObject :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSvrObject, &CLSID_SvrObject>,
	public IConnectionPointContainerImpl<CSvrObject>,
	public CProxy_ISvrObjectEvents<CSvrObject>,
    public ISvrObject
{
public:

    CSvrObject()
    {
        InternalAddRef();
        InternalAddRef();
    }

static CSvrObject& instance();

DECLARE_CLASSFACTORY_EX(CSvrObjectFactory);

DECLARE_REGISTRY_RESOURCEID(IDR_SVROBJECT)


BEGIN_COM_MAP(CSvrObject)
	COM_INTERFACE_ENTRY(ISvrObject)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CSvrObject)
    CONNECTION_POINT_ENTRY(__uuidof(IDispatch))
END_CONNECTION_POINT_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

    STDMETHOD(Advise)(IUnknown* pUnkSink, DWORD* pdwCookie);
    STDMETHOD(Unadvise)(DWORD dwCookie);


public:

    STDMETHOD(GetTypeInfoCount)(UINT* pctinfo);
    STDMETHOD(GetTypeInfo)(UINT itinfo, LCID lcid, ITypeInfo** pptinfo);
    STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR* rgszNames, UINT cNames,
        LCID lcid, DISPID* rgdispid);
    STDMETHOD(Invoke)(DISPID dispidMember, REFIID riid,
        LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult,
        EXCEPINFO* pexcepinfo, UINT* puArgErr);

    HRESULT STDMETHODCALLTYPE CallSvrFunc( 
        /* [in] */ ULONG uDesModuleId,
        /* [in] */ ULONG uSrcCallerId,
        /* [in] */ BSTR bstrFunctionName,
        /* [in] */ BSTR bstrParameter,
        /* [out] */ BSTR *bstrResult);


};

OBJECT_ENTRY_AUTO(__uuidof(SvrObject), CSvrObject)
