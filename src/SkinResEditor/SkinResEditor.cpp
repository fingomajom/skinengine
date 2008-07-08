// SkinResEditor.cpp : main source file for SkinResEditor.exe
//

#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>

#include "resource.h"

#include "aboutdlg.h"
#include "MainFrm.h"

extern HWND g_hWndFrame;

KSGUI::CSkinAppModule _Module;

KSGUI::SkinRichEditInit _RichEditInit;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainFrame wndMain;

    SkinWindowCreator::Instance().AddSkinCreator( 
        KSGUI::skinxmlhtmlctrl::GetSkinWndClassName(),
        KSGUI::CSkinHtmlCtrl::SkinCreate_Static);


    if(wndMain.CreateEx(NULL, 
        0, 
        ATL::CFrameWinTraits::GetWndStyle(0) |  WS_MAXIMIZE , 
        0) == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

    g_hWndFrame = wndMain;
    
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

    SkinHookMouse::instance().InitHookMouse();

	int nRet = Run(lpstrCmdLine, nCmdShow);

    SkinHookMouse::instance().UnitHookMouse();

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
