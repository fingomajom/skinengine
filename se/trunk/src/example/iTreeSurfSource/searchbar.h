#if !defined( _ITREESURF__SEARCHBAR_H__INCLUDED_ )
#define _ITREESURF__SEARCHBAR_H__INCLUDED_

#if _MSC_VER >= 1000         
#pragma once                    
#endif // _MSC_VER >= 1000                  
//  : header file     
//     

#include "BaseExpObj.h"
#include "HtmlWnd.h"

////////////////////////////////////////////////////////////////////////////
// CHistoryTree window

class CSearchbar : public CBaseExpObj
{
// Construction
public:
	CSearchbar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHistoryTree)
	//}}AFX_VIRTUAL
	CHtmlWnd m_wndSearch;

// Implementation
public:
	BOOL Update(int nLevel);
	virtual void Destroy();
	LPCSTR GetTitle();
	virtual ~CSearchbar();
	CString strTitle;
	virtual BOOL Initialize();
	virtual BOOL Create(CWnd* pParent);
	virtual CWnd* GetWindow();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSearchbar)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

};

////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined( _ITREESURF__SEARCHBAR_H__INCLUDED_ )
