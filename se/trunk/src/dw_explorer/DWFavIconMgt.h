
#pragma once


#define WM_DOWNLOAD_FAV_ICON   (WM_USER + 1000)


class CDWFavIconMgt
{
    CDWFavIconMgt( const CDWFavIconMgt& );

    CDWFavIconMgt();
public:
    ~CDWFavIconMgt();

    static CDWFavIconMgt& Instance();

    HICON GetFavIcon( LPCTSTR URL, HWND hWnd, LPARAM lParam, BOOL bDownload = TRUE );

public:

    static void GetIconFileName(LPCTSTR URL, ATL::CString & strFavFile);
    static void GetFavIconURL(LPCTSTR URL, ATL::CString & strFavURL, LPCTSTR pszSp = L"");

    static HICON LoadFavIconFile( LPCTSTR pszFile );

    static ATL::CString GetFavPath();

protected:
    static DWORD WINAPI FavDownloadThread( LPVOID p );
        
    ATL::CComAutoCriticalSection m_cs;
    ATL::CAtlMap<ATL::CString, HICON> m_mapFavIcon;
    
    DWORD m_dwThreadId;
};


inline CDWFavIconMgt::CDWFavIconMgt()
{
    CloseHandle( CreateThread(NULL, 0, FavDownloadThread, this, 0, &m_dwThreadId) );
}
inline CDWFavIconMgt::~CDWFavIconMgt()
{
    ::PostThreadMessage(m_dwThreadId, WM_QUIT, 0, 0);
    m_cs.Lock();

    for ( POSITION pos = m_mapFavIcon.GetStartPosition(); pos != NULL ; )
    {
        CIconHandle icon = m_mapFavIcon.GetNextValue( pos );
        icon.DestroyIcon();
    }

    m_mapFavIcon.RemoveAll();

    m_cs.Unlock();
}

inline CDWFavIconMgt& CDWFavIconMgt::Instance()
{
    static CDWFavIconMgt* __CDWFavIconMgt_Instance__ = new CDWFavIconMgt();
    return *__CDWFavIconMgt_Instance__;
}


inline  HICON CDWFavIconMgt::GetFavIcon( LPCTSTR URL, HWND hWnd, LPARAM lParam, BOOL bDownload )
{
    HICON hRet = NULL;
    
    ATL::CString strFavFile;

    GetIconFileName( URL, strFavFile );
    if ( strFavFile.GetLength() <= 0 )
        return NULL;

    m_cs.Lock();
    
    ATL::CAtlMap<ATL::CString, HICON>::CPair* pFind = 
        m_mapFavIcon.Lookup( strFavFile );

    if ( pFind != NULL )
    {
        hRet = pFind->m_value;
    }

    m_cs.Unlock();

    if ( hRet == NULL && bDownload )
    {
        LPARAM * p = new LPARAM[3];
        if ( p == NULL )
            return NULL;

        p[0] = (LPARAM)hWnd;
        p[1] = lParam;
        p[2] = bDownload;

        ::PostThreadMessage(m_dwThreadId, 
            WM_DOWNLOAD_FAV_ICON, 
            (WPARAM)SysAllocStringLen(URL, lstrlenW(URL)), 
            (LPARAM)p);
    }
    else if ( hRet == NULL && !bDownload )
    {
        ATL::CString strFavPathFile;
        ATL::CString strFavPath;

        strFavPath = GetFavPath();

        GetIconFileName( URL, strFavFile );
        if ( strFavFile.GetLength() <= 0 )
            return NULL;

        strFavPathFile = strFavPath;
        strFavPathFile += L"\\";
        strFavPathFile += strFavFile;

        hRet = LoadFavIconFile( strFavPathFile );
        
        if ( hRet )
        {
            m_cs.Lock();
            m_mapFavIcon[strFavFile] = hRet;
            m_cs.Unlock();
        }
    }
    
    return hRet;
}



inline DWORD WINAPI CDWFavIconMgt::FavDownloadThread( LPVOID p )
{
    MSG msg;

    CDWFavIconMgt& fimgt = CDWFavIconMgt::Instance();

    LPCTSTR lpszDumyAgent = L"Mozilla/4.0 (compatible; MSIE 5.00; Windows 98)" ;

    ATL::CString strFavPath;

    strFavPath = GetFavPath();

    while ( ::GetMessage(&msg, NULL, 0, 0) )
    {
        if ( msg.message == WM_DOWNLOAD_FAV_ICON )
        {
            BSTR    URL = (BSTR)msg.wParam;
            LPARAM* p   = (LPARAM*)msg.lParam;

            if ( URL == NULL || lstrlenW(URL) <= 0 || p == NULL )
            {
                if ( URL != NULL )
                    SysFreeString(URL);
                if ( p != NULL )
                    delete []p;
                continue;
            }

            HWND   hWnd      = (HWND)p[0];
            LPARAM lParam    = p[1];
            BOOL   bDownload = p[2];

            ATL::CString strURL = URL;          
            ATL::CString strFavFile;
            ATL::CString strFavPathFile;
            ATL::CString strFavURL;

            SysFreeString(URL);
            delete []p;

            GetIconFileName( strURL, strFavFile );
            if ( strFavFile.GetLength() <= 0 )
                continue;

            strFavPathFile = strFavPath;
            strFavPathFile += L"\\";
            strFavPathFile += strFavFile;

            HICON hIcon = LoadFavIconFile( strFavPathFile );
            DWORD dwLoop = 0;
            while ( hIcon == NULL && bDownload )
            {
                DWORD dwRet = 0;

                if ( dwLoop == 0 )
                    GetFavIconURL(strURL, strFavURL);
                else if ( dwLoop == 1)
                    GetFavIconURL(strURL, strFavURL, L"images/");
                else 
                    break;                
                dwLoop++;

                if ( strFavURL.GetLength() <= 0 )
                    continue;

                HINTERNET hSession = ::InternetOpen(lpszDumyAgent, 
                    INTERNET_OPEN_TYPE_PRECONFIG, 
                    NULL, NULL, 0);
                if ( hSession != NULL )
                {
                    DWORD dwTimeOut = 500;

                    InternetSetOption(hSession, INTERNET_OPTION_CONNECT_TIMEOUT, &dwTimeOut, sizeof(dwTimeOut) );
                    InternetSetOption(hSession, INTERNET_OPTION_RECEIVE_TIMEOUT, &dwTimeOut, sizeof(dwTimeOut) );
                    InternetSetOption(hSession, INTERNET_OPTION_SEND_TIMEOUT   , &dwTimeOut, sizeof(dwTimeOut) );

                    HINTERNET hRequest = ::InternetOpenUrl(hSession, strFavURL, 
                        NULL, 0, INTERNET_FLAG_RELOAD, NULL);

                    if ( hRequest != NULL )
                    {
                        HANDLE hFile = ::CreateFile( strFavPathFile, 
                            GENERIC_WRITE, NULL, NULL, 
                            CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

                        while ( INVALID_HANDLE_VALUE != hFile )
                        {
                            char  btBuffer[400] = { 0 };
                            DWORD dwFileLength  = 0;
                            DWORD dwWriteLength = 0;

                            if (!::InternetReadFile(hRequest, btBuffer, 
                                400, &dwFileLength))
                                break;
                            if (dwFileLength == 0)
                            {
                                break;
                            }

                            ::WriteFile(hFile, btBuffer, dwFileLength, 
                                &dwWriteLength, NULL);
                        }
                        
                        if ( INVALID_HANDLE_VALUE != hFile )
                            ::CloseHandle(hFile);

                        ::InternetCloseHandle(hRequest);
                    }
                    ::InternetCloseHandle(hSession);
                }

                hIcon = LoadFavIconFile( strFavPathFile );
            }

            if ( hIcon != NULL )
            {
                fimgt.m_cs.Lock();
                fimgt.m_mapFavIcon[strFavFile] = hIcon;
                fimgt.m_cs.Unlock();
            }

            if ( hIcon != NULL && ::IsWindow(hWnd) )
            {
                ::PostMessage( hWnd, WM_FAV_ICON_REFLASH, (WPARAM)hIcon, lParam );
            }
        }
    }

    return 0L;
}


inline void CDWFavIconMgt::GetIconFileName(LPCTSTR URL, ATL::CString & strFavFile)
{
    TCHAR szHost[512]  = { 0 };
    TCHAR szScheme[32] = { 0 };

    URL_COMPONENTS urlComponents = { 0 };
    urlComponents.dwStructSize = sizeof(URL_COMPONENTS);
    urlComponents.lpszHostName = szHost;
    urlComponents.dwHostNameLength = 512;
    urlComponents.lpszScheme = szScheme;
    urlComponents.dwSchemeLength = 32;

    if (!::InternetCrackUrl(URL, 0, 0, &urlComponents))
    {
        strFavFile = L"";
        return;
    }

    strFavFile.Format(_T("%s_%s_%d_fav.ico"), szScheme, szHost, urlComponents.nPort);
    strFavFile.MakeLower();
}

inline void CDWFavIconMgt::GetFavIconURL(LPCTSTR URL, ATL::CString & strFavURL, LPCTSTR pszSp)
{
    TCHAR szHost[512] = { 0 };
    TCHAR szScheme[32] = { 0 };
    URL_COMPONENTS urlComponents = { 0 };
    urlComponents.dwStructSize = sizeof(URL_COMPONENTS);
    urlComponents.lpszHostName = szHost;
    urlComponents.dwHostNameLength = 512;
    urlComponents.lpszScheme = szScheme;
    urlComponents.dwSchemeLength = 32;

    if (!::InternetCrackUrl(URL, 0, 0, &urlComponents))
    {
        strFavURL = L"";
        return;
    }

    strFavURL.Format(_T("%s://%s:%d/%sfavicon.ico"), szScheme, szHost, urlComponents.nPort, pszSp);
    strFavURL.MakeLower();
}

inline HICON CDWFavIconMgt::LoadFavIconFile( LPCTSTR pszFile )
{
    CIconHandle ico;
    ico.LoadIcon( pszFile, 16, 16, LR_LOADFROMFILE);

    return ico;
}

inline ATL::CString CDWFavIconMgt::GetFavPath()
{
    TCHAR szPath[MAX_PATH] = { 0 };
    
    ::SHGetSpecialFolderPath(NULL, szPath, CSIDL_APPDATA, TRUE);    
    ::PathAppend( szPath, L"sexplorer" ); 
    ::CreateDirectory( szPath, NULL );
    ::PathAppend( szPath, L"favicon" );
    ::CreateDirectory( szPath, NULL );


    return ATL::CString(szPath);
}
