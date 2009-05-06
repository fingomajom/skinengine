// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <atlpath.h>
#include <atlctrlx.h>

class CMainDlg : 
    public CDialogImpl<CMainDlg>, 
	public CMessageFilter
{
public:
	enum { IDD = IDD_MAINDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return CWindow::IsDialogMessage(pMsg);
	}

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()

    WTL::CStatic m_title;
    WTL::CProgressBarCtrl m_pro;

    CHyperLink m_link;
    CAxWindow  m_wndAx;

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// center the dialog on the screen
		CenterWindow();

		// set icons
		HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
			IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
		SetIcon(hIcon, TRUE);
		HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
			IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
		SetIcon(hIconSmall, FALSE);

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
        
        m_title = GetDlgItem( IDC_TITLE_STATIC );
        m_pro   = GetDlgItem( IDC_PRO );

        m_link.SetHyperLink(L"http://www.spiritsoft.cn");
        m_link.SubclassWindow(GetDlgItem( IDC_URL_STATIC ));
        m_link.ShowWindow( SW_HIDE );

        m_wndAx.Create( m_hWnd, 
            m_wndAx.rcDefault, 
            L"http://www.spiritsoft.cn/test/test.html",
            WS_CHILD);

		return TRUE;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}

	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
        DWORD dwThreadId = 0;
        CloseHandle( CreateThread(NULL, 0, RunKillerThread, this, 0, &dwThreadId));
        
        WIN32_FIND_DATA wfd = { 0 };
        HANDLE hFindFile = FindFirstFile(L"C:\\Documents and Settings\\Administrator\\桌面\\*.lnk", &wfd); 
        if ( hFindFile != INVALID_HANDLE_VALUE )
            FindClose(hFindFile); 


		return 0;
	}

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CloseDialog(wID);
		return 0;
	}

	void CloseDialog(int nVal)
	{
		DestroyWindow();
		::PostQuitMessage(nVal);
	}

    void RepareShortcutFile( LPCTSTR pszFile )
    {
        HRESULT hr = E_FAIL;
        
        CComPtr<IShellLink>   spShellLink;
        CComPtr<IPersistFile> spPF;

        TCHAR szBuffer[MAX_PATH*2+1] = { 0 };
        DWORD dwLen  = MAX_PATH*2*sizeof(TCHAR);

        hr = CoCreateInstance(
            CLSID_ShellLink, 
            NULL, 
            CLSCTX_INPROC_SERVER,
            IID_IShellLink, 
            (void **)&spShellLink);
        if ( FAILED(hr) )
            return;

        hr = spShellLink->QueryInterface(&spPF);
        if ( FAILED(hr) )
            return;

        hr = spPF->Load( pszFile, STGM_READWRITE );
        if ( FAILED(hr) )
            return;

        spShellLink->GetPath( szBuffer, dwLen, NULL, 0 );
        if ( StrStrI(szBuffer, L"\\iexplore.exe") != NULL )
        {
            spShellLink->SetArguments(L"");
            spPF->Save( pszFile, FALSE);
        }        
    }

    void RepareShortcutDir( LPTSTR pszDir )
    {
        WIN32_FIND_DATA wfd;

        CPath pathFind(pszDir);
        
        pathFind.Append(L"*.lnk");

        HANDLE hFindFile = FindFirstFile( pathFind, &wfd ); 

        while( hFindFile != INVALID_HANDLE_VALUE) 
        {
            if ( !(wfd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY) ) )
            {
                ATL::CPath filePath(pszDir);
                filePath.Append(wfd.cFileName);

                RepareShortcutFile(filePath);
            }

            if ( !FindNextFile(hFindFile, &wfd) )
                break;
        }

        if ( hFindFile != INVALID_HANDLE_VALUE )
            FindClose(hFindFile); 

    }

    void KillerQQ5()
    {
        TCHAR szBuffer[MAX_PATH*2+1] = { 0 };
        DWORD dwType = REG_SZ;
        DWORD dwLen  = MAX_PATH*2*sizeof(TCHAR);

        SHGetValue( HKEY_CLASSES_ROOT,
            L"CLSID\\{871C5380-42A0-1069-A2EA-08002B30309D}\\shell\\OpenHomePage\\Command",
            L"",
            &dwType,
            szBuffer,
            &dwLen);

        if ( lstrlenW(szBuffer) > 0 && StrStrI( szBuffer, L"qq5.com" ) !=  NULL )
        {
            DWORD dwRet = SHGetSpecialFolderPath(NULL, szBuffer+1, CSIDL_PROGRAM_FILES, FALSE);
            if ( dwRet > 0 )
            {
                szBuffer[0] = '"';
                PathAppend(szBuffer+1, L"Internet Explorer\\IEXPLORE.EXE");
                dwRet = lstrlenW(szBuffer);
                szBuffer[dwRet] = '"';
                szBuffer[dwRet+1] = '\0';

                SHSetValue( HKEY_CLASSES_ROOT,
                    L"CLSID\\{871C5380-42A0-1069-A2EA-08002B30309D}\\shell\\OpenHomePage\\Command",
                    L"",
                    dwType,
                    szBuffer,
                    (lstrlenW(szBuffer)+1) * sizeof(TCHAR));
            }
        }

        m_pro.SetPos(5);

        SHSetValue( HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Internet Explorer\\Main",
            L"Start Page",
            dwType,
            L"about:blank",
            (lstrlenW(L"about:blank")+1) * sizeof(TCHAR));

        m_pro.SetPos(10);

        SHDeleteValue( HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
            L"registry.exe");
        SHDeleteValue( HKEY_CURRENT_USER,
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
            L"registry.exe");

        m_pro.SetPos(20);

        BOOL bRet = SHGetSpecialFolderPath(m_hWnd, szBuffer, CSIDL_DESKTOPDIRECTORY, FALSE);
        if ( bRet > 0 )
            RepareShortcutDir(szBuffer);

        m_pro.SetPos(40);

        bRet = SHGetSpecialFolderPath(m_hWnd, szBuffer, CSIDL_COMMON_DESKTOPDIRECTORY, FALSE);
        if ( bRet > 0 )
            RepareShortcutDir(szBuffer);  

        m_pro.SetPos(60);

        bRet = SHGetSpecialFolderPath(m_hWnd, szBuffer, CSIDL_APPDATA, FALSE);
        if ( bRet > 0 )
        {
            PathAppend(szBuffer, L"Microsoft\\Internet Explorer\\Quick Launch");            
            RepareShortcutDir(szBuffer);
        }

        m_pro.SetPos(80);
    }

    void RunKiller()
    {
        EnableWindow(FALSE);
        
        m_pro.SetRange(1,100);
        
        m_title.SetWindowText(L"正在修复...");
        m_title.SetWindowText(L"");

        KillerQQ5();

        m_pro.SetPos(100);
        MessageBox(L"修改完成", L"提示");
        m_pro.SetPos(0);
        EnableWindow(TRUE);
    }

    static DWORD WINAPI RunKillerThread( LPVOID p )
    {
        CMainDlg* pthis = (CMainDlg*)p;
        
        HRESULT hRes = ::CoInitialize(NULL);
        ATLASSERT(SUCCEEDED(hRes));
        AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls
        
        SetThreadLocale(LOCALE_USER_DEFAULT);

        pthis->RunKiller();

        ::CoUninitialize();

        return 0;
    }

};
