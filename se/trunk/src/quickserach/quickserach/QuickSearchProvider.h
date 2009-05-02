/********************************************************************
	created:	2009/04/21
	created:	21:4:2009   16:55
	filename: 	d:\Simple\quickserach\quickserach\QuickSearchProvider.h
	file path:	d:\Simple\quickserach\quickserach
	file base:	QuickSearchProvider
	file ext:	h
	author:		lidengwang@sohu-rd.com
	
	purpose:	
*********************************************************************/

#pragma once

#include <atlstr.h>
#include <atlimage.h>
#include <atlrx.h>

#include "QuickSearchConfig.h"
#include "QSearchComDef.h"

class IQuickSearchProvider
{
public:

    virtual ~IQuickSearchProvider() {}

    virtual HRESULT GetProviderInfo ( LPQSPINFO pInfo    ) = 0;
    virtual HRESULT TestKeyword     ( LPCTSTR pszKeyword ) = 0;
    virtual HRESULT GetSearchResults( LPCTSTR pszKeyword, ATL::CString& strOut ) = 0;
};


class CQuickSearchProviderImpl : public IQuickSearchProvider
{
public:
    
    CQuickSearchProviderImpl();
    ~CQuickSearchProviderImpl();

    //////////////////////////////////////////////////////////////////////////
    
    HRESULT InitProvider( LPQSPCINFO pCfgInfo );

    //////////////////////////////////////////////////////////////////////////

   
    virtual HRESULT GetProviderInfo ( LPQSPINFO pInfo    );
    virtual HRESULT TestKeyword     ( LPCTSTR pszKeyword );
    virtual HRESULT GetSearchResults( LPCTSTR pszKeyword, ATL::CString& strOut ) = 0;

    //////////////////////////////////////////////////////////////////////////

    static BOOL MacthRegExp( 
        LPCTSTR pszKeyword, 
        ATL::CSimpleArray<ATL::CString>& vtRegExp );

    static IQuickSearchProvider* CreateInstance( LPQSPCINFO pCfgInfo );
    

    static void StringMd5(const ATL::CStringA& strText, ATL::CString& md5);
    // return get length
    static DWORD HttpGet( LPCTSTR pszURL, ATL::CStringA& strOut, DWORD dwTimeOut = 5000);

private:

    // 基本信息
    
    ATL::CString m_strName;
    CImage       m_image;
    
    // 扩展信息
    ATL::CSimpleArray<ATL::CString> m_vtRegExp;
    ATL::CString m_strRequestURL;
};


//////////////////////////////////////////////////////////////////////////

class CSoGouQSearchProvider : public CQuickSearchProviderImpl
{
public:

    virtual HRESULT GetProviderInfo ( LPQSPINFO pInfo    );
    virtual HRESULT GetSearchResults( LPCTSTR pszKeyword, ATL::CString& strOut );
};

//////////////////////////////////////////////////////////////////////////
    
class CDictQSearchProvider : public CQuickSearchProviderImpl
{
public:

    virtual HRESULT GetProviderInfo ( LPQSPINFO pInfo    );
    virtual HRESULT GetSearchResults( LPCTSTR pszKeyword, ATL::CString& strOut );
};

//////////////////////////////////////////////////////////////////////////