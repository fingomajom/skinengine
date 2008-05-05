// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <atlpath.h>

#include "../../publish/kismoduleiddef.h"
#include "../kissctl/kissctl.h"
#include "../../import/kis_com_s/kis_esc/KComInterfacePtr.h"

class CNotifyCallback : 
    public CComObjectRootEx<CComSingleThreadModel>,
    public INotifyCallback
{
public:
    BEGIN_COM_MAP(CNotifyCallback)
        COM_INTERFACE_ENTRY(INotifyCallback)
    END_COM_MAP()

    HRESULT STDMETHODCALLTYPE NotifyMessage( 
    /* [in] */ ULONG uDesCallerId,
    /* [in] */ ULONG uSrcModuleId,
    /* [in] */ BSTR bstrFunctionName,
    /* [in] */ BSTR bstrParameter,
    /* [out] */ BSTR *bstrResult)
    {
        return S_OK;
    }


};

class CMainDlg : 
    public CDialogImpl<CMainDlg>
{
public:

	enum { IDD = IDD_MAINDLG };

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()

    KComInterfacePtr<ISvrCaller, &__uuidof(SvrCaller)> m_spSvrCaller;
    

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)


	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// center the dialog on the screen
		CenterWindow();

		// set icons
		HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
			IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
		SetIcon(hIcon, TRUE);
		HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
			IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
		
        SetIcon(hIconSmall, FALSE);

        CPath pathLoadDll;

        GetModuleFileName(NULL, pathLoadDll.m_strPath.GetBuffer(MAX_PATH), MAX_PATH );

        pathLoadDll.RemoveFileSpec();
        pathLoadDll.RemoveFileSpec();
        pathLoadDll.RemoveFileSpec();
        pathLoadDll.Append(_T("kissctl\\Debug\\kissctl.dll"));

        m_spSvrCaller.LoadInterface(pathLoadDll);

        HRESULT hr = m_spSvrCaller->Initialize( CALLERID_KIS, NULL );

        CComVariant var0 = _T("var0");
        CComVariant var1 = _T("var1");
        
        BSTR bstr = var0.bstrVal;

        var0.Clear();

        CComVariant var3 = _T("var3");
        CComVariant var4 = _T("var4");

        if (FAILED(hr))
        {   
            m_spSvrCaller.Free();
        }
        else
        {
            CComObject<CNotifyCallback>* piCallback;

            CComObject<CNotifyCallback>::CreateInstance(&piCallback);

            m_spSvrCaller->RegisterCallback( piCallback );
        }

        if (SUCCEEDED(hr))
        {
            _bstr_t bstrOut;
            _bstr_t bstrOut1;

            m_spSvrCaller->CallSvrFunc( 
                CALLERID_KIS, 
                _bstr_t(_T("123123123")),
                NULL,
                bstrOut.GetAddress()
                );

            m_spSvrCaller->CallSvrFunc( 
                CALLERID_KIS, 
                _bstr_t(_T("44444444444")),
                NULL,
                bstrOut1.GetAddress()
                );

           
            BSTR bstrOut2 = NULL;
        }

		return TRUE;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CSimpleDialog<IDD_ABOUTBOX, FALSE> dlg;
		dlg.DoModal();
		return 0;
	}

	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// TODO: Add validation code 
		//EndDialog(wID);
		return 0;
	}

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
        if (m_spSvrCaller != NULL)
            m_spSvrCaller->Uninitialize();

		EndDialog(wID);
		return 0;
	}
};
