/********************************************************************
	created:	2009/05/02
	created:	2:5:2009   10:06
	filename: 	d:\lidengwang\se\trunk\src\dw_explorer\DWFrameMgt.h
	file path:	d:\lidengwang\se\trunk\src\dw_explorer
	file base:	DWFrameMgt
	file ext:	h
	author:		lidengwang@s-zone.com
	
	purpose:	
*********************************************************************/

#pragma once

#include "RpcMsgDef.h"
#include "RpcMsg.h"

class CDWMainFrame;

class CDWFrameMgt : public CDWSingleton<CDWFrameMgt>
{
    friend class CDWSingleton<CDWFrameMgt>;
    CDWFrameMgt(void);
public:
    ~CDWFrameMgt(void);

    static int RunMainMsgLoop( LPTSTR lpstrCmdLine = NULL );

public:

    BOOL CreateNewFrame( LPVOID p );


public:

    static DWORD WINAPI FrameMsgLoopThread( LPVOID p );

    void _AddFrame   ( CDWMainFrame* );
    void _RemoveFrame( CDWMainFrame* );

public:
    static DWORD m_dwMainThreadId;
protected:

    CComAutoCriticalSection      m_cs;
    ATL::CAtlList<CDWMainFrame*> m_listFrame;

    CRpcMsgServer m_rpcSvr;
};
