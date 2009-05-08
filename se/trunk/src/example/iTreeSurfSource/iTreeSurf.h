// iTreeSurf.h : main header file for the iTreeSurf application
//

#if !defined( _ITREESURF__iTreeSurf_H__INCLUDED_ )
#define _ITREESURF__iTreeSurf_H__INCLUDED_

#if _MSC_VER > 1000         
#pragma once                    
#endif // _MSC_VER > 1000                  

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "iTreeSurf_i.h"

#define WM_USER_SHELL_OPEN (WM_USER + 0x1000) 
#define WM_UPDATE_FAV	(WM_USER+0x1001)
#define WM_UPDATE_TAB	(WM_USER + 0x1002)
#define WM_UPDATE_TAB_TIP (WM_USER + 0x1003)
#define WM_UPDATE_TOOLBAR	(WM_USER + 0x1004)
#define WM_ACTIVATE_WINDOW	(WM_USER + 0x1005)
#define WM_DELAYLOAD_CONFIG	(WM_USER + 0x1006)
#define WM_SEL_TAB	(WM_USER + 0x1007)

#include "LanguageTable.h"
#include "ConfigTable.h"
#include "GlobalSpace.h"


////////////////////////////////////////////////////////////////////////////
// CiTreeSurfApp:
// See iTreeSurf.cpp for the implementation of this class
//

//long LoadBytes();

class CiTreeSurfApp : public CWinApp
{
public:
	//2002-05-21
	CLanguageTable _languageTable;
	//2002-06-14
	CConfigTable _configFile;
	//2002-07-18
	CGlobalSpace _globalSpace;

	CString m_striTreeSurfRoot;
	CString m_strLinkListPath;

	bool _bAutoStart;
	int m_nAutoStart;
//	POSITION m_posAutoStart;
	int nDelay;
	CString m_strProfile;
	CString m_strSaveTxtFile;
//	BOOL m_hasEmptyDoc;
	CiTreeSurfApp();
	void SaveConfg();
	bool SetMainFrameAccordingToConfig();

	class CImpIDispatch* m_pDispOM; 
	class CImpIDropTarget* m_pDropTarget;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CiTreeSurfApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CiTreeSurfApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bATLInited;
	BOOL InitATL();
	BOOL ThreadOnIdle(LONG lCount);

	//2002-05-21
	void LoadResourceDLL();
	HINSTANCE m_hResourceDll;

};

extern CiTreeSurfApp theApp; 



////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined( _ITREESURF__iTreeSurf_H__INCLUDED_ )
