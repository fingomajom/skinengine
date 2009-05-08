#include "stdafx.h"
#include "iTreeSurf.h"
#include "ConfigTable.h"


////////////////////////////////////////////////////////////////////////////
// Global

BOOL CALLBACK TerminateAppEnum( HWND hwnd, LPARAM lParam )
{
	DWORD dwID ;

	// If we can't open the process with PROCESS_TERMINATE rights,
	// then we give up immediately.

	GetWindowThreadProcessId(hwnd, &dwID) ;

	if(dwID == (DWORD)lParam)
		PostMessage(hwnd, WM_CLOSE, 0, 0) ;

	return TRUE ;
}

DWORD WINAPI TerminateApp( DWORD dwPID, DWORD dwTimeout )
{
	DWORD   dwRet = 0;

	HANDLE hProc = OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE, FALSE,
	 dwPID);

	if(hProc == NULL)
	{
	 return 1 ;
	}

	// TerminateAppEnum() posts WM_CLOSE to all windows whose PID
	// matches your process's.
	EnumWindows((WNDENUMPROC)TerminateAppEnum, (LPARAM) dwPID) ;

	// Wait on the handle. If it signals, great. If it times out,
	// then you kill it.
	/*      if(WaitForSingleObject(hProc, dwTimeout)!=WAIT_OBJECT_0)
	 dwRet=(TerminateProcess(hProc,0)?TA_SUCCESS_KILL:TA_FAILED);
	else
	 dwRet = TA_SUCCESS_CLEAN ;
	*/
	CloseHandle(hProc) ;

	return dwRet ;
}

////////////////////////////////////////////////////////////////////////////
// CConfigTable Contruction

CConfigTable::CConfigTable()
{
	Initial();
}

CConfigTable::~CConfigTable()
{
	Remove_UserAgentInfo();
}


void CConfigTable::Initial()
{
	TCHAR szfname[MAX_PATH];
	DWORD nSize = MAX_PATH;
	int i=0;

	szfname[0] = 0;
    GetModuleFileName( NULL , szfname , nSize );
    for( i = _tcslen( szfname );i >= 0;i -- )
    {
        if( szfname[i] == '\\' )
            break;
    }
	szfname[i + 1] = 0;
	_csConfigRootFolder = szfname;
	_csConfigFile = _csConfigRootFolder + _ITREE_SURF_INI_FILE_;
	_csQuickSearchFile = _csConfigRootFolder + _QUICK_SEARCH_INI_FILE_;
	_csUrlFilterFile = _csConfigRootFolder + _URL_FILTER_INI_FILE_;
	_csFormDataFile = _csConfigRootFolder + _FORM_DATA_INI_FILE_;
	_csBitmapRoot = _csConfigRootFolder + _RELATIVE_BMP_ROOT_;

	HANDLE hfile = CreateFile( _csConfigFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hfile == INVALID_HANDLE_VALUE)
	{
		hfile = CreateFile( _csConfigFile, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hfile != INVALID_HANDLE_VALUE)
		{
			CString def; 
			def.LoadString(IDS_DEFAULTPOS);
			DWORD dwW;
			WriteFile(hfile, def.GetBuffer(300), def.GetLength(), &dwW, NULL);
			def.ReleaseBuffer();
			CloseHandle(hfile);
		}
	}
	else
		CloseHandle(hfile);

	//Get values
	GetPath_Group();
	GetPath_Skin();
	GetPath_Toolbar();
	GetPath_Save();
	GetPath_Favorite();

	GetBool_OneInstance();
	GetBool_TopTab();
	GetBool_Running();
	GetBool_FullScreen();
	
	GetInt_EnableProxy();
	GetInt_OpenType();
	GetInt_MaxWins();

	_ExternalUtilList.RemoveAll();
	_aBands.RemoveAll();
	_aBottomBands.RemoveAll();
	_aDownloadTools.RemoveAll();
	_cslBGPics.RemoveAll();
	_aProxyCategories.RemoveAll();
	_bProxyNew = false;

	_pDownload = NULL;
	_csApplicationName = _ITREE_SURF_APP_NAME;
	_csApplicationName += _ITREE_SURF_VERSION;
	_csApplicationName += _ITREE_SURF_BUILD_NUMBER;
	Set_UserAgentInfo();

}

bool CConfigTable::GetStringArray_LastVisit( CStringArray& csaReturn )
{
	//? OK, but not good
	//load last visit
	char *tmp = NULL;
	UINT len;
	bool bResult = AfxGetApp()->GetProfileBinary( "Settings", _INI_VAR_LAST_VISIT, (LPBYTE*)(&tmp), &len);
	if( bResult )
	{
		//load last visit list
		char *token;
		char seps[] = "\n";
		token = strtok( tmp, seps);
		while( token != NULL )
		{
			csaReturn.Add(token);
			/* Get next token: */
			token = strtok( NULL, seps );
		}
		delete[] tmp;
	}

	return bResult;
}

bool CConfigTable::GetInt_OpenType()
{
	GetInteger( "", _INI_VAR_OPEN_TYPE, _INI_VAR_OPEN_TYPE__D, _nOpenType );
	return true;
}

bool CConfigTable::GetInt_MaxWins()
{
	GetInteger( "", _INI_VAR_MAX_WINDOWS_NUMBER, _INI_VAR_MAX_WINDOWS_NUMBER__D, _nMaxWins );
	return true;
}

bool CConfigTable::GetInt_EnableProxy()
{
	GetInteger( "", _INI_VAR_ENABLE_PROXY, _INI_VAR_ENABLE_PROXY__D, _nEnableProxy );
	return true;
}

bool CConfigTable::GetBool_FullScreen()
{
	GetBool( "", _INI_VAR_FULL_SCREEN, _INI_VAR_FULL_SCREEN__D, _bFullScreen );
	return _bFullScreen;
}

bool CConfigTable::GetBool_OneInstance()
{
	GetBool( "", _INI_VAR_ONE_INSTANCE, _INI_VAR_ONE_INSTANCE__D, _bOneInstance );
	return _bOneInstance;
}

bool CConfigTable::GetBool_Running()
{
	GetBool( "", _INI_VAR_RUNNING, _INI_VAR_RUNNING__D, _bRunning );
	return _bRunning;
}

bool CConfigTable::GetBool_TopTab()
{
	GetBool( "", _INI_VAR_TAB_POSITION, _INI_VAR_TAB_POSITION__D, _bTopTab );
	return _bTopTab;
}

bool CConfigTable::GetPath_Group()
{
	GetString( "", _INI_VAR_GROUP_PATH, _INI_VAR_GROUP_PATH__D, _csGroupPath );

	MakePathValid( _csGroupPath, _INI_VAR_GROUP_PATH__D );

	//debug
	//CString csI;
	//csI.Format("   i=%d",i);
	//AfxMessageBox( _csGroupPath+csI );

	return true;
}

bool CConfigTable::GetPath_Skin()
{
	GetString( "", _INI_VAR_SKIN_PATH, _INI_VAR_SKIN_PATH__D, _csSkinPath );

	MakePathValid( _csSkinPath, _INI_VAR_SKIN_PATH__D );

	return true;
}

bool CConfigTable::GetPath_Save()
{
	GetString( "", _INI_VAR_SAVE_PATH, _INI_VAR_SAVE_PATH__D, _csSavePath );

	//get my document folder
	TCHAR szPath[MAX_PATH];
	bool bResult = true;

	try
	{
	   LPITEMIDLIST pidlMyDoc = NULL;
	   // Get a pointer to the IMalloc interface
	   IMalloc * m_pMalloc = NULL;
	   HRESULT hr = SHGetMalloc(&m_pMalloc);
	   if (SUCCEEDED(hr))
	   {
		   HRESULT hr = SHGetSpecialFolderLocation(NULL, CSIDL_PERSONAL, &pidlMyDoc);
		   if (NOERROR != hr)
			  bResult = false;
		   else
		   {
			   bResult = SHGetPathFromIDList( pidlMyDoc,szPath);
		   }
		   m_pMalloc->Free(pidlMyDoc);
		   m_pMalloc->Release();
		   m_pMalloc = NULL;
	   }
	}catch(...)
	{
	}

	if( bResult )
		MakePathValid( _csSavePath, szPath );
	else
		MakePathValid( _csSavePath, _INI_VAR_SAVE_PATH__D );

	return true;
}

bool CConfigTable::GetPath_Toolbar()
{
	GetString( "", _INI_VAR_TOOLBAR_BMP_PATH, _INI_VAR_TOOLBAR_BMP_PATH__D, _csToolbarPath );

	MakePathValid( _csSkinPath, _INI_VAR_TOOLBAR_BMP_PATH__D );

	return true;
}

bool CConfigTable::GetPath_Favorite()
{
	GetString( "", _INI_VAR_FAVORITE_PATH, _INI_VAR_FAVORITE_PATH__D, _csFavoritePath );

	//get favorites folder
	TCHAR szPath[MAX_PATH];
	bool bResult = true;
	HRESULT hr = SHGetSpecialFolderPath(NULL, szPath, CSIDL_FAVORITES, true);
	if (NOERROR != hr)
		bResult = false;

	if( bResult )
		MakePathValid( _csFavoritePath, szPath );
	else
		MakePathValid( _csFavoritePath, _INI_VAR_FAVORITE_PATH__D );

	return true;
}

bool CConfigTable::GetString( const CString csSection, const CString csVar, const CString csValueDefault, CString& csReturn )
{
	memset( _buffer, 0, sizeof(_buffer) );
	
	if( csSection.IsEmpty() )
		GetPrivateProfileString( _INI_SECTION_SETTING, csVar, csValueDefault, (char *)_buffer, sizeof(_buffer)-1, _csConfigFile);
	else
		GetPrivateProfileString( csSection, csVar, csValueDefault, (char *)_buffer, sizeof(_buffer)-1, _csConfigFile);

	csReturn = _buffer;

	return true;
}

bool CConfigTable::GetStruct( const CString csSection, const CString csVar, LPVOID lpStruct, UINT uSizeStruct )
{
	memset( lpStruct, 0, uSizeStruct );
	
	if( csSection.IsEmpty() )
		return GetPrivateProfileStruct( _INI_SECTION_SETTING, csVar, lpStruct, uSizeStruct, _csConfigFile );
	else
		return GetPrivateProfileStruct( csSection, csVar, lpStruct, uSizeStruct, _csConfigFile );
}


int CConfigTable::MakePathValid( CString& csPath, const CString csDefault )
{
	int nResult=0;

	if( csPath.IsEmpty() )
	{
		csPath = csDefault;
		nResult = __LT_PATH_EMPTY__;
	}

	if( csPath.Left(2) == ".\\" )
	{
		csPath = csPath.Right( csPath.GetLength() - 2 );
		csPath = _csConfigRootFolder + csPath;
	}

	//?
	//#include "Shlwapi.h"
	//PathFileExists(
	//if( PATH_NOT_EXIST() )
	//{
	//	nResult = __LT_PATH_NOT_EXIST__;
	//}

	if( csPath.Right(1) != "\\" )
	{
		csPath += "\\";
	}

	return nResult;
}

bool CConfigTable::FirstRun()
{
	int ver = _ITREE_SURF_VERSION_INT;
	int oldver=0;
	GetInteger("", "Version", 0, oldver );
	if(ver!=oldver)
	{
		//the first time run this version
		WriteInteger( "", "Version", ver );
		return TRUE;
	}
	return FALSE;
}

bool CConfigTable::LoadOtherSettings()
{

	//enable popup
	GetBool("", "EnablePop", true, _bEnablePopupFilter );
	GetBool("", "EnableDupFilter", false, _bEnableDupFilter );
	GetBool("", "EnableAutoFilter", false, _bEnableAutoFilter );
	//3.2
	GetBool("", "InAutoFilter", false, _bInAutoFilter );
	if( _bInAutoFilter )
		_bEnableAutoFilter = false;

	//window pos
	GetInteger("", "top", 25, _nWindowsTop );
	GetInteger("", "left", 10, _nWindowsLeft );
	GetInteger("", "width", 778, _nWindowsWidth );
	GetInteger("", "height", 560, _nWindowsHeight );

	GetBool("", "Zoomed", false, _bWindowMaxed );
	if( _bWindowMaxed )
		AfxGetApp()->m_nCmdShow = SW_SHOWMAXIMIZED;

	GetInteger("", "TabSize", 20, _nTitleLen );

	//part 1 end

	//===Proxy
	//load defaul proxy
	GetString("", "DefaultProxy", "", _csCurrentProxy );

	//proxy by pass
	GetString("", "ProxyByPass", "<local>", _csProxyByPass );
	//proxy list number
	GetInteger("", "ProxyList", 1, _nCurrentProxyList );
	//defaul web proxy
	GetString("", "DefaultWebProxy", "", _csDefaultWebProxy );

	//===
	//active new window
	GetBool("", "ActiveNew", false, _bActiveNewWindow );
	//always new window
	GetBool("", "AddressInNewWindow", false, _bAddressInNewWindow );
	GetBool("", "LinkInNewWindow", false, _bLinkInNewWindow );
	GetBool("", "FavInNewWindow", true, _bFavInNewWindow );
	GetBool("", "HisInNewWindow", true, _bHisInNewWindow );
	GetBool("", "FileInNewWindow", true, _bFileInNewWindow );

	//part 2 end

	//insert to next tab
	GetBool("", "NextTab", true, _bNextTab );

	//auto save
	GetInteger("", "AutoSave", 0, _nAutoSave );
	GetBool("", "CateWithSite", false, _bCateWithSite );

	//Exp width
	GetInteger("", "ExpWidth", 200, _nExpWidth );

	//Exp Bar
	GetString("", "ExpBar", "", _csExpBar );

	//autosizing tab
	GetBool("", "AutoTab", true, _bAutoTab );
	//max autosizing tab width
	GetInteger("", "MaxTabWidth", 82, _nMaxTabWidth );
	//min autosizing tab width
	GetInteger("", "MinTabWidth", 5, _nMinTabWidth );

	//confirm when exit
	GetBool("", "ConfirmExit", true, _bConfirmExit );
	//use shortcuts
	GetBool("", "UseSC", false, _bUseSC );
	//use alias
	GetBool("", "UseAlias", true, _bUseAlias );
	//small to tray
	GetBool("", "SysTray", false, _bSysTray );
	//3721
	GetBool("", "Use3721", true, _bUse3721 );
	//sequence
	GetInteger("", "Sequence", 1, _nSeq );
	//silent
	GetBool("", "Silent", true, _bSilent );
	//default font size
	GetInteger("", "DefFontSize", 2, _nDefFontSize );
	//disable scroll text on status bar
	GetBool("", "DisScrollText", false, _bURLOnly );
	//filte blank popup window from same parent
	GetBool("", "FilteBlank", false, _bFilteBlank );
	//interval between opening windows
	GetInteger("", "Interval", 0, _nInterval );

	//browser mode
	GetBool("BModes", "ActiveNewWindow", _bActiveNewWindow, _bActiveNewWindow_default );
	GetBool("BModes", "AlwaysNewWindow", _bAddressInNewWindow, _bAlwaysNewWindow_default );
	GetInteger("BModes", "Seq", _nSeq, _nSeq_default );
	GetBool("BModes", "LinkInNewWin", _bLinkInNewWindow, _bLinkInNewWin_default );

	_bActiveNewWindow_news = false;
	_bAlwaysNewWindow_news = true;
	_nSeq_news = 1 ;
	_bLinkInNewWin_news = true;

	_bActiveNewWindow_standard = true;
	_bAlwaysNewWindow_standard = false;
	_nSeq_standard = 1;
	_bLinkInNewWin_standard = false;

	//auto scroll
	GetBool("", "AutoScrollAfterDL", false, _bDefScrollAfterDL );
	GetBool("", "UseAutoScroll", false, _bDefUseAutoScroll );
	GetInteger("", "SrSpeed", 1, _nDefSrSpeed );

	//scroll speed
	int nSeed=0;
	GetInteger("", "SpeedSeed", 100, nSeed );
	if( nSeed == 0 )
		_fgSeed = 1.0;
	else
		_fgSeed = (float) nSeed/100;

	GetBool("", "ConfirmCloseAll", true, _bConfirmCloseAll );
	//start group
	GetString("", "StartGroup", "", _csStartGroup );
	//new window
	GetInteger("", "NewWindow", 0, _nNewWindow );
	GetInteger("", "NewWindow2", 0, _nNewWindow2 );

	//allow double click to close window
	GetInteger("", "AllowDBC", 1, _nAllowDBC );
	GetInteger("", "AllowMC", 1, _nAllowMC );
	GetInteger("", "AllowRC", 0, _nAllowRC );

	//default bg color
	GetString("", "BGColor", "#FFFFFF", _csBGColor );
	//default auto refresh
	GetInteger("", "AutoRefresh", 60, _nDefRefreshInterval );
	//security
	GetBool("", "Secure", false, _bSecure );
	//auto set background
	GetBool("", "AutoSetBG", false, _bAutoSetBG );
	//animate icon
	GetBool("", "AnimateIcon", true, _bAnimatIcon );
	//mouse gestures
	GetBool("", "MouseGestures", true, _bAllowMouseGestures );
	//lock homepage
	GetBool("", "LockHomepage", false, _bLockHomepage );
	//most favorite checked
	GetBool("", "MostFavChked", false, _bMostFavChecked );

	GetBool("", "MouseCtrlSpeed", false, _bMouseCtrlScrSpeed );
	GetBool("", "UseUrlFilter", false, _bUseUrlFilter );

	GetBool("", "DispSpeed", true, _bDispBP );

	//file types
	GetString("", "FileTypes", "*.htm;*.html;*.shtm;*.shtml;*.mht;*.txt;", _csFileTypes );

	GetBool("", "NoJava", false, _bNoJava );
	GetBool("", "NoActivex", false, _bNoActivex );
	GetBool("", "NoScript", false, _bNoScript );

	GetString("", "oldshell", "", _csOldShell );

	//load download tools
	LoadDownloadTools();

	//read backgorund pic name
	GetString("", "Background", "", _csBGPic );

	//load shortcuts
	char key[9] = "F";
	for(int i=0; i<11; i++)
	{
		itoa(i+2, key+1, 10);
		GetString("Shortcuts", key, "", _csShortCuts[i] );
	}

	//Domains
	GetString("Domains", "CE1", "http://www.", _csDomainCE1 );
	GetString("Domains", "CE2", ".com", _csDomainCE2 );
	GetString("Domains", "SE1", "http://www.", _csDomainSE1 );
	GetString("Domains", "SE2", ".net", _csDomainSE2 );
	GetString("Domains", "CSE1", "http://www.", _csDomainCSE1 );
	GetString("Domains", "CSE2", ".com.cn", _csDomainCSE2 );

	//bottom bar
	GetBool( "Bands", "bandb0", true, _bTaskBar );
	GetBool( "Bands", "Status", true, _bStatusBar );

	//load win buttons
	char key1[5]="W";
	for( i=0; i<6; i++)
	{
		itoa(i, key1+1, 10);
		GetBool("WinButtons", key1, true, _baWinButtons[i] );
	}

	//compact fav bar
	GetBool("", "CompactFavBar", false, _bCompactFavBar );

	//max menu size
	GetInteger("", "MenuSize", 26, _nMenuWidth );
	GetBool("", "DisplayOpenAll", true, _bDisplayOpenAll );
	GetBool("", "DisplayAddFav", true, _bDisplayAddFav );
	//single column menu
	GetBool("", "SingleMenu", false, _bSingleMenu );

	// Create and set the hot toolbar image list.
	GetBool("", "SmallToolbar", true, _bSmallToolBar );
	GetBool("", "LockToolbar", false, _bLockToolbar );

	GetInteger("", "TextLabel", 0, _nTextLabel );
	GetBool("", "ColorIcon", true, _bColorIcon );

	//display ip
	GetBool("", "DisplayIP", false, _bDisplayIP );
	//display mem
	GetBool("", "DisplayMem", true, _bDispMem );
	//multiline tab
	GetBool("", "MultiLineTab", true, _bMultiLineTab );
	GetBool("", "FBTab", true, _bFBTab );

	//load band position
	char temp[20]="band", rectstr[20]="bandrc";
	UINT len = 0;
	i = -1;
	CBand* pBand = new CBand;
	do
	{
		i++;
		itoa(i, temp+4, 10);
		GetInteger( "", temp, 0, pBand->_nID );
		GetBool( "Bands", temp, true, pBand->_bShow );
		itoa(i, rectstr+6,10);
		pBand->_bRect = GetStruct( "", rectstr, (LPVOID)&( pBand->_rect ), sizeof( pBand->_rect ) );
		if( pBand->_nID != 0 )
			_aBands.Add( *pBand );
//			AddBand( *pBand );
	}while( pBand->_nID != 0 );
	delete pBand;

	//get Utils name list
	CString wp;
	CExternalUtilItem* eui;
	GetString("ExUtils", "name1", "", wp );
	if(wp == "" || wp.IsEmpty())
	{
	}
	else
	{
		//new format
		char Keyname1[10] = "name", Keyname2[10] = "cmds", Keyname3[10] = "sta", Keyname4[10] = "cls";
		CString cmd;
		BOOL bstart, bClose;
		int i=1;
		GetString("ExUtils", "cmds1", "", cmd );
		GetInteger("ExUtils", "sta1", FALSE, bstart );
		GetInteger("ExUtils", "cls1", bstart, bClose );
		while(wp != "" && !wp.IsEmpty())
		{
			eui = new CExternalUtilItem;
			eui->m_strUtilName = wp;
			eui->m_strUtilCmd = "\""+cmd+"\"";
			eui->m_bUtilStart = bstart;
			eui->m_bUtilClose = bClose;
			_ExternalUtilList.m_UtilList.Add(eui);
			i++;
			itoa(i, Keyname1+4, 10);
			itoa(i, Keyname2+4, 10);
			itoa(i, Keyname3+3, 10);
			itoa(i, Keyname4+3, 10);
			GetString("ExUtils", Keyname1, "", wp );
			GetString("ExUtils", Keyname2, "", cmd );
			GetInteger("ExUtils", Keyname3, FALSE, bstart );
			GetInteger("ExUtils", Keyname4, bstart, bClose );
		}
	}

	//download them?
	HKEY            hKey;
	TCHAR           sz[MAX_PATH];
	DWORD           dwSize = MAX_PATH;

	if(RegOpenKey(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Internet Explorer\\Main"), &hKey) != ERROR_SUCCESS)
	{
		_bDLImage = true;
		_bDLVideo = true;
		_bDLSound = true;
		_bDLAnimation = true;
	}
	else
	{
		dwSize = sizeof(sz);
		RegQueryValueEx(hKey, _T("Display Inline Images"), NULL, NULL, (LPBYTE)sz, &dwSize);
		if(strcmp(sz, "yes")==0)
			_bDLImage = true;
		
		dwSize = MAX_PATH;
		RegQueryValueEx(hKey, _T("Display Inline Videos"), NULL, NULL, (LPBYTE)sz, &dwSize);
		if(strcmp(sz, "yes")==0)
			_bDLVideo = true;

		dwSize = MAX_PATH;
		RegQueryValueEx(hKey, _T("Play_Background_Sounds"), NULL, NULL, (LPBYTE)sz, &dwSize);
		if(strcmp(sz, "yes")==0)
			_bDLSound = true;

		dwSize = MAX_PATH;
		RegQueryValueEx(hKey, _T("Play_Animations"), NULL, NULL, (LPBYTE)sz, &dwSize);
		if(strcmp(sz, "yes")==0)
			_bDLAnimation = true;

		RegCloseKey(hKey);
	}

	return true;

}

int CConfigTable::GetToolBarID( const CString csBar )
{
	int iID = _INI_VAR_TOOLBAR_ID__D;
	GetInteger( _INI_SECTION_TOOLBAR, csBar, _INI_VAR_TOOLBAR_ID__D, iID );

	return iID;
}

bool CConfigTable::WriteToolBarID( const CString csBar, const int iID )
{
	return WriteInteger( _INI_SECTION_TOOLBAR, csBar, iID );
}

bool CConfigTable::WriteInteger( const CString csSection, const CString csVar, int iToWrite )
{
	bool bReturn = false;

	CString csToWrite="";
	csToWrite.Format( "%d", iToWrite );

	if( csSection.IsEmpty() )
		bReturn = WritePrivateProfileString( _INI_SECTION_SETTING, csVar, csToWrite, _csConfigFile);
	else
		bReturn = WritePrivateProfileString( csSection, csVar, csToWrite, _csConfigFile);

	return bReturn;
}

bool CConfigTable::WriteString( const CString csSection, const CString csVar, const CString csToWrite )
{
	bool bReturn = false;

	if( csSection.IsEmpty() )
		bReturn = WritePrivateProfileString( _INI_SECTION_SETTING, csVar, csToWrite, _csConfigFile );
	else
		bReturn = WritePrivateProfileString( csSection, csVar, csToWrite, _csConfigFile );

	return bReturn;
}

bool CConfigTable::WriteStruct( const CString csSection, const CString csVar, LPVOID lpStruct, UINT uSizeStruct )
{
	bool bReturn = false;

	if( csSection.IsEmpty() )
		bReturn = WritePrivateProfileStruct( _INI_SECTION_SETTING, csVar, lpStruct, uSizeStruct, _csConfigFile );
	else
		bReturn = WritePrivateProfileStruct( csSection, csVar, lpStruct, uSizeStruct, _csConfigFile );

	return bReturn;
}

/*
BOOL WritePrivateProfileStruct(
  LPCTSTR lpszSection,  // section name
  LPCTSTR lpszKey,      // key name
  LPVOID lpStruct,      // data buffer
  UINT uSizeStruct,     // size of data buffer
  LPCTSTR szFile        // initialization file
);
*/

bool CConfigTable::LoadRemainingSettings()
{
	//load proxy lists
	_aProxyCategories.LoadProxies();

	//load popup filter list
	char *tmp = NULL;
	CString pop;
	GetString( "Popups", "popup1", "", pop );

	if(pop == "" || pop.IsEmpty())
	{
/*3.2		r = app->GetProfileBinary( "", "Popups", (LPBYTE*)(&tmp), &len);
		if(r)
		{
			//load pop list
			char *token;
			char seps[] = "\n";
			token = strtok( tmp, seps);
			while( token != NULL )
			{
				  /* While there are tokens in "string" * /
				  _cslPopup.AddTail(token);
				  /* Get next token: * /
				  token = strtok( NULL, seps );
			}
			delete[] tmp;

		}*/
	}
	else
	{
		//load new one
		char Keyname[12] = "popup";
		int i=1;
		while(pop != "" && !pop.IsEmpty())
		{
			_cslPopup.AddTail(pop);

			i++;
			itoa(i, Keyname+5, 10);
			GetString( "Popups", Keyname, "", pop );
		}
	}

	//load url filter
	TCHAR buf[1024];
	DWORD rlens;
	int i = 0;
	char Keyname[12] = "url";
	CString filename = _csConfigRootFolder + _URL_FILTER_INI_FILE_;
	bool r = TRUE;
	while(r)
	{
		itoa(i, Keyname+3, 10);
		rlens = GetPrivateProfileString( "Filter", Keyname, _T(""), buf, 1024, filename );
		if(rlens==0)
			r=FALSE;
		else
		{
			pop = buf;
			pop.TrimLeft();
			pop.TrimRight();
			_csaUrlFilter.Add(pop);
		}
		i++;
	}

	//load quich search
	TCHAR key[10];
	CString url,alias;
	r = TRUE;
	i = 0;
	while(r)
	{
		key[0] = 'N';
		itoa(i, key+1, 10);
		rlens = ::GetPrivateProfileString("QuickSearch", key, _T(""), buf, 1024,
									  _csQuickSearchFile);
		if(rlens==0)
			r=FALSE;
		else
		{
			alias = buf;
			key[0] = 'S';
			itoa(i, key+1, 10);
			rlens = ::GetPrivateProfileString("QuickSearch", key, _T(""), buf, 1024,
										  _csQuickSearchFile);
			if(rlens==0)
				r=FALSE;
			else
			{
				url = buf;
				alias.TrimLeft();
				alias.TrimRight();
				_mapQuickSearch.SetAt(alias, url);
			}
		}
		i++;
	}

	//load alias
	i=0;
	r = TRUE;
	while(r)
	{
		key[0] = 'A';
		itoa(i, key+1, 10);
		GetString("Alias", key, "", alias );
		if(alias=="" || alias.IsEmpty())
			r=FALSE;
		else
		{
			alias.TrimLeft();
			alias.TrimRight();
			key[0] = 'U';
			itoa(i, key+1, 10);
			GetString("Alias", key, "", url );
			if(url=="" || url.IsEmpty())
				r=FALSE;
			else
				_mapAlias.SetAt(alias, url);
		}
		i++;
	}

	//all links window
	GetInteger("", "LinkListtop", 100, _nLinkListTop );
	GetInteger("", "LinkListleft", 40, _nLinkListLeft );
	GetInteger("", "LinkListwidth", 300, _nLinkListWidth );
	GetInteger("", "LinkListheight", 430, _nLinkListHeight );

	//collect window
	GetInteger("", "collecttop", 400, _nCollectTop );
	GetInteger("", "collectleft", 600, _nCollectLeft );
	GetInteger("", "collectwidth", 180, _nCollectWidth );
	GetInteger("", "collectheight", 140, _nCollectHeight );

	return true;
}

bool CConfigTable::SetDownloadTool(CString &ProgID)
{
	CLSID clsID;
	HRESULT hr;

	try{

	int i = GetIDFromName( _csDefaultDownloadTool );
	if(i>=0)
	{
		if( ProgID == _aDownloadTools[i].ProgID )
			return TRUE;
	}

	if( _pDownload!=NULL )
		_pDownload->Release();
	_pDownload = NULL;

	CoFreeUnusedLibraries();

	if(ProgID=="")
		return TRUE;

	//CLSIDFromString...
	BSTR s = ProgID.AllocSysString();
	hr= CLSIDFromProgID(s, &clsID);//
	SysFreeString(s);
	if(FAILED(hr))
	{
	   //AfxMessageBox("Retrieval of ProgID failed");
		return FALSE;
	}
	else
	{
		hr = CoCreateInstance(clsID, NULL,
						CLSCTX_INPROC_SERVER,
						IID_IDispatch,
						(void**)&( _pDownload ) );
		if(SUCCEEDED(hr))
		{
/*			//update all ini flag;
			int n = nWin; //m_wndTab.GetItemCount();
			TCITEM TabCtrlItem;

			try{

			for(int i=0; i<n; i++)
			{
				TabCtrlItem.mask = TCIF_PARAM;
				TabCtrlItem.lParam = 0;
				m_wndTab.GetItem(i, &TabCtrlItem);
				if(TabCtrlItem.lParam != NULL)
					//((CChildFrame *)TabCtrlItem.lParam)->GetWindowText(str);
					((CChildFrame *)TabCtrlItem.lParam)->m_pView->m_bIniDL= TRUE;
			}
			}
			catch(...)
			{
			}*/
			return TRUE;
		}
		else
			return FALSE;
	}

	}catch(...)
	{
	}
	return FALSE;
}

int CConfigTable::GetIDFromName(LPCSTR name)
{
	for (int i=0;i <= _aDownloadTools.GetUpperBound();i++)
	{
	   if( _aDownloadTools[i].DName == name)
		   return i;
	}
	return -1;
}

bool CConfigTable::LoadDownloadTools()
{
	CDownloadTool  dtNew;
	dtNew.DName = "NetAnts";
	dtNew.ProgID = "NetAnts.IE.Monitor";
	_aDownloadTools.Add( dtNew );
	dtNew.DName = "FlashGet";
	dtNew.ProgID = "Jccatch.IeCatch2.1";
	_aDownloadTools.Add( dtNew );

	char Keyname1[6] = "DT", Keyname2[7] = "PID";
	int i, l=1;
	CString dt, pid;
	char Section[]="DownloadTool";
	
	i=1;
	GetString( Section, "DT1", "", dt );
	GetString( Section, "PID1", "", pid );
	while(dt != "" && !dt.IsEmpty())
	{
		dtNew.DName = dt;
		dtNew.ProgID = pid;
		_aDownloadTools.Add( dtNew );
			
		i++;
		itoa(i, Keyname1+2, 10);
		itoa(i, Keyname2+3, 10);
		GetString( Section, Keyname1, "", dt );
		GetString( Section, Keyname2, "", pid );
	}

	GetString( "DownloadTool", "DefaultTool", "FlashGet", _csDefaultDownloadTool );
	GetString( "DownloadTool", "FileExt", ".ZIP;.EXE;.BIN;.TGZ;.GZ;.Z;.TAR;.ARJ;.LZH;.MP3;.IMP;.COM;.RAR;.ACE;.RM;.A??", _csDownloadFileExt );
	if( _csDefaultDownloadTool != "" )
	{
		int i = GetIDFromName( _csDefaultDownloadTool );
		if(i>=0)
		{
			if( ! SetDownloadTool( _aDownloadTools[i].ProgID ) )
				_csDefaultDownloadTool = "";
		}
		else
			_csDefaultDownloadTool = "";
	}

	return TRUE;
}

bool CConfigTable::Set_CollectWindowRect( const CRect& crWnd )
{
	WriteInteger( "", "collecttop", crWnd.top );
	WriteInteger( "", "collectleft", crWnd.left );
	WriteInteger( "", "collectwidth", crWnd.Width() );
	WriteInteger( "", "collectheight", crWnd.Height() );

	return true;
}

bool CConfigTable::Set_LinkWindowRect( const CRect& crWnd )
{
	WriteInteger( "", "LinkListtop", crWnd.top );
	WriteInteger( "", "LinkListleft", crWnd.left );
	WriteInteger( "", "LinkListwidth", crWnd.Width() );
	WriteInteger( "", "LinkListheight", crWnd.Height() );

	return true;
}

bool CConfigTable::Set_FileTypes( const CString& csToWrite )
{
	return WriteString("", "FileTypes", csToWrite );
}

bool CConfigTable::Set_InAutoFilter( const int& iToWrite )
{
	return 	WriteInteger( "", "InAutoFilter", iToWrite );
}

bool CConfigTable::SaveSettings()
{
 try
 {
	//===Proxy
	if( _bProxyNew )
		_aProxyCategories.SaveProxies();
	_bProxyNew = false;

	UINT	i = 1;
	char Keyname[12] = "popup"; 
	POSITION pos = _cslPopup.GetHeadPosition();
	while(pos!=NULL)
	{
		itoa(i, Keyname+5, 10);
		WriteString("Popups", Keyname, _cslPopup.GetAt(pos) );
		_cslPopup.GetNext(pos);
		i++;
	}
	WriteInteger( "", "EnablePop", _bEnablePopupFilter );
	WriteInteger( "", "EnableDupFilter", _bEnableDupFilter );
	WriteInteger( "", "EnableAutoFilter", _bEnableAutoFilter );

	//url filter
	i = 1;
	strcpy(Keyname, "url"); 
	//3.6.1 add (UINT)
	for(i=0; i< (UINT) _csaUrlFilter.GetSize(); i++)
	{
		itoa(i, Keyname+3, 10);
		WritePrivateProfileString("Filter", Keyname, _csaUrlFilter.GetAt(i), _csUrlFilterFile );
	}
	WriteInteger( "", "UseUrlFilter", _bUseUrlFilter );

	//save web proxies
	WriteInteger( "", "EnableProxy", _nEnableProxy );
	//save default web proxy
	WriteString( "", "DefaultWebProxy", _csDefaultWebProxy );
	//save by pass
	WriteString( "", "ProxyByPass", _csProxyByPass );
	//save proxy list
	WriteInteger( "", "ProxyList", _nCurrentProxyList );
	//save defaul  proxy
	WriteString( "", "DefaultProxy", _csCurrentProxy );

	//tab pos
	WriteInteger( "", "TabPos", _bTopTab );
	WriteInteger( "", "TabSize", _nTitleLen );
	WriteInteger( "", "goHome", _nOpenType );

	//save utils names 
	char Keyname1[15], Keyname2[15], Keyname3[15], Keyname4[15];
	strcpy(Keyname1, "name"); 
	strcpy(Keyname2, "cmds");
	strcpy(Keyname3, "sta");
	strcpy(Keyname4, "cls");
	CExternalUtilItem* eui;
	for (int ir=0;ir <= _ExternalUtilList.m_UtilList.GetUpperBound();ir++)
	{
		eui = _ExternalUtilList.m_UtilList.GetAt(ir);
		itoa(ir+1, Keyname1+4, 10);
		itoa(ir+1, Keyname2+4, 10);
		itoa(ir+1, Keyname3+3, 10);
		itoa(ir+1, Keyname4+3, 10);
		WriteString( "ExUtils", Keyname1, eui->m_strUtilName );
		WriteString( "ExUtils", Keyname2, eui->m_strUtilCmd );
		WriteInteger( "ExUtils", Keyname3, eui->m_bUtilStart );
		WriteInteger( "ExUtils", Keyname4, eui->m_bUtilClose );
	}

	//active new
	WriteInteger( "", "ActiveNew", _bActiveNewWindow );
	//in new
	WriteInteger( "", "AddressInNewWindow", _bAddressInNewWindow);
	WriteInteger( "", "LinkInNewWindow", _bLinkInNewWindow);
	WriteInteger( "", "FavInNewWindow", _bFavInNewWindow);
	WriteInteger( "", "HisInNewWindow", _bHisInNewWindow);
	WriteInteger( "", "FileInNewWindow", _bFileInNewWindow);

	//multiline tab
	WriteInteger( "", "MultiLineTab", _bMultiLineTab );
	//insert to next tab
	WriteInteger( "", "NextTab", _bNextTab);
	//display ip
	WriteInteger( "", "DisplayIP", _bDisplayIP );
	//display mem
	WriteInteger( "", "DisplayMem", _bDispMem );
	//display bp
	WriteInteger( "", "DispSpeed", _bDispBP);
	//Background PIC
	WriteString( "", "Background", _csBGPic );
	//start favorite folder
	WriteString( "", "StartFolder", _csFavoritePath );

	//text label
	WriteInteger( "", "TextLabel", _nTextLabel );
	//ColorIcon
	WriteInteger( "", "ColorIcon", _bColorIcon );

	//autosizing tab
	WriteInteger( "", "AutoTab", _bAutoTab );
	//flat button tab
	WriteInteger( "", "FBTab", _bFBTab );
	//max autosizing tab width
	WriteInteger( "", "MaxTabWidth", _nMaxTabWidth );
	//min autosizing tab width
	WriteInteger( "", "MinTabWidth", _nMinTabWidth );

	//confirm when exit
	WriteInteger( "", "ConfirmExit", _bConfirmExit );
	//use shortcut
	WriteInteger( "", "UseSC", _bUseSC );
	//use alias
	WriteInteger( "", "UseAlias", _bUseAlias );

	//save shortcuts
	char key[9]="F";
	for( i=0; i<11; i++ )
	{
		itoa( i+2, key+1, 10);
		WriteString( "Shortcuts", key, _csShortCuts[i] );
	}

	//save alias
	CString alias, url;
	pos = _mapAlias.GetStartPosition();
	i=0;
	while(pos!=NULL)
	{
		_mapAlias.GetNextAssoc(pos, alias, url);
		key[0]='A';
		itoa(i, key+1, 10);
		WriteString( "Alias", key, alias );
		key[0]='U';
		itoa(i, key+1, 10);
		WriteString( "Alias", key, url );
		i++;
	}
	key[0]='A';
	itoa(i, key+1, 10);
	WriteString( "Alias", key, "" );
	key[0]='U';
	itoa(i, key+1, 10);
	WriteString( "Alias", key, "" );

	//max menu width
	WriteInteger( "", "MenuSize", _nMenuWidth );

	//domains
	WriteString( "Domains", "CE1", _csDomainCE1 );
	WriteString( "Domains", "CE2", _csDomainCE2 );
	WriteString( "Domains", "SE1", _csDomainSE1 );
	WriteString( "Domains", "SE2", _csDomainSE2 );
	WriteString( "Domains", "CSE1", _csDomainCSE1 );
	WriteString( "Domains", "CSE2", _csDomainCSE2 );

	//single menu
	WriteInteger( "", "SingleMenu", _bSingleMenu );
	//sys tray
	WriteInteger( "", "SysTray", _bSysTray );
	//3721
	WriteInteger( "", "Use3721", _bUse3721 );
	//max wins
	WriteInteger( "", "MaxWins", _nMaxWins );
	//sequence
	WriteInteger( "", "Sequence", _nSeq );
	//silent
	WriteInteger( "", "Silent", _bSilent );
	//defaut font size
	WriteInteger( "", "DefFontSize", _nDefFontSize );
	//disable scroll text on statusbar
	WriteInteger( "", "DisScrollText", _bURLOnly );
	//interval
	WriteInteger( "", "Interval", _nInterval );
	//open link in new window

	//default browse mode
	WriteInteger( "BModes", "ActiveNewWindow", _bActiveNewWindow_default );
	WriteInteger( "BModes", "AlwaysNewWindow", _bAlwaysNewWindow_default );
	WriteInteger( "BModes", "Seq", _nSeq_default );
	WriteInteger( "BModes", "LinkInNewWin", _bLinkInNewWin_default );

	//auto scroll
	WriteInteger( "", "AutoScrollAfterDL", _bDefScrollAfterDL ); 
	WriteInteger( "", "UseAutoScroll", _bDefUseAutoScroll );
	WriteInteger( "", "SrSpeed", _nDefSrSpeed );  

	WriteInteger( "", "ConfirmCloseAll", _bConfirmCloseAll );
	WriteInteger( "", "OneInstance", _bOneInstance );

	//start group
	WriteString( "", "StartGroup", _csStartGroup );

	//new window
	WriteInteger( "", "NewWindow", _nNewWindow );
	WriteInteger( "", "NewWindow2", _nNewWindow2 );

	//allow double click to close window
	WriteInteger( "", "AllowDBC", _nAllowDBC );
	WriteInteger( "", "AllowMC", _nAllowMC );
	WriteInteger( "", "AllowRC", _nAllowRC );

	//group path
	WriteString( "", "GroupPath", _csGroupPath );
	//toolbar buttons image path
	WriteString( "", "ImagePath", _csToolbarPath );
	//default bg color
	WriteString( "", "BGColor", _csBGColor );

	//display open all fav
	WriteInteger( "", "DisplayOpenAll", _bDisplayOpenAll );
	WriteInteger( "", "DisplayAddFav", _bDisplayAddFav );

	//default auto refresh
	WriteInteger( "", "AutoRefresh", _nDefRefreshInterval );

	//toolbar icon
	WriteInteger( "", "SmallToolbar", _bSmallToolBar );

	//auto set background
	WriteInteger( "", "AutoSetBG", _bAutoSetBG );

	WriteInteger( "", "CompactFavBar", _bCompactFavBar );
	WriteInteger( "", "AnimateIcon", _bAnimatIcon );

	WriteInteger( "", "MouseGestures", _bAllowMouseGestures );

	WriteInteger( "", "LockHomepage", _bLockHomepage );

	//auto save
	WriteInteger( "", "AutoSave", _nAutoSave );
	WriteInteger( "", "CateWithSite", _bCateWithSite );

	//most favorite checked
	WriteInteger( "", "MostFavChked", _bMostFavChecked );

	//most favorite checked
	WriteInteger( "", "MouseCtrlSpeed", _bMouseCtrlScrSpeed );

	//lock toolbar
	WriteInteger( "", "LockToolbar", _bLockToolbar );

	//save win buttons
	key[0]='W';
	for( i=0; i<6; i++ )
	{
		itoa(i, key+1, 10);
		WriteInteger( "WinButtons", key, _baWinButtons[i] );
	}

	//3.2 clear in autofilter flag
	WriteInteger( "", "InAutoFilter", 0);

	//save full screen
	WriteInteger( "", "FullScreen", _bFullScreen );
	WriteInteger( "", "Zoomed", _bWindowMaxed );

	//bands
	{
	char temp[20]="band", rect[20]="bandrc";
	for( i=0; i <= _aBands.GetUpperBound(); i++ )
	{
		CBand* pBand = &(_aBands[i]);
		itoa(i, temp+4, 10);
		WriteInteger( "", temp, pBand->_nID );
		WriteInteger( "Bands", temp, pBand->_bShow );

		itoa(i, rect+6, 10);
		WriteStruct( "", rect, (LPBYTE)&( pBand->_rect ), sizeof( pBand->_rect ) );
	}
	//write last band as 0
	itoa(i, temp+4, 10);
	WriteInteger( "", temp, 0 );
	}

	//bottom bands
	{
	char temp[20]="bandb";
	for( i=0; i <= _aBottomBands.GetUpperBound(); i++ )
	{
		CBottomBand* pBand = &(_aBottomBands[i]);
		itoa(i, temp+5, 10);
		WriteInteger( "Bands", temp, pBand->_bShow );
	}
	//write last bottom band as 0
	itoa(i, temp+5, 10);
	WriteInteger( "Bands", temp, 0 );
	}

	//save download tool
	WriteString( "DownloadTool", "DefaultTool", _csDefaultDownloadTool );
	WriteString( "DownloadTool", "FileExt", _csDownloadFileExt );
	//status bar
	WriteInteger( "Bands", "Status", _bStatusBar );


	//save other settings that must get something from main frame
	theApp.SaveConfg();
 }
 catch(...)
 {
 }

	return true;
}

bool CConfigTable::Set_Running( const bool& bToWrite )
{
	return 	WriteInteger( "", "Running", bToWrite );
}

bool CConfigTable::SaveSettingsWhenExit()
{
 try
 {
	SaveSettings();

	//2.5 release BHO for download
	if( _pDownload!=NULL )
		_pDownload->Release();

	CExternalUtilItem* eui;
	for (int i=0;i <= _ExternalUtilList.m_UtilList.GetUpperBound();i++)
	{
		eui = _ExternalUtilList.m_UtilList.GetAt(i);
		if(eui!=NULL)
		{
			if(eui->m_iProcess!=0 && eui->m_bUtilClose)
			{
				TerminateApp(eui->m_iProcess, 500);
			}
		}
	}

	//erase running flag
	Set_Running( false );

 }catch(...){}

	return true;
}

bool CConfigTable::SetDefault(LPCSTR lpszKey, BOOL setdef)
{
	TCHAR           sz[MAX_PATH];
	HKEY            hKey, hSubKey, hSubKey2;
	DWORD           dwSize = MAX_PATH;
	if(RegOpenKey(HKEY_CLASSES_ROOT, lpszKey, &hKey) != ERROR_SUCCESS)
	{
		return FALSE;
	}
	else
	{
		if(setdef)
		{
			DWORD dwRes;
			dwSize = sizeof(sz);
			*sz='\0';
			char lpszModule[_MAX_PATH];
			HINSTANCE hInstance = AfxGetInstanceHandle();

			//2002-06-10
			//3.6.1
			char szProgramName[]="iTreeSurf";
			RegSetValueEx(hKey, _T(""), NULL, REG_SZ, (LPBYTE)szProgramName, strlen(szProgramName)+1 );

			//open
			RegCreateKeyEx(hKey, "open", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,  &hSubKey, &dwRes);
			RegCreateKeyEx(hSubKey, "command", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,  &hSubKey2, &dwRes);

			if (GetModuleFileName(hInstance, lpszModule+1, _MAX_PATH))
			{
				*lpszModule = '\"';
				strcat(lpszModule, "\" \"%1\"");
				RegSetValueEx(hSubKey2, _T(""), NULL, REG_SZ, (LPBYTE)(lpszModule), strlen(lpszModule));
				
			}

			RegCloseKey(hSubKey);
			RegCloseKey(hSubKey2);
			

			//itreesurf
			RegCreateKeyEx(hKey, "iTreeSurf", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,  &hSubKey, &dwRes);
			RegCloseKey(hKey);
			RegCreateKeyEx(hSubKey, "command", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,  &hKey, &dwRes);


			if (GetModuleFileName(hInstance, lpszModule+1, _MAX_PATH))
			{
				*lpszModule = '\"';
				strcat(lpszModule, "\" \"%1\"");
				RegSetValueEx(hKey, _T(""), NULL, REG_SZ, (LPBYTE)(lpszModule), strlen(lpszModule));
				
			}

			RegCloseKey(hKey);
			RegCloseKey(hSubKey);


		}
		else
		{
			if(strcmp(lpszKey, "mhtmlfile\\shell")==0)
				_csOldShell = "";
			if( ! _csOldShell.IsEmpty() )
				RegSetValueEx(hKey, _T(""), NULL, REG_SZ, (LPBYTE)((LPCSTR)_csOldShell), _csOldShell.GetLength()+1);
		}

		RegCloseKey(hKey);

		return TRUE;
	}

}

bool CConfigTable::SetAsDefaultBrowser()
{
	// TODO: Add your command handler code here
		// find out from the registry where the favorites are located.
	TCHAR           sz[MAX_PATH];
	HKEY            hKey;
	DWORD           dwSize = MAX_PATH;

	BOOL setdef=FALSE;
	int re = AfxMessageBox(IDS_SET_DEFAULT, MB_YESNOCANCEL|MB_ICONQUESTION);
	if(re==IDYES)
	{
		setdef=TRUE;
	}
	else if(re == IDCANCEL)
		return false;


	//htm,html
	if(RegOpenKey(HKEY_CLASSES_ROOT, _T(".htm"), &hKey) != ERROR_SUCCESS)
	{
	}
	else
	{
		dwSize = sizeof(sz);
//		RegQueryValueEx(hKey, _T(""), NULL, NULL, (LPBYTE)sz, &dwSize);

//		if(strcmp(sz, "htmlfile")!=0)
		{
			//set back the default value.
			RegSetValueEx(hKey, _T(""), NULL, REG_SZ, (LPBYTE)("htmlfile"), 9);

		}
		RegCloseKey(hKey);
	}

	if(RegOpenKey(HKEY_CLASSES_ROOT, _T(".html"), &hKey) != ERROR_SUCCESS)
	{
	}
	else
	{
		dwSize = sizeof(sz);
		*sz='\0';
//		RegQueryValueEx(hKey, _T(""), NULL, NULL, (LPBYTE)sz, &dwSize);

//		if(strcmp(sz, "htmlfile")!=0)
		{
			//set back the default value.
			RegSetValueEx(hKey, _T(""), NULL, REG_SZ, (LPBYTE)("htmlfile"), 9);

		}
		RegCloseKey(hKey);
	}

	//shell
	if(RegOpenKey(HKEY_CLASSES_ROOT, _T("htmlfile\\shell"), &hKey) != ERROR_SUCCESS)
	{
	}
	else
	{
		if(setdef)
		{
			//save the old one
			dwSize = sizeof(sz);
			*sz='\0';
			RegQueryValueEx(hKey, _T(""), NULL, NULL, (LPBYTE)sz, &dwSize);

			RegSetValueEx(hKey, _T(""), NULL, REG_SZ, (LPBYTE)("open"), 4);
				
			if(strcmp(sz, "open")!=0)
				WriteString( "", "oldshell", sz );
		}
		else
		{
			if( ! _csOldShell.IsEmpty() )
				RegSetValueEx(hKey, _T(""), NULL, REG_SZ, (LPBYTE)((LPCSTR)_csOldShell), _csOldShell.GetLength()+1);
		}

		RegCloseKey(hKey);
	}

	//set the key value of default browser, open
	SetDefault(_T("htmlfile\\shell"), setdef);

	//url
	if(RegOpenKey(HKEY_CLASSES_ROOT, _T(".url"), &hKey) != ERROR_SUCCESS)
	{
	}
	else
	{
		dwSize = sizeof(sz);
		RegQueryValueEx(hKey, _T(""), NULL, NULL, (LPBYTE)sz, &dwSize);

		if(strcmp(sz, "InternetShortcut")!=0)
		{
			//set back the default value.
			RegSetValueEx(hKey, _T(""), NULL, REG_SZ, (LPBYTE)("InternetShortcut"), 16);

		}
		RegCloseKey(hKey);
	}
	SetDefault(_T("InternetShortcut\\shell"), setdef);


	//mht, mhtml
	if(RegOpenKey(HKEY_CLASSES_ROOT, _T(".mht"), &hKey) != ERROR_SUCCESS)
	{
	}
	else
	{
		dwSize = sizeof(sz);
		RegQueryValueEx(hKey, _T(""), NULL, NULL, (LPBYTE)sz, &dwSize);

		if(strcmp(sz, "mhtmlfile")!=0)
		{
			//set back the default value.
			RegSetValueEx(hKey, _T(""), NULL, REG_SZ, (LPBYTE)("mhtmlfile"), 10);

		}
		RegCloseKey(hKey);
	}


	if(RegOpenKey(HKEY_CLASSES_ROOT, _T(".mhtml"), &hKey) != ERROR_SUCCESS)
	{
	}
	else
	{
		dwSize = sizeof(sz);
		*sz='\0';
		RegQueryValueEx(hKey, _T(""), NULL, NULL, (LPBYTE)sz, &dwSize);

		if(strcmp(sz, "mhtmlfile")!=0)
		{
			//set back the default value.
			RegSetValueEx(hKey, _T(""), NULL, REG_SZ, (LPBYTE)("mhtmlfile"), 10);

		}
		RegCloseKey(hKey);
	}

	//set the key value of default browser, open
	SetDefault(_T("mhtmlfile\\shell"), setdef);

	//http
	SetDefault( _T("http\\shell"), setdef);

	//https
	SetDefault( _T("https\\shell"), setdef);

	//file
	SetDefault( _T("file\\shell"), setdef);
		
	//ftp
	SetDefault( _T("ftp\\shell"), setdef);

	//shtm, shtml
	if(setdef)
	{
		if(RegOpenKey(HKEY_CLASSES_ROOT, _T(".shtm"), &hKey) != ERROR_SUCCESS)
		{
			return false;
		}
		dwSize = sizeof(sz);
		RegQueryValueEx(hKey, _T(""), NULL, NULL, (LPBYTE)sz, &dwSize);

		if(strcmp(sz, "htmlfile")!=0)
		{
			//set back the default value.
			RegSetValueEx(hKey, _T(""), NULL, REG_SZ, (LPBYTE)("htmlfile"), 9);

		}
		RegSetValueEx(hKey, _T("Content Type"), NULL, REG_SZ, (LPBYTE)("text/html"), 10);
		RegCloseKey(hKey);

		if(RegOpenKey(HKEY_CLASSES_ROOT, _T(".shtml"), &hKey) != ERROR_SUCCESS)
		{
			return false;
		}
		dwSize = sizeof(sz);
		*sz='\0';
		RegQueryValueEx(hKey, _T(""), NULL, NULL, (LPBYTE)sz, &dwSize);

		if(strcmp(sz, "htmlfile")!=0)
		{
			//set back the default value.
			RegSetValueEx(hKey, _T(""), NULL, REG_SZ, (LPBYTE)("htmlfile"), 9);

		}
		RegSetValueEx(hKey, _T("Content Type"), NULL, REG_SZ, (LPBYTE)("text/html"), 10);
		RegCloseKey(hKey);

	}

	return true;
}

bool CConfigTable::SaveOptions_ITreeSurf()
{
	if( _csGroupPath.Right(1) != "\\" )
		_csGroupPath += "\\";
	if( _csSkinPath.Right(1) != "\\" )
		_csSkinPath += "\\";
	if( _csImagePath.Right(1) != "\\" )
		_csImagePath += "\\";
	if( !_csSavePath.IsEmpty() && _csSavePath.Right(1) != "\\" )
		_csSavePath += "\\";

	WriteInteger( "", "NoScript", _bNoScript );
	WriteInteger( "", "NoActivex", _bNoActivex );
	WriteInteger( "", "NoJava", _bNoJava );

	WriteString( "", "SkinPath", _csSkinPath );
	WriteString( "", "DefaultDir", _csSavePath );


	TCHAR           sz[MAX_PATH];
	HKEY            hKey;
	DWORD           dwSize = MAX_PATH;
	char yes[] = "yes", no[]="no";
	SECURITY_DESCRIPTOR sd;
	SECURITY_ATTRIBUTES sa;
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = FALSE;
	sa.lpSecurityDescriptor = &sd;

	if ( RegCreateKeyEx(
		HKEY_CURRENT_USER, 
		_T("Software\\Microsoft\\Internet Explorer\\Main"), 
		NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, &sa, 
		&hKey, 
		NULL) 
		!= ERROR_SUCCESS )
	{
		return false;
	}

	if( _bDLImage )
		RegSetValueEx(hKey, _T("Display Inline Images"), 0, REG_SZ, (LPBYTE)yes, 3);
	else
		RegSetValueEx(hKey, _T("Display Inline Images"), 0, REG_SZ, (LPBYTE)no, 2);
	
	if( _bDLVideo )
		RegSetValueEx(hKey, _T("Display Inline Videos"), 0, REG_SZ, (LPBYTE)yes, 3);
	else
		RegSetValueEx(hKey, _T("Display Inline Videos"), 0, REG_SZ, (LPBYTE)no, 2);
	
	if( _bDLSound )
		RegSetValueEx(hKey, _T("Play_Background_Sounds"), 0, REG_SZ, (LPBYTE)yes, 3);
	else
		RegSetValueEx(hKey, _T("Play_Background_Sounds"), 0, REG_SZ, (LPBYTE)no, 2);
	
	if( _bDLAnimation )
		RegSetValueEx(hKey, _T("Play_Animations"), 0, REG_SZ, (LPBYTE)yes, 3);
	else
		RegSetValueEx(hKey, _T("Play_Animations"), 0, REG_SZ, (LPBYTE)no, 2);

	if( _bSilent )
		RegSetValueEx(hKey, _T("Disable Script Debugger"), 0, REG_SZ, (LPBYTE)yes, 3);

	RegCloseKey(hKey);		

	_bSaveConfig = true;

	return true;
}

bool CConfigTable::SetStringArray_LastVisit( CStringArray& csaLast )
{
	CString csLast="";
	for( int i=0; i<=csaLast.GetUpperBound(); i++ )
	{
		csLast += csaLast[i];
		csLast += "\n";	
	}
	//? not good
	AfxGetApp()->WriteProfileBinary( "Settings", "lastvisit", (LPBYTE)((LPCSTR) csLast), csLast.GetLength()+1 );

	return true;
}

bool CConfigTable::Set_MainWindowRect( const CRect& crWnd )
{
	WriteInteger( "", "top", crWnd.top );
	WriteInteger( "", "left", crWnd.left );
	WriteInteger( "", "width", crWnd.Width() );
	WriteInteger( "", "height", crWnd.Height() );

	return true;
}

bool CConfigTable::ClearBandsArray()
{
	_aBands.RemoveAll();

	return true;
}

bool CConfigTable::AddBand( const CBand& cbAdd )
{
	CBand cbNew;
	cbNew._nID = cbAdd._nID;
	cbNew._bShow = cbAdd._bShow;
	cbNew._bRect = cbAdd._bRect;
	memcpy( &cbNew._rect, &cbAdd._rect, sizeof( cbAdd._rect ) );

	_aBands.Add( cbNew );
	return true;
}

bool CConfigTable::ClearBottomBandsArray()
{
	_aBottomBands.RemoveAll();

	return true;
}

bool CConfigTable::AddBottomBand( const CBottomBand& cbAdd )
{
	CBottomBand cbNew;
	cbNew._bShow = cbAdd._bShow;

	_aBottomBands.Add( cbNew );
	return true;
}

bool CConfigTable::GetInteger( const CString csSection, const CString csVar, const int iValueDefault, int& iReturn )
{
	if( csSection.IsEmpty() )
		iReturn = GetPrivateProfileInt( _INI_SECTION_SETTING, csVar, iValueDefault, _csConfigFile);
	else
		iReturn = GetPrivateProfileInt( csSection, csVar, iValueDefault, _csConfigFile);

	return true;
}

bool CConfigTable::GetBool( const CString csSection, const CString csVar, const bool bValueDefault, bool& bReturn )
{
	int iGet=-1;
	int iValueDefault=-1;
	if( bValueDefault )
		iValueDefault = 1;
	else
		iValueDefault = 0;

	if( csSection.IsEmpty() )
		iGet = GetPrivateProfileInt( _INI_SECTION_SETTING, csVar, iValueDefault, _csConfigFile );
	else
		iGet = GetPrivateProfileInt( csSection, csVar, iValueDefault, _csConfigFile );

	if( iGet == 0 )
		bReturn = false;
	else
		bReturn = true;

	return true;
}

bool CConfigTable::Set_QuickSearch( const CListCtrl& clSearch )
{
	// TODO: Add extra validation here
	_mapQuickSearch.RemoveAll();
	int n = clSearch.GetItemCount();
	CString alias;
	CString filename = _csQuickSearchFile;
	char key[9];
	ClearSection( "QuickSearch", filename );
	//save quick search
	for( int i=0; i<n; i++)
	{
		alias = clSearch.GetItemText(i,0);
		alias.TrimLeft(); 
		alias.TrimRight();
		_mapQuickSearch.SetAt( alias, clSearch.GetItemText(i,1) );
		key[0]='N';
		itoa(i, key+1, 10);
		WritePrivateProfileString( "QuickSearch", key, alias, filename );
		key[0]='S';
		itoa(i, key+1, 10);
		WritePrivateProfileString( "QuickSearch", key, clSearch.GetItemText(i,1), filename );
	}

	return true;
}

bool CConfigTable::Set_GroupUrl( const CListCtrl& clGroup, const CString& csFile )
{
	CString filename = _csGroupPath + csFile + ".cgp";

	int n = clGroup.GetItemCount();
	ClearSection( "CaptorGroup", filename );
	for( int i=0; i<n; i++)
	{
		WritePrivateProfileString( "CaptorGroup", clGroup.GetItemText(i,0), clGroup.GetItemText(i,1), filename );
	}

	return true;
}

bool CConfigTable::AddUrlToGroup( const CString& csFile, const CString& csName, const CString& csUrl )
{
	CString filename = _csGroupPath + csFile + ".cgp";

	WritePrivateProfileString( "CaptorGroup", csName, csUrl, filename );

	return true;
}

bool CConfigTable::AddUrlToFavor( const CString& csUrl, const CString& csFolder, const CString& csTitle )
{
	CString csFavFolder = csFolder;
	CString csUrlTitle = csTitle;

	if( csFavFolder.Right(1)!="\\" )
		csFavFolder += "\\";

	csUrlTitle.Replace('\\', '-');
	csUrlTitle.Replace('/', '-');
	csUrlTitle.Replace(':', '_');
	
	WritePrivateProfileString( _T("InternetShortcut"), _T("URL"), csUrl, csFavFolder + csUrlTitle + ".url" );

	return true;
}

bool CConfigTable::ClearSection( const CString& csSection, const CString& csFile )
{
	return WritePrivateProfileSection( csSection, "", csFile );
}

bool CConfigTable::WriteFormData( const CString& csSection, const CString& csVar, const CString& csData )
{
	return WritePrivateProfileString( csSection, csVar, csData, _csFormDataFile );
}

bool CConfigTable::Get_FormData( const CString& csSection, const CString& csVar, CString& csData )
{
	GetPrivateProfileString( csSection, csVar, _T(""), (char *)_buffer, sizeof(_buffer)-1, _csFormDataFile );
	csData = _buffer;
	return true;
}

bool CConfigTable::AddUrlToNewGroup( const CString& csVar, const CString& csUrl, const CString& csFile )
{
	return WritePrivateProfileString( "CaptorGroup", csVar, csUrl, csFile );
}

bool CConfigTable::Set_MostFavor( const CString& csData, const CString& csFile )
{
	return WritePrivateProfileString( "iTreeSurf", "MostFav", csData, csFile );
}

bool CConfigTable::Get_MostFavor( CString& csData, const CString& csFile )
{
	memset( _buffer, 0, sizeof(_buffer) );

	GetPrivateProfileString( _T("iTreeSurf"), _T("MostFav"), _T(""), (char *)_buffer, sizeof(_buffer)-1, csFile );
	csData = _buffer;

	return true;
}

bool CConfigTable::GetFavorUrl( CString& csUrl,  const CString& csFile )
{
	memset( _buffer, 0, sizeof(_buffer) );
	
	// an .URL file is formatted just like an .INI file, so we can
	// use GetPrivateProfileString() to get the information we want
	GetPrivateProfileString( _T("InternetShortcut"), _T("URL"), _T(""), (char *)_buffer, sizeof(_buffer)-1, csFile );
	csUrl = _buffer;

	return true;
}

bool CConfigTable::GetGroupUrl( CListCtrl& clList,  const CString& csFile )
{
	char * valuepair = NULL;
	int nsize = 0;
	HANDLE hfile = INVALID_HANDLE_VALUE;
 try
 {
		hfile = CreateFile(csFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hfile == INVALID_HANDLE_VALUE)
			return false;
		
		nsize = GetFileSize(hfile, NULL);
		CloseHandle(hfile);
		hfile = INVALID_HANDLE_VALUE;

		if(nsize>0)
		{
			nsize*=2;
			valuepair = new char[nsize+1];
			GetPrivateProfileSection( "CaptorGroup", valuepair, nsize, csFile );

			char * value, * tmp;
			int i =0;
			value = valuepair;
			while(value!=NULL && strlen(value)>0)
			{
				tmp = strchr(value, '=');
				*tmp = '\0';
				if(value!=NULL)
				{
					//value is name
					clList.InsertItem(i, value);
					value = tmp;
					value ++;
					clList.SetItemText(i, 1, value);
					i++;
				}
				value = strchr(value, '\0');
				value ++;
			}

			delete[] valuepair;
			valuepair = NULL;
		}
 }catch(...)
 {//?
		if(valuepair != NULL)
			delete[] valuepair;
		if(hfile!=INVALID_HANDLE_VALUE)
			CloseHandle(hfile);
 }

	return true;
}

bool CConfigTable::GetGroupUrl( CStringArray& csaUrl,  const CString& csFile )
{
	char * valuepair = NULL;
	int nsize = 0;
	HANDLE hfile = INVALID_HANDLE_VALUE;
 try
 {
		hfile = CreateFile(csFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hfile == INVALID_HANDLE_VALUE)
			return false;
		
		nsize = GetFileSize(hfile, NULL);
		CloseHandle(hfile);
		hfile = INVALID_HANDLE_VALUE;

		if(nsize>0)
		{
			nsize*=2;
			valuepair = new char[nsize+1];
			GetPrivateProfileSection( "CaptorGroup", valuepair, nsize, csFile );

			char * value, * tmp;
			int i =0;
			value = valuepair;
			while(value!=NULL && strlen(value)>0)
			{
				value = strchr(value, '=');
				if(value!=NULL)
				{
					value ++;
					csaUrl.Add( value );
				}
				value = strchr(value, '\0');
				value ++;
			}
			delete[] valuepair;
			valuepair = NULL;
		}
 }catch(...)
 {//?
		if(valuepair != NULL)
			delete[] valuepair;
		if(hfile!=INVALID_HANDLE_VALUE)
			CloseHandle(hfile);
 }

	return true;
}

bool CConfigTable::Get_FormDataSection( long& lSize, const CString& csSection )
{
	DWORD dwSize = GetPrivateProfileSection( csSection, (char *)_buffer, 20, _csFormDataFile );
	lSize = dwSize;
	return true;
}

bool CConfigTable::GetBool_ShowGoButton( bool& bResult )
{
	bResult = true;
	HKEY            hKey;
	TCHAR           sz[MAX_PATH];
	DWORD           dwSize = MAX_PATH;

	if(RegOpenKey(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Internet Explorer\\Main"), &hKey) != ERROR_SUCCESS)
	{
		TRACE0("Main setting not found\n");
		return false;
	}
	else
	{
		dwSize = sizeof(sz);
		RegQueryValueEx(hKey, _T("ShowGoButton"), NULL, NULL, (LPBYTE)sz, &dwSize);
		if(strcmp(sz, "no")==0)
			bResult = false;

		RegCloseKey(hKey);
	}

	return true;
}

bool CConfigTable::GetDw_ShowHidden( DWORD& dwResult )
{
	HKEY hKey;
	DWORD dwSize;
	//does display hidden files
	if(RegOpenKey(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"), &hKey) == ERROR_SUCCESS)
	{
		RegQueryValueEx(hKey, _T("Hidden"), NULL, NULL, (LPBYTE)&(dwResult), &dwSize );
		RegCloseKey(hKey);	
		return true;
	}

	return false;
}

bool CConfigTable::GetDw_TotalBytesRecvd( DWORD& dwResult )
{
	HKEY hKey;
	DWORD dwValue=(DWORD)-999;

	if(0==RegOpenKey(HKEY_DYN_DATA,"PerfStats\\StatData",&hKey))
	{
		DWORD dwType,dwLen=sizeof(DWORD);
		RegQueryValueEx( hKey, "Dial-Up Adapter\\TotalBytesRecvd", NULL, &dwType, (BYTE*)&dwValue, &dwLen );
		RegCloseKey(hKey);
		dwResult = dwValue;
		return true;
	}

	return false;
}

bool CConfigTable::Set_UserAgentInfo()
{
	//add user agent
	HKEY            hKey=NULL;

	if(RegOpenKey(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\User Agent\\Post Platform"), &hKey) != ERROR_SUCCESS)
	{
		RegCreateKey(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\User Agent\\Post Platform"), &hKey);
	}
	if(hKey!=NULL)
	{
		RegSetValueEx(hKey, _csApplicationName, 0, REG_SZ, (LPBYTE)"IEAK", 4);
		RegCloseKey(hKey);
		return true;
	}

	return false;
}

bool CConfigTable::Remove_UserAgentInfo()
{
	//delete agent
	HKEY            hKey=NULL;

	if(RegOpenKey(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\User Agent\\Post Platform"), &hKey) != ERROR_SUCCESS)
	{
		TRACE0("Main setting not found\n");
	}
	else
	{
		RegDeleteValue(hKey, _csApplicationName);
		RegCloseKey(hKey);
		return true;
	}

	return false;
}
