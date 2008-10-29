// ConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "mfckpstest.h"
#include "ConfigDlg.h"


// CConfigDlg dialog

IMPLEMENT_DYNAMIC(CConfigDlg, CDialog)

CConfigDlg::CConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDlg::IDD, pParent)
    , m_blog(FALSE)
    , m_bblock(FALSE)
    , m_bhide(FALSE)
{

}

CConfigDlg::~CConfigDlg()
{
}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Check(pDX, IDC_LOG_CHECK, m_blog);
    DDX_Check(pDX, IDC_UN_CHECK, m_bblock);
    DDX_Check(pDX, IDC_HIDE_CHECK, m_bhide);
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)
END_MESSAGE_MAP()


// CConfigDlg message handlers

BOOL CConfigDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_cfg.uiMonSwitch = 0xFFFFFFFF;
    
    m_blog   = m_cfg.usEnableReportLog;
    m_bblock = m_cfg.usEnableBlockUnknown;
    m_bhide  = m_cfg.usEnableHideProcess;

    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigDlg::OnOK()
{
    UpdateData(TRUE);

    m_cfg.uiMonSwitch           = 0xFFFFFFFF;
    m_cfg.usEnableReportLog     = m_blog;
    m_cfg.usEnableBlockUnknown  = m_bblock;
    m_cfg.usEnableHideProcess   = m_bhide;


    CDialog::OnOK();
}
