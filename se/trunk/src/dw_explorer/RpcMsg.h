/********************************************************************
	created:	2009/05/01
	created:	1:5:2009   15:17
	filename: 	e:\lidengwang\gcode\se\trunk\src\dw_explorer\RpcMsg.h
	file path:	e:\lidengwang\gcode\se\trunk\src\dw_explorer
	file base:	RpcMsg.h
	author:		lidengwang@sohu-rd.com
	
	purpose:	
*********************************************************************/

#pragma once

class CRpcMsgServer
{
public:

    CRpcMsgServer();
    
    BOOL InitRpcServer( LPCWSTR pszSvrName );
    BOOL UninitRpcServer();
    
protected:

    static DWORD WINAPI RpcSvrListenThreadProc( LPVOID );

    HANDLE m_hListenThread;
    DWORD  m_dwThreadID;
};

class CRpcMsgClient
{
public:

    CRpcMsgClient();

    BOOL InitRpcClient( LPCWSTR pszSvrName );
    BOOL UninitRpcClient();

protected:

    RPC_BINDING_HANDLE m_rpcHandle;
};