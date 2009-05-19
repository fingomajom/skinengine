

#pragma once

class CDWConfigDB
{
public:   
    CDWConfigDB( LPCTSTR pszFileName,  LPCTSTR pszAppName )
    {
        m_strPathFile = GetConfigFilePath(pszFileName);
        m_strAppName  = pszAppName;
    }
    ~CDWConfigDB() 
    {}
    
    BOOL Load()
    {        
        return TRUE;
    }

    BOOL Save()
    {
        return TRUE;
    }

    BOOL GetConfig( const ATL::CString& strKey, ATL::CString& strValue )
    {
        TCHAR szBuffer[2048] = { 0 };

        GetPrivateProfileString( m_strAppName, 
            strKey, 
            L"",
            szBuffer,
            2047,
            m_strPathFile);

        strValue = szBuffer;

        return TRUE;
    }

    BOOL GetConfig( const ATL::CString& strKey, int& nValue, int nDef = 0)
    {
        nValue = GetPrivateProfileInt( m_strAppName, strKey, nDef, m_strPathFile );

        return TRUE;
    }

    BOOL SetConfig( const ATL::CString& strKey, const ATL::CString& strValue )
    {
        return WritePrivateProfileString( m_strAppName, strKey, strValue, m_strPathFile);
    }

    BOOL SetConfig( const ATL::CString& strKey, int nValue )
    {
        ATL::CString strValue;
        strValue.Format(L"%d", nValue);
        return SetConfig( strKey, strValue );
    }

protected:

    ATL::CString GetConfigFilePath( LPCTSTR pszFileName )
    {
        TCHAR szPath[MAX_PATH] = { 0 };

        ::SHGetSpecialFolderPath(NULL, szPath, CSIDL_APPDATA, TRUE);    
        ::PathAppend( szPath, L"sexplorer" ); 
        ::CreateDirectory( szPath, NULL );
        ::PathAppend( szPath, pszFileName ); 

        return ATL::CString(szPath);
    }


private:    
    ATL::CString m_strPathFile;
    ATL::CString m_strAppName;
};


class CDWBaseConfig : public CDWConfigDB
{
public:
    CDWBaseConfig() :
        CDWConfigDB(L"sse.dat", L"BaseConfig")
    {
        Load();
    }

    ~CDWBaseConfig()
    {
        Save();
    }

    COLORREF get_Window_Color(COLORREF clrDef)  
    { 
        COLORREF clrRet = clrDef; 
        if ( !GetConfig( L"WindowColor", (int&)clrRet, clrDef ) )
            return clrRet;
        return clrRet;
    }

    COLORREF set_Window_Color(COLORREF clr)  
    { 
        return SetConfig(L"WindowColor", clr);
    }

};

class CDWURLHistoryCfg : public CDWConfigDB
{
public:
    CDWURLHistoryCfg() :
        CDWConfigDB(L"hty.dat", L"hty")
    {
        Load();
    }

    ~CDWURLHistoryCfg()
    {
        Save();
    }

    int GetCount()
    {
        int nCount = 0;
        GetConfig( L"CT", nCount );
        return nCount;
    }

    void GetURLHistory( int idx, 
        ATL::CString& strTitle,
        ATL::CString& strURL,
        int& nOrder)
    {
        ATL::CString strKey;
        strKey.Format(L"Title_%d", idx);
        GetConfig(strKey, strTitle);
        strKey.Format(L"URL_%d", idx);
        GetConfig(strKey, strURL);
        strKey.Format(L"Order_%d", idx);
        GetConfig(strKey, nOrder);        
    }

    void SetCount( int nCount )
    {
        SetConfig( L"CT", nCount );
    }

    void SetURLHistory( int idx, 
        const ATL::CString& strTitle,
        const ATL::CString& strURL,
        int& nOrder)
    {
        ATL::CString strKey;
        strKey.Format(L"Title_%d", idx);
        SetConfig(strKey, strTitle);
        strKey.Format(L"URL_%d", idx);
        SetConfig(strKey, strURL);
        strKey.Format(L"Order_%d", idx);
        SetConfig(strKey, nOrder);        
    }

};
