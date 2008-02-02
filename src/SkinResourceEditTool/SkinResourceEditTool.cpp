// SkinResourceEditTool.cpp : main source file for SkinResourceEditTool.exe
//

#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>

#include "resource.h"

#include "aboutdlg.h"
#include "MainFrm.h"

#include "ResDocument.h"

#include "skinwincreator.h"
#include "skindialog.h"


class CTestDlg : public KSG::SkinDialogImpl<CTestDlg, KSG::SkinWindow>
{
public:
    BEGIN_MSG_MAP(CTestDlg);
        COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnCloseCmd)
    END_MSG_MAP();

    LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        EndDialog(wID);
        return 0;
    }

};


CSkinAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainFrame wndMain;

    
    wchar_t szbuf[MAX_PATH] = L"宋体";

    {
        HINSTANCE hInst = _AtlBaseModule.GetResourceInstance();
        HRSRC hrsrc = ::FindResource(hInst, MAKEINTRESOURCE(IDD_EDITSTRING_DIALOG), RT_DIALOG);
        if (hrsrc)
        {
            HGLOBAL hResData = ::LoadResource(hInst, hrsrc);

            if (hResData)
            {
                UNALIGNED WORD* pDlgInit = (UNALIGNED WORD*)::LockResource(hResData);       

                WORD a = _T('宋');


            }
        }

    }

    SkinXmlDocument xmlDocument;

    xmlDocument.LoadXML(
        _T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")
            _T("<IDD_DlgTest Caption=\"中国人啊人\" Style=\"2160590976\" ExStyle=\"0\" Font=\"宋体,GB2312_CHARSET,20,1,800,0\" Left=\"10\" Top=\"10\" Width=\"200\" Height=\"200\">")
                _T("<ChildWindow>")
                _T("<IDS_STATIC SkinClassName=\"skinxmlstatic\" IdName=\"ids_abc\" Style=\"1342177280\" Caption=\"我是中国人\" Left=\"10\" Top=\"10\" Width=\"200\" Height=\"20\" />")
                _T("<IDS_STATIC SkinClassName=\"skinxmlstatic\" IdName=\"ids_abc1\" Style=\"1342177280\" Caption=\"我是中国人\" Left=\"10\" Top=\"40\" Width=\"200\" Height=\"20\" />")
                _T("<IDS_STATIC SkinClassName=\"skinxmlstatic\" IdName=\"ids_abc2\" Style=\"1342177280\" Caption=\"我是中国人\" Left=\"10\" Top=\"80\" Width=\"200\" Height=\"20\" />")
                _T("</ChildWindow>")
            _T("</IDD_DlgTest>") );


    DWORD dwStyle = WS_CHILD | WS_VISIBLE | SS_LEFT;

    SkinXmlElement xmlElement = xmlDocument.RootElement();

    //wndMain.SkinCreate(xmlElement, NULL, _T("STATIC") );
    
    CTestDlg dlg;

    dlg.m_xmlDlgElement.CopyFrom(xmlElement);
    
    dlg.DoModal();

    CFont font;

    font = AtlGetDefaultGuiFont();

    skinxmlfont xmlfont;

    xmlfont << font;

    return 0;

	if(wndMain.CreateEx() == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

	wndMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));


	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

    CResDocument::Instance().Init();

	int nRet = Run(lpstrCmdLine, nCmdShow);

    CResDocument::Instance().Uninit();

	_Module.Term();
	::CoUninitialize();


	return nRet;
}
