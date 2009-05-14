#include "stdafx.h"
#include "RpcMsg.h"
#include "msg_rpc_np.h"


int (*g_ReceiveRpcMsg)( 
    /* [in]  */ int nMsgId,
    /* [in]  */ IDataBuffer*  pParameter,
    /* [out] */ IDataBuffer** ppResult) = NULL;

int ReceiveRpcMsg( 
    /* [in]  */ int nMsgId,
    /* [in]  */ VARIANT pInBuf,
    /* [out] */ VARIANT *ppOutBuf)
{
    int nRet = -1;

    CComPtr<IDataBuffer> spParameter;
    CComPtr<IDataBuffer> spResult;

    if ( pInBuf.vt & VT_ARRAY )
        spParameter = SafeArray2DataBuffer( pInBuf.parray );


    if ( g_ReceiveRpcMsg != NULL )
    {
        nRet = g_ReceiveRpcMsg(
            nMsgId,
            spParameter,
            &spResult);
    }

    if (spResult.p != NULL)
        spResult.p->AddRef();

    if ( nRet >= 0 && spResult.p != NULL && ppOutBuf != NULL)
    {
        ppOutBuf[0].parray = DataBuffer2SafeArray( spResult );
        ppOutBuf[0].vt = ppOutBuf[0].parray ? VT_UI1 | VT_ARRAY : VT_EMPTY;
    }

    return nRet;
}


CRpcMsgServer::CRpcMsgServer()
{
    m_hListenThread = NULL;
    m_dwThreadID = 0;
}

BOOL CRpcMsgServer::InitRpcServer( LPCWSTR pszSvrName )
{
    if ( m_hListenThread != NULL )
        return TRUE;

    RPC_STATUS rpcStatus = RpcServerUseProtseqEp(
        reinterpret_cast< RPC_WSTR >(L"ncalrpc"),
        RPC_C_PROTSEQ_MAX_REQS_DEFAULT,
        reinterpret_cast< RPC_WSTR >( const_cast<LPWSTR>(pszSvrName)),
        NULL);
    if ( rpcStatus != RPC_S_OK )
        return FALSE;

    rpcStatus = RpcServerRegisterIfEx(
        msg_rpc_np_v1_0_s_ifspec, 
        NULL, 
        NULL,
        RPC_IF_ALLOW_CALLBACKS_WITH_NO_AUTH,
        0,
        NULL);
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

LPVOID CRpcMsgServer::SetReceiveFunc( LPVOID p )
{
    LPVOID pRet = g_ReceiveRpcMsg;

    *(LPVOID*)&g_ReceiveRpcMsg = p;

    return pRet;
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

    while( pszSvrName != NULL && lstrlenW(pszSvrName) > 0 )
    {
        rpcStatus = RpcStringBindingCompose(
            NULL, 
            reinterpret_cast< RPC_WSTR >(L"ncalrpc"),
            NULL,//reinterpret_cast< RPC_WSTR >("localhost"), 
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

int CRpcMsgClient::SendRpcMsg( 
    /* [in]  */ int nMsgId,
    /* [in]  */ IDataBuffer*  pParameter,
    /* [out] */ IDataBuffer** ppResult)
{
    int nRet = -1;

    VARIANT varIn;
    VARIANT varRet;

    VariantInit(&varIn);
    VariantInit(&varRet);

    varIn.parray = DataBuffer2SafeArray( pParameter );
    varIn.vt = varIn.parray ? VT_UI1 | VT_ARRAY : VT_EMPTY;

    RpcTryExcept
    {
        m_cs.Lock();
        hMsgRpcNpBinding = m_rpcHandle;
        nRet = ::SendRpcMsg( nMsgId, varIn, &varRet );
        m_cs.Unlock();

        if ( (varRet.vt & VT_ARRAY) && ppResult != NULL )
            *ppResult = SafeArray2DataBuffer( varRet.parray );

    }
    RpcExcept(1)
    {
        nRet = GetExceptionCode();
        nRet = -1;
    }
    RpcEndExcept;

    VariantClear(&varIn);
    VariantClear(&varRet);
    
    return nRet;
}



void* __RPC_USER midl_user_allocate(size_t size)
{
    return malloc(size);
}

void __RPC_USER midl_user_free(void* p)
{
    free(p);
}


