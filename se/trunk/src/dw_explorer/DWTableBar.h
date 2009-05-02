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

    CDWTableBar()
    {
        m_nHotIndex        = -1;
        m_nSelectIndex     = 0;
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



    BEGIN_MSG_MAP(CDWTableBar)

        MESSAGE_HANDLER(WM_CREATE , OnCreate )
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)

        MESSAGE_HANDLER(WM_SIZE   , OnSize   )

        MESSAGE_HANDLER(WM_MOUSEMOVE , OnMouseMove)
        MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)

        MESSAGE_HANDLER(WM_LBUTTONDOWN , OnLButtonDown)
        MESSAGE_HANDLER(WM_LBUTTONUP   , OnLButtonUp)

        MESSAGE_HANDLER(WM_PAINT     , OnPaint      )
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkGnd )

    END_MSG_MAP()

    void DrawBtnIndex( HDC hDC, CDWImage& pImage, int xPos, int yPos,  int nIdex )
    {
        RECT rcSrcImage = { 0 };
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        rcSrcImage.left   = pImage.GetWidth() / 4;
        rcSrcImage.bottom = pImage.GetHeight();

        rcSrcImage.right = rcSrcImage.left * (nIdex + 1);
        rcSrcImage.left  = rcSrcImage.left * nIdex;

        pImage.AlphaDraw( hDC, xPos, yPos, &rcSrcImage, skin.clrFrameWindow, 1, 2.0f);
    }

    void DrawTableItemIndex( HDC hDC, const RECT& rcDest , int nIdex )
    {
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();
        RECT rcSrcImage = { 0 };
        RECT rcSrcImage1 = { 0 };

        rcSrcImage.left   = m_image_item.GetWidth() / 4;
        rcSrcImage.bottom = m_image_item.GetHeight();
        rcSrcImage.right  = rcSrcImage.left * (nIdex + 1);
        rcSrcImage.left   = rcSrcImage.left * nIdex;

        int nDifWidth = (rcDest.right - rcDest.left) - (rcSrcImage.right - rcSrcImage.left);
        int nHufWidth = (rcSrcImage.right - rcSrcImage.left) / 2;
        
        rcSrcImage1 = rcSrcImage;
        rcSrcImage1.right = rcSrcImage1.left + nHufWidth;

        m_image_item.AlphaDraw( hDC, 
            rcDest.left, rcDest.top, 
            &rcSrcImage1 , 1, nIdex == 2 ? 1.0f : 1.5f );

        rcSrcImage1 = rcSrcImage;
        rcSrcImage1.left  = rcSrcImage1.left + nHufWidth;
        rcSrcImage1.right = rcSrcImage1.left + 1;

        for ( int i = 0; i < nDifWidth; i+= 1 )
        {
            m_image_item.AlphaDraw( hDC, 
                rcDest.left + nHufWidth + i, rcDest.top, 
                &rcSrcImage1 , 1, nIdex == 2 ? 1.0f : 1.5f );
        }

        rcSrcImage1 = rcSrcImage;
        rcSrcImage1.left = rcSrcImage1.left + nHufWidth;
        m_image_item.AlphaDraw( hDC, 
            rcDest.left + nDifWidth + nHufWidth, rcDest.top, 
            &rcSrcImage1 , 1, nIdex == 2 ? 1.0f : 1.5f );        
    }
    
    void DrawTableItemIndex( HDC hDC, TableBtnInfo& tabInfo, int nIdex )
    {
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        RECT rcSrcImage = { 0 };

        //rcSrcImage.left   = info.image->GetWidth() / 4;
        //rcSrcImage.bottom = info.image->GetHeight();

        //rcSrcImage.right = rcSrcImage.left * (nIdex + 1);
        //rcSrcImage.left  = rcSrcImage.left * nIdex;

        //CDCHandle(hDC).FillSolidRect( &info.rcBtn, skin.clrFrameWindow );

        //info.image->AlphaDraw( hDC, info.rcBtn.left, info.rcBtn.top, &rcSrcImage );

        DrawTableItemIndex( hDC, tabInfo.rcBtn, nIdex );
    }

    virtual void RePositionBtns()
    {   
        RECT rcBtn = { 0, 0, 0, 0 };

        for ( int idx = 0; idx < m_vtTableBtn.GetSize(); idx++ )
        {
            TableBtnInfo& info = m_vtTableBtn[idx];

            rcBtn.right += (idx == 0 ? 100 : 80);
            rcBtn.bottom = m_image_item.GetHeight();

            if ( idx > 0 )
                rcBtn.left -= 13;

            info.rcBtn = rcBtn;

            rcBtn.left = rcBtn.right;
        }
    }



    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        m_image_new.LoadFromResource(IDR_PNG_TABLE_NEW);
        m_image_item.LoadFromResource(IDR_PNG_TABLE_ITEM);
        m_image_close.LoadFromResource(IDR_PNG_TABLE_CLOSE);

        AddTableItem( L"", 1, 0 );
        AddTableItem( L"", 1, 0 );
        AddTableItem( L"", 1, 0 );
        AddTableItem( L"", 1, 0 );
        AddTableItem( L"", 1, 0 );
        AddTableItem( L"", 1, 0 );
        AddTableItem( L"", 1, 0 );
        AddTableItem( L"", 1, 0 );
        AddTableItem( L"", 1, 0 );
        AddTableItem( L"", 1, 0 );
        AddTableItem( L"", 1, 0 );
        AddTableItem( L"", 1, 0 );
        AddTableItem( L"", 1, 0 );
        AddTableItem( L"", 1, 0 );
        AddTableItem( L"", 1, 0 );
        AddTableItem( L"", 1, 0 );

        return 1L;
    }

    LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        RePositionBtns();

        return 1L;
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
        if ( m_nSelectIndex >= 0 && m_nSelectIndex < m_vtTableBtn.GetSize() )
        {
            rcSelBtn = m_vtTableBtn[m_nSelectIndex].rcBtn;
            rcSelBtn.right += 13;
        }

        if ( ::PtInRect(&rcSelBtn, pt) )
        {
            nHotPos = m_nSelectIndex;
            
            rcSelBtn = m_vtTableBtn[m_nSelectIndex].rcBtn;
            rcSelBtn.left = rcSelBtn.right - 30;
            rcSelBtn.right = rcSelBtn.left + 23;

            if ( ::PtInRect(&rcSelBtn, pt) )
            {
                nHotClosePos = m_nSelectIndex;
            }
        }
        else
        {
            for ( int idx = m_vtTableBtn.GetSize() - 1; idx >= 0 ; idx-- )
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

            if ( m_nHotIndex < 0 )
            {
                TRACKMOUSEEVENT evt;
                evt.cbSize = sizeof(evt);
                evt.dwFlags = TME_LEAVE;
                evt.hwndTrack = m_hWnd;

                TrackMouseEvent( &evt );
            }

            m_nHotIndex = nHotPos;

            bReflashWnd = TRUE;
        }

        if ( m_nHotCloseIndex != nHotClosePos )
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

    LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
    {
        BOOL bReflashWnd = FALSE;

        if ( m_nHotIndex >= 0 )
        {
            m_nSelectIndex = m_nHotIndex;
            bReflashWnd = TRUE;
            PostMessage(WM_MOUSEMOVE, 0, lParam);
        }

        if ( m_nHotCloseIndex >= 0 )
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
        BOOL bReflashWnd = FALSE;

        ReleaseCapture();
        if ( m_nCloseClickIndex >=0 && m_nCloseClickIndex == m_nHotCloseIndex )
        {
            m_vtTableBtn.RemoveAt( m_nCloseClickIndex );
            RePositionBtns();
            m_nCloseClickIndex = -1;
            bReflashWnd = TRUE;

        }

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

        for ( int idx = 0; idx < m_vtTableBtn.GetSize(); idx++ )
        {
            if ( idx != m_nSelectIndex )
            {
                TableBtnInfo& info = m_vtTableBtn[idx];
                DrawTableItemIndex( memDC, info, m_nHotIndex == idx ? 1 : 0 );
            }
        }

        if ( m_nSelectIndex >= 0 && m_nSelectIndex < m_vtTableBtn.GetSize() )
        {
            DrawTableItemIndex( memDC,  m_vtTableBtn[m_nSelectIndex], 2 );

            DrawBtnIndex( memDC, 
                m_image_close, 
                m_vtTableBtn[m_nSelectIndex].rcBtn.right-30, 
                m_vtTableBtn[m_nSelectIndex].rcBtn.top, 
                m_nCloseClickIndex == m_nSelectIndex ? 2 : (m_nHotCloseIndex == m_nSelectIndex ? 1 : 0) );
        }

        return 1L;
    }

    LRESULT OnEraseBkGnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return 1L;
    }

    DECLARE_WND_CLASS(_T("DWExplorer_TableBar"));

protected:

    ATL::CSimpleArray<TableBtnInfo> m_vtTableBtn;
    
    CDWImage m_image_item;
    CDWImage m_image_close;
    CDWImage m_image_new;
    //CDWImage m_image_item;

    int m_nHotIndex;
    int m_nSelectIndex;
    int m_nHotCloseIndex;
    int m_nCloseClickIndex;

};