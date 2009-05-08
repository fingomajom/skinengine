// AdvancedProp.cpp : implementation file
//

#include "stdafx.h"
#include "iTreeSurf.h"
#include "AdvancedProp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvancedProp property page

IMPLEMENT_DYNCREATE(CAdvancedProp, CPropertyPage)

CAdvancedProp::CAdvancedProp() : CPropertyPage(CAdvancedProp::IDD)
{
	//{{AFX_DATA_INIT(CAdvancedProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAdvancedProp::~CAdvancedProp()
{
}

void CAdvancedProp::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvancedProp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvancedProp, CPropertyPage)
	//{{AFX_MSG_MAP(CAdvancedProp)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvancedProp message handlers
