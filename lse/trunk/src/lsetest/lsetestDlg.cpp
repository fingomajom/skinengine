// lsetestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "lsetest.h"
#include "lsetestDlg.h"
#include "..\public\lsectl.h"
#include "..\public\kis_com_s\kis_esc\KComInterfacePtr.h"

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


// ClsetestDlg 对话框




ClsetestDlg::ClsetestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ClsetestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void ClsetestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(ClsetestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

class CLSECallback : public ILSECallback
{
public:

    STDMETHODIMP QueryInterface(const struct _GUID &iid,void ** ppv)
    {
        if ((iid) == __uuidof(ILSECallback)) 
        { 
            *ppv = static_cast<ILSECallback*>(this); 
            AddRef();
            return S_OK;
        }
        else if ((iid) == IID_IUnknown)
        {
            *ppv = static_cast<IUnknown*>(this); 
            AddRef();
            return S_OK;
        }

        return E_FAIL; 
    }

    ULONG __stdcall AddRef(void)
    {	return 1;	}

    ULONG __stdcall Release(void)
    {	return 0;	}	

    virtual HRESULT STDMETHODCALLTYPE CallbackFunction( 
        /* [in]  */ ULONG uTargetId,
        /* [in]  */ ULONG uCallerId,
        /* [in]  */ ULONG uFunctionId,
        /* [in]  */ IDataBuffer*  pParameter,
        /* [out] */ IDataBuffer** ppResult)
    {
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE OnQuit()
    {
        return S_OK;
    }

};

CLSECallback g_callback;


// ClsetestDlg 消息处理程序

BOOL ClsetestDlg::OnInitDialog()
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

    KComInterfacePtr<ILSECaller, &__uuidof(SvrCaller)> spSvrCaller;

    //spSvrCaller.LoadInterface( _T("D:\\lidengwang\\lse\\trunk\\src\\lsectl\\Debug\\lsectl.dll"));
    spSvrCaller.LoadInterface( _T("lsectl.dll"));
    
    IDataBuffer* piDataBuffer = LSECreateDataBuffer(10);
    IDataBuffer* piDataBufferOut = NULL ;

    ((BYTE*)piDataBuffer->GetDataBuffer())[0] = 0 ;
    ((BYTE*)piDataBuffer->GetDataBuffer())[1] = 1 ;
    ((BYTE*)piDataBuffer->GetDataBuffer())[2] = 2 ;
    ((BYTE*)piDataBuffer->GetDataBuffer())[3] = 3 ;
    ((BYTE*)piDataBuffer->GetDataBuffer())[4] = 4 ;
    ((BYTE*)piDataBuffer->GetDataBuffer())[5] = 5 ;
    ((BYTE*)piDataBuffer->GetDataBuffer())[6] = 6 ;



   // ATLASSERT(SUCCEEDED(hr));

    HRESULT hr = spSvrCaller->Initialize(1, &g_callback);

    MessageBox( SUCCEEDED(hr) ? _T("Ok") : _T("No"), _T("spSvrCaller->Initialize"));

    if ( FAILED(hr) )
        return 0L;

    DWORD dwLastTick = GetTickCount();

    for (int i = 0; i < 1; i++)
    {
        HRESULT hr = spSvrCaller->CallSvrFunc( 10, 0, piDataBuffer, &piDataBufferOut);

        MessageBox( SUCCEEDED(hr) ? _T("Ok") : _T("No"), _T("spSvrCaller->CallSvrFunc"));


        if (piDataBufferOut != NULL)
        {
            MessageBox( piDataBufferOut->GetBufferSize() == 5 ? _T("Ok") : _T("No"), _T("piDataBufferOut"));

            piDataBufferOut->AddRef();

            piDataBufferOut->Release();
        }

        DWORD dwLastError = GetLastError(); 

        dwLastError = 0;
    }
    

    dwLastTick = GetTickCount() - dwLastTick;

    spSvrCaller->Uninitialize();


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void ClsetestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void ClsetestDlg::OnPaint()
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
HCURSOR ClsetestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

