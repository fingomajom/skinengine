#include "stdafx.h"
#include "QuickSearchMgr.h"


class CQSPCreater
{
public:
    
    static IQuickSearchProvider* Create( const LPQSPCINFO pCfgInfo )
    {
        IQuickSearchProvider* piResult = NULL;

        if ( pCfgInfo == NULL )
            return NULL;

        piResult = CQuickSearchProviderImpl::CreateInstance( pCfgInfo );
        if ( piResult != NULL )
        {
        }        

        return piResult;
    }
};


CQuickSearchMgr::CQuickSearchMgr()
{
    //Initialize();
}

CQuickSearchMgr::~CQuickSearchMgr()
{
    Uninitialize();
}

CQuickSearchMgr& CQuickSearchMgr::Instance()
{
    static CComObject<CQuickSearchMgr> __CQuickSearchMgr_Instance__;
    return __CQuickSearchMgr_Instance__;
}

BOOL CQuickSearchMgr::Initialize( HWND hWnd )
{
    BOOL bRet = LoadQSearchProvider();
    if ( !bRet )
        return bRet;

    m_wndQSPopWnd.AddRef();
    m_wndQSPopWnd.AddRef();
    
    m_wndQSPopWnd.m_spQSService = this;
    m_spCallback = &m_wndQSPopWnd;

    m_wndQSPopWnd.Create( hWnd );
    
    CloseHandle(::CreateThread(NULL, 0, QSThreadProc, this, 0, &m_dwThreadId));
    
    return bRet;
}

BOOL CQuickSearchMgr::Uninitialize()
{
    if ( m_wndQSPopWnd.IsWindow() )
        m_wndQSPopWnd.DestroyWindow();

    for ( int idx = 0; idx < m_vtQSearchProvider.GetSize(); idx ++ )
    {
        IQuickSearchProvider* piProvider = m_vtQSearchProvider[idx];
        delete piProvider;
    }
    
    ::PostThreadMessage( m_dwThreadId, WM_QUIT, 0, 0 );
    
    m_spCallback.Release();

    return TRUE;
}

BOOL CQuickSearchMgr::QSearchKeyword( LPCTSTR pszKeyword )
{
    BOOL bResult = FALSE;
    
    m_cs.Lock();
    m_strKeyword = pszKeyword;
    bResult = ::PostThreadMessage( m_dwThreadId, WM_QS_KEYWORD, 0, 0 );
    m_cs.Unlock();
    
    return TRUE;
}


BOOL CQuickSearchMgr::LoadQSearchProvider()
{
    CQuickSearchConfig config;

    ATL::CAtlArray<QSPCINFO> vtcfg;

    if ( !config.GetQuickSearchProviderConfig(vtcfg) )
        return FALSE;
    
    for ( size_t idx = 0; idx < vtcfg.GetCount(); idx ++ )
    {
         IQuickSearchProvider* piProvider = CQSPCreater::Create( &vtcfg[idx] );
         if ( piProvider != NULL )
             m_vtQSearchProvider.Add( piProvider );
    }

    return TRUE;
}

HRESULT STDMETHODCALLTYPE CQuickSearchMgr::QuickSearchKeyword( 
    /* [out] */ LPCWSTR pszKeyword )
{
    ATL::CString strKeyword = pszKeyword;

    if ( strKeyword.GetLength() <= 0 )
        return FALSE;

    LPARAM lParam = (LPARAM)strKeyword.AllocSysString();

    return ::PostThreadMessage(m_dwThreadId, WM_QS_KEYWORD, 0, lParam) ? S_OK : E_FAIL;
}

HRESULT STDMETHODCALLTYPE CQuickSearchMgr::GetQSearchProviderCount( 
    /* [out] */ int *pCount )
{
    ATLASSERT( pCount != NULL );
    if ( pCount == NULL )
        return E_FAIL;

    *pCount = m_vtQSearchProvider.GetSize();

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CQuickSearchMgr::GetQSearchProviderInfo( 
    /* [in ] */ int iIndex,
    /* [out] */ LPQSPINFO pInfo )
{
    ATLASSERT( pInfo != NULL && iIndex < m_vtQSearchProvider.GetSize() );
    if ( pInfo == NULL || iIndex >= m_vtQSearchProvider.GetSize() )
        return E_FAIL;
    
    if ( m_vtQSearchProvider[iIndex] == NULL )
        return E_FAIL;

    return m_vtQSearchProvider[iIndex]->GetProviderInfo( pInfo );
}


BOOL CQuickSearchMgr::OnQSearchResult( LPCTSTR pszResult, LPQSPINFO pQSInfo)
{
    if ( m_spCallback.p != NULL )
        m_spCallback->OnQSearchResult( pszResult, pQSInfo );
    
    return TRUE;
}


DWORD CQuickSearchMgr::RunThreadFunc()
{
    MSG msg = {0};

    ATL::CString strLastKeyword;
    DWORD dwLastTick = 0;

    while( ::GetMessage( &msg, NULL, 0, 0 ) )
    {
        switch ( msg.message )
        {
        case WM_QS_KEYWORD:
            {
                ATL::CString strKeyword;
                m_cs.Lock();
                if ( strLastKeyword == m_strKeyword && (GetTickCount() - dwLastTick) < 500 )
                {
                    m_cs.Unlock();
                    continue;
                }
                strLastKeyword = m_strKeyword;
                strKeyword = m_strKeyword;
                dwLastTick = GetTickCount();
                m_cs.Unlock();

                int idx = 0;
                for ( idx = 0; idx < m_vtQSearchProvider.GetSize(); idx++)
                {
                    IQuickSearchProvider* piProvider = m_vtQSearchProvider[idx];
                    if ( piProvider->TestKeyword(strKeyword) == S_OK )
                    {
                        ATL::CString strOut;

                        if ( piProvider->GetSearchResults( strKeyword, strOut ) == S_OK )
                        {
                            QSPINFO info;
                            piProvider->GetProviderInfo( &info );
                            
                            m_cs.Lock();
                            if ( strKeyword != m_strKeyword )
                            {
                                m_cs.Unlock();
                                continue;
                            }
                            m_cs.Unlock();

                            OnQSearchResult( strOut, &info );
                        }
                        
                        break;
                    }
                }

                if ( idx >= m_vtQSearchProvider.GetSize() )
                {
                    QSPINFO info;
                    info.nPType = PTYPE_UNKNOWN;
                    
                    OnQSearchResult( L"", &info );
                }
            }
            break;
        }
    }

    return 0;
}

DWORD WINAPI CQuickSearchMgr::QSThreadProc( LPVOID p )
{
    CQuickSearchMgr* pThis = (CQuickSearchMgr*)p;
    ATLASSERT(pThis);

    return pThis->RunThreadFunc();
}
