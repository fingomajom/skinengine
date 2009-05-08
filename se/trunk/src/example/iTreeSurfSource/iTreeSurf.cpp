// iTreeSurf.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "iTreeSurf.h"

#include "iTreeSurfDefinition.h"

#include "ChildFrm.h"
//#include "iTreeSurfDoc.h"
#include "iTreeSurfView.h"

#include "afxpriv.h"
#include <..\src\occimpl.h>
#include "CustSite.h" 

//#include <afxsock.h>
#include <Winsock.h>
#include <afxmt.h>
#include <wininet.h>
#include "rasspdm.h"
#include <initguid.h>
#include "iTreeSurf_i.c"
#include "iTreeSurfNSHandle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define INTERNET_CONNECTION_OFFLINE         0x20 //18


////////////////////////////////////////////////////////////////////////////
// CiTreeSurfApp

BEGIN_MESSAGE_MAP(CiTreeSurfApp, CWinApp)
	//{{AFX_MSG_MAP(CiTreeSurfApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
//	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////
// CiTreeSurfApp construction

CiTreeSurfApp::CiTreeSurfApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
//	m_hasEmptyDoc=true;
	m_nAutoStart = 0;
}

////////////////////////////////////////////////////////////////////////////
// The one and only CiTreeSurfApp object
CiTreeSurfApp theApp;



///////////////////////////////////////////////////////////////////
//global exception handle, not good at this time.
/*LONG __stdcall ExcepCallBack ( EXCEPTION_POINTERS * pExPtrs )
{
  // AfxMessageBox ( "crash" ) ;

	//let it crash
    //return ( EXCEPTION_CONTINUE_SEARCH ) ;

   //let it terminal
    //return ( EXCEPTION_EXECUTE_HANDLER ) ;

	if(pExPtrs->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION )
	{
		AfxMessageBox("Memory access violation. \nYou can try to increase the virtual memory and keep enough disk space for it.");
	}
	
	return EXCEPTION_CONTINUE_SEARCH;
    
}
*/

////////////////////////////////////////////////////////////////////////////
// CiTreeSurfApp initialization

BOOL CiTreeSurfApp::InitInstance()
{

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

//	SetUnhandledExceptionFilter(ExcepCallBack);

	//only one instance is allowed except -m
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		return TRUE;
	}


	HANDLE hSem = CreateSemaphore(NULL, 1, 1, m_pszAppName);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
		_bAutoStart = false;
	else
		_bAutoStart = true;

	LPTSTR lpszModule;
	try
	{
		 lpszModule = new TCHAR[_MAX_PATH];
    }
	catch(...)
	{
//		if(e!=NULL)e->Delete();
		return FALSE;
	}

	//3.6.1
	//2002-06-26
	//load resource for a language
	LoadResourceDLL();


	//3.6
	//2002-06-04
	if (GetModuleFileName( NULL, lpszModule, _MAX_PATH))
    {
		m_strProfile = lpszModule;
		int i = m_strProfile.ReverseFind('\\');
		m_strProfile = m_strProfile.Left(i+1);
		m_strSaveTxtFile = m_strProfile;
		m_strLinkListPath = m_strProfile;
		m_striTreeSurfRoot = m_strProfile;

		//3.6
		//2002-06-04
		m_strProfile += _ITREE_SURF_INI_FILE_;
		m_strSaveTxtFile += _SAVE_TEXT_TXT_FILE_;
	}

    HINSTANCE hInstance = AfxGetResourceHandle();
    ASSERT(hInstance != NULL);
	if (GetModuleFileName(hInstance, lpszModule, _MAX_PATH))
	{
		m_strLinkListPath = lpszModule;
		int i = m_strLinkListPath.ReverseFind('\\');
		m_strLinkListPath = m_strLinkListPath.Left(i+1);
		m_strLinkListPath += _URL_LIST_HTM_FILE_;
    }
    delete [] lpszModule;
	
	//2.5 victor
	//First free the string allocated by MFC at CWinApp startup.
	//The string is allocated before InitInstance is called.
	free((void*)m_pszProfileName);
	//Change the name of the .INI file.
	//The CWinApp destructor will free the memory.
	m_pszProfileName=_tcsdup(m_strProfile);
	//delete[]  (char*)m_pszProfileName;
	//m_pszProfileName = m_strProfile;

	//2002-06-14
	_configFile.Initial();

	if (cmdInfo.m_nShellCommand!=CCommandLineInfo::FileNew )
		{
			
			if ( ! _bAutoStart )
			{
				CloseHandle(hSem);

				HWND hWndPrevious = ::GetWindow(::GetDesktopWindow(), GW_CHILD);
				while (::IsWindow(hWndPrevious))
				{
					if (::GetProp(hWndPrevious, m_pszAppName))
					{

						if (cmdInfo.m_nShellCommand==CCommandLineInfo::FileOpen)
						{
							if (::GetLastActivePopup(hWndPrevious)==hWndPrevious)
							{
								LPSTR lpData;

								//mutex
								HANDLE hiTreeSurfSem = CreateMutex(NULL, TRUE, "iTreeSurfMutex");
								WaitForSingleObject(hiTreeSurfSem, INFINITE);

								HANDLE hMapping = CreateFileMapping
								((HANDLE)0xFFFFFFFF,NULL,
								PAGE_READWRITE, 0, 2560, _ITREE_SURF_MAP_FILE_NAME );
								if (hMapping==NULL)
								{
									AfxMessageBox("App:Fail to create share memory!");
								}
								else
								{
									lpData = (LPSTR) MapViewOfFile(hMapping,FILE_MAP_ALL_ACCESS,0,0,0);
									if (lpData==NULL)
									{
										AfxMessageBox("App:MapViewOfFile Fail");
									}
									else {//MapViewOfFile??
										sprintf(lpData,"%s",cmdInfo.m_strFileName);
										::SendMessage(hWndPrevious,WM_USER_SHELL_OPEN,NULL,NULL);
									}
									CloseHandle(hMapping);
								}
								ReleaseMutex(hiTreeSurfSem);
							}

						}

						if (::IsIconic(hWndPrevious))
							::ShowWindow(hWndPrevious,SW_RESTORE);
						
						//::SetForegroundWindow(hWndPrevious);
						::SetForegroundWindow(::GetLastActivePopup(hWndPrevious));
			
							return FALSE;
						}

						hWndPrevious = ::GetWindow(hWndPrevious, GW_HWNDNEXT);
				}

				return FALSE;
			}
		}
	else if( _configFile._bOneInstance && ! _bAutoStart )
	{
		//only one instance
		HWND hWndPrevious = ::GetWindow(::GetDesktopWindow(), GW_CHILD);
		while (::IsWindow(hWndPrevious))
		{
			if (::GetProp(hWndPrevious, m_pszAppName))
			{
				if (::IsIconic(hWndPrevious))
					::ShowWindow(hWndPrevious,SW_RESTORE);
						
				//::SetForegroundWindow(hWndPrevious);
				::SetForegroundWindow(::GetLastActivePopup(hWndPrevious));
			
				return FALSE;
			}

			hWndPrevious = ::GetWindow(hWndPrevious, GW_HWNDNEXT);
		}	
	}

//2.2
	//if (!AfxSocketInit())
	//	return FALSE;
	// initialize Winsock library
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(1, 1);
	int nResult = WSAStartup(wVersionRequested, &wsaData);
	if (nResult == 0)
	{
		if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
		{
			WSACleanup();
		}
	}


#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
//3.1
#if _WIN32_WINNT >= 0x0400
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
#else
	CoInitialize(NULL);
#endif

	if (!InitATL())
		return FALSE;

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.

	//get a random ID
    srand( (unsigned)time( NULL ) );
	_globalSpace._nTrayIconID = rand()/16;


	//IShellUIHandle
	CCustomOccManager *pMgr = new CCustomOccManager;

	// Create an IDispatch class for extending the Dynamic HTML Object Model 
	m_pDispOM = new CImpIDispatch;
	//Drop target
	m_pDropTarget = new CImpIDropTarget;

	// Set our control containment up but using our control container 
	// management class instead of MFC's default
	AfxEnableControlContainer(pMgr);

	//3.6.1
	//! must load settings before create MainFrame
	_configFile.LoadOtherSettings();

	// create main MDI Frame window
	( _globalSpace._pMainFrame ) = new CMainFrame;

	//? when debug, this line fail
	//if no ini file, getresourcehandle fail
	//if ini file one line, version=300, toolbar::getbutton fail.
	if (!( _globalSpace._pMainFrame )->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	
	m_pMainWnd = ( _globalSpace._pMainFrame );

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();


	// Enable DDE Execute open
//	EnableShellOpen();
//	RegisterShellFileTypes(TRUE);

	//3.6.1
	SetMainFrameAccordingToConfig();

	// Parse command line for standard shell commands, DDE, file open
//	CCommandLineInfo cmdInfo;
//	ParseCommandLine(cmdInfo);

	CString strURL;

/*	if( _configFile._nOpenType ==0)
		((CMainFrame*)m_pMainWnd)->GetMenu()->CheckMenuItem(ID_OPTIONS_ITREESURFSTARTUP_OPENBLANKPAGE,MF_CHECKED);
	if( _configFile._nOpenType ==1)
		((CMainFrame*)m_pMainWnd)->GetMenu()->CheckMenuItem(ID_OPTIONS_ITREESURFGOHOME,MF_CHECKED);
	if( _configFile._nOpenType ==2)
		((CMainFrame*)m_pMainWnd)->GetMenu()->CheckMenuItem(ID_OPTIONS_ITREESURFSTARTUP_RESUMELASTVISITEDPAGES,MF_CHECKED);
	if( _configFile._nOpenType ==3)
		((CMainFrame*)m_pMainWnd)->GetMenu()->CheckMenuItem(ID_OPTIONS_ITREESURFSTARTUP_OPENFAVORITEPAGES,MF_CHECKED);
	if( _configFile._nOpenType ==4)
		((CMainFrame*)m_pMainWnd)->GetMenu()->CheckMenuItem(ID_OPTIONS_ITREESURFSTARTUP_OPENMOSTFAVORITEPAGES,MF_CHECKED);
	if( _configFile._nOpenType ==5)
		((CMainFrame*)m_pMainWnd)->GetMenu()->CheckMenuItem(ID_OPTIONS_ITREESURFSTARTUP_OPENSTARTUPGROUP,MF_CHECKED);
*/

	//disable the old menu
	::SetMenu(( _globalSpace._pMainFrame )->m_hWnd, NULL);

	( _globalSpace._pMainFrame )->ShowWindow(m_nCmdShow);



/*	if(OpenFile(m_strProfile,&fst ,OF_EXIST)==HFILE_ERROR)
	{
		delete[]  (char*)m_pszProfileName;
		m_pszProfileName = m_strProfile;
	}
	delete[]  (char*)m_pszRegistryKey;
	m_pszRegistryKey = NULL;
*/

	
	if( ! _bAutoStart )
		_configFile._bRunning = false;

	//restore proxy server
	if( _configFile._csCurrentProxy.Find("127.0.0.1")<0 && _configFile._csCurrentProxy.Find("localhost")<0 )
	{
		if( _configFile._nEnableProxy !=-1 && !_configFile._csCurrentProxy.IsEmpty() )
		{
			//disable proxy first
			INTERNET_PROXY_INFO ipi;
			DWORD dwSize=sizeof(ipi);
			ipi.dwAccessType = INTERNET_OPEN_TYPE_DIRECT ;
			InternetSetOption(NULL, INTERNET_OPTION_PROXY, &ipi, dwSize);
		}

		if( _configFile._nEnableProxy == 1 )
		{
			if( ! _configFile._csCurrentProxy.IsEmpty() )
			{
				::SendMessage(m_pMainWnd->m_hWnd, WM_COMMAND, ID_OPTIONS_USE_PROXY, 0);
			}
		}
	}

/*	//3.0 auto detect offline
	DWORD dwOffline;
	if(InternetGetConnectedState(&dwOffline, 0))
	{
		if(dwOffline == INTERNET_CONNECTION_OFFLINE || dwOffline==0x20)
		{
			//set offline
			HKEY            hKey;
			DWORD           dwSize = sizeof(DWORD), sz;

			if(RegOpenKey(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"), &hKey) != ERROR_SUCCESS)
			{
				TRACE0("Global settings not found\n");
			}
			else
			{
				sz = 1;
				RegSetValueEx(hKey, _T("GlobalUserOffline"), NULL, REG_DWORD, (LPBYTE)(&sz), dwSize);


				RegCloseKey(hKey);

				//infor the system that setting is changed.
				InternetSetOption(
					NULL,
					INTERNET_OPTION_SETTINGS_CHANGED,
					NULL,
					0
					);
			}
		}
			
	}*/



	if (cmdInfo.m_nShellCommand==CCommandLineInfo::FileOpen)
	{
		strURL = cmdInfo.m_strFileName;
//		strURL.MakeLower();
		if(strURL.Right(4).CompareNoCase(".url") == 0)
		{
			//strURL is filename before get url
			_configFile.GetFavorUrl( strURL, strURL );
		}
		//2002-07-11
		//( _globalSpace._pMainFrame )->NewChildWindow(1,  _configFile._nOpenType , strURL);
		( _globalSpace._pMainFrame )->NewChildWindow( 1, 2, strURL );
	}
	else
	{
		//2002-06-14
		bool bOpenLast = true;

		//continue last error session
		if( _configFile._bRunning )
		{	
			( _globalSpace._pMainFrame )->UpdateWindow();
			if(AfxMessageBox(IDS_RESUME, MB_YESNO)==IDYES)
				bOpenLast = true;
			else
				bOpenLast = false;
		}

		//2002-06-14
		if(
			bOpenLast
			&&
			(
			 (  _configFile._nOpenType  == 2 )
			 || 
			 _configFile._bRunning
			)
		  )
		{
			//load last visit
			CStringArray csaLast;
			bool bResult = _configFile.GetStringArray_LastVisit( csaLast );
			if( bResult )
			{
				for( int i=0; i < csaLast.GetSize(); i++ )
				{
					BOOL bnext = _configFile._bNextTab;
					_configFile._bNextTab = false;
					((CMainFrame*)m_pMainWnd)->NewChildWindow( 1, 2, csaLast[i] );
					_configFile._bNextTab  = bnext;
				}
			}
		}
		else if( _configFile._nOpenType  == 3)
		{
			( _globalSpace._pMainFrame )->OpenAllFavs( _configFile._csFavoritePath );
		}
		else if( _configFile._nOpenType  == 4)
		{
			( _globalSpace._pMainFrame )->OpenAllFavs( _configFile._csFavoritePath, TRUE);
		}
		else if( _configFile._nOpenType  == 5)
		{
			//start group
			( _globalSpace._pMainFrame )->OpenGroup( _configFile._csStartGroup );
		}
		else if( _configFile._nOpenType  == 6)
		{
			//do nothing			
		}
		else if( _configFile._nOpenType  == 1)//3.2
			( _globalSpace._pMainFrame )->PostMessage(WM_COMMAND, ID_FILE_NEW_S, 0);
		else
		//3.2	( _globalSpace._pMainFrame )->NewChildWindow(1,  _configFile._nOpenType , strURL, NULL, TRUE);
			( _globalSpace._pMainFrame )->PostMessage(WM_COMMAND, ID_FILE_NEW_BLANK, 0);
	}
	
	//full screen
	if( _configFile._bFullScreen )
	{
		((CMainFrame*)m_pMainWnd)->FullScreenModeOn();
	}
	
	( _globalSpace._pMainFrame )->UpdateWindow();

	//delay load
	((CMainFrame*)m_pMainWnd)->PostMessage(WM_DELAYLOAD_CONFIG);


	_configFile.Set_Running( true );

	return TRUE;
}

//#pragma optimize( "s", on )

////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CiTreeSurfApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	//2002-05-22
	CString csVersion;
	CStatic* pStatic = (CStatic* )GetDlgItem( IDC_STATIC_VERSION );
	csVersion += _ITREE_SURF_VERSION;
	csVersion += _ITREE_SURF_BUILD_NUMBER;
	pStatic->SetWindowText( csVersion );
	
	//2002-05-30
	CString csHome;
	pStatic = (CStatic* )GetDlgItem( IDC_STATIC_HOME );
	csHome += _ITREE_SURF_HOME;
	pStatic->SetWindowText( csHome );

	CString csBBS;
	pStatic = (CStatic* )GetDlgItem( IDC_STATIC_BBS );
	csBBS += _ITREE_SURF_BBS;
	pStatic->SetWindowText( csBBS );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

////////////////////////////////////////////////////////////////////////////
// CiTreeSurfApp message handlers


/*CDocument* CiTreeSurfApp::OpenDocumentFile(LPCTSTR lpszFileName) 
{
	// TODO: Add your specialized code here and/or call the base class
	//load file to the empty doc which is opend when initializing.
//	ASSERT(m_pDocManager != NULL);
	POSITION pos=m_pDocManager->GetFirstDocTemplatePosition();
	if(pos!=NULL)
	{
		CDocTemplate* pTemplate = m_pDocManager->GetNextDocTemplate(pos);
		POSITION pos2 = pTemplate->GetFirstDocPosition();
		if(pos2!=NULL)
		{
			CiTreeSurfDoc * pDoc = (CiTreeSurfDoc*)pTemplate->GetNextDoc(pos2);
			if(pDoc!=NULL && pDoc->m_bEmpty)
			{
				pDoc->m_bEmpty=false;
				POSITION dpos = pDoc->GetFirstViewPosition();
				CFixedHtmlView* pv =((CFixedHtmlView*)(pDoc->GetNextView(dpos)));
				pv->Navigate2(lpszFileName,NULL,NULL);
				pDoc->SetPathName(lpszFileName);
				
				//bring the MDI child window to the top, 
				//we should bring the frame to top, not the view itself.
				//a view has no title bar and border.
				pv->GetParentFrame()->BringWindowToTop(); 
				return pDoc;
			}
		}
	}
	return CWinApp::OpenDocumentFile(lpszFileName);
}*/

//#pragma optimize( "", on )


int CiTreeSurfApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	try{
	//2.2
	WSACleanup();

	if(m_pDispOM)
		delete m_pDispOM;

	if(m_pDropTarget)
		delete m_pDropTarget;

	}
	catch(...)
	{
	}

//	try{
	if (m_bATLInited)
	{
		_Module.RevokeClassObjects();
		_Module.Term();
	
	//	CoUninitialize();
	}

	CoUninitialize();
	return CWinApp::ExitInstance();
//	}catch(...)
//	{
//		return -1;
//	}
}

void CiTreeSurfApp::SaveConfg()
{
 try
 {

	//Exp Bar
	if(((CMainFrame*)m_pMainWnd)->m_wndInstantBar.GetSafeHwnd()==NULL)
		_configFile._csExpBar = "";
	else if(((CMainFrame*)m_pMainWnd)->m_wndInstantBar.IsVisible())
		((CMainFrame*)m_pMainWnd)->m_wndInstantBar.GetWindowText( _configFile._csExpBar );
	_configFile.WriteString( "", "ExpBar", _configFile._csExpBar );

	if(((CMainFrame*)m_pMainWnd)->m_wndInstantBar.GetSafeHwnd()!=NULL)
	{
		CRect rect;
		((CMainFrame*)m_pMainWnd)->m_wndInstantBar.GetWindowRect(&rect);
		_configFile._nExpWidth = rect.Width();
		_configFile.WriteInteger( "", "ExpWidth", _configFile._nExpWidth );
	}

 }
 catch(...)
 {
 }
}



BOOL CiTreeSurfApp::OnIdle(LONG lCount) 
{
	// TODO: Add your specialized code here and/or call the base class
	try{

	if(( _globalSpace._pMainFrame )!=NULL)
	{
		if(( ( _globalSpace._pMainFrame ) )->m_bFavDirty==3 || (( ( _globalSpace._pMainFrame ) )->m_bFavDirty && nDelay>30))
		{
			//( ( _globalSpace._pMainFrame ) )->InitFavorites();
			( ( _globalSpace._pMainFrame ) )->PostMessage(WM_UPDATE_FAV);
			( ( _globalSpace._pMainFrame ) )->m_bFavDirty = FALSE;
			nDelay=0;
		}
		else if(( ( _globalSpace._pMainFrame ) )->m_bFavDirty)
			nDelay++;

		//auto start
		if( _bAutoStart )
		{
			int max = _configFile._ExternalUtilList.m_UtilList.GetUpperBound();
			if(m_nAutoStart <= max)
			{
				while(m_nAutoStart <= max && _configFile._ExternalUtilList.m_UtilList.GetAt(m_nAutoStart)->m_bUtilStart==FALSE)
				{
					m_nAutoStart++;
				}
				if(m_nAutoStart <= max)
				{
					//start the tool
					( ( _globalSpace._pMainFrame ) )->StartUtil(m_nAutoStart);
					m_nAutoStart++;
					( ( _globalSpace._pMainFrame ) )->m_bNeedActive = TRUE;
				}
				else
					_bAutoStart = false;
			}
			else
				_bAutoStart = false;

		}
		
		
		if( _configFile._bSaveConfig )
		{
			_configFile._bSaveConfig = false;
			AfxBeginThread(TSaveConfig, NULL);
		}
	}

/*2.2

//	return CWinApp::OnIdle(lCount);
	if (lCount <=0)
	{
		// send WM_IDLEUPDATECMDUI to the main window
/*		CWnd* pMainWnd = m_pMainWnd;
		if (pMainWnd != NULL && pMainWnd->m_hWnd != NULL &&
			pMainWnd->IsWindowVisible())
		{
//			AfxCallWndProc(pMainWnd, pMainWnd->m_hWnd,
//				WM_IDLEUPDATECMDUI, (WPARAM)TRUE, 0);
		}* /
	}
	else if (lCount >= 0)
	{
		AFX_MODULE_THREAD_STATE* pState = _AFX_CMDTARGET_GETSTATE()->m_thread;
		if (pState->m_nTempMapLock == 0)
		{
			// free temp maps, OLE DLLs, etc.
			AfxLockTempMaps();
			AfxUnlockTempMaps();
		}
	//	_heapmin();

	}

	return lCount < 1;  // more to do if lCount < 1
*/
//added at 2.2
	if (lCount <= 0)
	{
		ThreadOnIdle(lCount);

		// call doc-template idle hook
/*328		POSITION pos = NULL;
		if (m_pDocManager != NULL)
			pos = m_pDocManager->GetFirstDocTemplatePosition();

		while (pos != NULL)
		{
			CDocTemplate* pTemplate = m_pDocManager->GetNextDocTemplate(pos);
			ASSERT_KINDOF(CDocTemplate, pTemplate);
			pTemplate->OnIdle();
		}

*/
	}
	else if (lCount == 1)
	{
		ThreadOnIdle(lCount);
	}
	return lCount < 1;  // more to do if lCount < 1

	}
	catch(...)
	{
		return lCount < 1;
	}

}

BOOL CiTreeSurfApp::ThreadOnIdle(LONG lCount)
{
	ASSERT_VALID(this);
/*
#if defined(_DEBUG) && !defined(_AFX_NO_DEBUG_CRT)
	// check MFC's allocator (before idle)
	if (_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) & _CRTDBG_CHECK_ALWAYS_DF)
		ASSERT(AfxCheckMemory());
#endif*/

	try{

	if (lCount <= 0)
	{
		// send WM_IDLEUPDATECMDUI to the main window
/*		CWnd* pMainWnd = m_pMainWnd;
		if (pMainWnd != NULL && pMainWnd->m_hWnd != NULL &&
			pMainWnd->IsWindowVisible())
		{
			AfxCallWndProc(pMainWnd, pMainWnd->m_hWnd,
				WM_IDLEUPDATECMDUI, (WPARAM)TRUE, 0);
			pMainWnd->SendMessageToDescendants(WM_IDLEUPDATECMDUI,
				(WPARAM)TRUE, 0, TRUE, TRUE);
		}
		// send WM_IDLEUPDATECMDUI to all frame windows
		AFX_MODULE_THREAD_STATE* pState = _AFX_CMDTARGET_GETSTATE()->m_thread;
		CFrameWnd* pFrameWnd = pState->m_frameList;
		while (pFrameWnd != NULL)
		{
			if (pFrameWnd->m_hWnd != NULL && pFrameWnd != pMainWnd)
			{
				if (pFrameWnd->m_nShowDelay == SW_HIDE)
					pFrameWnd->ShowWindow(pFrameWnd->m_nShowDelay);
				if (pFrameWnd->IsWindowVisible() ||
					pFrameWnd->m_nShowDelay >= 0)
				{
					AfxCallWndProc(pFrameWnd, pFrameWnd->m_hWnd,
						WM_IDLEUPDATECMDUI, (WPARAM)TRUE, 0);
					pFrameWnd->SendMessageToDescendants(WM_IDLEUPDATECMDUI,
						(WPARAM)TRUE, 0, TRUE, TRUE);
				}
				if (pFrameWnd->m_nShowDelay > SW_HIDE)
					pFrameWnd->ShowWindow(pFrameWnd->m_nShowDelay);
				pFrameWnd->m_nShowDelay = -1;
			}
			pFrameWnd = pFrameWnd->m_pNextFrameWnd;
		}*/
	}
	else if (lCount >= 0)
	{
		AFX_MODULE_THREAD_STATE* pState = _AFX_CMDTARGET_GETSTATE()->m_thread;
		if (pState->m_nTempMapLock == 0)
		{
			// free temp maps, OLE DLLs, etc.
			AfxLockTempMaps();
			AfxUnlockTempMaps();
		}
	}

	}catch(...)
	{
	}

/*
#if defined(_DEBUG) && !defined(_AFX_NO_DEBUG_CRT)
	// check MFC's allocator (after idle)
	if (_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) & _CRTDBG_CHECK_ALWAYS_DF)
		ASSERT(AfxCheckMemory());
#endif*/

	return lCount < 0;  // nothing more to do if lCount >= 0
}


/*long LoadBytes()
{
	HKEY            hKey;
	long            sz = 0;
	DWORD           dwSize = 4;

	try{

	if(RegOpenKey(HKEY_DYN_DATA, _T("PerfStats\\StatData"), &hKey) != ERROR_SUCCESS)
	{
		TRACE0("Main setting not found\n");
		return -300;
	}
	dwSize = sizeof(sz);
	sz=0;
	RegQueryValueEx(hKey, _T("Dial-Up Adapter\\TotalBytesRecvd"), NULL, NULL, (LPBYTE)&sz, &dwSize);
	
	}
	catch(...)
	{
	}

	RegCloseKey(hKey);
	return sz;
}*/







	
CiTreeSurfModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_iTreeSurfNSHandle, CiTreeSurfNSHandle)
END_OBJECT_MAP()

LONG CiTreeSurfModule::Unlock()
{
	AfxOleUnlockApp();
	return 0;
}

LONG CiTreeSurfModule::Lock()
{
	AfxOleLockApp();
	return 1;
}
LPCTSTR CiTreeSurfModule::FindOneOf(LPCTSTR p1, LPCTSTR p2)
{
	while (*p1 != NULL)
	{
		LPCTSTR p = p2;
		while (*p != NULL)
		{
			if (*p1 == *p)
				return CharNext(p1);
			p = CharNext(p);
		}
		p1++;
	}
	return NULL;
}


BOOL CiTreeSurfApp::InitATL()
{
	m_bATLInited = TRUE;
/*
#if _WIN32_WINNT >= 0x0400
	HRESULT hRes = CoInitializeEx(NULL, COINIT_MULTITHREADED);
#else
	HRESULT hRes = CoInitialize(NULL);
#endif*/

/*	if (FAILED(hRes))
	{
		m_bATLInited = FALSE;
		return FALSE;
	}*/

	_Module.Init(ObjectMap, AfxGetInstanceHandle());
	_Module.dwThreadID = GetCurrentThreadId();

	LPTSTR lpCmdLine = GetCommandLine(); //this line necessary for _ATL_MIN_CRT
	TCHAR szTokens[] = _T("-/");

	BOOL bRun = TRUE;
	LPCTSTR lpszToken = _Module.FindOneOf(lpCmdLine, szTokens);
	while (lpszToken != NULL)
	{
		if (lstrcmpi(lpszToken, _T("UnregServer"))==0)
		{
			_Module.UpdateRegistryFromResource(IDR_ITREESURF, FALSE);
			_Module.UnregisterServer(TRUE); //TRUE means typelib is unreg'd
			bRun = FALSE;
			break;
		}
		if (lstrcmpi(lpszToken, _T("RegServer"))==0)
		{
			_Module.UpdateRegistryFromResource(IDR_ITREESURF, TRUE);
			_Module.RegisterServer(TRUE);
			bRun = FALSE;
			break;
		}
		lpszToken = _Module.FindOneOf(lpszToken, szTokens);
	}

	if (!bRun)
	{
		m_bATLInited = FALSE;
		_Module.Term();
//		CoUninitialize();
		return FALSE;
	}

	HRESULT hRes = _Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER, 
		REGCLS_MULTIPLEUSE);
	if (FAILED(hRes))
	{
		m_bATLInited = FALSE;
//		CoUninitialize();
		return FALSE;
	}	

	return TRUE;

}


void CiTreeSurfApp::LoadResourceDLL()
{
	m_hResourceDll = NULL;

	_languageTable.ReadFromFile();

	m_hResourceDll = LoadLibrary( _languageTable._csLanguageDllFile );

	if( m_hResourceDll == NULL )
		return;

	AfxSetResourceHandle( m_hResourceDll );
}

bool CiTreeSurfApp::SetMainFrameAccordingToConfig()
{
	//Exp Bar
	CString str1; 
	str1.LoadString(IDS_TITLE_SEARCH);
	if( _configFile._csExpBar == str1 )
		PostMessage(m_pMainWnd->m_hWnd, WM_COMMAND, ID_VIEW_EXPLOREBAR_SEARCH,0);
	str1.LoadString(IDS_TITLE_HIS);
	if( _configFile._csExpBar == str1 )
		PostMessage(m_pMainWnd->m_hWnd, WM_COMMAND, ID_VIEW_HIS,0);
	str1.LoadString(IDS_TITLE_FAVORITE);
	if( _configFile._csExpBar == str1 )
		PostMessage(m_pMainWnd->m_hWnd, WM_COMMAND, ID_FAVORITES_DROPDOWN,0);
	str1.LoadString(IDS_TITLE_SHELL);
	if( _configFile._csExpBar == str1 )
		PostMessage(m_pMainWnd->m_hWnd, WM_COMMAND, ID_VIEW_FOLDER,0);
	str1.LoadString(IDS_TITLE_RESOURCE);
	if( _configFile._csExpBar == str1 )
		PostMessage(m_pMainWnd->m_hWnd, WM_COMMAND, ID_VIEW_EXPLOREBAR_RESOURCE,0);

	if( _configFile._bEnablePopupFilter )
		((CMainFrame*)m_pMainWnd)->GetMenu()->CheckMenuItem(ID_TOOLS_EABLEPOPUPFILTER,MF_CHECKED);
	else
		((CMainFrame*)m_pMainWnd)->GetMenu()->CheckMenuItem(ID_TOOLS_EABLEPOPUPFILTER,MF_UNCHECKED);

	if( _configFile._nWindowsWidth!=0 && _configFile._nWindowsTop<2000 && _configFile._nWindowsTop>=-20)
		m_pMainWnd->SetWindowPos(&CWnd::wndTop, _configFile._nWindowsLeft, _configFile._nWindowsTop, _configFile._nWindowsWidth, _configFile._nWindowsHeight,SWP_HIDEWINDOW);   

	//===Proxy
	_configFile._csCurrentProxyName = ((CMainFrame*)m_pMainWnd)->GetProxyName( _configFile._csCurrentProxy );

	//get my document folder
	//default path
	SetCurrentDirectory( _configFile._csSavePath );

	//set save directory
	HKEY hKey;
	if(RegOpenKey(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Internet Explorer\\Main"), &hKey) == ERROR_SUCCESS)
	{
		RegSetValueEx(hKey, _T("Save Directory"), 0, REG_SZ, (LPBYTE)(_configFile._csSavePath.GetBuffer(10)), _configFile._csSavePath.GetLength());
		//?_configFile._csSavePath.ReleaseBuffer();
	}
	
	return true;
}

UINT _cdecl TSaveConfig(LPVOID con)
{
	theApp._configFile.SaveSettings();
	return 0;
}

