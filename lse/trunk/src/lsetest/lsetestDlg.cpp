// lsetestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "lsetest.h"
#include "lsetestDlg.h"
#include "..\public\lsectl.h"
#include "..\public\kis_com_s\kis_esc\KComInterfacePtr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// ClsetestDlg �Ի���




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


// ClsetestDlg ��Ϣ�������

BOOL ClsetestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

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


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void ClsetestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
//
HCURSOR ClsetestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

