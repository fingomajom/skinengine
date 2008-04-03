// SkinGUITest.cpp : main source file for SkinGUITest.exe
//

#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"

#include <skinstr.h>
#include <skinctrlx.h>

CSkinAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainDlg dlgMain;


    SkinXmlDocument xmlDocument;

    xmlDocument.LoadXML(
        _T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")
        _T("<IDD_DlgTest Caption=\"中国人啊人\" Style=\"2160590976\" ExStyle=\"0\" Font=\"宋体,GB2312_CHARSET,9,1,400,0\" Left=\"10\" Top=\"10\" Width=\"292\" Height=\"151\">")
        _T("<ChildWindow>")
        _T("<IDS_STATIC SkinClassName=\"skinxmlstatic\" IdName=\"ids_abc\" Style=\"1342177280\" Caption=\"我是中国人\" Left=\"10\" Top=\"10\" Width=\"200\" Height=\"20\" />")
        _T("<IDS_STATIC SkinClassName=\"skinxmlstatic\" IdName=\"ids_abc1\" Style=\"1342177280\" Caption=\"我是中国人\" Left=\"10\" Top=\"40\" Width=\"200\" Height=\"20\" />")
        _T("<IDS_STATIC SkinClassName=\"skinxmllistbox\" IdName=\"ids_abc2\" Style=\"1342177280\" Caption=\"我是中国人\" Left=\"10\" Top=\"80\" Width=\"200\" Height=\"120\" />")
        _T("</ChildWindow>")
        _T("</IDD_DlgTest>") );
    
    //dlgMain.InitDlgXMLElement(xmlDocument.RootElement());

	if(dlgMain.Create(NULL) == NULL)
	{
		ATLTRACE(_T("Main dialog creation failed!\n"));
		return 0;
	}

	dlgMain.ShowWindow(nCmdShow);

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

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

    _Module.init_skin(_T("KSG.XML"));

	int nRet = Run(lpstrCmdLine, nCmdShow);
    
    _Module.destroy_skin();
	_Module.Term();
	::CoUninitialize();

	return nRet;
}
