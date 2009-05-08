
#ifndef __LT_CONFIG_TABLE_H__
#define __LT_CONFIG_TABLE_H__

#include "iTreeSurfDefinition.h"
#include "ProxyList.h"
#include "externalUtils.h"

////////////////////////////////////////////////////////////////////////////
// CDownloadTool

class CDownloadTool
{
public:
	CString DName;
	CString ProgID;
};


////////////////////////////////////////////////////////////////////////////
// CBand

class CBand
{
public:
	int _nID;
	bool _bShow;
	bool _bRect;
	RECT _rect;
};

class CBottomBand
{
public:
	bool _bShow;
};


////////////////////////////////////////////////////////////////////////////
// CConfigTable

class CConfigTable
{
// Construction
public:
	CConfigTable();
	~CConfigTable();

	void Initial();
	void WriteToFile();
	int MakePathValid( CString& csPath, const CString csDefault );

	bool GetBool( const CString csSection, const CString csVar, const bool bValueDefault, bool& bReturn );
	bool GetInteger( const CString csSection, const CString csVar, const int iValueDefault, int& iReturn );
	bool GetString( const CString csSection, const CString csVar, const CString csValueDefault, CString& csReturn );
	bool GetStruct( const CString csSection, const CString csVar, LPVOID lpStruct, UINT uSizeStruct );
	bool GetStringArray_LastVisit( CStringArray& csaReturn );

	int GetToolBarID( const CString csBar );
	bool WriteToolBarID( const CString csBar, const int iID );

	bool GetPath_Group();
	bool GetPath_Skin();
	bool GetPath_Toolbar();
	bool GetPath_Save();
	bool GetPath_Favorite();

	bool GetBool_OneInstance();
	bool GetBool_TopTab();
	bool GetBool_Running();
	bool GetBool_FullScreen();
	
	bool GetInt_EnableProxy();
	bool GetInt_OpenType();
	bool GetInt_MaxWins();

	bool LoadOtherSettings();
	bool LoadRemainingSettings();

	bool LoadDownloadTools();
	int GetIDFromName(LPCSTR name);
	bool SetDownloadTool(CString& ProgID);

	bool AddBand( const CBand& cbAdd );
	bool ClearBandsArray();
	bool AddBottomBand( const CBottomBand& cbAdd );
	bool ClearBottomBandsArray();

	bool GetFavorUrl( CString& csUrl,  const CString& csFile );
	bool GetGroupUrl( CListCtrl& clList,  const CString& csFile );
	bool GetGroupUrl( CStringArray& csaUrl,  const CString& csFile );
	bool Get_MostFavor( CString& csData, const CString& csFile );
	bool Get_FormDataSection( long& lSize, const CString& csSection );
	bool Get_FormData( const CString& csSection, const CString& csVar, CString& csData );
	bool GetBool_ShowGoButton( bool& bResult );
	bool GetDw_ShowHidden( DWORD& dwResult );
	bool GetDw_TotalBytesRecvd( DWORD& dwResult );

	//write to file
	bool WriteString( const CString csSection, const CString csVar, const CString csToWrite );
	bool WriteInteger( const CString csSection, const CString csVar, int iToWrite );
	bool WriteStruct( const CString csSection, const CString csVar, LPVOID lpStruct, UINT uSizeStruct );
	bool ClearSection( const CString& csSection, const CString& csFile );

	bool Set_MainWindowRect( const CRect& crWnd );
	bool Set_CollectWindowRect( const CRect& crWnd );
	bool Set_LinkWindowRect( const CRect& crWnd );
	bool Set_FileTypes( const CString& csToWrite );
	bool Set_InAutoFilter( const int& iToWrite );
	bool SaveSettings();
	bool Set_Running( const bool& bToWrite );
	bool SaveSettingsWhenExit();
	bool SetStringArray_LastVisit( CStringArray& csaLast );
	bool Set_QuickSearch( const CListCtrl& clSearch );
	bool Set_GroupUrl( const CListCtrl& clGroup, const CString& csFile );
	bool AddUrlToGroup( const CString& csFile, const CString& csName, const CString& csUrl );
	bool AddUrlToNewGroup( const CString& csVar, const CString& csUrl, const CString& csFile );
	bool AddUrlToFavor( const CString& csUrl, const CString& csFolder, const CString& csTitle );
	bool WriteFormData( const CString& csSection, const CString& csVar, const CString& csData );
	bool Set_MostFavor( const CString& csData, const CString& csFile );

	//registry
	bool SetDefault(LPCSTR lpszKey, BOOL setdef);
	bool SetAsDefaultBrowser();
	bool Remove_UserAgentInfo();
	bool Set_UserAgentInfo();

	bool SaveOptions_ITreeSurf();

	//useless now
	bool FirstRun();

// Attributes
public:
	CStringList _cslBGPics;
	CString _csBGPic;

	CString _csOldShell;

	CString _csQuickSearchFile;
	CString _csUrlFilterFile;
	CString _csFormDataFile;

	CString _csApplicationName;
	CString _csBitmapRoot;
	CString _csGroupPath;
	CString _csSkinPath;
	CString _csImagePath;
	CString _csToolbarPath;
	CString _csSavePath;
	CString _csFavoritePath;
	CString _csExpBar;
	CString _csShortCuts[12];
	CString _csStartGroup;
	CString _csBGColor;
	CString _csFileTypes;

	CString _csDomainCE1;
	CString _csDomainCE2;
	CString _csDomainSE1;
	CString _csDomainSE2;
	CString _csDomainCSE1;
	CString _csDomainCSE2;

	CString _csCurrentProxy;
	CString _csCurrentProxyName;
	CString _csProxyByPass;
	CString _csDefaultWebProxy;
	int _nCurrentProxyList;
	bool _bProxyNew;
	
	bool _bSaveConfig;

	bool _bOneInstance;
	bool _bTopTab;
	bool _bRunning;
	bool _bFullScreen;
	bool _bEnablePopupFilter;
	bool _bEnableDupFilter;
	bool _bEnableAutoFilter;
	bool _bWindowMaxed;
	bool _bAddressInNewWindow;
	bool _bActiveNewWindow;
	bool _bLinkInNewWindow;
	bool _bFavInNewWindow;
	bool _bHisInNewWindow;
	bool _bFileInNewWindow;
	bool _bNextTab;
	bool _bAutoTab;
	bool _bCateWithSite;
	bool _bConfirmExit;
	bool _bUseSC;
	bool _bUseAlias;
	bool _bSysTray;
	bool _bUse3721;
	bool _bSilent;
	bool _bURLOnly;
	bool _bFilteBlank;
	bool _bDefScrollAfterDL;
	bool _bDefUseAutoScroll;
	bool _bConfirmCloseAll;
	bool _bSecure;
	bool _bAutoSetBG;
	bool _bAnimatIcon;
	bool _bAllowMouseGestures;
	bool _bLockHomepage;
	bool _bMostFavChecked;
	bool _bMouseCtrlScrSpeed;
	bool _bUseUrlFilter;
	bool _bDispBP;
	bool _bInAutoFilter;
	bool _bNoJava;
	bool _bNoActivex;
	bool _bNoScript;
	bool _bTaskBar;
	bool _baWinButtons[6];
	bool _bCompactFavBar;
	bool _bDisplayOpenAll;
	bool _bSingleMenu;
	bool _bSmallToolBar;
	bool _bLockToolbar;
	bool _bStatusBar;
	bool _bColorIcon;
	bool _bDisplayAddFav;
	bool _bDisplayIP;
	bool _bDispMem;
	bool _bMultiLineTab;
	bool _bFBTab;
	bool _bDLImage;
	bool _bDLVideo;
	bool _bDLSound;
	bool _bDLAnimation;

	int _nEnableProxy;
	int _nOpenType;
	int _nMaxWins;
	int _nWindowsTop;
	int _nWindowsLeft;
	int _nWindowsWidth;
	int _nWindowsHeight;
	int _nAutoSave;
	int _nExpWidth;
	int _nTitleLen;
	int _nMaxTabWidth;
	int _nMinTabWidth;
	int _nSeq; //0-default, 1-next, 2- pre
	int _nDefFontSize;
	int _nInterval;
	int _nDefSrSpeed; //1-slow, 2-medium, 3-fast
	int _nNewWindow;
	int _nNewWindow2;
	int _nAllowDBC;
	int _nAllowMC;
	int _nAllowRC;
	int _nDefRefreshInterval;
	int _nLinkListTop;
	int _nLinkListLeft;
	int _nLinkListWidth;
	int _nLinkListHeight;
	int _nCollectTop;
	int _nCollectLeft;
	int _nCollectWidth;
	int _nCollectHeight;
	int _nTextLabel; //0-no label, 1-show label, 2-selective text
	int _nMenuWidth;

	float _fgSeed;

	bool _bAlwaysNewWindow_default;
	bool _bActiveNewWindow_default;
	bool _bLinkInNewWin_default;
	int _nSeq_default;

	bool _bAlwaysNewWindow_news;
	bool _bActiveNewWindow_news;
	bool _bLinkInNewWin_news;
	int _nSeq_news;

	bool _bAlwaysNewWindow_standard;
	bool _bActiveNewWindow_standard;
	bool _bLinkInNewWin_standard;
	int _nSeq_standard;

	CArray<CDownloadTool, CDownloadTool&> _aDownloadTools;
	CString _csDefaultDownloadTool;
	CString _csDownloadFileExt;
	//2.5 for integrate with download tools
	IDispatch * _pDownload; 

	CArray<CBand, CBand&> _aBands;
	CArray<CBottomBand, CBottomBand&> _aBottomBands;

	CProxyCategory _aProxyCategories;
	CStringList _cslPopup;
	CStringArray _csaUrlFilter;
	CMapStringToString _mapQuickSearch;
	CMapStringToString _mapAlias;

	CExternalUtilList _ExternalUtilList;

// Attributes
private:
	CString _csConfigFile;
	CString _csConfigRootFolder;

	//INTERNET_MAX_URL_LENGTH == 2083
	byte _buffer[2100];


// Implementation
protected:
};


#endif //__LT_CONFIG_TABLE_H__
