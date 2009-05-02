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

class CDWProcessMgt
{
    CDWProcessMgt(void);
public:
    ~CDWProcessMgt(void);

    static CDWProcessMgt& Instance();


    BOOL CreateWebWnd( HWND hParent, HWND* );

protected:

    HWND _CreateWebWnd(HWND hParent);

protected:

    static DWORD WINAPI AsynMsgLoopThread( LPVOID p );

    DWORD CreateSEProcess( );
    DWORD CreateSEProcess( LPCTSTR pszCmdLine );

private:

    DWORD m_dwThreadId;
};
