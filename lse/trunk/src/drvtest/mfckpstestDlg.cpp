// mfckpstestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "mfckpstest.h"
#include "mfckpstestDlg.h"
#include "CommonFunc.h"
#include "EditRuleDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CmfckpstestDlg 对话框




CmfckpstestDlg::CmfckpstestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CmfckpstestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CmfckpstestDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BLACK_LIST, m_black_rule_list);
    DDX_Control(pDX, IDC_WHITE_LIST, m_white_rule_list);
    DDX_Control(pDX, IDC_PROTECT_LIST, m_protect_rule_list);
    DDX_Control(pDX, IDC_LOG_LIST, m_log_list);
}

BEGIN_MESSAGE_MAP(CmfckpstestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_INSTALL, &CmfckpstestDlg::OnBnClickedInstall)
    ON_BN_CLICKED(IDC_UNINSTALL, &CmfckpstestDlg::OnBnClickedUninstall)
    ON_BN_CLICKED(IDC_ADD_PROTECT_FILE, &CmfckpstestDlg::OnBnClickedAddProtectFile)
    ON_BN_CLICKED(IDC_ADD_BLACK_FILE, &CmfckpstestDlg::OnBnClickedAddBlackFile)
    ON_BN_CLICKED(IDC_ADD_WHITE_FILE, &CmfckpstestDlg::OnBnClickedAddWhiteFile)
    ON_BN_CLICKED(IDC_CLEAR_BLACK_FILE, &CmfckpstestDlg::OnBnClickedClearBlackFile)
    ON_BN_CLICKED(IDC_CLEAR_WHITE_FILE, &CmfckpstestDlg::OnBnClickedClearWhiteFile)
    ON_BN_CLICKED(IDC_CLEAR_PROTECT_FILE, &CmfckpstestDlg::OnBnClickedClearProtectFile)
    ON_NOTIFY(NM_DBLCLK, IDC_BLACK_LIST, &CmfckpstestDlg::OnNMDblclkBlackList)
    ON_NOTIFY(NM_DBLCLK, IDC_WHITE_LIST, &CmfckpstestDlg::OnNMDblclkWhiteList)
    ON_NOTIFY(NM_DBLCLK, IDC_PROTECT_LIST, &CmfckpstestDlg::OnNMDblclkProtectList)
    ON_BN_CLICKED(IDC_SET_BLACK_FILE, &CmfckpstestDlg::OnBnClickedSetBlackFile)
    ON_BN_CLICKED(IDC_SET_WHITE_FILE, &CmfckpstestDlg::OnBnClickedSetWhiteFile)
    ON_BN_CLICKED(IDC_SET_PROTECT_FILE, &CmfckpstestDlg::OnBnClickedSetProtectFile)
END_MESSAGE_MAP()


// CmfckpstestDlg 消息处理程序

void CmfckpstestDlg::ReportLog( LP_DRIVER_EVENT_INFO EventInfo )
{
    CString strtmp;

    strtmp.Format(L"%d", EventInfo->uEventType);
    int idx = m_log_list.InsertItem( 0xFFFFFF, strtmp);


    strtmp.Format(L"%d", EventInfo->uSrcPID);
    m_log_list.SetItemText(idx, 1, strtmp);
    m_log_list.SetItemText(idx, 2, EventInfo->wszSrcFileName);
    strtmp.Format(L"%d", EventInfo->uTagPID);
    m_log_list.SetItemText(idx, 3, strtmp);
    m_log_list.SetItemText(idx, 4, EventInfo->wszTagFileName);
}


BOOL CmfckpstestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标


    InitRuleList(m_black_rule_list);
    InitRuleList(m_white_rule_list);
    InitRuleList(m_protect_rule_list);

    m_log_list.InsertColumn(0, _T("Type")   , 0, 60);
    m_log_list.InsertColumn(1, _T("SrcPID") , 0, 60);
    m_log_list.InsertColumn(2, _T("SrcFile"), 0, 300);
    m_log_list.InsertColumn(3, _T("TagPID") , 0, 60);
    m_log_list.InsertColumn(4, _T("TagFile"), 0, 300);



    if ( !m_devc.InitDevC() )
        AfxMessageBox(_T("打开驱动设置错误。。。"));

    m_devc.SetCallback(this);



	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CmfckpstestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CmfckpstestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CmfckpstestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LPCTSTR DriverName = PTTDRV_DEVICE_NAME;

void CmfckpstestDlg::OnBnClickedInstall()
{
    BOOL bResult = FALSE;
    
    
    bResult = InstallDriver( DriverName );

    bResult = StartDriver( DriverName );


    MessageBox( bResult ? _T("Install Successed") : _T("Install Failed"), _T("Install"), MB_OK);
}

void CmfckpstestDlg::OnBnClickedUninstall()
{
    BOOL bResult = FALSE;

    bResult = StopDriver( DriverName );

    bResult = RemoveDriver( DriverName );

    MessageBox( bResult ? _T("Uninstall Successed") : _T("Uninstall Failed"), _T("Install"), MB_OK);

}


void CmfckpstestDlg::InitRuleList(CListCtrl& ListCtrl)
{
    ListCtrl.InsertColumn(0, _T("Type"), 0, 50);
    ListCtrl.InsertColumn(1, _T("Content"), 0, 215);

    ListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT );
}

void CmfckpstestDlg::UpdateRuleList( CListCtrl& ListCtrl, DRIVER_RULE_INFO& RuleInfo, int idx )
{
    CString strType;
    CString strContent;

    switch( RuleInfo.uContentType )
    {
    case CT_ID:
        
        strType = L"ID";

        break;

    case CT_NAME:

        strType = L"NAME";

        break;

    case CT_PATH:

        strType = L"PATH";

        break;

    case CT_PATHFILE:

        strType = L"PATHFILE";

        break;

    default:

        strType = L"NONE";

    }

    if ( RuleInfo.uContentType == CT_ID )
        strContent.Format( _T("%d"), RuleInfo.uProcessId );
    else
        strContent = RuleInfo.wszFileName;


    ListCtrl.SetItemText( idx, 0, strType);
    ListCtrl.SetItemText( idx, 1, strContent);
}



void CmfckpstestDlg::AddRule( CListCtrl& ListCtrl, std::vector<DRIVER_RULE_INFO>& vtRule )
{
    CEditRuleDlg dlg;

    if ( dlg.DoModal() == IDOK )
    {
        vtRule.push_back( dlg.m_RuleInfo );

        int idx = ListCtrl.InsertItem( 0xFFFF, L"" );

        UpdateRuleList( ListCtrl, dlg.m_RuleInfo, idx);
    }

}

void CmfckpstestDlg::DelRule( CListCtrl& ListCtrl, std::vector<DRIVER_RULE_INFO>& vtRule , int idx )
{
    ListCtrl.DeleteItem( idx );
    vtRule.erase( vtRule.begin() + idx );

    if ( idx >= ListCtrl.GetItemCount() )
        ListCtrl.SetItemState( idx - 1, LVIS_SELECTED, LVIS_SELECTED);
    else
        ListCtrl.SetItemState( idx, LVIS_SELECTED, LVIS_SELECTED);

}

void CmfckpstestDlg::EditRule( CListCtrl& ListCtrl, std::vector<DRIVER_RULE_INFO>& vtRule , int idx )
{

    CEditRuleDlg dlg;

    dlg.m_RuleInfo = vtRule[idx];

    if ( dlg.DoModal() == IDOK )
    {
        vtRule[idx] = dlg.m_RuleInfo;

        UpdateRuleList( ListCtrl, dlg.m_RuleInfo, idx);
    }

}


void CmfckpstestDlg::OnBnClickedAddBlackFile()
{
    AddRule( m_black_rule_list, m_vtblack_rule_list );
}

void CmfckpstestDlg::OnBnClickedAddWhiteFile()
{
    AddRule( m_white_rule_list, m_vtwhite_rule_list );
}

void CmfckpstestDlg::OnBnClickedAddProtectFile()
{
    AddRule( m_protect_rule_list, m_vtprotect_rule_list );
}


void CmfckpstestDlg::OnBnClickedClearBlackFile()
{
    int idx = m_black_rule_list.GetNextItem( -1, LVIS_SELECTED );
    if (idx < 0)
        return;

    DelRule( m_black_rule_list, m_vtblack_rule_list, idx);
}

void CmfckpstestDlg::OnBnClickedClearWhiteFile()
{
    int idx = m_white_rule_list.GetNextItem( -1, LVIS_SELECTED );
    if (idx < 0)
        return;

    DelRule( m_white_rule_list, m_vtwhite_rule_list, idx);
}

void CmfckpstestDlg::OnBnClickedClearProtectFile()
{
    int idx = m_protect_rule_list.GetNextItem( -1, LVIS_SELECTED );
    if (idx < 0)
        return;

    DelRule( m_protect_rule_list, m_vtprotect_rule_list, idx);
}

void CmfckpstestDlg::OnNMDblclkBlackList(NMHDR *pNMHDR, LRESULT *pResult)
{

    int idx = m_black_rule_list.GetNextItem( -1, LVIS_SELECTED );
    if (idx < 0)
        return;
    
    EditRule( m_black_rule_list, m_vtblack_rule_list, idx);

    *pResult = 0;
}

void CmfckpstestDlg::OnNMDblclkWhiteList(NMHDR *pNMHDR, LRESULT *pResult)
{

    int idx = m_white_rule_list.GetNextItem( -1, LVIS_SELECTED );
    if (idx < 0)
        return;

    EditRule( m_white_rule_list, m_vtwhite_rule_list, idx);

    *pResult = 0;
}

void CmfckpstestDlg::OnNMDblclkProtectList(NMHDR *pNMHDR, LRESULT *pResult)
{
    int idx = m_protect_rule_list.GetNextItem( -1, LVIS_SELECTED );
    if (idx < 0)
        return;

    EditRule( m_protect_rule_list, m_vtprotect_rule_list, idx);

    *pResult = 0;
}

void CmfckpstestDlg::OnBnClickedSetBlackFile()
{
    if ( !m_devc.ClearRule( RT_BLACKRULE) )
        return;

    for (size_t idx = 0; idx < m_vtblack_rule_list.size(); idx++)
    {
        DRIVER_RULE_INFO RuleInfo = m_vtblack_rule_list[idx];

        RuleInfo.uRuleType = RT_BLACKRULE;
        RuleInfo.uEnable = TRUE;

        if ( !m_devc.AppendRule(&RuleInfo) )
            return;
    }

    AfxMessageBox(_T("重置成功！！"));
}

void CmfckpstestDlg::OnBnClickedSetWhiteFile()
{
    if ( !m_devc.ClearRule( RT_WHITERULE) )
        return;

    for (size_t idx = 0; idx < m_vtwhite_rule_list.size(); idx++)
    {
        DRIVER_RULE_INFO RuleInfo = m_vtwhite_rule_list[idx];

        RuleInfo.uRuleType = RT_WHITERULE;
        RuleInfo.uEnable = TRUE;

        if ( !m_devc.AppendRule(&RuleInfo) )
            return;
    }

    AfxMessageBox(_T("重置成功！！"));
}

void CmfckpstestDlg::OnBnClickedSetProtectFile()
{
    if ( !m_devc.ClearRule( RT_PROTECTRULE) )
        return;

    for (size_t idx = 0; idx < m_vtprotect_rule_list.size(); idx++)
    {
        DRIVER_RULE_INFO RuleInfo = m_vtprotect_rule_list[idx];

        RuleInfo.uRuleType = RT_PROTECTRULE;
        RuleInfo.uEnable = TRUE;

        if ( !m_devc.AppendRule(&RuleInfo) )
            return;
    }

    AfxMessageBox(_T("重置成功！！"));
}
