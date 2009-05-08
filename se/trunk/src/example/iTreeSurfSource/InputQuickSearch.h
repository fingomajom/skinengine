#if !defined( _ITREESURF__INPUTQUICKSEARCH_H__INCLUDED_ )
#define _ITREESURF__INPUTQUICKSEARCH_H__INCLUDED_

#if _MSC_VER > 1000         
#pragma once                    
#endif // _MSC_VER > 1000                  
// InputQuickSearch.h : header file     
//     

////////////////////////////////////////////////////////////////////////////
// CInputQuickSearch dialog

class CInputQuickSearch : public CDialog
{
// Construction
public:
	CInputQuickSearch(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInputQuickSearch)
	enum { IDD = IDD_INPUT_QUICKSEARCH };
	CEdit	m_cEngine;
	CEdit	m_cName;
	CString	m_strName;
	CString	m_strEngine;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputQuickSearch)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputQuickSearch)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined( _ITREESURF__INPUTQUICKSEARCH_H__INCLUDED_ )
