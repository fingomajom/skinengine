/********************************************************************
	created:	2009/05/01
	created:	1:5:2009   12:03
	filename: 	e:\lidengwang\gcode\se\trunk\src\dw_explorer\DWTableBar.h
	file path:	e:\lidengwang\gcode\se\trunk\src\dw_explorer
	file base:	DWTableBar.h
	author:		lidengwang@sohu-rd.com
	
	purpose:	
*********************************************************************/

#pragma once

#include "DWComDef.h"

class CDWTableBar : public CWindowImpl<CDWTableBar>
{

    struct TableBtnInfo 
    {
        BOOL         bVisual;
        UINT         uID;
        RECT         rcBtn;
        LPARAM       lParam;
        CIconHandle  icon;
        ATL::CString strCaption;
    };

public:

    enum
    {
        sys_tbi_new   = 0,
        sys_tbi_sleft = 1,
        sys_tbi_rleft = 2,
        sys_tbi_menu  = 3,
        sys_tbi_prev  = 4,
        sys_tbi_count = 5
    };

public:

    CDWTableBar()
    {
        m_nHotIndex        = -1;
        m_nSelectIndex     =  sys_tbi_count;
        m_nHotCloseIndex   = -1;
        m_nCloseClickIndex = -1;
    }

    BOOL AddTableItem( LPCTSTR pszCaption, UINT uID, LPARAM lParam )
    {
        TableBtnInfo ItemInfo;
        ItemInfo.bVisual = TRUE;
        ItemInfo.uID = uID;
        ItemInfo.lParam = lParam;
        ItemInfo.strCaption = pszCaption;

        m_vtTableBtn.Add( ItemInfo );
        
        RePositionBtns();

        return TRUE;
    }

    BOOL InsertTableItem( int nIndex, LPCTSTR pszCaption, UINT uID, LPARAM lParam )
    {
        nIndex += sys_tbi_count;

        TableBtnInfo ItemInfo;
        ItemInfo.bVisual = TRUE;
        ItemInfo.uID = uID;
        ItemInfo.lParam = lParam;
        ItemInfo.strCaption = pszCaption;

        m_vtTableBtn.InsertAt(nIndex, ItemInfo);

        RePositionBtns();

        return TRUE;
    }


    BOOL RemoveTableItem( int nIndex, BOOL bReflash = TRUE )
    {
        nIndex += sys_tbi_count;

        if ( nIndex >= sys_tbi_count && nIndex < (int)m_vtTableBtn.GetCount() )
        {
            m_vtTableBtn.RemoveAt( nIndex );
            RePositionBtns();
            if ( bReflash && ::IsWindow(m_hWnd) )
                CWindow::Invalidate();
        }

        return TRUE;
    }

    BOOL SelectIndex( int nIndex, BOOL bReflash = TRUE  )
    {
        nIndex += sys_tbi_count;

        if ( nIndex >= sys_tbi_count && nIndex < (int)m_vtTableBtn.GetCount() )
        {
            m_nSelectIndex = nIndex;
            if ( bReflash && ::IsWindow(m_hWnd) )
                CWindow::Invalidate();
        }
        return TRUE;
    }

    int GetSelectIndex() const
    {
        if ( m_nSelectIndex >= sys_tbi_count && m_nSelectIndex < (int)m_vtTableBtn.GetCount() )
            return m_nSelectIndex - sys_tbi_count;
        return -1;
    }

    UINT GetItemParam(int nIndex) const
    {
        nIndex += sys_tbi_count;

        if ( nIndex >= sys_tbi_count && nIndex < (int)m_vtTableBtn.GetCount() )
            return m_vtTableBtn[nIndex].lParam;
        
        return 0;
    }

    LPARAM SetItemParam( int nIndex, LPARAM lParam )
    {
        LPARAM lRet = 0;

        nIndex += sys_tbi_count;

        if ( nIndex >= sys_tbi_count && nIndex < (int)m_vtTableBtn.GetCount() )
        {
            lRet = m_vtTableBtn[nIndex].lParam;
            m_vtTableBtn[nIndex].lParam = lParam;
        }

        return lRet;
    }

    void SetItemCaption( int nIndex, LPCTSTR pszCaption, BOOL bRePaint = TRUE )
    {
        nIndex += sys_tbi_count;

        if ( nIndex >= sys_tbi_count && nIndex < (int)m_vtTableBtn.GetCount() )
        {
            m_vtTableBtn[nIndex].strCaption = pszCaption;
            if ( bRePaint )
                CWindow::Invalidate();
        }
    }

    LPARAM GetItemId(int nIndex) const
    {
        nIndex += sys_tbi_count;

        if ( nIndex >= sys_tbi_count && nIndex < (int)m_vtTableBtn.GetCount() )
            return m_vtTableBtn[nIndex].uID;

        return 0;
    }

    int GetItemCount() const
    {
        return m_vtTableBtn.GetCount() - sys_tbi_count;
    }

    int FindID( UINT id ) const
    {
        for ( size_t i = sys_tbi_count; i < (int)m_vtTableBtn.GetCount(); i++ )
            if ( m_vtTableBtn[i].uID == id )
                return int(i - sys_tbi_count);

        return -1;
    }

    int FindParam( LPARAM lParam ) const
    {
        for ( size_t i = sys_tbi_count; i < (int)m_vtTableBtn.GetCount(); i++ )
            if ( m_vtTableBtn[i].lParam == lParam )
                return int(i - sys_tbi_count);

        return -1;
    }


    BEGIN_MSG_MAP(CDWTableBar)

        MESSAGE_HANDLER(WM_CREATE , OnCreate )
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)

        MESSAGE_HANDLER(WM_SIZE   , OnSize   )

        MESSAGE_HANDLER(WM_MOUSEMOVE , OnMouseMove)
        MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
        MESSAGE_HANDLER(WM_MOUSEHOVER, OnMouseHover)
        MESSAGE_HANDLER(WM_LBUTTONDBLCLK , OnLButtonDBClk)

        MESSAGE_HANDLER(WM_LBUTTONDOWN , OnLButtonDown)
        MESSAGE_HANDLER(WM_LBUTTONUP   , OnLButtonUp)

        MESSAGE_HANDLER(WM_PAINT     , OnPaint      )
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkGnd )

    END_MSG_MAP()

    void DrawBtnIndex( HDC hDC, CDWImage& pImage, int xPos, int yPos, int nImageIdx, COLORREF clrDest = -1, int nFlags = 1, float fAlpha=1.0f)
    {
        RECT rcSrcImage = { 0 };
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        rcSrcImage.left   = pImage.GetWidth() / 4;
        rcSrcImage.bottom = pImage.GetHeight();

        rcSrcImage.right = rcSrcImage.left * (nImageIdx + 1);
        rcSrcImage.left  = rcSrcImage.left * nImageIdx;
        
        if ( clrDest == -1 )
            pImage.AlphaDraw( hDC, xPos, yPos, &rcSrcImage, nFlags, fAlpha);
        else
            pImage.AlphaDraw( hDC, xPos, yPos, &rcSrcImage, clrDest, nFlags, fAlpha);
    }

    void DrawTableItemIndex( HDC hDC, int nItemIndex , int nImageIdx )
    {
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();
        RECT rcSrcImage = { 0 };
        RECT rcSrcImage1 = { 0 };

        TableBtnInfo& tabInfo = m_vtTableBtn[nItemIndex];
        if ( !tabInfo.bVisual )
            return;

        rcSrcImage.left   = m_image_item.GetWidth() / 4;
        rcSrcImage.bottom = m_image_item.GetHeight();
        rcSrcImage.right  = rcSrcImage.left * (nImageIdx + 1);
        rcSrcImage.left   = rcSrcImage.left * nImageIdx;

        int nDifWidth = (tabInfo.rcBtn.right - tabInfo.rcBtn.left) - (rcSrcImage.right - rcSrcImage.left);
        int nHufWidth = (rcSrcImage.right - rcSrcImage.left) / 2;
        
        rcSrcImage1 = rcSrcImage;
        rcSrcImage1.right = rcSrcImage1.left + nHufWidth;
        
        // µ×Í¼
        m_image_item.AlphaDraw( hDC, 
            tabInfo.rcBtn.left, tabInfo.rcBtn.top, 
            &rcSrcImage1 , 1, nImageIdx == 2 ? 1.0f : 1.5f );

        rcSrcImage1 = rcSrcImage;
        rcSrcImage1.left  = rcSrcImage1.left + nHufWidth;
        rcSrcImage1.right = rcSrcImage1.left + 1;

        for ( int i = 0; i < nDifWidth; i+= 1 )
        {
            m_image_item.AlphaDraw( hDC, 
                tabInfo.rcBtn.left + nHufWidth + i, tabInfo.rcBtn.top, 
                &rcSrcImage1 , 1, nImageIdx == 2 ? 1.0f : 1.5f );
        }

        rcSrcImage1 = rcSrcImage;
        rcSrcImage1.left = rcSrcImage1.left + nHufWidth;
        m_image_item.AlphaDraw( hDC, 
            tabInfo.rcBtn.left + nDifWidth + nHufWidth, tabInfo.rcBtn.top, 
            &rcSrcImage1 , 1, nImageIdx == 2 ? 1.0f : 1.5f );

        // Í¼±ê
        CIconHandle icon = tabInfo.icon;
        if ( icon.IsNull() )
        {
            icon = skin.iconNull;
        }
        icon.DrawIconEx( hDC, tabInfo.rcBtn.left + 15, tabInfo.rcBtn.top + 7, 16, 16 );
        
        rcSrcImage1 = tabInfo.rcBtn;
        rcSrcImage1.top   += 2;
        rcSrcImage1.left  += 33;
        rcSrcImage1.right -= 12;

        if ( nItemIndex == m_nSelectIndex )
        {
            rcSrcImage1.right -= 13;

            int nidx = 0;
            if (m_nCloseClickIndex == m_nSelectIndex && m_nCloseClickIndex == m_nHotIndex ) 
                nidx = 2;
            else if (m_nHotCloseIndex == m_nSelectIndex && m_nHotCloseIndex == m_nHotIndex ) 
                nidx = 1;

            DrawBtnIndex( hDC, 
                m_image_close, 
                m_vtTableBtn[m_nSelectIndex].rcBtn.right-30, 
                m_vtTableBtn[m_nSelectIndex].rcBtn.top, 
                nidx , skin.clrFrameWindow, 1, 2.0f);
        }
        
        int nBkMode = ::SetBkMode( hDC, TRANSPARENT );
        HGDIOBJ hOldObj = ::SelectObject( hDC, (HGDIOBJ)skin.fontDefault );

        DrawText( hDC, tabInfo.strCaption, -1, &rcSrcImage1, 
            DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);

        ::SelectObject( hDC, hOldObj );
        ::SetBkMode( hDC, nBkMode );
    }
    
    void DrawTableBtnIndex( HDC hDC, int nItemIndex , int nImageIdx )
    {
        if ( nItemIndex >= sys_tbi_count )
            DrawTableItemIndex( hDC, nItemIndex, nImageIdx );
        else
        {
            TableBtnInfo& tabInfo = m_vtTableBtn[nItemIndex];
            if ( !tabInfo.bVisual )
                return;

            CDWImage* pimage[] = { 
                &m_image_new, 
                &m_image_new, 
                &m_image_new, 
                &m_image_new, 
                &m_image_new };

            DrawBtnIndex( hDC, *pimage[nItemIndex], 
                tabInfo.rcBtn.left, tabInfo.rcBtn.top, m_nCloseClickIndex == nItemIndex ? 2 : nImageIdx, -1, 1, 1.2f);
        }

    }

    virtual void RePositionBtns()
    {   
        RECT rcBtn = { 0, 0, 0, 0 };

        const int nMaxWidth = 200;
        const int nMinWidth = 70 ;
        const int nDifWidth = 13 ;

        RECT rcClient = { 0 };
        GetClientRect( &rcClient );

        m_vtTableBtn[sys_tbi_new].bVisual   = TRUE;
        m_vtTableBtn[sys_tbi_sleft].bVisual = FALSE;
        m_vtTableBtn[sys_tbi_rleft].bVisual = FALSE;
        m_vtTableBtn[sys_tbi_menu].bVisual  = FALSE;
        m_vtTableBtn[sys_tbi_prev].bVisual  = FALSE;

        if (m_vtTableBtn.GetCount() > sys_tbi_count )
        {
            int nWidth = ( rcClient.right - rcClient.left - nDifWidth - m_image_new.GetWidth()/4 + 10 ) / (m_vtTableBtn.GetCount() - sys_tbi_count);
            if ( nWidth < nMinWidth )
                nWidth = nMinWidth;
            else if (nWidth > nMaxWidth)
                nWidth = nMaxWidth;

            rcBtn = rcClient;
            rcBtn.right = 0;

            for ( int idx = sys_tbi_count; idx < (int)m_vtTableBtn.GetCount(); idx++ )
            {
                TableBtnInfo& info = m_vtTableBtn[idx];

                rcBtn.right += (idx == sys_tbi_count ? nWidth + nDifWidth : nWidth);
                rcBtn.bottom = m_image_item.GetHeight();

                if ( idx > sys_tbi_count )
                    rcBtn.left -= nDifWidth;

                info.rcBtn = rcBtn;

                rcBtn.left = rcBtn.right;
            }
        }
        
        rcBtn.left -= nDifWidth/2;
        rcBtn.right = rcBtn.left + m_image_new.GetWidth() / 4;
        m_vtTableBtn[sys_tbi_new].rcBtn = rcBtn;

    }



    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    { 
        LRESULT lResult = DefWindowProc();

        m_image_new.LoadFromResource(IDR_PNG_TABLE_NEW);
        m_image_item.LoadFromResource(IDR_PNG_TABLE_ITEM);
        m_image_close.LoadFromResource(IDR_PNG_TABLE_CLOSE);

        TableBtnInfo ItemInfo;

        ItemInfo.bVisual = FALSE;
        ItemInfo.uID = 0;
        ItemInfo.lParam = 0;
        ItemInfo.strCaption ;
        m_vtTableBtn.Add( ItemInfo );
        m_vtTableBtn.Add( ItemInfo );
        m_vtTableBtn.Add( ItemInfo );
        m_vtTableBtn.Add( ItemInfo );
        m_vtTableBtn.Add( ItemInfo );

        return lResult;
    }

    LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        RePositionBtns();

        return DefWindowProc();
    }

    LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
    {
        int xPos = GET_X_LPARAM(lParam); 
        int yPos = GET_Y_LPARAM(lParam); 

        POINT pt = { xPos, yPos };

        int nHotPos = -1;
        int nHotClosePos = -1;

        BOOL bReflashWnd = FALSE;

        RECT rcSelBtn = { 0 };
        if ( m_nSelectIndex >= 0 && m_nSelectIndex < (int)m_vtTableBtn.GetCount() )
        {
            rcSelBtn = m_vtTableBtn[m_nSelectIndex].rcBtn;
        }
        if ( ::PtInRect(&rcSelBtn, pt) )
        {
            nHotPos = m_nSelectIndex;
            
            rcSelBtn = m_vtTableBtn[m_nSelectIndex].rcBtn;
            rcSelBtn.left = rcSelBtn.right - 30;
            rcSelBtn.right = rcSelBtn.left + 23;

            if ( m_nSelectIndex >= sys_tbi_count && ::PtInRect(&rcSelBtn, pt) )
            {
                nHotClosePos = m_nSelectIndex;
            }
        }
        else
        {
            for ( int idx = (int)m_vtTableBtn.GetCount() - 1; idx >= 0 ; idx-- )
            {
                TableBtnInfo& info = m_vtTableBtn[idx];

                if ( ::PtInRect(&info.rcBtn, pt) )
                {
                    nHotPos = idx;
                    break;
                }
            }
        }


        if ( m_nHotIndex != nHotPos )
        {
            CClientDC dc(m_hWnd);

            TRACKMOUSEEVENT evt;
            evt.cbSize   = sizeof(evt);
            evt.dwFlags  = TME_LEAVE | TME_HOVER;
            evt.dwHoverTime = 300;
            evt.hwndTrack = m_hWnd;

            TrackMouseEvent( &evt );

            m_nHotIndex = nHotPos;

            bReflashWnd = TRUE;
        }

        if ( m_nHotCloseIndex != nHotClosePos && m_nHotIndex >= sys_tbi_count)
        {
            m_nHotCloseIndex = nHotClosePos;
            bReflashWnd = TRUE;
        }

        if ( bReflashWnd )
            CWindow::Invalidate();

        return 1L;
    }

    LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
    {
        BOOL bReflashWnd = FALSE;

        if ( m_nHotIndex >= 0 )
        {
            m_nHotIndex = -1;
            bReflashWnd = TRUE;
        }

        if ( m_nHotCloseIndex >= 0 )
        {
            m_nHotCloseIndex = -1;
            bReflashWnd = TRUE;
        }

        if ( bReflashWnd )
            CWindow::Invalidate();

        return 1L;
    }

    LRESULT OnMouseHover(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
    {
        //BOOL bReflashWnd = FALSE;

        //if ( m_nHotIndex >= 0 )
        //{
        //    m_nHotCloseIndex = m_nHotIndex;
        //    bReflashWnd = TRUE;
        //}

        //if ( bReflashWnd )
        //    CWindow::Invalidate();

        return 1L;
    }

    LRESULT OnLButtonDBClk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
    {
        BOOL bReflashWnd = FALSE;

        if ( m_nHotIndex >= sys_tbi_count )
        {
            bReflashWnd = TRUE;
            SendMsgToParent(WM_TABLE_BAR_MSG, TGM_ITEM_CLOSE_CLICK, m_nHotIndex-sys_tbi_count);
        }

        if ( bReflashWnd )
            CWindow::Invalidate();

        return 1L;


        return 1L;
    }

    
    LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
    {
        BOOL bReflashWnd = FALSE;

        if ( m_nHotIndex >= sys_tbi_count )
        {
            if ( m_nSelectIndex != m_nHotIndex )
            {
                SendMsgToParent(WM_TABLE_BAR_MSG, 
                    TGM_SELECT_CHANGE, 
                    m_nHotIndex - sys_tbi_count);
            }

            m_nSelectIndex = m_nHotIndex;
            PostMessage(WM_MOUSEMOVE, 0, lParam);
            bReflashWnd = TRUE;
        }
        else if ( m_nHotIndex >= 0 )
        {
            m_nHotCloseIndex = m_nHotIndex;
            bReflashWnd = TRUE;
        }

        if ( m_nHotCloseIndex>= sys_tbi_count )
        {
            m_nCloseClickIndex = m_nHotCloseIndex;
            bReflashWnd = TRUE;
            SetCapture();
        }

        if ( bReflashWnd )
            CWindow::Invalidate();

        return 1L;
    }

    LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        BOOL bReflashWnd = TRUE;

        ReleaseCapture();
        if ( m_nCloseClickIndex >=sys_tbi_count && m_nCloseClickIndex == m_nHotCloseIndex )
        {
            SendMsgToParent( WM_TABLE_BAR_MSG, 
                TGM_ITEM_CLOSE_CLICK, 
                m_nCloseClickIndex - sys_tbi_count );

            //RemoveTableItem( m_nCloseClickIndex - sys_tbi_count, FALSE);
        }

        if (  m_nHotCloseIndex >= 0 && m_nHotCloseIndex == m_nHotIndex )
        {
            OnSysBtnClick(m_nHotCloseIndex);
        }

        m_nCloseClickIndex = -1;
        m_nHotCloseIndex = -1;

        if ( bReflashWnd )
            CWindow::Invalidate();

        return 1L;
    }

    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return 1L;
    }

    LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        CPaintDC dc(m_hWnd);

        RECT rcClient = { 0 };
        GetClientRect(&rcClient);

        CMemoryDC memDC(dc, rcClient);

        memDC.FillSolidRect( &rcClient, skin.clrFrameWindow );
        rcClient.top = rcClient.bottom - 2;
        memDC.FillSolidRect( &rcClient, RGB(240,240,240) );

        for ( int idx = 0; idx < (int)m_vtTableBtn.GetCount(); idx++ )
        {
            if ( idx != m_nSelectIndex )
            {
                DrawTableBtnIndex( memDC, idx, m_nHotIndex == idx ? 1 : 0 );
            }
        }

        if ( m_nSelectIndex >= 0 && m_nSelectIndex < (int)m_vtTableBtn.GetCount() )
        {
            DrawTableBtnIndex( memDC,  m_nSelectIndex, 2 );            
        }

        return 1L;
    }

    LRESULT OnEraseBkGnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return 1L;
    }

    void OnSysBtnClick( int nSysBtn )
    {   
        switch ( nSysBtn )
        {
        case sys_tbi_new:
            SendMsgToParent( WM_TABLE_BAR_MSG, 
                TGM_SYS_BTN_CLICK, 
                sys_tbi_new);
            break;
        }
    }

    void SendMsgToParent( UINT uMsg, WPARAM wParam, LPARAM lParam )
    {
        ::SendMessage(GetParent(), uMsg, wParam, lParam);
    }

    DECLARE_WND_CLASS(_T("DWExplorer_TableBar"));

protected:

    ATL::CAtlArray<TableBtnInfo> m_vtTableBtn;
    
    CDWImage m_image_item;
    CDWImage m_image_close;
    CDWImage m_image_new;
    //CDWImage m_image_item;

    int m_nHotIndex;
    int m_nSelectIndex;

    int m_nHotCloseIndex;
    int m_nCloseClickIndex;

};