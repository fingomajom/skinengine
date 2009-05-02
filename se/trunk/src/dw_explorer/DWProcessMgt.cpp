#include "StdAfx.h"
#include "DWProcessMgt.h"
#include "RpcMsg.h"

CDWProcessMgt::CDWProcessMgt(void)
{
    CloseHandle(
        CreateThread( NULL, 0, AsynMsgLoopThread, this, 0, &m_dwThreadId) );
}

CDWProcessMgt::~CDWProcessMgt(void)
{
    ::PostThreadMessage(m_dwThreadId, WM_QUIT, 0, 0);
}

CDWProcessMgt& CDWProcessMgt::Instance()
{
    static CDWProcessMgt* __CDWProcessMgt_Instance__ = new CDWProcessMgt();
    return *__CDWProcessMgt_Instance__;
}

CRpcMsgClient gg_clt;

BOOL CDWProcessMgt::CreateWebWnd(HWND hParent, HWND* pCeatedWnd )
{
    return ::PostThreadMessage( m_dwThreadId,
        0, (WPARAM)pCeatedWnd, (LPARAM)hParent );
}

HWND CDWProcessMgt::_CreateWebWnd(HWND hParent)
{
    CreateSEProcess();

    static LSEDataBufferImpl<ULONG> BufParent;
    BufParent = (ULONG)hParent;

    CComPtr<IDataBuffer> spResult;
    int nRet = gg_clt.SendRpcMsg( s2c_create_webwnd, BufParent.GetDataBuffer(), &spResult );
    if (spResult.p != NULL)
        spResult.p->AddRef();

    if ( nRet == 0 )
    {
        if ( spResult.p != NULL && spResult->GetBufferSize() == 4 )
            return (HWND)(*((ULONG*)spResult->GetDataBuffer()));
    }


    return NULL;
}

DWORD WINAPI CDWProcessMgt::AsynMsgLoopThread( LPVOID p )
{
    MSG msg;

    CDWProcessMgt& prtmgt = CDWProcessMgt::Instance();

    while ( ::GetMessage(&msg, NULL, 0, 0) )
    {
        *((HWND*)msg.wParam) = prtmgt._CreateWebWnd( (HWND)msg.lParam );
    }

    return 0L;
}


DWORD CDWProcessMgt::CreateSEProcess()
{
    ATL::CString strCmdLine;
    ATL::CString strRpcEPoint;

    strRpcEPoint.Format(L"%s%x", Rpc_Clt_EPoint_Name, GetTickCount());
    strCmdLine.Format(L"[csp]:%s", strRpcEPoint);

    DWORD dwRet = CreateSEProcess(strCmdLine);
    
    if ( dwRet != 0 )
    {
        gg_clt.InitRpcClient( strRpcEPoint );
    }
    
    return dwRet;
}

DWORD CDWProcessMgt::CreateSEProcess( LPCTSTR pszCmdLine )
{
    DWORD dwPID = 0;

    ATL::CString strCmdLine;

    STARTUPINFO			stInfo		= { 0 };
    PROCESS_INFORMATION psInfo		= { 0 };
    stInfo.cb = sizeof(STARTUPINFO);

    TCHAR szModuleFile[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, szModuleFile, MAX_PATH);

    if ( pszCmdLine != NULL && _tcslen(pszCmdLine) > 0 )
        strCmdLine.Format(L"\"%s\" %s", szModuleFile, pszCmdLine);
    else
        strCmdLine = szModuleFile;

    BOOL bCreate = CreateProcess(
        NULL, 
        strCmdLine.GetBuffer(0), 
        NULL, 
        NULL, 
        TRUE, 
        0, 
        NULL, 
        NULL, 
        &stInfo, 
        &psInfo);

    WaitForInputIdle(
        psInfo.hProcess,
        INFINITE);

    ::CloseHandle(psInfo.hProcess);
    ::CloseHandle(psInfo.hThread);    

    return psInfo.dwProcessId;
}

