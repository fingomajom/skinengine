#include "StdAfx.h"
#include "ProcessModuleMgt.h"
#include "lsesvr.h"

DWORD g_dwThreadId = 0;

CProcessModuleMgt::CProcessModuleMgt(void) :
    m_hQuitEvent(NULL)
{
}

CProcessModuleMgt::~CProcessModuleMgt(void)
{
}

HRESULT CProcessModuleMgt::ProcessModule( LPCTSTR pszDllFile )
{
    HRESULT hResult = E_FAIL;

    m_hQuitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (m_hQuitEvent == NULL)
        return E_FAIL;
    
    g_dwThreadId = GetCurrentThreadId();

    
    hResult = m_ModuleInfo.InitModule( pszDllFile, static_cast<ISvrCallbackObject*>(this) );
    if (FAILED(hResult))
        return hResult;
    
    hResult = m_lseCaller.Initialize( m_ModuleInfo.m_uModuleId, 
        static_cast<ILSECallback*>(this) );

    if ( SUCCEEDED(hResult) )
    {
        //WaitForSingleObject(m_hQuitEvent, INFINITE);

        MSG msg;

        while (GetMessage(&msg, 0, 0, 0) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        m_lseCaller.Uninitialize();
    }
    
    m_ModuleInfo.Uninit();
    if ( m_hQuitEvent != NULL)
        CloseHandle( m_hQuitEvent );

    return hResult;
}


HRESULT STDMETHODCALLTYPE CProcessModuleMgt::CallSvrFunc( 
    /* [in]  */ ULONG uTargetId,
    /* [in]  */ ULONG uCallerId,
    /* [in]  */ ULONG uFunctionId,
    /* [in]  */ IDataBuffer*  pParameter,
    /* [out] */ IDataBuffer** ppResult)
{
    if ( uTargetId == uCallerId || uCallerId != m_ModuleInfo.m_uModuleId )
        return E_FAIL;

    return m_lseCaller.CallSvrFunc( 
        uTargetId, uFunctionId, pParameter, ppResult);
}

HRESULT STDMETHODCALLTYPE CProcessModuleMgt::CallbackFunction( 
    /* [in]  */ ULONG uTargetId,
    /* [in]  */ ULONG uCallerId,
    /* [in]  */ ULONG uFunctionId,
    /* [in]  */ IDataBuffer*  pParameter,
    /* [out] */ IDataBuffer** ppResult)
{
    return m_ModuleInfo.CallSvrFunc( 
        uTargetId, uCallerId, uFunctionId, pParameter, ppResult);
}

HRESULT STDMETHODCALLTYPE CProcessModuleMgt::OnQuit()
{
    if (m_hQuitEvent != NULL)
        SetEvent(m_hQuitEvent);

    PostThreadMessage(g_dwThreadId, WM_QUIT, 0, 0);

    return S_OK;
}