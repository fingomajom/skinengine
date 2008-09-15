// SvrCaller.cpp : Implementation of CSvrCaller

#include "stdafx.h"
#include "SvrCaller.h"
#include "..\public\lsesvr.h"

// CSvrCaller

HRESULT STDMETHODCALLTYPE CSvrCaller::Initialize( 
    /* [in] */ ULONG     uCallerId,
    /* [in] */ IUnknown *piCallback)
{
    return m_lseCaller.Initialize( uCallerId, piCallback );
}

HRESULT STDMETHODCALLTYPE CSvrCaller::Uninitialize( void )
{
    return m_lseCaller.Uninitialize( );
}

HRESULT STDMETHODCALLTYPE CSvrCaller::CallSvrFunc( 
    /* [in]  */ ULONG uTargetId,
    /* [in]  */ ULONG uFunctionId,
    /* [in]  */ IDataBuffer*  pParameter,
    /* [out] */ IDataBuffer** ppResult)
{
    return m_lseCaller.CallSvrFunc( 
        uTargetId, uFunctionId, pParameter, ppResult);
}

