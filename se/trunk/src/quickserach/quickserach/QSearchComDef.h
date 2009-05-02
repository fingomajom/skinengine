/********************************************************************
	created:	2009/04/28
	created:	28:4:2009   10:32
	filename: 	d:\Simple\quickserach\quickserach\QSearchComDef.h
	file path:	d:\Simple\quickserach\quickserach
	file base:	QSearchComDef.h
	author:		lidengwang@sohu-rd.com
	
	purpose:	
*********************************************************************/

#pragma once

enum {
    PTYPE_SOGOU,
    PTYPE_DICT
};

typedef struct tag_QuickSearchProviderInfo
{    
    LPCTSTR pszName;
    HBITMAP hImage;
    int     nPType;

}QSPINFO, *LPQSPINFO;


MIDL_INTERFACE("1EBC45EF-F095-433e-AD2F-8B8554F10DBD")
IQuickSearchEventCallback : IUnknown
{
public:

    virtual HRESULT STDMETHODCALLTYPE OnQSearchResult( 
        /* [in ] */ LPCWSTR   pszResult,
        /* [in ] */ LPQSPINFO pQSInfo ) = 0;

};

MIDL_INTERFACE("9D7A83DE-53BD-4070-BCD9-215AB538F95D")
IQuickSearchService : IUnknown
{
public:

    virtual HRESULT STDMETHODCALLTYPE QuickSearchKeyword( 
        /* [out] */ LPCWSTR pszKeyword ) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetQSearchProviderCount( 
        /* [out] */ int *pCount ) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetQSearchProviderInfo( 
        /* [in ] */ int iIndex,
        /* [out] */ LPQSPINFO pInfo ) = 0;

};
