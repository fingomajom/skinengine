// iTreeSurfNSHandle.h: Definition of the CiTreeSurfNSHandle class
//
//////////////////////////////////////////////////////////////////////

#if !defined( _ITREESURF__ITREESURFNSHANDLE_H__INCLUDED_ )
#define _ITREESURF__ITREESURFNSHANDLE_H__INCLUDED_

#if _MSC_VER > 1000         
#pragma once                    
#endif // _MSC_VER > 1000                  

#include "resource.h"       // main symbols

////////////////////////////////////////////////////////////////////////////
// CiTreeSurfNSHandle

class CiTreeSurfNSHandle : 
	public IiTreeSurfNSHandle,
	public CComObjectRoot,
	public CComCoClass<CiTreeSurfNSHandle,&CLSID_iTreeSurfNSHandle>,
	public IInternetProtocolInfo,
	public IInternetProtocol
{
public:
	CiTreeSurfNSHandle() ;

BEGIN_COM_MAP(CiTreeSurfNSHandle)
	COM_INTERFACE_ENTRY(IiTreeSurfNSHandle)
	COM_INTERFACE_ENTRY_IID(IID_IInternetProtocol, IInternetProtocol)
	COM_INTERFACE_ENTRY_IID(IID_IInternetProtocolRoot, IInternetProtocol)
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(CiTreeSurfNSHandle) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

DECLARE_REGISTRY_RESOURCEID(IDR_iTreeSurfNSHandle)

// IiTreeSurfNSHandle
public:
	IInternetProtocolSink* m_pProtSink;	// sink interface handed to us a

// IInternetProtocol interface
public:
    STDMETHOD(Start)(
            LPCWSTR szUrl,
            IInternetProtocolSink *pIProtSink,
            IInternetBindInfo *pIBindInfo,
            DWORD grfSTI,
            DWORD dwReserved);
    STDMETHOD(Continue)(PROTOCOLDATA *pStateInfo);
    STDMETHOD(Abort)(HRESULT hrReason,DWORD dwOptions);
    STDMETHOD(Terminate)(DWORD dwOptions);
    STDMETHOD(Suspend)();
    STDMETHOD(Resume)();
    STDMETHOD(Read)(void *pv,ULONG cb,ULONG *pcbRead);
    STDMETHOD(Seek)(
            LARGE_INTEGER dlibMove,
            DWORD dwOrigin,
            ULARGE_INTEGER *plibNewPosition);
    STDMETHOD(LockRequest)(DWORD dwOptions);
    STDMETHOD(UnlockRequest)();

public:
	BOOL PatternFilter(CString & url);
	STDMETHOD(CombineUrl)(LPCWSTR pwzBaseUrl, LPCWSTR pwzRelativeUrl, DWORD dwCombineFlags,
		LPWSTR pwzResult, DWORD cchResult, DWORD *pcchResult, DWORD dwReserved);
	STDMETHOD(CompareUrl)(LPCWSTR pwzUrl1, LPCWSTR pwzUrl2, DWORD dwCompareFlags);
	STDMETHOD(ParseUrl)(LPCWSTR pwzUrl, PARSEACTION ParseAction, DWORD dwParseFlags,
		LPWSTR pwzResult, DWORD cchResult, DWORD *pcchResult, DWORD dwReserved);
	STDMETHOD(QueryInfo)( LPCWSTR pwzUrl, QUERYOPTION QueryOption, DWORD dwQueryFlags,
		LPVOID pBuffer, DWORD cbBuffer, DWORD *pcbBuf, DWORD dwReserved);
};



#endif // !defined( _ITREESURF__ITREESURFNSHANDLE_H__INCLUDED_ )
