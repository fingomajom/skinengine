

#pragma once

#include "import/Markup.h"

class CDWConfigDB
{
public:   
    CDWConfigDB( LPCTSTR pszFileName ) :
        m_strFileName(pszFileName)
    {}
    ~CDWConfigDB() 
    {}
    
    BOOL Load()
    {
        BOOL bRet = FALSE;

        bRet = m_xmlfile.Load( GetConfigFileName() );
        if ( !bRet )
        {
            m_xmlfile.SetDoc( L"<?xml version=\"1.0\" encoding=\"UTF-8\"?><Config />" );
            m_bmodify = TRUE;
            Save();
        }
        m_bmodify = FALSE;
        
        return TRUE;
    }

    BOOL Save()
    {
        if ( m_bmodify )
            m_xmlfile.Save(GetConfigFileName());

        return TRUE;
    }

protected:

    ATL::CString GetConfigFileName()
    {
        TCHAR szPath[MAX_PATH] = { 0 };

        ::SHGetSpecialFolderPath(NULL, szPath, CSIDL_APPDATA, TRUE);    
        ::PathAppend( szPath, L"sexplorer" ); 
        ::CreateDirectory( szPath, NULL );
        ::PathAppend( szPath, m_strFileName ); 

        return ATL::CString(szPath);
    }


private:
    
    ATL::CString m_strFileName;
    BOOL         m_bmodify;
    CMarkup      m_xmlfile;
};