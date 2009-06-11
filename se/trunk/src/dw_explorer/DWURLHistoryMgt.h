/********************************************************************
	created:	2009/05/19
	created:	19:5:2009   23:56
	filename: 	d:\lidengwang\se\trunk\src\dw_explorer\DWURLHistoryMgt.h
	file path:	d:\lidengwang\se\trunk\src\dw_explorer
	file base:	DWURLHistoryMgt
	file ext:	h
	author:		lidengwang@s-zone.com
	
	purpose:	
*********************************************************************/


#pragma once

struct URLHistoryInfo
{
    URLHistoryInfo()
    {
        nOrder = 1;
    }
    ATL::CString strTitle;
    ATL::CString strURL;
    int nOrder;
};

typedef ATL::CRBMap<ATL::CString, URLHistoryInfo*> URLHistoryInfoUrlMap;
typedef ATL::CRBMultiMap<unsigned int, URLHistoryInfo*> URLHistoryInfoOrderMap;

class CDWURLHistoryMgt : 
    public CDWSingleton<CDWURLHistoryMgt>,
    public CDWThread
{
    friend class CDWSingleton<CDWURLHistoryMgt>;
public:
    CDWURLHistoryMgt()
    {
        m_bmodify = FALSE;
        CreateThread();
        m_dwThreadId = 0;
    }

    ~CDWURLHistoryMgt()
    {
        SuspendThread();
        Save();
        for ( POSITION pos = m_mapOrder.GetHeadPosition(); pos != NULL; )
        {
            delete m_mapOrder.GetNext( pos )->m_value;
        }
        m_mapURL.RemoveAll();
        m_mapOrder.RemoveAll();
    }

    void AddURLHistory( LPCTSTR pszTitle, LPCTSTR pszURL )
    {

        if ( pszTitle == NULL || lstrlenW(pszTitle) <= 0 || !StrCmpI(pszURL, BLANK_URL) )
            return;

        m_bmodify = TRUE;

        URLHistoryInfoUrlMap::CPair* pFind = m_mapURL.Lookup( pszURL );
        if ( pFind )
        {            
            POSITION pos = m_mapOrder.FindFirstWithKey( pFind->m_value->nOrder );
            while ( pos != NULL )
            {
                if ( m_mapOrder.GetValueAt(pos) == pFind->m_value )
                {
                    break;
                }
                m_mapOrder.GetNextWithKey(pos, pFind->m_value->nOrder);
            }        
            
            pFind->m_value->nOrder++;
            m_mapOrder.RemoveAt( pos );
            m_mapOrder.Insert(pFind->m_value->nOrder, pFind->m_value);

        }

        if ( m_mapOrder.GetCount() >= 1000 )
        {
            POSITION pos = m_mapOrder.GetHeadPosition();
            URLHistoryInfo* info = m_mapOrder.GetValueAt(pos);
            m_mapURL.RemoveKey( info->strURL );
            m_mapOrder.RemoveAt(pos);
            delete info;
        }
        
        URLHistoryInfo* info = new URLHistoryInfo;
        info->strTitle = pszTitle;
        info->strURL   = pszURL;
        m_mapURL.SetAt(info->strTitle, info);
        m_mapOrder.Insert(info->nOrder, info);
    }

    const URLHistoryInfoOrderMap& GetList() 
    {
        return m_mapOrder;
    }
 
    void Lock()
    {
        m_cs.Lock();
    }
    void Unlock()
    {
        m_cs.Unlock();
    }

protected:

    void Load()
    {
        CDWURLHistoryCfg cfg;
        int nCount = cfg.GetCount();
        for ( int i = 0; i < nCount && i < 1000; i++ )
        {
            URLHistoryInfo* info = new URLHistoryInfo;

            cfg.GetURLHistory(i,
                info->strTitle,
                info->strURL,
                info->nOrder);
            if ( info->strTitle.GetLength() <= 0 ||
                 info->strURL.GetLength() <= 0 )
                 continue;
            
            m_mapURL.SetAt(info->strTitle, info);
            m_cs.Lock();
            m_mapOrder.Insert(info->nOrder, info);
            m_cs.Unlock();
            Sleep(5);
        }
    }

    void Save()
    {
        if ( !m_bmodify )
            return;

        CDWURLHistoryCfg cfg;
        cfg.SetCount((int)m_mapOrder.GetCount());
        int idx = 0;

        for ( POSITION pos = m_mapOrder.GetTailPosition(); pos != NULL; )
        {
            URLHistoryInfo& info = *m_mapOrder.GetPrev( pos )->m_value;
            cfg.SetURLHistory( idx++,
                info.strTitle,
                info.strURL,
                info.nOrder);
        }

    }

    virtual DWORD _RunThreadFunc()
    {
        Load();
        return 0;
    }

    BOOL m_bmodify;

    URLHistoryInfoUrlMap    m_mapURL;
    URLHistoryInfoOrderMap  m_mapOrder;
    CComAutoCriticalSection m_cs;
};