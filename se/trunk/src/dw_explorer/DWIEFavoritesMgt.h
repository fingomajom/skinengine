/********************************************************************
	created:	2009/05/04
	created:	4:5:2009   16:31
	filename: 	e:\lidengwang\gcode\se\trunk\src\dw_explorer\DWIEFavoritesMgt.h
	file path:	e:\lidengwang\gcode\se\trunk\src\dw_explorer
	file base:	DWIEFavoritesMgt.h
	author:		lidengwang@sohu-rd.com
	
	purpose:	
*********************************************************************/

#pragma once

#include <atlpath.h>

struct IEFavoriteItem
{
    IEFavoriteItem()
    {
        pChildList  = NULL;
        pParentList = NULL;
    }

    ATL::CString strTitle;
    ATL::CString strURL  ;

    ATL::CAtlList<IEFavoriteItem>* pChildList;
    ATL::CAtlList<IEFavoriteItem>* pParentList;
};


class CDWIEFavoritesMgt
{
    CDWIEFavoritesMgt() {}
public:
    
    static CDWIEFavoritesMgt& Instance();
    

    BOOL BuildIEFavorites();
    
    ATL::CAtlList<IEFavoriteItem>& GetFavoriteList()
    {
        return m_FavoriteList;
    }
    

protected:

    BOOL _BuildIEFavorites( LPCTSTR pszDirectory, ATL::CAtlList<IEFavoriteItem>* pList );

    ATL::CAtlList<IEFavoriteItem> m_FavoriteList;
};



inline CDWIEFavoritesMgt& CDWIEFavoritesMgt::Instance()
{
    static CDWIEFavoritesMgt __CDWIEFavoritesMgt_Instance__;
    return __CDWIEFavoritesMgt_Instance__;
}


inline BOOL CDWIEFavoritesMgt::BuildIEFavorites()
{
    TCHAR szDirectroy[MAX_PATH] = { 0 };
    
    if ( !SHGetSpecialFolderPath( NULL, szDirectroy, CSIDL_FAVORITES, FALSE ) )
        return FALSE;

    return _BuildIEFavorites( szDirectroy, &m_FavoriteList );
}

inline BOOL CDWIEFavoritesMgt::_BuildIEFavorites( LPCTSTR pszDirectory, ATL::CAtlList<IEFavoriteItem>* pList )
{
    BOOL bResult = FALSE;

    WIN32_FIND_DATA wfd = { 0 };
    ATL::CPath path(pszDirectory);

    path.Append(L"*.*");


    HANDLE hFindFile = FindFirstFile(path, &wfd); 

    while( hFindFile != INVALID_HANDLE_VALUE) 
    {
        if( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) 
        { 
            if( StrCmp(wfd.cFileName, _T(".")) && 
                StrCmp(wfd.cFileName, _T("..")) ) 
                
            {
                IEFavoriteItem fItem;
                fItem.strTitle = wfd.cFileName;
                fItem.pParentList = pList;
                fItem.pChildList = new ATL::CAtlList<IEFavoriteItem>;
                ATLASSERT(fItem->pChildList);

                if ( fItem.pChildList )
                {
                    ATL::CPath subPath(pszDirectory);
                    subPath.Append(wfd.cFileName);

                    if ( _BuildIEFavorites( subPath, fItem.pChildList ) )
                        pList->AddHead( fItem );
                    else
                    {
                        delete fItem.pChildList;
                    }
                }
            }
        }
        else if ( !(wfd.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM) ) )
        {
            TCHAR szURL[INTERNET_MAX_PATH_LENGTH] = { 0 }; 

            ATL::CPath filePath(pszDirectory);
            filePath.Append(wfd.cFileName);

            GetPrivateProfileString(_T("InternetShortcut"), _T("URL"), 
                _T(""), szURL, INTERNET_MAX_PATH_LENGTH, filePath); 
            
            if ( lstrlenW(szURL) > 0 )
            {
                IEFavoriteItem fItem;

                fItem.strTitle = wfd.cFileName;
                fItem.strURL   = szURL;
                fItem.pParentList = pList;
                
                pList->AddTail( fItem );
            }
        }
        
        if ( !FindNextFile(hFindFile, &wfd) )
            break;
    }

    if ( hFindFile != INVALID_HANDLE_VALUE )
        FindClose(hFindFile); 

    return hFindFile != INVALID_HANDLE_VALUE;
}
