#if !defined( _ITREESURF__HISTREECTRL_H__INCLUDED_ )
#define _ITREESURF__HISTREECTRL_H__INCLUDED_

#if _MSC_VER > 1000         
#pragma once                    
#endif // _MSC_VER > 1000                  
// HisTreeCtrl.h : header file     
//     
#include "iTreeSurfHelper.h"
#include "BaseExpObj.h"

////////////////////////////////////////////////////////////////////////////
// CHisTreeCtrl window

class CHisTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CHisTreeCtrl();

// Attributes
public:
	CStringArray m_astrHist;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHisTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
//	void GetItemPath(HTREEITEM hti, CString &path);
//	BOOL GetHistoryFolder(CString& strHis);
//	HTREEITEM m_hItem;
	LPITEMIDLIST FindFolder(LPCSTR foldername, IShellFolder* pFolder);
	CBaseExpObj* m_pHisTree;
	virtual ~CHisTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHisTreeCtrl)
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
//	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
//	afx_msg void OnHisfolderDelete();
//	afx_msg void OnHisurlDelete();
//	afx_msg void OnHisurlAddtofavorites();
//	afx_msg void OnHisurlProperties();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined( _ITREESURF__HISTREECTRL_H__INCLUDED_ )
