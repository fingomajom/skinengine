// DW_Explorer.cpp : main source file for DW_Explorer.exe
//

#include "stdafx.h"


#include "resource.h"

#include "DW_ExplorerView.h"
#include "aboutdlg.h"
#include "CDWMainFrame.h"

CAppModule _Module;


HANDLE  g_hMutex = NULL;
LPCTSTR g_pszMutexName =  L"Global\\{96D77347-C566-4749-A1B9-C5B6CCE35106}";

int Run(LPTSTR lpstrCmdLine = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
    int nRet = 0;

	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

    if ( wcslen(lpstrCmdLine) == 0 )
    {
        g_hMutex = OpenMutex( MUTEX_MODIFY_STATE, TRUE, g_pszMutexName );
        if ( g_hMutex != NULL )
        {
            ReleaseMutex (g_hMutex);
            return 0;
        }

        g_hMutex = CreateMutex(NULL, TRUE, g_pszMutexName );

        CDWMainFrame wndMain;
        if(wndMain.CreateEx() == NULL)
        {
            ATLTRACE(_T("Main window creation failed!\n"));
            return 0;
        }
        wndMain.ShowWindow(nCmdShow);

        nRet = theLoop.Run();

        ReleaseMutex ( g_hMutex );
    }
    else
    {
        HWND hParent = (HWND)StrToInt(lpstrCmdLine);
        CAxWindow wndAx;

        RECT rcDefault = {0, 100, 500, 600};
        if(wndAx.Create(hParent, &rcDefault, L"http://sogou.com", WS_CHILD) == NULL)
        {
            ATLTRACE(_T("Main window creation failed!\n"));
            return 0;
        }
        wndAx.ShowWindow(nCmdShow);

        nRet = theLoop.Run();
    }

    _Module.RemoveMessageLoop();
    return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);

    ATLASSERT(SUCCEEDED(hRes));

	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
