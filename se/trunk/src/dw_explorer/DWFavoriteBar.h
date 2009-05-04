/********************************************************************
	created:	2009/05/01
	created:	1:5:2009   12:03
	filename: 	e:\lidengwang\gcode\se\trunk\src\dw_explorer\DWFavoriteBar.h
	file path:	e:\lidengwang\gcode\se\trunk\src\dw_explorer
	file base:	DWFavoriteBar.h
	author:		lidengwang@sohu-rd.com
	
	purpose:	
*********************************************************************/

#pragma once

#include "DWToolbar.h"
#include "DWIEFavoritesMgt.h"

class CDWFavoriteBar : public CDWToolbar
{
    enum {
        WM_LOADFAVORITE_OK = WM_USER + 3000
    };
public:

    BEGIN_MSG_MAP(CDWFavoriteBar)

        MESSAGE_HANDLER(WM_CREATE          , OnCreate )
        MESSAGE_HANDLER(WM_LOADFAVORITE_OK , OnLoadFavoritesOK )
        
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)

        MESSAGE_HANDLER(WM_SIZE   , OnSize   )

        //MESSAGE_HANDLER(WM_MOUSEMOVE , OnMouseMove)
        //MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)

        //MESSAGE_HANDLER(WM_LBUTTONDOWN , OnLButtonDown)
        //MESSAGE_HANDLER(WM_LBUTTONUP   , OnLButtonUp)

        CHAIN_MSG_MAP(CDWToolbar)

    END_MSG_MAP()

    CCommandBarCtrl m_wndCmd;

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        DWORD dwThreadId = 0;
        CloseHandle( CreateThread(NULL, 0, LoadFavoritesThread, this, 0, &dwThreadId));

        bHandled = FALSE;

        return 1L;    
    }

    virtual void RePositionBtns()
    {   
        RECT rcClient = { 0 };
        GetClientRect(&rcClient);

        RECT rcBtn = { m_nLeftSpace, m_nTopSpace, m_nLeftSpace, m_nTopSpace};

        for ( int idx = 0; idx < m_vtToolBtn.GetSize(); idx++ )
        {
            ToolBtnInfo& info = m_vtToolBtn[idx];

            rcBtn.right += 50;
            rcBtn.bottom = m_nTopSpace + rcClient.bottom;

            info.rcBtn = rcBtn;

            rcBtn.left = rcBtn.right;
        }
    }

    virtual void DrawToolBtn( HDC hDC, ToolBtnInfo& info, int nIndex )
    {
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        CDCHandle dc = hDC;
        
        dc.FillSolidRect( &info.rcBtn, skin.clrFrameWindow );
            
        int   nBkMode  = dc.SetBkMode( TRANSPARENT );
        HFONT hOldFont = dc.SelectFont( skin.fontDefault );
        
        dc.SetTextColor( nIndex * 0xFFFFF);

        dc.DrawText(info.strCaption, -1, &info.rcBtn, DT_LEFT | DT_SINGLELINE | DT_VCENTER );
        
        dc.SelectFont( hOldFont );
        dc.SetBkMode ( nBkMode );


        //info.image->AlphaDraw( hDC, info.rcBtn.left, info.rcBtn.top, &rcSrcImage );
    }

    LRESULT OnLoadFavoritesOK(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        ATL::CAtlList<IEFavoriteItem>& list = CDWIEFavoritesMgt::Instance().GetFavoriteList();

        for ( POSITION pos = list.GetHeadPosition(); pos != NULL; )
        {
            IEFavoriteItem& item = list.GetNext(pos);
            AddToolBtn2( item.strTitle, 0, 0, (LPARAM)&item );
        }

        RePositionBtns();
        CWindow::Invalidate();
       
        return 1L;
    }


    LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 1L;
    }

    LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
    {
        return 1L;
    }

    LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {

        return 1L;
    }

    LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return 1L;
    }
    LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return 1L;
    }

    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return 1L;
    }

    LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        CPaintDC dc(m_hWnd);

        return 1L;
    }

    LRESULT OnEraseBkGnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return DefWindowProc();
    }

    static DWORD WINAPI LoadFavoritesThread( LPVOID p )
    {
        CDWFavoriteBar* pthis = (CDWFavoriteBar*)p;

        CDWIEFavoritesMgt::Instance().BuildIEFavorites();
        
        if ( pthis->IsWindow() )
            pthis->PostMessage( WM_LOADFAVORITE_OK );

        return 0;
    }


    DECLARE_WND_CLASS(_T("DWExplorer_FavoriteBar"));

};