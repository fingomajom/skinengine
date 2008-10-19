// EditRuleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "mfckpstest.h"
#include "EditRuleDlg.h"


// CEditRuleDlg dialog

IMPLEMENT_DYNAMIC(CEditRuleDlg, CDialog)

CEditRuleDlg::CEditRuleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditRuleDlg::IDD, pParent)
    , m_strTarget(_T(""))
    , m_ctype(0)
{
    memset( &m_RuleInfo, 0, sizeof(m_RuleInfo));
}

CEditRuleDlg::~CEditRuleDlg()
{
}

void CEditRuleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_C_EDIT, m_strTarget);
    DDX_Radio(pDX, IDC_CT_RADIO, m_ctype);
}


BEGIN_MESSAGE_MAP(CEditRuleDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CEditRuleDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDC_B_BUTTON, &CEditRuleDlg::OnBnClickedBButton)
END_MESSAGE_MAP()


// CEditRuleDlg message handlers

BOOL CEditRuleDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_ctype = m_RuleInfo.uContentType;

    if ( m_ctype == CT_ID )
        m_strTarget.Format( _T("%d"), m_RuleInfo.uProcessId );
    else
        m_strTarget = m_RuleInfo.wszFileName;

    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
}


void CEditRuleDlg::OnBnClickedOk()
{
    UpdateData(TRUE);


    m_RuleInfo.uContentType = m_ctype;

    if ( m_ctype == CT_ID )
    {
        m_RuleInfo.uProcessId = _wtol(m_strTarget);

        if ( m_RuleInfo.uProcessId <= 0 )
            return;
    }
    else
    {
        if (m_strTarget.GetLength() <= 0)
            return;

        wcsncpy_s( m_RuleInfo.wszFileName, m_strTarget, MAX_PATH);
    }



    OnOK();
}

void CEditRuleDlg::OnBnClickedBButton()
{
    DWORD   dwFlag =  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

    TCHAR szOpenTitle[MAX_PATH] = L"打开文件";
    TCHAR szFilter   [MAX_PATH] = L"可执行文件(*.dll;*.exe)|*.dll;*.exe|可执行文件(*.exe)|*.exe|动态链接库(*.dll)|*.dll||";

    CFileDialog openDlg(TRUE, NULL, NULL, dwFlag, szFilter, this);


    if (IDOK == openDlg.DoModal())
    {
        m_strTarget = openDlg.GetPathName();

        UpdateData(FALSE);

        m_strTarget.MakeLower();

        UpdateData(FALSE);
    }
}
