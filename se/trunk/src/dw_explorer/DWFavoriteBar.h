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
#include "DWEventSvr.h"
#include "DWIEFavoritesMgt.h"


class CDWFavoriteBar : public CDWToolbar
{
    enum {
        WM_LOADFAVORITE_OK = WM_USER + 3000
    };
public:

    CDWFavoriteBar()
    {
        m_nLeftSpace = 10;
        m_bPopMenu   = FALSE;
        m_dwMenuThread = 0;
    }

    BEGIN_MSG_MAP(CDWFavoriteBar)

        MESSAGE_HANDLER(WM_CREATE          , OnCreate )
        MESSAGE_HANDLER(WM_LOADFAVORITE_OK , OnLoadFavoritesOK )
        
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)

        MESSAGE_HANDLER(WM_SIZE   , OnSize   )

        MESSAGE_HANDLER(WM_MOUSEMOVE , OnMouseMove)
        MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)

        MESSAGE_HANDLER(WM_LBUTTONDOWN , OnLButtonDown)
        MESSAGE_HANDLER(WM_LBUTTONUP   , OnLButtonUp)

        CHAIN_MSG_MAP(CDWToolbar)

    END_MSG_MAP()
        
    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        DWORD dwThreadId = 0;
        CloseHandle( CreateThread(NULL, 0, LoadFavoritesThread, this, 0, &dwThreadId));
        CloseHandle( CreateThread(NULL, 0, PopupMenuThread, this, 0, &m_dwMenuThread));

        bHandled = FALSE;


        return 1L;    
    }

    virtual void RePositionBtns()
    {   
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        RECT rcClient = { 0 };
        GetClientRect(&rcClient);

        CClientDC dc(m_hWnd);
        HFONT hOldFont = dc.SelectFont( skin.fontDefault );

        RECT rcBtn = { m_nLeftSpace, m_nTopSpace, m_nLeftSpace, m_nTopSpace};

        for ( int idx = 0; idx < m_vtToolBtn.GetSize(); idx++ )
        {
            ToolBtnInfo& info = m_vtToolBtn[idx];

            SIZE sizeText = { 0 };
            dc.GetTextExtent( info.strCaption, -1, &sizeText );

            rcBtn.right += ( sizeText.cx + 30 );
            rcBtn.bottom = m_nTopSpace + rcClient.bottom;

            info.rcBtn = rcBtn;

            rcBtn.left = rcBtn.right + 2;
        }

        dc.SelectFont( hOldFont );
    }

    virtual void DrawToolBtn( HDC hDC, ToolBtnInfo& info, int nIndex )
    {
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        CDCHandle dc = hDC;
        
        dc.FillSolidRect( &info.rcBtn, skin.clrFrameWindow );
            
        int   nBkMode  = dc.SetBkMode( TRANSPARENT );
        HFONT hOldFont = dc.SelectFont( skin.fontDefault );
        

        RECT rcText = info.rcBtn;
        rcText.left += 22;

        COLORREF clrText = HLS_TRANSFORM( skin.clrFrameWindow, -50, 0 );
        COLORREF clrL    = HLS_TRANSFORM( skin.clrFrameWindow, +50, 0 );
        COLORREF clrS    = HLS_TRANSFORM( skin.clrFrameWindow, -50, 0 );

        if ( nIndex == 1 )
        {
            clrText = clrL ;
            dc.Draw3dRect(&info.rcBtn, clrL, clrS );
        }
        else if ( nIndex == 2  )
        {
            rcText.top+=2;
            dc.Draw3dRect(&info.rcBtn, clrS , clrL );
        }

        dc.SetTextColor( clrText );

        skin.iconFavDir.DrawIconEx( dc, info.rcBtn.left + 4, rcText.top + 2, 16, 16 );

        dc.DrawText(info.strCaption, -1, &rcText, DT_LEFT | DT_SINGLELINE | DT_VCENTER );
        
        dc.SelectFont( hOldFont );
        dc.SetBkMode ( nBkMode );
    }

    LRESULT OnLoadFavoritesOK(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        ATL::CAtlList<IEFavoriteItem>& fList = CDWIEFavoritesMgt::Instance().GetFavoriteList();

        for ( POSITION pos = fList.GetHeadPosition(); pos != NULL; )
        {
            IEFavoriteItem& item = fList.GetNext(pos);
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

    LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        int nOldHotIndex = m_nHotIndex;

        CDWToolbar::OnMouseMove( uMsg, wParam, lParam, bHandled );

        if (  m_bPopMenu && (nOldHotIndex < 0 || m_nHotIndex < 0) && m_nClickIndex >= 0)
        {
            CClientDC dc(m_hWnd);
            DrawToolBtn( dc, m_vtToolBtn[m_nClickIndex], 2 );
        }

        if ( m_bPopMenu && m_nHotIndex >= 0 )
        {

            if ( m_nHotIndex != m_nClickIndex && m_nHotIndex >= 0 )
            {
                m_nClickIndex = m_nHotIndex;

                POINT pt;
                GetCursorPos(&pt);
                mouse_event( MOUSEEVENTF_LEFTDOWN, pt.x, pt.y, NULL, NULL);
                mouse_event( MOUSEEVENTF_LEFTUP, pt.x, pt.y, NULL, NULL);

                m_bPopMenu = TRUE;
            }
        }
        
        return 1L;
    }

    LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CDWToolbar::OnMouseLeave( uMsg, wParam, lParam, bHandled );

        if ( m_bPopMenu && m_nClickIndex >= 0 )
        {
            CClientDC dc(m_hWnd);
            DrawToolBtn( dc, m_vtToolBtn[m_nClickIndex], 2 );
        }
        return 1L;
    }

    LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CDWToolbar::OnLButtonDown( uMsg, wParam, lParam, bHandled );
        
        if ( m_nClickIndex >= 0 && m_nHotIndex == m_nClickIndex )
        {
            ::PostMessage(m_hWndMenu, WM_MOUSELEAVE, (WPARAM)this, 0 );
            m_bPopMenu = TRUE;
        }


        return 1L;
    }
    LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CDWToolbar::OnLButtonUp( uMsg, wParam, lParam, bHandled );

        if ( m_bPopMenu && m_nHotIndex >= 0 )
        {
            CClientDC dc(m_hWnd);

            m_nClickIndex = m_nHotIndex;
            DrawToolBtn( dc, m_vtToolBtn[m_nHotIndex], 2 );
        }


        return 1L;
    }

    LRESULT OnDestroy(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
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

    HMENU CreateFavoriteMenu()
    {
        CMenuHandle menu;

        ATL::CAtlList<IEFavoriteItem>& fList = CDWIEFavoritesMgt::Instance().GetFavoriteList();

        static CMenu smenu;
        if ( !smenu.IsMenu() )
        {
            smenu.CreatePopupMenu();
            _CreateFavoriteMenu( smenu.m_hMenu, fList );
        }

        ATLASSERT( m_nClickIndex>= 0 );
        if ( m_nClickIndex < 0 )
            return NULL;

        int nIndex = m_nClickIndex;
        IEFavoriteItem * pItem = NULL;

        POSITION pos = fList.GetHeadPosition();
        while ( pos != NULL && nIndex-- >= 0 )
        {
            pItem = &fList.GetNext( pos );
        }

        if ( pItem == NULL && pItem->pChildList == NULL )
            return NULL;

        m_ptMenu.x = m_vtToolBtn[m_nClickIndex].rcBtn.left;
        m_ptMenu.y = m_vtToolBtn[m_nClickIndex].rcBtn.bottom;

        ClientToScreen(&m_ptMenu);

        menu = smenu.GetSubMenu( m_nClickIndex );
        
        return menu;
    }

    protected:

    HMENU _CreateFavoriteMenu( CMenuHandle menu, ATL::CAtlList<IEFavoriteItem>& fList )
    {
        
        for ( POSITION pos = fList.GetHeadPosition(); pos != NULL; )
        {
            IEFavoriteItem& item = fList.GetNext(pos);
            
            if ( item.pChildList != NULL )
            {
                CMenuHandle subMenu;
                subMenu.CreatePopupMenu();                
                _CreateFavoriteMenu(subMenu, *item.pChildList );

                menu.AppendMenu( MF_STRING, subMenu, item.strTitle );
            }
            else
            {
                menu.AppendMenu( MF_STRING, (UINT_PTR)&item, item.strTitle );
            }
        }

        return menu;
    }


    static DWORD WINAPI LoadFavoritesThread( LPVOID p )
    {
        CDWFavoriteBar* pthis = (CDWFavoriteBar*)p;

        CDWIEFavoritesMgt::Instance().BuildIEFavorites();
        
        if ( pthis->IsWindow() )
            pthis->PostMessage( WM_LOADFAVORITE_OK );

        return 0;
    }

    class CNullMenuWnd : public CWindowImpl<CNullMenuWnd>
    {
    public:
        BEGIN_MSG_MAP(CDWFavoriteBar)

            MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)

        END_MSG_MAP()

        LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
        {   
            CDWFavoriteBar* pthis = (CDWFavoriteBar*)wParam;

            CMenuHandle menu = pthis->CreateFavoriteMenu();

            HWND hFocus = GetFocus();

            SetForegroundWindow(m_hWnd);

            TPMPARAMS tpmParams;
            tpmParams.cbSize = sizeof(tpmParams);
            pthis->GetWindowRect(&tpmParams.rcExclude);

            pthis->m_bPopMenu = TRUE;                
            int nCmdId = menu.TrackPopupMenuEx( 
                TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, 
                pthis->m_ptMenu.x, pthis->m_ptMenu.y, m_hWnd, &tpmParams);
            pthis->m_bPopMenu = FALSE;
            
            pthis->Invalidate();            
            ::SetFocus(hFocus);

            if ( nCmdId != NULL )
            {
                IEFavoriteItem* pitem = (IEFavoriteItem*)nCmdId;

                CDWEventSvr::Instance().OnMessage( edi_open_url, 
                    (WPARAM)(LPCTSTR)pitem->strURL, TRUE );
            }

            return 1L;
        }


        DECLARE_WND_CLASS(_T("DW_NullMenuWnd"))
    };



    static DWORD WINAPI PopupMenuThread( LPVOID p )
    {
        CDWFavoriteBar* pthis = (CDWFavoriteBar*)p;

        HRESULT hRes = ::CoInitialize(NULL);
        ATLASSERT(SUCCEEDED(hRes));

        AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	

        CMessageLoop theLoop;
        _Module.AddMessageLoop(&theLoop);

        //MSG msg;
        CNullMenuWnd wndBk;

        RECT rcDefault = {0,0,0,0};

        pthis->m_hWndMenu = wndBk.Create( NULL, &rcDefault, 0, 
            WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW );
        
        theLoop.Run();

        pthis->m_bPopMenu = FALSE;                
        wndBk.DestroyWindow();

        _Module.RemoveMessageLoop();
        ::CoUninitialize();

        return 0;
    }

    POINT m_ptMenu;
    BOOL  m_bPopMenu;
    DWORD m_dwMenuThread;
    HWND  m_hWndMenu;

public:

    DECLARE_WND_CLASS(_T("DWExplorer_FavoriteBar"));

};