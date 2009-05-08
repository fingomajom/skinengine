// SetAliasDlg.cpp : implementation file
//

#include "stdafx.h"
#include "iTreeSurf.h"
#include "SetAliasDlg.h"

#include "InputAlias.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//#pragma optimize( "s", on )

////////////////////////////////////////////////////////////////////////////
// CSetAliasDlg dialog


CSetAliasDlg::CSetAliasDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetAliasDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetAliasDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSetAliasDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetAliasDlg)
	DDX_Control(pDX, IDC_LIST1, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetAliasDlg, CDialog)
	//{{AFX_MSG_MAP(CSetAliasDlg)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_UPDATE, OnUpdate)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////
// CSetAliasDlg message handlers

BOOL CSetAliasDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CRect rect;
	CString str;
	str.LoadString(IDS_ALIAS);
	m_list.InsertColumn(0,str);
	m_list.GetWindowRect(rect);
	m_list.SetColumnWidth(0, 60);
	m_list.InsertColumn(1,"URL");
	m_list.SetColumnWidth(1, rect.Width()-78);

	POSITION pos = theApp._configFile._mapAlias.GetStartPosition();
	CString alias, url;
	int i=0;
	while(pos!=NULL)
	{
		theApp._configFile._mapAlias.GetNextAssoc(pos, alias, url);
		m_list.InsertItem(i, alias);
		m_list.SetItemText(i, 1, url);
		i++;
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetAliasDlg::OnOK() 
{
	// TODO: Add extra validation here
	theApp._configFile._mapAlias.RemoveAll();
	int n = m_list.GetItemCount();
	CString alias;
	for( int i=0; i<n; i++)
	{
		alias = m_list.GetItemText(i,0);
		alias.TrimLeft(); alias.TrimRight();
		theApp._configFile._mapAlias.SetAt(alias,m_list.GetItemText(i,1));
	}
	
	CDialog::OnOK();
}

void CSetAliasDlg::OnNew() 
{
	// TODO: Add your control notification handler code here
	CInputAlias dlg;
	if(dlg.DoModal() == IDOK)
	{
		CString alias = dlg.m_alias;
		CString url = dlg.m_url;
		alias.TrimLeft();
		url.TrimLeft();
		int i = m_list.GetItemCount();
		if(alias !="" && !alias.IsEmpty() && url!="" && !url.IsEmpty())
		{
			m_list.InsertItem(i, alias);
			m_list.SetItemText(i, 1, url);
		}
	}
}

void CSetAliasDlg::OnUpdate() 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int i = m_list.GetNextSelectedItem(pos);
		CInputAlias dlg;
		dlg.m_alias = m_list.GetItemText(i,0);
		dlg.m_url = m_list.GetItemText(i,1);
		if(dlg.DoModal() == IDOK)
		{
			CString alias = dlg.m_alias;
			CString url = dlg.m_url;
			alias.TrimLeft();
			url.TrimLeft();
			if(alias !="" && !alias.IsEmpty() && url!="" && !url.IsEmpty())
			{
				m_list.SetItemText(i, 0, alias);
				m_list.SetItemText(i, 1, url);
			}
		}
	}
}

void CSetAliasDlg::OnDel() 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int i = m_list.GetNextSelectedItem(pos);
		m_list.DeleteItem(i);
	}	
}

//#pragma optimize( "s", off )
