// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../../comExe.h"

class DECLSPEC_UUID("902ED49C-E3EF-4930-B50B-9E6A8BA5A671")
    SvrObject;

class CMainDlg : 
    public CDialogImpl<CMainDlg>,
    public IDispEventSimpleImpl<1, CMainDlg, &__uuidof(IDispatch)>
{
public:

    CComPtr<IObjTest> m_spObjTest;

	enum { IDD = IDD_MAINDLG };

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()


// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

    BEGIN_SINK_MAP(CMainDlg)
    END_SINK_MAP()

    HRESULT __stdcall OnResult( BSTR strParam)
    {


        return S_OK;
    }

    STDMETHODIMP QueryInterface(const struct _GUID &iid,void ** ppv)
    {
        *ppv=this;
        return S_OK;
    }

    ULONG __stdcall AddRef(void)
    {	return 1;	}	// 做个假的就可以，因为反正这个对象在程序结束前是不会退出的

    ULONG __stdcall Release(void)
    {	return 0;	}	// 做个假的就可以，因为反正这个对象在程序结束前是不会退出的

    STDMETHODIMP GetTypeInfoCount(unsigned int *)
    {	return E_NOTIMPL;	}	// 不用实现，反正也不用

    STDMETHODIMP GetTypeInfo(unsigned int,unsigned long,struct ITypeInfo ** )
    {	return E_NOTIMPL;	}	// 不用实现，反正也不用


    STDMETHODIMP GetIDsOfNames( 
        /* [in] */ REFIID riid,
        /* [size_is][in] */ LPOLESTR *rgszNames,
        /* [in] */ UINT cNames,
        /* [in] */ LCID lcid,
        /* [size_is][out] */ DISPID *rgDispId)
    {	return E_NOTIMPL;	}	// 不用实现，反正也不用

    STDMETHODIMP Invoke(
        long dispID,
        const struct _GUID &,
        unsigned long,
        unsigned short,
        struct tagDISPPARAMS * pParams,
        struct tagVARIANT * pvarResult,
        struct tagEXCEPINFO *,
        unsigned int *)
    {		// 只需要实现这个就足够啦

        MessageBox( L"终于调回来啦", L"太好了", MB_OK);

        VARIANTARG & arg = pParams->rgvarg[0];

        *pvarResult = CComVariant(_T("12313123"));

        *((BSTR*)arg.byref) = pvarResult->bstrVal;



        return 0;
    }
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

        TCHAR szFilePath[MAX_PATH] =
            _T("D:\\lidengwang\\COM\\comSvr\\comExe\\Debug\\comExe.exe");
        
        ::GetModuleFileName(_AtlBaseModule.GetModuleInstance(), szFilePath, MAX_PATH);

        ITypeLib* pTypeLib = NULL;
        
        HRESULT hRes = E_FAIL;


        //hRes = LoadTypeLib(szFilePath, &pTypeLib);

        //hRes = RegisterTypeLib(pTypeLib, szFilePath , NULL);

        //AtlRegisterTypeLib( _AtlBaseModule.GetModuleInstance(),  NULL);
        //AtlUnRegisterTypeLib( _AtlBaseModule.GetModuleInstance(),  NULL);

        //CComPtr<IMarshal> spMarshal;

        //hRes = CoGetStandardMarshal(__uuidof(ObjTest),
        //    NULL,
        //    MSHCTX_LOCAL,
        //    NULL,
        //    MSHLFLAGS_NORMAL,
        //    &spMarshal
        //    );

        //////////////////////////////////////////////////////////////////////////

        CComPtr<IClassFactory> spFactory;

        HRESULT hr = CoGetClassObject( __uuidof(SvrObject),  CLSCTX_LOCAL_SERVER ,0, 
            IID_IClassFactory, (void**)&spFactory );

        CComPtr<IDispatch> spDispatch;
        CComPtr<IUnknown> spUnknown;

        //hr = spFactory->CreateInstance(NULL, __uuidof(IUnknown), (void**)&spUnknown );


        //hr = spFactory->CreateInstance(NULL, __uuidof(IObjTest), (void**)&m_spObjTest );
        //m_spObjTest.Detach()->Release();
        //hr = spFactory->CreateInstance(NULL, __uuidof(IObjTest), (void**)&m_spObjTest );
        //m_spObjTest.Detach()->Release();
        //hr = spFactory->CreateInstance(NULL, __uuidof(IObjTest), (void**)&m_spObjTest );

        //{C9B6B21F-EF98-4402-9755-125A85C02D92}
        //{00020424-0000-0000-C000-000000000046}
        //{C9B6B21F-EF98-4402-9755-125A85C02D92}
        //spDispatch->QueryInterface(&m_spObjTest);

        _bstr_t bstrRet;

        //m_spObjTest->MsgBox(_bstr_t(_T("CallMsgBox")), bstrRet.GetAddress());

        //

        hr = spFactory->CreateInstance(NULL, __uuidof(IDispatch), (void**)&spDispatch );

        hr = DispEventAdvise(spDispatch);

        size_t ucount = 0;
        size_t ulasttime = GetTickCount();

        for (ucount = 0; ucount < 1 ; ucount++)
        {
            _bstr_t bstrMsg(_T(""));

            ULONG uCallerId = 0;
            BSTR bstrFunctionName = bstrMsg;
            BSTR bstrParameter = bstrMsg;
            BSTR * bstrResult = NULL;

            static CComVariant avarParams[3];
            avarParams[2] = uCallerId;
            avarParams[2].vt = VT_UI4;
            avarParams[1] = bstrFunctionName;
            avarParams[1].vt = VT_BSTR;
            avarParams[0] = bstrParameter;
            avarParams[0].vt = VT_BSTR;
            CComVariant varResult;

            DISPPARAMS params = { avarParams, NULL, 3, 0 };

            hr = spDispatch->Invoke(1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);

            if ( hr == S_OK && bstrResult != NULL && varResult.vt == VT_BSTR )
            {
                * bstrResult = varResult.bstrVal;
            }

            if (GetTickCount() - ulasttime > 1000)
                break;
        }

        DispEventUnadvise(spDispatch);

        WTL::CString strMsg;
        strMsg.Format(_T("%d"), ucount);
        MessageBox(strMsg,strMsg);


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
        HRESULT hr = DispEventUnadvise(m_spObjTest);

		EndDialog(wID);
		return 0;
	}
};
