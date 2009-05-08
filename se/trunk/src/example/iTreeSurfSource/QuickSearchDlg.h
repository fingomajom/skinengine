#if !defined( _ITREESURF__QUICKSEARCHDLG_H__INCLUDED_ )
#define _ITREESURF__QUICKSEARCHDLG_H__INCLUDED_

#if _MSC_VER > 1000         
#pragma once                    
#endif // _MSC_VER > 1000                  
// QuickSearchDlg.h : header file     
//     

////////////////////////////////////////////////////////////////////////////
// CQuickSearchDlg dialog

class CQuickSearchDlg : public CDialog
{
// Construction
public:
	CQuickSearchDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CQuickSearchDlg)
	enum { IDD = IDD_QUICKSEARCH };
	CEdit	m_cSearchEngine;
	CEdit	m_cName;
	CListCtrl	m_EngineList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuickSearchDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CQuickSearchDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnUpdate();
	afx_msg void OnInsert();
	afx_msg void OnDelete();
	afx_msg void OnItemchangedSeachlist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bUpdating;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined( _ITREESURF__QUICKSEARCHDLG_H__INCLUDED_ )
