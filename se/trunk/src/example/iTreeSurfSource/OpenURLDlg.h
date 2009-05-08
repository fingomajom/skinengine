#if !defined( _ITREESURF__OPENURLDLG_H__INCLUDED_ )
#define _ITREESURF__OPENURLDLG_H__INCLUDED_

#if _MSC_VER >= 1000         
#pragma once                    
#endif // _MSC_VER >= 1000                  
// OpenURLDlg.h : header file     
//     

////////////////////////////////////////////////////////////////////////////
// COpenURLDlg dialog

class COpenURLDlg : public CDialog
{
// Construction
public:
	COpenURLDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COpenURLDlg)
	enum { IDD = IDD_OPEN };
	CString	m_strURL;
	BOOL	m_bInNewWin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenURLDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COpenURLDlg)
	afx_msg void OnBrowser();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined( _ITREESURF__OPENURLDLG_H__INCLUDED_ )
