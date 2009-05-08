// MainFrm.h : interface of the CMainFrame class
//
////////////////////////////////////////////////////////////////////////////

#if !defined( _ITREESURF__MAINFRM_H__INCLUDED_ )
#define _ITREESURF__MAINFRM_H__INCLUDED_

#if _MSC_VER > 1000         
#pragma once                    
#endif // _MSC_VER > 1000                  

//Define the CommandTarget ids
#define HTMLID_FIND 1
#define HTMLID_VIEWSOURCE 2
#define HTMLID_OPTIONS 3 


#include "StatusBarWithProgress.h"
//#include "iTreeSurfDoc.h"
#include "iTreeSurfView.h"
#include "ChildFrm.h"

#include "BaseExpObj.h"
//#include "OOExToolBar.hpp"
#include "ChevBar.h"
#include "MenuBar.h"
#include "AddressBarEx.h"
#include "TabDropTarget.h"
#include "AnimateIcon.h"	// Added by ClassView
#include "AdvTabCtrl.h"


class CCollectorDlg;
class CLinkListDlg;

extern const UINT MSWHELL_ROLLMSG;

#define WSM_TRAY	WM_USER+3
#define BORDER  2

//work threads
UINT _cdecl SendDocuments(LPVOID file);
typedef struct stagDocuments
{
	char filename[256];
	BOOL IsLocalFile;
}sDocuments;

UINT _cdecl QuickSaveDocuments(LPVOID savefiles);
typedef struct stagSaveFiles
{
	CString base;
	CString filename;
	CMapStringToString mapDLFiles;
	CString root;
}sSaveFiles;

UINT _cdecl TSaveConfig(LPVOID con);


class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:
	CStringList m_astrFavoriteFolders;
//	CStringArray m_astrFavoriteURLs;
	BOOL m_bUrlFilterDirty;

// Operations
public:
	friend CTxtDropTarget;
	void SetBackground(CiTreeSurfView* pview, VARIANT& color);
	TCHAR           m_strHome[MAX_PATH], m_strCurHome[MAX_PATH];
	void SetHomePage(LPCTSTR homepage);
	void GetHomePage(LPTSTR homepage);
	CString m_strAutoSavePath;
	BOOL m_bIsAutoSave;
	void InitialUrlFilter();

private:
	void SessionHistory(CMenu* pMenu, int ID, CChildFrame* pcf);
	void BuildWinButtons();
	void ChangTabPos(int npos);
	BOOL GetAllLinks(CiTreeSurfView* pView, CString & strlinks);
	CCollectorDlg* pCollectDlg;
	CLinkListDlg* pLinkListDlg;
	int m_SBIPWidth, m_SBMemWidth;
	HICON m_hSecuIcon;
	void SaveForm(BOOL bIsGeneral);
	BOOL m_bLastIB, m_bIB;
	int m_nNeedIB;
	BOOL IsBrowsing();
	BOOL fTaskBarIsAutohide;
	UINT uEdge;
	void LaunchEditor(LPCSTR editor, LPCSTR file);
	void LaunchEmail(LPCSTR mailclient);
	CTabDropTarget* ms_TabDropTarget;

	//2002-06-14
	bool CloseApp(bool bAsk=true);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	//control used by search

// Implementation
public:
	BOOL m_bAllScroll;
	BOOL m_bNeedUpdProxy;
	void OnOptionFBtab();
	void FixToolbar(BOOL bFixed);
	void UninitialNameSpace();
	void InitialNameSpace();
	DWORD m_dwHidden;
	IInternetSession* m_pSession;
	IClassFactory* m_pFactory;
	int m_nScrollArrowH;
	int m_nScrollWidth;
	void BuildWebProxyMenu(CMenu* pMenu, int nType);
	CiTreeSurfView* m_pCurView;

	DWORD m_dwWebProxyFrom;
	BOOL m_bIsSecure, m_bLastIsSecure;
//	BOOL GetCurSecureInfo();
	CAnimateIcon m_animIcon;
	BOOL m_bNeedActive;
	int nDelay;
	void StartUtil(int i);
	int FindDupURL(LPCSTR lpszURL, CChildFrame* pSelf);
	void UpdateUndoMenu();
	COLORREF m_colBGC;
	CString GetProxyName(CString& proxy);
	void DefaultMail(LPCSTR mailto);
	void SaveLastVisit();
	void AddtoFavorite(BSTR bstrURL, VARIANT* pTitle);
	void OpenGroup(LPCSTR gname);
	void AddtoGroup(LPCSTR pszName, LPCSTR pszURL, UINT nID);
	void MenuLoadGroupList(CMenu* pMenu);
	void InitGroups();
	UINT m_nState;
	UINT nWin;
	void ToolBarSetCheck(CCmdUI* pCmdUI, BOOL bCheck);
	UINT m_nFavSize;
	void FavMenuAddURL(LPCSTR path, LPCSTR root, BOOL IsLink, CMenu *pMenu, int nPos, int& nMostFavs);
	void FavMenuAddFolder(LPCSTR path, LPCSTR root, BOOL FirstLevel, BOOL IsLink, CMenu* pMenu, int& nEndPos);
	CString m_strUndoURL[16];
	CString m_strUndoName[16];
	int undoStart, undoEnd;

	HWND m_hEdit;
	UINT m_nTimerID, m_nUpdateTimer, m_nAnimateIconTimer;
	CStringList m_astrOpenAll;
	int m_nProtectNum;
	BOOL m_bRemPsw;
	CString m_strProtectPwd;
	CTime m_tLastRes, m_tLastProxyUpdate; //m_tLastWinsUpdate, 
//	BOOL m_bWin9x;
	BOOL m_bNoModemSpeed;
	long m_lPreBytes;
    LARGE_INTEGER m_tPreBytes; //, m_tDBClick;
	CSize m_sTBTextSize, m_sTBTextSizeSM;
	int AdjustTabWidth(BOOL bNoReCalSize = FALSE);
	void InitialToolBar(BOOL bReplaceIcon=FALSE);
	void ShowBackground(BOOL bShow);
	HBITMAP m_bmBGPic;
	CString m_strLastIP;
	CTime m_tLastIPUpdate;
	CMenu* GetMenu();
	BOOL DecodeEscap(CString& strUrl);
	int SetTabIcon(int nIcon, int nItem);
	int m_nPreLines;
	BOOL m_bFavDirty;
	BOOL GetFavoriteFolder(CString& strFav);
	int OpenAllFavs(LPCSTR lpszPath, BOOL bMostFav=FALSE);
	void AddFavorite(CChildFrame* tcf);
	CString m_strLinkFolder;
	BOOL HasExpBar(int nStr);
	CChildFrame* NewChildWindow(int nIniLevel=1, int nOpenType=1, LPCSTR strUrl=NULL, CiTreeSurfView * pParentView=NULL, BOOL bForceActive=FALSE);
	BOOL ResolveLink(LPCSTR lpszLinkFile, LPSTR lpszPath);
	BOOL ResolveLink(LPCSTR lpszLinkFile, CString& csPath);
	BOOL GetProxySetting(CString& proxy, CString& bypass, CString& user, CString& pwd);
//	CStringList m_astrProxyPwd;
//	CStringList m_astrProxyUser;
//2.3CList<BYTE, BYTE> m_aProxySpeed;	, m_aWebProxySpeed;
	CRect m_mainRect;
	BOOL m_bChildMax;
	BOOL m_bStatusBarWasVisible;
	BOOL m_bLinkBarWasVisible;
	BOOL m_bFavBarWasVisible;
	BOOL m_bAddressBarWasVisible;
	BOOL m_bExternalBarWasVisible;
	void FullScreenModeOff( bool bChangeValue=true );
	void FullScreenModeOn();
	void RemoveExp();
	void BuildExpBar(LPCSTR lpszTitle);
	void BuildUtilMenu();
/*2.3	CStringList m_lstrUtilCmds;
	CStringList m_lstrUtilNames;
	CList<BOOL, BOOL> m_lbUtilStart, m_lbUtilClose;*/
//2.3	CStringList m_astrProxy;
//2.3	CStringList m_astrWebProxy;
//2.3	CStringList m_astrProxyName;
//2.3	CStringList m_astrWebProxyName;
	BOOL DelTempFiles(DWORD CacheType, BOOL op=TRUE);
	void AddToPopup(CChildFrame* pChFrm);
	void AddMenuBreak(CMenu * pMenu,BOOL bFavMenu=TRUE);
	void InitFavorites(BOOL bOnlyLink=FALSE);
	HRESULT ExecCmdTarget(DWORD nCmdID,CFixedHtmlView* pHVW);
	int nRTabID;
	void SaveTypedURL();
	void LoadTypedURL();
	int FindTab(CChildFrame* lpFrame, BOOL bCleanBlank=FALSE);
	void SetTabTitle(LPCSTR lpszTitle, int nItem);
	int nTotalTabs;
	int AddNewTab(CWnd* lpMvw, int CurID=-1, LPTSTR label=NULL, BOOL bAddLast=TRUE);
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void SetAddress(LPCTSTR lpszUrl);
	int BuildFavoritesMenu(LPCTSTR pszPath, int nStartPos, CMenu* pMenu, int nFixMenu /*=0*/, int FirstLevel /*= FALSE*/, int& nFavs, int& nMostFavs);
	CStatusBarWithProgress  m_wndStatusBar;
	CAdvTabCtrl	m_wndTab;
	CChevBar      m_wndReBar;
	CChevBar		m_wndReBar2;
	baseCMyBar	m_wndInstantBar;
	CBaseExpObj*	m_pwndExpObj;
	CToolBar    m_wndToolBar;
	CComboBoxEx* m_wndAddress;
	CAddressBarEx m_AddressBar;
	BOOL m_bStatusBlank;
	CImageList m_TabImgList;
	CToolBar	m_SysMenuBar;

protected:  // control bar embedded members
	CToolBar	m_LinkBar;
	CMenuBar	m_wndMenuBar;
	CToolBar	m_FavBar;
	CToolBar	m_ExternalTool;
	HINSTANCE   hResInst;

	CString ExpStr, Expstr1, Expstr2, Expstr3, Expstr4, Expstr5;
	CString tab, strTipText, gString1, gString2, gaddre, res, url, useproxy, tip, bmppath, fstr, bp,favurl, lastvisits, lasturl, strlinks;
//	POINT tabpt, tabpt2;
//	float dbtime;
	short ks;
	HWND hAddr;

// Generated message map functions
protected:
	void OnOptionMultiTab();
	void OnOptionAutotab();
	int m_nSwitch;
//	CComboBoxEx m_wndCombo;
	//CAnimateCtrl m_wndAnimate;
	afx_msg void OnOptionsiTreeSurfstartupGroup();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFileNewBlank();
	afx_msg void OnFileNewCurrentpage();
	afx_msg void OnViewAddressbar();
	afx_msg void OnViewTaskbar();
//	afx_msg void OnFileSaveas();
//	afx_msg void OnFileSavehtml();
	afx_msg void OnFileCloseall();
	afx_msg void OnFileWorkoffline();
	afx_msg void OnClose();
	afx_msg void OnViewSource();
	afx_msg void OnFileClose();
	afx_msg void OnFileClose2();
	afx_msg void OnToolsInternetoptions();
	afx_msg void OnFavoritesAddtofavorites();
	afx_msg void OnFavoritesExportfavorites();
	afx_msg void OnFavoritesImportfavorites();
	afx_msg void OnFavoritesOrgnizefavorites();
	afx_msg void OnRtabLock();
	afx_msg void OnToolsAddtopopupfilter();
	afx_msg void OnRtabAddtopopupfilter();
	afx_msg void OnToolsEablepopupfilter();
	afx_msg void OnToolsEditpopupfilterlist();
	afx_msg void OnUpdateFileWorkoffline(CCmdUI* pCmdUI);
	afx_msg void OnOptionsiTreeSurfgohome();
	afx_msg void OnUpdateMenu(CCmdUI* pCmdUI);
	afx_msg void OnToolsEmail();
	afx_msg void OnToolsAddr();
	afx_msg void OnUpdateOptionsUseProxy(CCmdUI* pCmdUI);
	afx_msg void OnOptionsUseProxy();
	afx_msg void OnToolsCleanCookie();
	afx_msg void OnToolsCache();
	afx_msg void OnToolsAll();
	afx_msg void OnToolsHistory();
	afx_msg void OnOptionsSetProxy();
//	afx_msg void OnOptionTabTop();
//	afx_msg void OnOptionTabBot();
//	afx_msg void OnUpdateOptionTabPos(CCmdUI* pCmdUI);
	afx_msg void OnToolsExternalUtilitymanager();
	afx_msg void OnViewHis();
	afx_msg void OnViewSearch();
	afx_msg void OnUpdateViewExp(CCmdUI* pCmdUI);
	afx_msg void OnViewFullscreen();
	afx_msg void OnUpdateViewFullscreen(CCmdUI* pCmdUI);
	afx_msg void OnOptionsSetasdefaultbrowser();
	afx_msg void OnAddrDrop();
	afx_msg void OnAddrFocus();
	afx_msg void OnFileNew();
	afx_msg void OnHelpHelp();
	afx_msg void OnOptionsActivenewwindow();
	afx_msg void OnUpdateOptionsActivenewwindow(CCmdUI* pCmdUI);
	afx_msg void OnOptionsiTreeSurfstartupOpenblankpage();
	afx_msg void OnOptionsiTreeSurfstartupOpenfavoritepages();
	afx_msg void OnOptionsiTreeSurfstartupResumelastvisitedpages();
	afx_msg void OnCloseOther();
	afx_msg void OnRtabAddfav();
	afx_msg void OnViewLinks();
	afx_msg void OnViewToolbar();
	afx_msg void OnUpdateViewToolbar(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHelpForum();
	afx_msg void OnHelpCoDev();
	afx_msg void OnHelpMail();
	afx_msg void OnToolsOpenalllinks();
	afx_msg void OnOptionAlwaysNew();
	afx_msg void OnUpdateOptionAlwaysNew(CCmdUI* pCmdUI);
	afx_msg void OnFileCloseKeepcur();
//	afx_msg void OnOptionsNewtab();
//	afx_msg void OnUpdateOptionsNewtab(CCmdUI* pCmdUI);
	afx_msg void OnViewRefreshall();
	afx_msg void OnViewStopAll();
	afx_msg void OnHelpHomepage();
	afx_msg void OnFileNewClip();
	afx_msg void OnWindowCascade();
	afx_msg void OnWindowTileHorz();
	afx_msg void OnWindowTileVert();
//	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnToolsIp();
	afx_msg void OnUpdateToolsIp(CCmdUI* pCmdUI);
	afx_msg void OnWindowRestore();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnWindowMin();
	afx_msg void OnViewMenubar();
	afx_msg void OnToolsSetbackgroundpic(UINT nID);
	afx_msg void OnUpdateToolsSetbackgroundpic(CCmdUI* pCmdUI);
	afx_msg void OnFileNewitreesurf();
	afx_msg void OnFileOpen();
	afx_msg void OnViewToolbartextlabel();
	afx_msg void OnUpdateViewToolbartextlabel(CCmdUI* pCmdUI);
	afx_msg void OnToolsOpenstart();
	afx_msg void OnChangeProxy(UINT nID);
	afx_msg void OnChangeWebProxy(UINT nID);
	afx_msg void OnMostFav(UINT nID);
//	afx_msg void OnUpdateOptionMultiTab(CCmdUI* pCmdUI);
//	afx_msg void OnUpdateOptionAutotab(CCmdUI* pCmdUI);
	afx_msg void OnShortcutMfl();
	afx_msg void OnF2(UINT nID);
	afx_msg void OnShortcutUseSc();
	afx_msg void OnUpdateShortcutUseSc(CCmdUI* pCmdUI);
	afx_msg void OnShortcutSc();
	afx_msg void OnShortcutAlias();
	afx_msg void OnUpdateShortcutAlias(CCmdUI* pCmdUI);
	afx_msg void OnShortcutManalias();
	afx_msg void OnViewToolbarsCustomize();
	afx_msg void OnOptionsiTreeSurfoptions();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnOptionsiTreeSurfstartupOpenmostfavpages();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnNotifyTray(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSelTab(WPARAM wParam, LPARAM lParam);
	afx_msg void OnShortcutsUse3721();
	afx_msg void OnUpdateShortcutsUse3721(CCmdUI* pCmdUI);
	afx_msg void OnRtabStickname();
//	afx_msg void OnOptionSeqDefault();
//	afx_msg void OnUpdateOptionSeqDefault(CCmdUI* pCmdUI);
//	afx_msg void OnOptionSeqNext();
//	afx_msg void OnUpdateOptionSeqNext(CCmdUI* pCmdUI);
//	afx_msg void OnOptionSeqPre();
//	afx_msg void OnUpdateOptionSeqPre(CCmdUI* pCmdUI);
	afx_msg void OnTabPre();
	afx_msg void OnTabNext();
	//{{AFX_MSG(CMainFrame)
	afx_msg void OnOptionsUrlinstatus();
	afx_msg void OnUpdateOptionsUrlinstatus(CCmdUI* pCmdUI);
	afx_msg void OnViewFavs();
	afx_msg void OnRtabProtect();
	afx_msg void OnViewExTool();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnFileStopOpenall();
	afx_msg void OnUpdateFileStopOpenall(CCmdUI* pCmdUI);
	afx_msg void OnFileHide();
	afx_msg void OnViewFolder();
	afx_msg void OnOptionsOpenlinkinnewwindow();
	afx_msg void OnUpdateOptionsOpenlinkinnewwindow(CCmdUI* pCmdUI);
	afx_msg void OnOptionsBrowsemodesDefault();
	afx_msg void OnUpdateOptionsBrowsemodesDefault(CCmdUI* pCmdUI);
	afx_msg void OnOptionsBrowsemodesNews();
	afx_msg void OnUpdateOptionsBrowsemodesNews(CCmdUI* pCmdUI);
	afx_msg void OnOptionsBrowsemodesStandard();
	afx_msg void OnUpdateOptionsBrowsemodesStandard(CCmdUI* pCmdUI);
	afx_msg void OnViewExplorebarResource();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnEditUndoclose();
	afx_msg void OnUpdateEditUndoclose(CCmdUI* pCmdUI);
	afx_msg void OnEditEdit();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnOrggroup();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnUpdateWindowMin(CCmdUI* pCmdUI);
	afx_msg void OnNewwindow();
	afx_msg void OnNewwindowFav();
	afx_msg void OnUpdateNewwindowFav(CCmdUI* pCmdUI);
	afx_msg void OnNewwindowHistory();
	afx_msg void OnUpdateNewwindowHistory(CCmdUI* pCmdUI);
	afx_msg void OnNewwindowFile();
	afx_msg void OnUpdateNewwindowFile(CCmdUI* pCmdUI);
	afx_msg void OnFileSendpagebyemail();
	afx_msg void OnFileQuicksave();
	afx_msg void OnFileSendlinkbyemail();
	afx_msg void OnToolsSelbg();
	afx_msg void OnToolsSetbg();
	afx_msg void OnOptionsExportproxylist();
	afx_msg void OnOptionsImportproxylist();
	afx_msg void OnToolsMem();
	afx_msg void OnUpdateToolsMem(CCmdUI* pCmdUI);
	afx_msg void OnRtabAutoref();
	afx_msg void OnUpdateRtabAutoref(CCmdUI* pCmdUI);
	afx_msg void OnRtabReffre();
	afx_msg void OnViewLock();
	afx_msg void OnToolsAllBk();
	afx_msg void OnToolsQuicksearch();
	afx_msg void OnToolsSync();
	afx_msg void OnScFillform();
	afx_msg void OnScSaveform();
	afx_msg void OnScSavegenform();
	afx_msg void OnToolsCollect();
	afx_msg void OnToolsCleanaddr();
	afx_msg void OnAllPageLinks();
	afx_msg void OnToolsAutosetbg();
	afx_msg void OnUpdateToolsAutosetbg(CCmdUI* pCmdUI);
	afx_msg void OnViewEncodingAuto();
	afx_msg void OnUpdateViewEncodingAuto(CCmdUI* pCmdUI);
	afx_msg void OnSaveasGroup();
	afx_msg void OnGroup();
	afx_msg void OnUpdateFileOpenf(CCmdUI* pCmdUI);
	afx_msg void OnToolCleanitreesurfform();
	afx_msg void OnRtabAddalias();
	afx_msg void OnRtabAddsc();
	afx_msg void OnRtabAutosave();
	afx_msg void OnUpdateOptionsiTreeSurfgohome(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsiTreeSurfstartupOpenblankpage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsiTreeSurfstartupOpenfavoritepages(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsiTreeSurfstartupOpenmostfavoritepages(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsiTreeSurfstartupOpenstartupgroup(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsiTreeSurfstartupResumelastvisitedpages(CCmdUI* pCmdUI);
	afx_msg void OnToolsMouseScr();
	afx_msg void OnUpdateToolsMouseScr(CCmdUI* pCmdUI);
	afx_msg void OnToolsUrlFilter();
	afx_msg void OnUpdateToolsUrlFilter(CCmdUI* pCmdUI);
	afx_msg void OnOptionsiTreeSurfstartupNone();
	afx_msg void OnUpdateOptionsiTreeSurfstartupNone(CCmdUI* pCmdUI);
	afx_msg void OnToolsBp();
	afx_msg void OnUpdateToolsBp(CCmdUI* pCmdUI);
	afx_msg void OnViewLocktoolbar();
	afx_msg void OnUpdateViewLocktoolbar(CCmdUI* pCmdUI);
	afx_msg void OnToolsAutoscrollAll();
	afx_msg void OnUpdateToolsAutoscrollAll(CCmdUI* pCmdUI);
	afx_msg BOOL OnQueryEndSession();
	//}}AFX_MSG
	afx_msg void OnNewTab();
	afx_msg void OnUpdateNewTab(CCmdUI* pCmdUI);
	afx_msg void OnViewWinCust(UINT nID);
	afx_msg void OnUpdateViewWinCust(CCmdUI* pCmdUI);
	afx_msg void OnNewAddress();
	afx_msg void DoNothing();
	afx_msg void OnNewAddressEnter();
	afx_msg void OnFavorite(UINT nID);
	afx_msg void OnDropDown(NMHDR* pNotifyStruct, LRESULT* pResult);
	void OnUpdateProgress(CCmdUI* pCmdUI);
	void OnUpdateWins(CCmdUI* pCmdUI);
	void OnUpdateIP(CCmdUI* pCmdUI);
	afx_msg void OnSelChange(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnRClick(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnEncoding(UINT nID);
	afx_msg void OnFonts(UINT nID);
	afx_msg void OnUtils(UINT nID);
	afx_msg void OnShellOpen(WPARAM wParam,LPARAM);
	afx_msg void OnViewFavorite();
	afx_msg void OnLinkPopup();
	afx_msg void OnFavPopup();
	afx_msg void OnUpdateFav(WPARAM wParam,LPARAM);
	afx_msg void OnUpdateTab(WPARAM wParam,LPARAM);
	afx_msg void OnDelayLoadConfig(WPARAM wParam,LPARAM);
	afx_msg void OnAllFav(UINT nID);
	afx_msg void OnUpdateTabTips(WPARAM wParam,LPARAM);
	afx_msg void OnTabNeedText(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnUpdateToolbar(WPARAM wParam,LPARAM);
	void OnUpdateBP(CCmdUI* pCmdUI);
	void OnUpdateRES(CCmdUI* pCmdUI);
	void OnUpdateSecu(CCmdUI* pCmdUI);
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	afx_msg void OnSwitchWins(UINT nID);
	afx_msg void OnRtabRefresh();
	afx_msg void OnRtabStop();
	afx_msg void OnActivateWindow(WPARAM wParam, LPARAM lParam);
	afx_msg void OnHotKey(WPARAM wParam,LPARAM);
	afx_msg void OnScRll(WPARAM wParam, LPARAM lParam);
	afx_msg void OnToolsAutoscrollSpeed(UINT nID);
	afx_msg void OnViewStop();
	afx_msg void OnViewRefresh();
	afx_msg void OnFilePrintpreview();
	afx_msg void OnFilePrint();
	afx_msg void OnOpenGroup(UINT nID);
	afx_msg void OnAddtoGroup(UINT nID);
	afx_msg void OnWindowList(UINT nID);
	afx_msg void OnUndoList(UINT nID);
	afx_msg void OnAddThisFav(UINT nID);

	DECLARE_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined( _ITREESURF__MAINFRM_H__INCLUDED_ )
