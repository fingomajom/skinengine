#if !defined( _ITREESURF__ADVANCEDPROP_H__INCLUDED_ )
#define _ITREESURF__ADVANCEDPROP_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdvancedProp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAdvancedProp dialog

class CAdvancedProp : public CPropertyPage
{
	DECLARE_DYNCREATE(CAdvancedProp)

// Construction
public:
	CAdvancedProp();
	~CAdvancedProp();

// Dialog Data
	//{{AFX_DATA(CAdvancedProp)
	enum { IDD = IDD_PROPPAGE_ADVANCED };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAdvancedProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAdvancedProp)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined( _ITREESURF__ADVANCEDPROP_H__INCLUDED_ )
