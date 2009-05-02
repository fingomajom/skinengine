#include "stdafx.h"
#include "QuickSearchProvider.h"

#include <atlhttp.h>
#include <atlcrypt.h>


CQuickSearchProviderImpl::CQuickSearchProviderImpl()
{
}

CQuickSearchProviderImpl::~CQuickSearchProviderImpl()
{
}


HRESULT CQuickSearchProviderImpl::InitProvider( LPQSPCINFO pCfgInfo )
{
    if ( pCfgInfo == NULL )
        return E_INVALIDARG;

    m_strName       = pCfgInfo->strQSName;
    m_vtRegExp      = pCfgInfo->m_vtReg;
    m_strRequestURL = pCfgInfo->strRequestInfo;

    m_image.Load( pCfgInfo->strIconFile );

    return S_OK;
}


HRESULT CQuickSearchProviderImpl::GetProviderInfo ( LPQSPINFO pInfo    )
{
    ATLASSERT( pInfo != NULL );
    if ( pInfo == NULL )
        return E_INVALIDARG;

    pInfo->pszName = m_strName;
    pInfo->hImage  = m_image;

    return S_OK;
}

HRESULT CQuickSearchProviderImpl::TestKeyword( LPCTSTR pszKeyword )
{
    return MacthRegExp(pszKeyword, m_vtRegExp) ? S_OK : E_FAIL;
}


BOOL CQuickSearchProviderImpl::MacthRegExp( 
    LPCTSTR pszKeyword, 
    ATL::CSimpleArray<ATL::CString>& vtRegExp )
{
    BOOL bResult = FALSE;

    CAtlRegExp<CAtlRECharTraitsMB> exp;

    CW2AEX<> key( pszKeyword, CP_ACP );

    for ( int idx = 0; idx < vtRegExp.GetSize(); idx++ )
    {
        CAtlREMatchContext<CAtlRECharTraitsMB> MatchContext;

        CW2AEX<> strExp( vtRegExp[idx], CP_ACP );

        if ( exp.Parse( (CAtlRECharTraitsMB::RECHARTYPE*)strExp.m_psz ) != REPARSE_ERROR_OK )
            continue;

        if ( !exp.Match( (CAtlRECharTraitsMB::RECHARTYPE*)key.m_psz, &MatchContext) )
            continue;

        if ( MatchContext.m_uNumGroups >= 0 )
        {
            bResult = TRUE;

            break;
        }
    }

    return bResult;
}

IQuickSearchProvider* CQuickSearchProviderImpl::CreateInstance( LPQSPCINFO pCfgInfo )
{
    IQuickSearchProvider* piResult = NULL;

    if ( pCfgInfo->strFileName == L"00_sogou.xml" )
    {
        piResult = new CSoGouQSearchProvider;
        if ( piResult != NULL)
            ((CSoGouQSearchProvider*)piResult)->InitProvider( pCfgInfo );
    }
    else if ( pCfgInfo->strFileName == L"01_dict.xml" )
    {
        piResult = new CDictQSearchProvider;
        if ( piResult != NULL)
            ((CDictQSearchProvider*)piResult)->InitProvider( pCfgInfo );
    }
    
    return piResult;
}

void CQuickSearchProviderImpl::StringMd5(const ATL::CStringA& strText, ATL::CString& md5)
{
    CCryptProv prov;

    HRESULT hr = prov.Initialize();
    if( hr == 0x8009016L )
        prov.Initialize( PROV_RSA_FULL,NULL,MS_DEF_PROV,CRYPT_NEWKEYSET );

    CCryptMD5Hash hash;
    hash.Initialize( prov );
    hash.AddData( (BYTE*)(LPCSTR)strText, strText.GetLength() );

    BYTE buf[16];
    DWORD outLen;
    hash.GetValue( buf , &outLen );

    md5.Format(L"%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
        buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7],
        buf[8],buf[9],buf[10],buf[11],buf[12],buf[13],buf[14],buf[15]);
}

DWORD CQuickSearchProviderImpl::HttpGet( LPCTSTR pszURL, ATL::CStringA& strOut, DWORD dwTimeOut)
{
    DWORD dwRet = 0;
    ATL::CAtlHttpClient   http;
    ATL::CAtlNavigateData navData;
    
    navData.SetSocketTimeout( dwTimeOut );
    http.SetSocketTimeout( dwTimeOut );

    if ( !http.Navigate( pszURL, &navData) )
        return dwRet;

    dwRet = http.GetBodyLength();
    if ( dwRet == 0 )
        return dwRet;
    
    LPSTR pOutBuf = strOut.GetBuffer( dwRet + 1 );
    if ( pOutBuf == NULL )
        return 0;

    memcpy(pOutBuf, http.GetBody(), dwRet);
    pOutBuf[dwRet] = '\0';

    strOut.ReleaseBuffer();

    return dwRet;
}

HRESULT CSoGouQSearchProvider::GetProviderInfo ( LPQSPINFO pInfo )
{
    HRESULT hResult = CQuickSearchProviderImpl::GetProviderInfo(pInfo);

    pInfo->nPType = PTYPE_SOGOU;

    return hResult;
}

CStringA EscapeUrl( LPCTSTR pszURL )
{
    CStringA strURL;
    CStringA strResult;

    strURL = pszURL;
    
    DWORD dwLen = strURL.GetLength() * 8 + 1;
        
    LPSTR pszBuffer = strResult.GetBuffer( dwLen + 1 );
    if ( pszBuffer == NULL )
        return strResult;
   

    AtlEscapeUrl(strURL, pszBuffer, &dwLen, dwLen, ATL_URL_ENCODE_PERCENT);
    strResult.ReleaseBuffer();
    
    return strResult;
}

HRESULT CSoGouQSearchProvider::GetSearchResults( LPCTSTR pszKeyword, ATL::CString& strOut )
{
    LPCTSTR pszReqURL  = L"http://www.sogou.com/features/ip.jsp?query=%s";

    ATL::CString strKeyword;
    ATL::CString strURL;

    strKeyword = pszKeyword;
    strKeyword.TrimLeft();
    strKeyword.TrimRight();

    strKeyword = EscapeUrl(strKeyword);
    strURL.Format( pszReqURL, strKeyword );

    ATL::CStringA strOutA;

    DWORD dwLastTick = GetTickCount();
    HttpGet( strURL, strOutA, 1000 );
    dwLastTick = GetTickCount() - dwLastTick;

    strOut = CA2WEX<>(strOutA);

    return S_OK;
}


HRESULT CDictQSearchProvider::GetProviderInfo ( LPQSPINFO pInfo )
{
    HRESULT hResult = CQuickSearchProviderImpl::GetProviderInfo(pInfo);

    pInfo->nPType = PTYPE_DICT;

    return hResult;
}



HRESULT CDictQSearchProvider::GetSearchResults( LPCTSTR pszKeyword, ATL::CString& strOut )
{
    LPCTSTR pszReqURL  = L"http://api.dict.cn/api.php?q=%s&client=sohu&t=%s";
    LPCSTR  pszDictKey = "HETSDQVPUZXAJFQOSTNBVID";

    ATL::CString strKeyword;
    ATL::CString strURL;
    ATL::CString strMd5;
    ATL::CStringA strKey = pszDictKey;

    strKeyword = pszKeyword;
    strKeyword.TrimLeft();
    strKeyword.TrimRight();

    strKey += strKeyword;
    
    StringMd5(strKey, strMd5);  strMd5.MakeLower();
    
    strKeyword = EscapeUrl(strKeyword);
    strURL.Format(pszReqURL, strKeyword, (LPCTSTR)strMd5);

    ATL::CStringA strOutA;

    DWORD dwLastTick = GetTickCount();
    HttpGet( strURL, strOutA, 2000 );
    dwLastTick = GetTickCount() - dwLastTick;

    int nPos = strOutA.Find("<?xml ");
    if ( nPos >= 0 )
        strOut = CA2WEX<>(strOutA.Mid( nPos ), CP_UTF8);

    return S_OK;
}
