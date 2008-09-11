#include "StdAfx.h"
#include "ProcessModuleMgt.h"
#include "lsesvr.h"

DWORD g_dwThreadId = 0;

CProcessModuleMgt::CProcessModuleMgt(void) :
    m_hQuitEvent(NULL),
    m_bDisped(FALSE)
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

    CComPtr<IClassFactory> spFactory;

    hResult = CoGetClassObject( 
        __uuidof(SvrObject), CLSCTX_LOCAL_SERVER, 0, 
        IID_IClassFactory, (void**)&spFactory );

    if (FAILED(hResult))
        return hResult;

    hResult = spFactory->CreateInstance(NULL, 
        __uuidof(IDispatch), (void**)&m_spSvrObject );

    if (FAILED(hResult))
        return hResult;

    
    hResult = m_ModuleInfo.InitModule( pszDllFile, this );
    if (FAILED(hResult))
        return hResult;
    
    DispEventAdvise( m_spSvrObject );

    //WaitForSingleObject(m_hQuitEvent, INFINITE);

    {
        MSG msg;
        while (GetMessage(&msg, 0, 0, 0) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    DispEventUnadvise ( m_spSvrObject );

    m_ModuleInfo.Uninit();

    return hResult;
}



STDMETHODIMP CProcessModuleMgt::GetTypeInfoCount(UINT* pctinfo)
{	
    if ( pctinfo != NULL )
        *pctinfo = m_ModuleInfo.m_uModuleId;

    return S_OK;
}	

STDMETHODIMP CProcessModuleMgt::GetTypeInfo(unsigned int,unsigned long,struct ITypeInfo ** )
{	
    return E_NOTIMPL;	
}	

STDMETHODIMP CProcessModuleMgt::GetIDsOfNames( 
    /* [in] */ REFIID riid,
    /* [size_is][in] */ LPOLESTR *rgszNames,
    /* [in] */ UINT cNames,
    /* [in] */ LCID lcid,
    /* [size_is][out] */ DISPID *rgDispId)
{	
    if ( rgDispId != NULL )
        *rgDispId = m_ModuleInfo.m_uModuleId;

    return E_NOTIMPL;	
}	


STDMETHODIMP CProcessModuleMgt::Invoke(
    /* [in] */ DISPID dispIdMember,
    /* [in] */ REFIID riid,
    /* [in] */ LCID lcid,
    /* [in] */ WORD wFlags,
    /* [out][in] */ DISPPARAMS *pDispParams,
    /* [out] */ VARIANT *pVarResult,
    /* [out] */ EXCEPINFO *pExcepInfo,
    /* [out] */ UINT *puArgErr)
{

    if ( m_ModuleInfo.m_spModuleObject.m_pT == NULL )
        return E_FAIL;

    if ( dispIdMember ==  dispAPI_CallFunction)
    {
        CComPtr<IDataBuffer> spParameter;
        CComPtr<IDataBuffer> spResult;

        if (pDispParams == NULL || pDispParams->cArgs != 5)
            return E_FAIL;

        if (   pDispParams->rgvarg[4].vt != VT_UI4 ||
               pDispParams->rgvarg[3].vt != VT_UI4 ||
               pDispParams->rgvarg[2].vt != VT_UI4 ||
            !( pDispParams->rgvarg[1].vt & VT_ARRAY ) )
            return E_FAIL;

        spParameter = SafeArray2DataBuffer( pDispParams->rgvarg[1].parray );

        HRESULT hr = m_ModuleInfo.m_spModuleObject->CallModuleFunc (
            pDispParams->rgvarg[3].ulVal,
            pDispParams->rgvarg[2].ulVal,
            spParameter,
            &spResult);

        if (spResult.p != NULL)
            spResult.p->AddRef();

        if ( SUCCEEDED(hr) && spResult != NULL && pVarResult != NULL)
        {
            pVarResult[0].parray = DataBuffer2SafeArray( spResult );
            pVarResult[0].vt = pVarResult[0].parray ? VT_UI1 | VT_ARRAY : VT_EMPTY;
        }
    }
    else if ( dispIdMember == dispAPI_Quit )
    {
        SetEvent(m_hQuitEvent);
        PostThreadMessage(g_dwThreadId, WM_QUIT, 0, 0);
    }

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CProcessModuleMgt::CallSvrFunc( 
    /* [in]  */ ULONG uTargetId,
    /* [in]  */ ULONG uCallerId,
    /* [in]  */ ULONG uFunctionId,
    /* [in]  */ IDataBuffer*  pParameter,
    /* [out] */ IDataBuffer** ppResult)
{
    HRESULT hr = E_FAIL;

    if ( uTargetId == uCallerId )
        return E_FAIL;

    ATLASSERT( m_ModuleInfo.m_uModuleId != CALLERID_UNKNOWN );
    if ( m_ModuleInfo.m_uModuleId == CALLERID_UNKNOWN )
        return E_FAIL;


    CComVariant avarParams[5];
    CComVariant varResult;

    avarParams[4] = uTargetId;
    avarParams[4].vt = VT_UI4;
    avarParams[3] = m_ModuleInfo.m_uModuleId;
    avarParams[3].vt = VT_UI4;
    avarParams[2] = uFunctionId;
    avarParams[2].vt = VT_UI4;

    avarParams[1].vt = VT_EMPTY;

    if (pParameter != NULL)
    {
        avarParams[1].parray = DataBuffer2SafeArray( pParameter );
        avarParams[1].vt = avarParams[0].parray ? VT_UI1 | VT_ARRAY : VT_EMPTY;
    }

    DISPPARAMS params = { avarParams, NULL, 5, 0 };


    bool bfirst = true;

    do 
    {
        if (m_spSvrObject.p != NULL)
        {
            hr = m_spSvrObject->Invoke(dispAPI_CallFunction, 
                IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, 
                &params, &varResult, NULL, NULL);
        }

        if ( !bfirst || (m_spSvrObject.p != NULL && hr != ERROR_RPC_SERVER_UNAVAILABLE ) )
            break; 

        Relive();

        bfirst = false;

    } while( true );

    if (SUCCEEDED(hr) && ppResult != NULL && (varResult.vt & VT_ARRAY) )
    {
        *ppResult = SafeArray2DataBuffer( varResult.parray );
    }

    return hr;
}


HRESULT CProcessModuleMgt::Relive()
{
    HRESULT hResult = E_FAIL;

    CComPtr<IClassFactory> spFactory;

    if ( m_spSvrObject.p != NULL )
    {
        DispEventUnadvise( m_spSvrObject );

        m_spSvrObject.Release();
    }

    hResult = CoGetClassObject( 
        __uuidof(SvrObject), CLSCTX_LOCAL_SERVER, 0, 
        IID_IClassFactory, (void**)&spFactory );

    if (FAILED(hResult))
        return hResult;

    hResult = spFactory->CreateInstance(NULL, 
        __uuidof(IDispatch), (void**)&m_spSvrObject );

    if (FAILED(hResult))
        return hResult;

    DispEventAdvise( m_spSvrObject );

    return hResult;
}
