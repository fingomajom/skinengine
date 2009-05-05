#pragma once

#include "RpcMsgDef.h"
#include "RpcMsg.h"

class CDWWebWnd;

class CDWClientMgt
{
    CDWClientMgt(void);
public:
    ~CDWClientMgt(void);

    static CDWClientMgt& Instance();

    static int RunMainMsgLoop( LPTSTR lpstrCmdLine = NULL );

public:

    HWND CreateWebWnd ( HWND hParent, LPCTSTR pszOpenURL, IWebBrowser2** ppOut = NULL );
    BOOL DestroyWebWnd( HWND hWnd );

protected:

    static DWORD WINAPI WebWndMsgLoopThread( LPVOID p );
    static DWORD WINAPI WaitForQuitThread( LPVOID p );

    void _AddWebWnd   ( CDWWebWnd* );
    void _RemoveWebWnd( CDWWebWnd* );

public:
    static DWORD m_dwMainThreadId;
public:

    CComAutoCriticalSection   m_cs;
    ATL::CAtlList<CDWWebWnd*> m_listWebWnd;

    CComAutoCriticalSection   m_CreateWebWndCS;
    HANDLE m_hCreateWebWndEvent;
    
    ATL::CString  m_strOpenURL;
    IWebBrowser2* m_piWebBrowser;

    CRpcMsgServer m_rpcSvr;
    CRpcMsgClient m_rpcClt;
};
