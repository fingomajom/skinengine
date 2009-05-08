#if !defined( _ITREESURF__POPUPDLG_H__INCLUDED_ )
#define _ITREESURF__POPUPDLG_H__INCLUDED_

#if _MSC_VER > 1000         
#pragma once                    
#endif // _MSC_VER > 1000                  
// PopupDlg.h : header file     
//     

////////////////////////////////////////////////////////////////////////////
// CPopupDlg dialog

class CPopupDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CPopupDlg)
// Construction
public:
	CPopupDlg();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPopupDlg)
	enum { IDD = IDD_POPUPLIST };
	CListCtrl	m_PopupList;
	BOOL	m_EnableFilter;
	BOOL	m_EnableDupFilter;
	BOOL	m_EnableAutoFilter;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPopupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPopupDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDelete();
	afx_msg void OnEdit();
	afx_msg void OnInsert();
	afx_msg void OnEndlabeleditPopuplist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkPopuplist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined( _ITREESURF__POPUPDLG_H__INCLUDED_ )
