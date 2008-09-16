// lsetest.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "lsetest.h"
#include "lsetestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#include "..\lsesvr\ModuleConfig.h"

// ClsetestApp

BEGIN_MESSAGE_MAP(ClsetestApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()



LONG WINAPI LSEUnhandledExceptionFilter(
    __in struct _EXCEPTION_POINTERS *ExceptionInfo )
{
    MessageBox( NULL, _T("LSEUnhandledExceptionFilter"), _T("Error"), MB_OK );

    return EXCEPTION_EXECUTE_HANDLER;
}

// ClsetestApp ����

ClsetestApp::ClsetestApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��

    //CModuleConfigReg config;

    ////config.LoadConfig();

    //Module_Config_Info info = { 1, 1, L"agccc", L"aa234234234" }; 

    //config.SetModuleConfig( info );
    //
    //config.LoadConfig();
}


// Ψһ��һ�� ClsetestApp ����

ClsetestApp theApp;


// ClsetestApp ��ʼ��

BOOL ClsetestApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

    CoInitialize( 0 );
    HRESULT    hr = CoInitializeSecurity( NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IDENTIFY, NULL, EOAC_NONE, NULL );
    ATLASSERT(SUCCEEDED(hr));

    SetUnhandledExceptionFilter( LSEUnhandledExceptionFilter ); 


	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	ClsetestDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˴����ô����ʱ�á�ȷ�������ر�
		//  �Ի���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ�á�ȡ�������ر�
		//  �Ի���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}
