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

typedef ATL::CAtlList<URLHistoryInfo> URLHistoryInfoList;

class CDWURLHistoryMgt : 
    public CDWSingleton<CDWURLHistoryMgt>,
    public CDWThread
{
    friend class CDWSingleton<CDWURLHistoryMgt>;
public:
    CDWURLHistoryMgt()
    {
        m_bmodify = FALSE;
        CloseHandle(CreateThread());
    }

    ~CDWURLHistoryMgt()
    {
        Save();
    }

    void AddURLHistory( LPCTSTR pszTitle, LPCTSTR pszURL )
    {

        if ( !StrCmpI(pszURL, BLANK_URL) )
            return;

        m_bmodify = TRUE;

        for ( POSITION pos = m_listURLHistory.GetHeadPosition(); pos != NULL; )
        {
            URLHistoryInfo& info = m_listURLHistory.GetNext( pos );
            
            if ( !StrCmpI(info.strURL, pszURL ) )
            {
                info.strTitle = pszTitle;
                info.nOrder++;
                
                int nPrevOrder = -1;

                if ( pos == NULL )
                    pos = m_listURLHistory.GetTailPosition();
                else
                    m_listURLHistory.GetPrev(pos);
                
                POSITION posMe = pos; 

                m_listURLHistory.GetPrev( pos );

                while ( pos != NULL && info.nOrder > nPrevOrder )
                {
                    POSITION posTo = pos;
                    nPrevOrder = m_listURLHistory.GetAt(pos).nOrder;

                    if ( info.nOrder > nPrevOrder )
                    {
                        m_listURLHistory.SwapElements(posTo, posMe);
                    }    

                    m_listURLHistory.GetPrev( pos );
                }                
                return;
            }
        }

        if ( m_listURLHistory.GetCount() >= 1000 )
            m_listURLHistory.RemoveTail();
        
        URLHistoryInfo info;
        info.strTitle = pszTitle;
        info.strURL   = pszURL;
        m_listURLHistory.AddTail(info);
    }

    const URLHistoryInfoList& GetList() 
    {
        return m_listURLHistory;
    }
 

protected:

    void Load()
    {
        CDWURLHistoryCfg cfg;
        int nCount = cfg.GetCount();
        for ( int i = 0; i < nCount; i++ )
        {
            URLHistoryInfo info;
            cfg.GetURLHistory(i,
                info.strTitle,
                info.strURL,
                info.nOrder);
            if ( info.strTitle.GetLength() <= 0 ||
                 info.strURL.GetLength() <= 0 )
                 continue;
            m_listURLHistory.AddTail( info );
        }
    }

    void Save()
    {
        if ( !m_bmodify )
            return;

        CDWURLHistoryCfg cfg;
        cfg.SetCount((int)m_listURLHistory.GetCount());
        int idx = 0;

        for ( POSITION pos = m_listURLHistory.GetHeadPosition(); pos != NULL; )
        {
            URLHistoryInfo& info = m_listURLHistory.GetNext( pos );
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
    URLHistoryInfoList m_listURLHistory;
};