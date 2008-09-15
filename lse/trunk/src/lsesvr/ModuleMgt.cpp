#include "StdAfx.h"
#include "ModuleMgt.h"

CModuleMgt::CModuleMgt(void)
{
}

CModuleMgt::~CModuleMgt(void)
{
}


CModuleMgt& CModuleMgt::Instance()
{
    static CModuleMgt gs_CModuleMgt_Instance;

    return gs_CModuleMgt_Instance;
}

HRESULT CModuleMgt::AddModuleObject( LPCTSTR pszModuleFile )
{
    HRESULT hResult = E_FAIL;

    if ( pszModuleFile == NULL )
        return hResult;

    CModuleInfo* pModuleInfo = new CModuleInfo;
    if (pModuleInfo == NULL)
        return E_FAIL;

    hResult = pModuleInfo->InitModule ( pszModuleFile );
    if (FAILED(hResult))
        return hResult;

    hResult = AddModuleInfo( pModuleInfo, FALSE );
    if (FAILED( hResult ))
        delete pModuleInfo;


    return hResult;
}

HRESULT CModuleMgt::AddCallerCallback( IUnknown* pUnkSink, DWORD dwCookie )
{
    HRESULT hResult = E_FAIL;
    
    if ( pUnkSink == NULL )
        return hResult;

    CModuleInfo* pModuleInfo = new CModuleInfo;
    if (pModuleInfo == NULL)
        return E_FAIL;

    hResult = pModuleInfo->InitCaller( pUnkSink );
    if (FAILED(hResult))
        return hResult;

    pModuleInfo->m_uCookie = dwCookie;

    m_cs.Lock();
    m_mapCookie2ModuleId[dwCookie] = pModuleInfo->m_uModuleId;
    m_cs.Unlock();

    hResult = AddModuleInfo( pModuleInfo, TRUE );
    if (FAILED( hResult ))
        delete pModuleInfo;

    return hResult;
}


HRESULT CModuleMgt::RemoveCallerCallback ( DWORD dwCookie )
{
    HRESULT hResult = E_FAIL;
    
    ULONG uCallid = CALLERID_UNKNOWN;
    
    m_cs.Lock();

    if ( m_mapCookie2ModuleId.Lookup( dwCookie ) )
    {
        uCallid = m_mapCookie2ModuleId[dwCookie];
    }

    m_cs.Unlock();

    
    if ( uCallid != CALLERID_UNKNOWN )
        RemoveModuleInfo( uCallid , dwCookie );

    return hResult;
}


HRESULT CModuleMgt::AddModuleInfo( CModuleInfo* pModuleInfo, BOOL bCaller )
{
    HRESULT hResult = S_OK;

    m_cs.Lock();

    ATL::CAtlMap<DWORD, ATL::CSimpleArray<CModuleInfo*> >::CPair* pPair = 
        m_mapModuleId2Info.Lookup( pModuleInfo->m_uModuleId );
    if (pPair == NULL)
    {
        ATL::CSimpleArray<CModuleInfo*> newList;
        newList.Add( pModuleInfo );

        m_mapModuleId2Info[ pModuleInfo->m_uModuleId ] = newList ;

    }
    else if ( bCaller )
    {
        ATL::CSimpleArray<CModuleInfo*>& ModuleList = pPair->m_value;

        ModuleList.Add( pModuleInfo );
    }
    else
        hResult = E_FAIL;
    
    m_cs.Unlock();

    return hResult;
}


HRESULT CModuleMgt::RemoveModuleInfo ( ULONG uModuleId, DWORD dwCookie  )
{

    while (true)
    {

        ATL::CSimpleArray<CModuleInfo*> ModuleList;

        m_cs.Lock();

        ATL::CAtlMap<DWORD, ATL::CSimpleArray<CModuleInfo*> >::CPair* pPair = 
            m_mapModuleId2Info.Lookup( uModuleId );

        if (pPair != NULL)
        {
            ModuleList = pPair->m_value;

            m_mapModuleId2Info.RemoveKey( uModuleId );
        }

        if ( dwCookie != -1)
            m_mapCookie2ModuleId.RemoveKey( dwCookie );


        m_cs.Unlock();

        if (pPair == NULL)
            break;

        for ( int idx = 0; idx < ModuleList.GetSize(); idx++ )
        {
            if ( dwCookie != -1 && dwCookie != ModuleList[idx]->m_uCookie )
                continue;
            
           
            while ( ::InterlockedExchange( (LONG*)&ModuleList[idx]->m_uEntryCount ,
                ModuleList[idx]->m_uEntryCount ) > 0 )
            {
                Sleep(5);
            }

            ModuleList[idx]->Uninit();

            delete ModuleList[idx];
            ModuleList.RemoveAt( idx );
            idx--;
        }


        if ( ModuleList.GetSize() > 0 )
        {
            m_cs.Lock();

            m_mapModuleId2Info[uModuleId] = ModuleList;

            m_cs.Unlock();

        }

        break;
    }


    return S_OK;
}

HRESULT CModuleMgt::RemoveAllModule()
{
    HRESULT hResult = E_FAIL;
    
    while ( true )
    {
        DWORD uModuleId = CALLERID_UNKNOWN;

        m_cs.Lock();
        
        POSITION pFirstPos = m_mapModuleId2Info.GetStartPosition();
        if (pFirstPos != NULL)
            uModuleId = m_mapModuleId2Info.GetNextKey( pFirstPos );

        m_cs.Unlock();

        if ( uModuleId == CALLERID_UNKNOWN )
            break;

        RemoveModuleInfo( uModuleId );

    }


    return S_OK;
}


HRESULT STDMETHODCALLTYPE CModuleMgt::Invoke(
    DISPPARAMS* pdispparams, 
    VARIANT*    pvarResult)
{
    if ( pdispparams == NULL )
        return E_FAIL;

    if (pdispparams->cArgs != 5 || pdispparams->rgvarg[4].vt != VT_UI4)
        return E_FAIL;

    m_cs.Lock();

    ATL::CAtlMap<DWORD, ATL::CSimpleArray<CModuleInfo*> >::CPair* pPair = 
        m_mapModuleId2Info.Lookup( pdispparams->rgvarg[4].ulVal );

    m_cs.Unlock();

    if (pPair == NULL)
        return E_NOINTERFACE;

    ATL::CSimpleArray<CModuleInfo*>& ModuleList = pPair->m_value;


    if ( ModuleList.GetSize() > 1 )
        pvarResult = NULL;

    for ( int idx = 0; idx < ModuleList.GetSize(); idx++ )
    {
        ATLASSERT( ModuleList[idx] != NULL );

        ModuleList[idx]->Invoke( pdispparams, pvarResult );     
    }


    return S_OK;
}

HRESULT STDMETHODCALLTYPE CModuleMgt::CallSvrFunc( 
    /* [in]  */ ULONG uTargetId,
    /* [in]  */ ULONG uCallerId,
    /* [in]  */ ULONG uFunctionId,
    /* [in]  */ IDataBuffer*  pParameter,
    /* [out] */ IDataBuffer** ppResult)
{

    m_cs.Lock();

    ATL::CAtlMap<DWORD, ATL::CSimpleArray<CModuleInfo*> >::CPair* pPair = 
        m_mapModuleId2Info.Lookup( uTargetId );

    m_cs.Unlock();

    if (pPair == NULL)
        return E_NOINTERFACE;

    ATL::CSimpleArray<CModuleInfo*>& ModuleList = pPair->m_value;
    
    if ( ModuleList.GetSize() > 1 )
        ppResult = NULL;

    for ( int idx = 0; idx < ModuleList.GetSize(); idx++ )
    {
        ATLASSERT( ModuleList[idx] != NULL );

        ModuleList[idx]->CallSvrFunc( uTargetId,
            uCallerId, uFunctionId, pParameter, ppResult);        
    }


    return S_OK;
}


