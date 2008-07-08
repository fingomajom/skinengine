/********************************************************************
* CreatedOn: 2008-5-12   14:57
* FileName: HeartBeatViewCtrl.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include <vector>

namespace KSGUI{

class SkinHeartBeatCtrl : 
    public CWindowImpl<SkinHeartBeatCtrl, KSGUI::CSkinStatic>,
    public CDoubleBufferImpl<SkinHeartBeatCtrl>
{
public:

    SkinHeartBeatCtrl()
    {
        m_nMinPos = 1;
        m_nMaxPos = 100;

        m_nMaxCount = 150;
        m_nTableCount = 150;
        m_nTableLineSpace = 5;
        
        m_nAddCount = 0;

        m_clrBkGnd = RGB(255, 255, 255);

        m_clrBorder = RGB(160, 160, 160);        
        m_clrTable  = RGB(200, 200, 200);
    }

    void SetRange( int nmax, int nmin )
    {
        m_nMaxPos = nmax;
        m_nMinPos = nmin;
    }

    void SetTableInfo( int nmaxcount, int nTableLineSpace )
    {
        m_nTableCount = nmaxcount;
        m_nMaxCount   = nmaxcount;

        m_nTableLineSpace = nTableLineSpace;
    }

    int AppendHeartBeatLine( COLORREF clrLine, INT nPenWdith = 1)
    {
        HeartBeatInfo info = { clrLine ,nPenWdith, TRUE};
        info.vtpos.push_back( m_nMinPos );

        m_vtData.push_back(info);

        return m_vtData.size() - 1;
    }

    void SetColors( COLORREF clrBkGnd, COLORREF clrBorder, COLORREF clrTable )
    {
        m_clrBkGnd  = clrBkGnd;
        m_clrBorder = clrBorder;
        m_clrTable  = clrTable;
    }


    void AppendHeartBeatData(int nfirst, ...)
    {
        int ndata = nfirst;

        va_list arglist;
        va_start(arglist, nfirst);
        
        for (int idx = 0 ; ndata != -1; idx++)
        {
            ATLASSERT(idx < m_vtData.size());
            if (idx >= m_vtData.size())
                break;
            
            std::vector<int>& vtpos =  m_vtData[idx].vtpos;

            vtpos.push_back(ndata);

            if ( vtpos.size() > m_nMaxCount )
                vtpos.erase(vtpos.begin());

            ndata = va_arg(arglist, int);
        }

        va_end(arglist);

        m_nAddCount++;

        if (IsWindow())
            CWindow::Invalidate(FALSE);
    }

	void SetHeartBeatData(int nindex, int* pdata, int ncount )
	{
		ATLASSERT(nindex < m_vtData.size());
		if (nindex >= m_vtData.size())
			return;

		std::vector<int>& vtpos =  m_vtData[nindex].vtpos;
		vtpos.clear();

		for (int idx = 0; idx < ncount; idx++)
		{
			vtpos.push_back(pdata[idx]);

			if ( vtpos.size() > m_nMaxCount )
				vtpos.erase(vtpos.begin());
		}
		m_nAddCount++;
	}

	void SetLineVisible(int nindex, BOOL b)
	{
		ATLASSERT(nindex < m_vtData.size());
		if (nindex >= m_vtData.size())
			return;
		if (m_vtData[nindex].m_bVisible != b)
		{
			m_vtData[nindex].m_bVisible = b;
			Invalidate(FALSE);
		}
	}

    void AppendHeartBeatData( int nindex, int* pdata, int ncount )
    {
        ATLASSERT(nindex < m_vtData.size());
        if (nindex >= m_vtData.size())
            return;

        std::vector<int>& vtpos =  m_vtData[nindex].vtpos;

        for (int idx = 0; idx < ncount; idx++)
        {
            vtpos.push_back(pdata[idx]);

            if ( vtpos.size() > m_nMaxCount )
                vtpos.erase(vtpos.begin());
        }

        m_nAddCount++;

    }

    BEGIN_MSG_MAP(SkinHeartBeatCtrl)
        CHAIN_MSG_MAP(CDoubleBufferImpl<SkinHeartBeatCtrl>)
    END_MSG_MAP()

    void DoPaint(CDCHandle dc)
    {
        RECT rcClient = { 0 };
        GetClientRect(&rcClient);

        dc.FillSolidRect( &rcClient, m_clrBkGnd );
        dc.Draw3dRect(&rcClient, m_clrBorder, m_clrBorder);


        float ntablewidth  = (float)(rcClient.right - rcClient.left) / (float)m_nTableCount;
        float ntableheight = (float)(rcClient.bottom - rcClient.top) / (float)(m_nMaxPos - m_nMinPos);


        for ( int idxx = m_nTableLineSpace - (m_nAddCount % m_nTableLineSpace);
              idxx < m_nTableCount; 
              idxx += m_nTableLineSpace )
        {
            RECT rcline = { rcClient.left + idxx * ntablewidth-1, rcClient.top,
                rcClient.left + idxx * ntablewidth, rcClient.bottom};

            dc.FillSolidRect(&rcline, m_clrTable);
        }


        for (int idx = 0; idx < (int)m_vtData.size(); idx++)
        {
            HeartBeatInfo& info = m_vtData[idx];
			if (!info.m_bVisible)
				continue;

            int idxx = 0;

            CPen penLine;

            penLine.CreatePen(PS_SOLID, info.m_nPenWidth, info.m_clrLine);

            HPEN hOldPen = dc.SelectPen(penLine);

            dc.MoveTo(rcClient.right - 1, rcClient.bottom - 1 );

            int nPos = m_nTableCount;

            for (int idxx = (int)info.vtpos.size() - 1 ; idxx >= 0; idxx--)
            {
                int nPosData = info.vtpos[ idxx];

                int nPosX = nPos * ntablewidth;
                int nPosY = 0;

                if (nPosData < m_nMinPos)
                    nPosY = rcClient.bottom - 1;
                else if (nPosData > m_nMaxPos)
                    nPosY = rcClient.top + 1;
                else
                    nPosY = rcClient.bottom - (nPosData - m_nMinPos) * ntableheight;

                dc.LineTo( nPosX, nPosY );

                nPos--;
            }
            
            dc.SelectPen(hOldPen);
        }
    }


private:

    int m_nMinPos;
    int m_nMaxPos;

    int m_nTableCount;
    int m_nTableLineSpace;

    COLORREF m_clrBkGnd;
    COLORREF m_clrTable;
    COLORREF m_clrBorder;

    int m_nAddCount;
    int m_nMaxCount;

    struct HeartBeatInfo
    {
        COLORREF m_clrLine;
		INT		 m_nPenWidth;
		BOOL	 m_bVisible;
        std::vector<int> vtpos;
    };

    std::vector< HeartBeatInfo > m_vtData;
};

};