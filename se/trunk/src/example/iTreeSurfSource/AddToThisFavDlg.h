#if !defined( _ITREESURF__ADDTOTHISFAVDLG_H__INCLUDED_ )
#define _ITREESURF__ADDTOTHISFAVDLG_H__INCLUDED_

#if _MSC_VER > 1000         
#pragma once                    
#endif // _MSC_VER > 1000                  
// AddToThisFavDlg.h : header file     
//     

////////////////////////////////////////////////////////////////////////////
// CAddToThisFavDlg dialog

class CAddToThisFavDlg : public CDialog
{
// Construction
public:
	CAddToThisFavDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddToThisFavDlg)
	enum { IDD = IDD_ADDURLHERE };
	CString	m_strFolder;
	CString	m_strName;
	CString	m_strURL;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddToThisFavDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddToThisFavDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined( _ITREESURF__ADDTOTHISFAVDLG_H__INCLUDED_ )
