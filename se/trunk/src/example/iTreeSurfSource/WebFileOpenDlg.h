#if !defined( _ITREESURF__WEBFILEOPENDLG_H__INCLUDED_ )
#define _ITREESURF__WEBFILEOPENDLG_H__INCLUDED_

#if _MSC_VER >= 1000         
#pragma once                    
#endif // _MSC_VER >= 1000                  
// WebFileOpenDlg.h : header file     
//     

////////////////////////////////////////////////////////////////////////////
// CWebFileOpenDlg dialog

class CWebFileOpenDlg : public CDialog
{
// Construction
public:
	CWebFileOpenDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWebFileOpenDlg)
	enum { IDD = IDD_FILEOPEN };
	CComboBoxEx	m_conAddress;
	CString	m_strAddress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWebFileOpenDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWebFileOpenDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined( _ITREESURF__WEBFILEOPENDLG_H__INCLUDED_ )
