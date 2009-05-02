#include "StdAfx.h"
#include "QuickSearchConfig.h"
#include "QuickSearchProvider.h"
#include "Markup.h"
#include <shlobj.h>
#include <atlfile.h>
#include <atlutil.h>


CQuickSearchConfig::CQuickSearchConfig(void)
{
}

CQuickSearchConfig::~CQuickSearchConfig(void)
{
}

BOOL CQuickSearchConfig::GetQuickSearchProviderConfig( 
    ATL::CAtlArray<QSPCINFO>& cfg )
{
    TCHAR           szQSearchPath[MAX_PATH] = { 0 } ;
    HANDLE          hFindFile = NULL;
    ATL::CString    strQSearchFile;
    WIN32_FIND_DATA FindData = { 0 };


    ::SHGetSpecialFolderPath(NULL, szQSearchPath, CSIDL_APPDATA, TRUE );
    PathAppend(szQSearchPath, _T("SogouExplorer\\QSearch") );

    strQSearchFile  = szQSearchPath;
    strQSearchFile += _T("\\*.xml");

    hFindFile = ::FindFirstFile(strQSearchFile, &FindData);

    if ( hFindFile != INVALID_HANDLE_VALUE )
    {
        BOOL bLoop = TRUE;

        cfg.RemoveAll();

        while ( bLoop )
        {
            QSPCINFO qspcInfo;

            ATL::CPath pathFile;
            pathFile = (szQSearchPath);
            pathFile.Append(FindData.cFileName);

            qspcInfo.strFileName  = FindData.cFileName;
            qspcInfo.strIconFile  = pathFile.m_strPath;
            qspcInfo.strIconFile += _T(".ico");
            
            if ( AnalyzeQSearchFile(pathFile, qspcInfo) )
            {
                size_t ipos = 0;

                // 按文件名顺序排序
                for( ; ipos < cfg.GetCount() ; ipos++ )
                    if ( cfg[ipos].strFileName.CompareNoCase( FindData.cFileName ) > 0 )
                        break;

                cfg.InsertAt( ipos, qspcInfo );
            }

            bLoop = ::FindNextFile(hFindFile, &FindData);
        }

        ::FindClose(hFindFile);
    }

    return TRUE;
}

BOOL CQuickSearchConfig::AnalyzeQSearchFile( LPCTSTR pszCfgFile, QSPCINFO& info )
{
    BOOL    bResult = FALSE;    
    CMarkup markup;


    while ( markup.Load( pszCfgFile ) )
    {
        if ( !markup.IsWellFormed() ) 
            break;

        if (!markup.FindElem(_T("OpenQSearchDescription")) || !markup.IntoElem())
            break;
        
        if (!markup.FindElem(_T("ShortName")))
            break;
        info.strQSName = markup.GetData().c_str();

        markup.ResetMainPos();
        if (!markup.FindElem(_T("Description")))
            break;
        info.strDescription = markup.GetData().c_str();

        markup.ResetMainPos();
        if (!markup.FindElem(_T("RequestInfo")))
            break;
        info.strRequestInfo = markup.GetAttrib(_T("template")).c_str();


        markup.ResetMainPos();
        if (!markup.FindElem(_T("ReqExp")) || !markup.IntoElem() )
            break;
        
        info.m_vtReg.RemoveAll();

        while ( markup.FindElem() )
        {
            info.m_vtReg.Add( markup.GetAttrib(_T("regular")).c_str() );
        }

        markup.OutOfElem();
        markup.ResetMainPos();
        if (!markup.FindElem(_T("moz:Image")))
            break;
        
        ATL::CString strImage = markup.GetData().c_str();

        CreateIconFile( info.strIconFile, strImage );

        bResult = TRUE;
        break;
    }

    return bResult;
}

BOOL CQuickSearchConfig::CreateIconFile( LPCTSTR pszIconFile, LPCTSTR pszData )
{
    BOOL   bResult  = TRUE;
    size_t uDataLen = wcslen(pszData);

    if ( uDataLen == NULL )
        return FALSE;

    if (!::PathFileExists(pszIconFile) )
    {
        CAtlFile file;

        bResult = FALSE;
        if ( file.Create( pszIconFile, GENERIC_WRITE, 0 , CREATE_ALWAYS) == S_OK )
        {
            DWORD dwLen = 0;

            ATL::CString  strImageTemp;
            ATL::CStringA strImageTempA;

            AtlUnescapeUrl( pszData, 
                strImageTemp.GetBuffer( uDataLen + 1 ), 
                &dwLen, uDataLen );
            strImageTemp.ReleaseBuffer();

            strImageTempA = strImageTemp;
            int iStart = strImageTempA.Find(',');
            if (iStart >= 0)
                strImageTempA = strImageTempA.Mid(iStart + 1);

            int nDecodeLength = ATL::Base64DecodeGetRequiredLength(strImageTempA.GetLength());

            CTempBuffer<BYTE> buffer(nDecodeLength);

            ATL::Base64Decode(
                strImageTempA, strImageTempA.GetLength(), 
                buffer, &nDecodeLength);

            file.Write( buffer, nDecodeLength );

            bResult = TRUE;
        }
    }
    
    return bResult;
}
