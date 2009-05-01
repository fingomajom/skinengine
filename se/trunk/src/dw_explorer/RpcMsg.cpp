#include "stdafx.h"
#include "RpcMsg.h"
#include "msg_rpc_np.h"


//int SendRpcMsg( 
//    /* [in] */ int nMsg,
//    /* [in] */ VARIANT pBuffer,
//    /* [out] */ VARIANT *szString)
//{
//    return 0;
//}
//

CRpcMsgServer::CRpcMsgServer()
{
    m_hListenThread = NULL;
    m_dwThreadID = 0;
}

BOOL CRpcMsgServer::InitRpcServer( LPCWSTR pszSvrName )
{
    if ( m_hListenThread != NULL )
        return TRUE;

    RPC_STATUS rpcStatus = RpcServerUseProtseqEpW(
        reinterpret_cast< RPC_WSTR >(L"ncacn_np"),
        RPC_C_PROTSEQ_MAX_REQS_DEFAULT,
        reinterpret_cast< RPC_WSTR >( const_cast<LPWSTR>(pszSvrName)),
        NULL);
    if ( rpcStatus != RPC_S_OK )
        return FALSE;

    rpcStatus = RpcServerRegisterIf(
        msg_rpc_np_v1_0_s_ifspec, 
        NULL, NULL);
    if ( rpcStatus != RPC_S_OK )
        return FALSE;

    m_hListenThread = CreateThread(NULL, 0, RpcSvrListenThreadProc,
        NULL, 0, &m_dwThreadID);
    if ( m_hListenThread == NULL )
        return FALSE;

    return TRUE;
}

BOOL CRpcMsgServer::UninitRpcServer()
{
    if ( m_hListenThread == NULL )
        return TRUE;

    RPC_STATUS rpcStatus = RpcMgmtStopServerListening(NULL);
    if ( rpcStatus != RPC_S_OK )
        return FALSE;

    while ( WaitForSingleObject(m_hListenThread, 100) == WAIT_TIMEOUT )
    { };

    m_hListenThread = NULL;
    m_dwThreadID = 0;

    return TRUE;
}

DWORD WINAPI CRpcMsgServer::RpcSvrListenThreadProc( LPVOID )
{
    RpcServerListen(
        1, 
        RPC_C_LISTEN_MAX_CALLS_DEFAULT, 
        FALSE);

    return 0L;
}


CRpcMsgClient::CRpcMsgClient()
{
    m_rpcHandle = NULL;
}

BOOL CRpcMsgClient::InitRpcClient( LPCWSTR pszSvrName )
{
    BOOL       bRet      = FALSE;
    RPC_STATUS rpcStatus = 0;
    RPC_WSTR   szStringBinding = NULL;

    if ( m_rpcHandle != NULL )
        return TRUE;

    while( pszSvrName != NULL && wcslen(pszSvrName) > 0 )
    {
        rpcStatus = RpcStringBindingCompose(
            NULL, 
            reinterpret_cast< RPC_WSTR >(L"ncacn_np"),
            reinterpret_cast< RPC_WSTR >("localhost"), 
            reinterpret_cast< RPC_WSTR >( const_cast<LPWSTR>(pszSvrName)),
            NULL, 
            &szStringBinding); 
        if ( rpcStatus != RPC_S_OK )
            break;

        rpcStatus = RpcBindingFromStringBinding(
            szStringBinding, 
            &m_rpcHandle); 
        if ( rpcStatus != RPC_S_OK )
            break;

        rpcStatus = RpcStringFree(&szStringBinding); 
        if ( rpcStatus != RPC_S_OK )
            break;

        rpcStatus = RpcEpResolveBinding(m_rpcHandle, msg_rpc_np_v1_0_c_ifspec);
        if ( rpcStatus != RPC_S_OK )
            break;

        rpcStatus = RpcMgmtIsServerListening(m_rpcHandle);
        if ( rpcStatus != RPC_S_OK )
            break;
        
        bRet = TRUE;

        break;
    }

    if ( !bRet )
        UninitRpcClient();

    return bRet;
}

BOOL CRpcMsgClient::UninitRpcClient()
{
    if ( m_rpcHandle != NULL )
    {
        RpcBindingFree(&m_rpcHandle); 
    }

    return TRUE;
}

void* __RPC_USER midl_user_allocate(size_t size)
{
    return malloc(size);
}

void __RPC_USER midl_user_free(void* p)
{
    free(p);
}
