// DW_Explorer.cpp : main source file for DW_Explorer.exe
//

#include "stdafx.h"


#include "resource.h"

#include "DW_ExplorerView.h"
#include "aboutdlg.h"
#include "DWFrameMgt.h"
#include "DWClientMgt.h"

#include "RpcMsg.h"

CAppModule _Module;



int Run(LPTSTR lpstrCmdLine = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
    int nRet = 0;


    if ( _wcsnicmp(lpstrCmdLine, L"[csp]:", 6 ) )
    {
        CDWFrameMgt::RunMainMsgLoop(lpstrCmdLine);
    }
    else
    {
        CDWClientMgt::RunMainMsgLoop(lpstrCmdLine);
    }

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
