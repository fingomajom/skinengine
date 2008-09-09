#include "StdAfx.h"
#include "ModuleInfo.h"
#include "SvrObject.h"

CModuleInfo::CModuleInfo(void)  :
    m_utype( em_info_type_module ),
    m_uModuleId( CALLERID_UNKNOWN ),
    m_uCookie(-1),
    m_uEntryCount(0)
{
}

CModuleInfo::~CModuleInfo(void)
{
}


HRESULT STDMETHODCALLTYPE CModuleInfo::Invoke(
    DISPPARAMS* pdispparams, 
    VARIANT*    pvarResult)
{
    HRESULT hResult = E_FAIL;

    ::InterlockedIncrement( (LONG*)&m_uEntryCount );

    if ( m_utype == em_info_type_caller )
    {
        ATLASSERT( m_spCallback.p != NULL);

        hResult = m_spCallback->Invoke(dispAPI_CallFunction, 
            IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, 
            pdispparams, pvarResult, NULL, NULL);
    }
    else do
    {
        ATLASSERT( m_spModuleObject.m_pT != NULL);


        if (    pdispparams->rgvarg[4].vt != VT_UI4 ||
                pdispparams->rgvarg[3].vt != VT_UI4 ||
                pdispparams->rgvarg[2].vt != VT_UI4 ||
              ( pdispparams->rgvarg[1].vt & VT_ARRAY ) )
            break;

        IDataBuffer*  pParameter = NULL;
        IDataBuffer*  ppResult   = NULL;

        pParameter = SafeArray2DataBuffer( pdispparams->rgvarg[1].parray );
        if (pParameter != NULL)
            pParameter->AddRef();

        hResult = m_spModuleObject->CallModuleFunc (
            pdispparams->rgvarg[4].ulVal,
            pdispparams->rgvarg[3].ulVal,
            pdispparams->rgvarg[2].ulVal,
            pParameter,
            &ppResult);

        if (pParameter != NULL)
            pParameter->Release();

        if (SUCCEEDED(hResult) && ppResult != NULL && pvarResult != NULL)
        {
            pvarResult[0].parray = DataBuffer2SafeArray( ppResult );
            pvarResult[0].vt = pvarResult[0].parray ? VT_UI1 | VT_ARRAY : VT_EMPTY;
        }

    } while ( false );

    ::InterlockedDecrement( (LONG*)&m_uEntryCount );

    return hResult;
}

HRESULT STDMETHODCALLTYPE CModuleInfo::CallSvrFunc( 
    /* [in]  */ ULONG uTargetId,
    /* [in]  */ ULONG uCallerId,
    /* [in]  */ ULONG uFunctionId,
    /* [in]  */ IDataBuffer*  pParameter,
    /* [out] */ IDataBuffer** ppResult)
{
    HRESULT hResult = E_FAIL;

    ::InterlockedIncrement( (LONG*)&m_uEntryCount );

    if ( m_utype == em_info_type_module )
    {
        ATLASSERT( m_spModuleObject.m_pT != NULL);

        hResult = m_spModuleObject->CallModuleFunc (
            uTargetId,
            uCallerId,
            uFunctionId,
            pParameter,
            ppResult);
    }
    else
    {
        ATLASSERT( m_spCallback.p != NULL);

        CComVariant avarParams[5];
        CComVariant varResult;

        avarParams[4] = uTargetId;
        avarParams[4].vt = VT_UI4;
        avarParams[3] = uCallerId;
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

        hResult = m_spCallback->Invoke(dispAPI_CallFunction, 
            IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, 
            &params, &varResult, NULL, NULL);

        if (SUCCEEDED(hResult) && ppResult != NULL && (varResult.vt & VT_ARRAY) )
        {
            *ppResult = SafeArray2DataBuffer( varResult.parray );
        }

    }

    ::InterlockedDecrement( (LONG*)&m_uEntryCount );

    return hResult;
}


HRESULT CModuleInfo::InitModule( LPCTSTR pszModuleFile )
{
    HRESULT hResult = E_FAIL;

    CComPtr<IUnknown> spObj;
    CSvrObject::instance().QueryInterface(IID_IUnknown, (void**)&spObj);

    if (spObj.p == NULL)
        return hResult;

    hResult = InitModule(pszModuleFile, spObj.p);
    if (FAILED(hResult))
        return hResult;


    return S_OK;
}

HRESULT CModuleInfo::InitModule( LPCTSTR pszModuleFile, LPUNKNOWN piSvrObject )
{
    HRESULT hResult = E_FAIL;

    m_utype = em_info_type_module;


    if ( !m_spModuleObject.LoadInterface( pszModuleFile ) )
        return E_FAIL;

    hResult = m_spModuleObject->Initialize( piSvrObject );
    if (FAILED(hResult))
        return hResult;


    hResult = m_spModuleObject->GetModuleId(&m_uModuleId);
    if (FAILED(hResult))
        return hResult;


    return S_OK;
}


HRESULT CModuleInfo::InitCaller( IUnknown* piCallback )
{
    HRESULT hResult = E_FAIL;

    m_utype = em_info_type_caller;


    hResult = piCallback->QueryInterface( __uuidof(IDispatch) , 
        (void**)&m_spCallback );

    if (FAILED(hResult))
        return hResult;

    hResult = m_spCallback->GetTypeInfoCount( (UINT*)&m_uModuleId );
    if (FAILED(hResult) || m_uModuleId == CALLERID_UNKNOWN || m_uModuleId == MODULEID_UNKNOWN)
        return E_FAIL;

    return S_OK;
}

HRESULT CModuleInfo::Uninit()
{
    if ( m_utype == em_info_type_module )
    {
        m_spModuleObject->Uninitialize();
    }

    return S_OK;
}
