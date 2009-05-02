/********************************************************************
	created:	2009/04/21
	created:	21:4:2009   16:51
	filename: 	d:\Simple\quickserach\quickserach\QuitSearchMgr.h
	file path:	d:\Simple\quickserach\quickserach
	file base:	QuickSearchMgr
	file ext:	h
	author:		lidengwang@sohu-rd.com
	
	purpose:	
*********************************************************************/

#pragma once

#include "QSearchPopWnd.h"
#include "QuickSearchProvider.h"

#define WM_QS_KEYWORD   ( WM_USER + 1001 )

class IQuickSearchProvider;

class CQuickSearchMgr :
    public CComObjectRootEx<CComSingleThreadModel>,
    public IQuickSearchService
{
public:
    
    CQuickSearchMgr();
    ~CQuickSearchMgr();

public:
    
    static CQuickSearchMgr& Instance();


    BOOL Initialize(HWND hWnd);
    BOOL Uninitialize();

    BOOL QSearchKeyword( LPCTSTR pszKeyword );

    virtual HRESULT STDMETHODCALLTYPE QuickSearchKeyword( 
        /* [out] */ LPCWSTR pszKeyword );

    virtual HRESULT STDMETHODCALLTYPE GetQSearchProviderCount( 
        /* [out] */ int *pCount );

    virtual HRESULT STDMETHODCALLTYPE GetQSearchProviderInfo( 
        /* [in ] */ int iIndex,
        /* [out] */ LPQSPINFO pInfo );


    BEGIN_COM_MAP(CQuickSearchMgr)
        COM_INTERFACE_ENTRY(IQuickSearchService)
        COM_INTERFACE_ENTRY(IUnknown)
    END_COM_MAP()

protected:

    BOOL LoadQSearchProvider();
    
    DWORD RunThreadFunc();
    static DWORD WINAPI QSThreadProc( LPVOID );

    BOOL OnQSearchResult( LPCTSTR pszResult, LPQSPINFO pQSInfo);

public:

    DWORD        m_dwThreadId;
    ATL::CString m_strKeyword;
    CComAutoCriticalSection m_cs;

    CComPtr<IQuickSearchEventCallback> m_spCallback;

    CComObject<CQSearchPopWnd> m_wndQSPopWnd;

    ATL::CSimpleArray<IQuickSearchProvider*> m_vtQSearchProvider;

};