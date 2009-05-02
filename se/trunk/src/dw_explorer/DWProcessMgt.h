/********************************************************************
	created:	2009/05/02
	created:	2:5:2009   10:59
	filename: 	d:\lidengwang\se\trunk\src\dw_explorer\DWProcessMgt.h
	file path:	d:\lidengwang\se\trunk\src\dw_explorer
	file base:	DWProcessMgt
	file ext:	h
	author:		lidengwang@s-zone.com
	
	purpose:	
*********************************************************************/

#pragma once

#include "RpcMsgDef.h"
#include "RpcMsg.h"

struct ProcessInfo
{
   CRpcMsgClient       rpcClt;
   ATL::CAtlList<HWND> m_listWnd;
   DWORD               dwWndCreated;
};

class CDWProcessMgt
{
    CDWProcessMgt(void);
public:
    ~CDWProcessMgt(void);

    static CDWProcessMgt& Instance();


    BOOL CreateWebWnd( HWND hParent, HWND* );
    BOOL DestryWebWnd( HWND hWnd );

protected:

    HWND _CreateWebWnd(HWND hParent);
    ProcessInfo* _FindProcessInfo(HWND hWnd);

protected:

    static DWORD WINAPI AsynMsgLoopThread( LPVOID p );

    ProcessInfo* CreateSEProcess( );
    DWORD CreateSEProcess( LPCTSTR pszCmdLine );

private:

    ATL::CComAutoCriticalSection m_cs;
    ATL::CAtlList<ProcessInfo*>  m_listProcess;

    DWORD m_dwThreadId;
    DWORD m_dwMaxWndCreate;
};
