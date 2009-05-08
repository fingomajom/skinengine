#if !defined( _ITREESURF__SHELLTREE_H__INCLUDED_ )
#define _ITREESURF__SHELLTREE_H__INCLUDED_

#if _MSC_VER >= 1000         
#pragma once                    
#endif // _MSC_VER >= 1000                  
// ShellTree.h : header file     
//     

#include "BaseExpObj.h"
#include "ShellWnd.h"


//work threads
UINT _cdecl InitTree(LPVOID Tree);


////////////////////////////////////////////////////////////////////////////
// CShellTree window

class CShellTree : public CBaseExpObj
{
// Construction
public:
	CShellTree();
	CShellWnd m_wndShell;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShellTree)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL Update(int nLevel);
	virtual void Destroy();
	LPCSTR GetTitle();
	virtual ~CShellTree();
	CString strTitle;
	virtual BOOL Initialize();
	virtual BOOL Create(CWnd* pParent);
	virtual CWnd* GetWindow();

	// Generated message map functions
protected:
	//{{AFX_MSG(CShellTree)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

};

////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined( _ITREESURF__SHELLTREE_H__INCLUDED_ )
