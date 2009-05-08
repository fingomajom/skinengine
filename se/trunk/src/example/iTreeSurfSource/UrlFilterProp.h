#if !defined( _ITREESURF__URLFILTERPROP_H__INCLUDED_ )
#define _ITREESURF__URLFILTERPROP_H__INCLUDED_

#if _MSC_VER > 1000         
#pragma once                    
#endif // _MSC_VER > 1000                  
// UrlFilterProp.h : header file     
//     


////////////////////////////////////////////////////////////////////////////
// CUrlFilterProp dialog

class CUrlFilterProp : public CPropertyPage
{
	DECLARE_DYNCREATE(CUrlFilterProp)

// Construction
public:
	CUrlFilterProp();
	~CUrlFilterProp();

// Dialog Data
	//{{AFX_DATA(CUrlFilterProp)
	enum { IDD = IDD_URL_FILTER };
	CListCtrl	m_cURLList;
	BOOL	m_bUseUrlFilter;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CUrlFilterProp)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CUrlFilterProp)
	afx_msg void OnInsert();
	afx_msg void OnEdit();
	afx_msg void OnDelete();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkUrlList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditUrlList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined( _ITREESURF__URLFILTERPROP_H__INCLUDED_ )
