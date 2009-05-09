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
#include "DWMenuorder.h"
#include "DWMenu.h"

static HHOOK g_hFavPopupMenuHook = NULL;
static HWND  g_wndFav = NULL;


class CDWFavoriteBar : 
    public CDWToolbar
{
    enum {
        WM_LOADFAVORITE_OK = WM_USER + 3000
    };
public:

    CDWFavoriteBar()
    {
        m_nLeftSpace = 10;
        m_nTopSpace  = 2;
        m_bPopMenu   = FALSE;
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

        MESSAGE_HANDLER(WM_TIMER       , OnTimer )

        MESSAGE_HANDLER(WM_MENUSELECT  , OnMenuSelect)

        CHAIN_MSG_MAP(CDWToolbar)

    END_MSG_MAP()

        
    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        LoadFavorites();

        bHandled = FALSE;

        return 0L;    
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
            rcBtn.bottom = rcClient.bottom ;

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
            CPen pen; pen.CreatePen( PS_SOLID, 1, HLS_TRANSFORM( skin.clrFrameWindow, +60, 0 ) );
            CBrush brush; brush.CreateSolidBrush( clrL );

            HPEN hOldPen = dc.SelectPen(pen);
            HBRUSH hOldBrush = dc.SelectBrush(brush);
            POINT pt = { 3, 3 };
            dc.RoundRect( &info.rcBtn, pt );
            dc.SelectPen(hOldPen);
            dc.SelectBrush(hOldBrush);
        }
        else if ( nIndex == 2  )
        {
            rcText.top+=1;
            rcText.bottom += 1;
            dc.Draw3dRect(&info.rcBtn, clrS , clrL );
        }

        dc.SetTextColor( clrText );

        CIconHandle icon = skin.iconFavDir;
        if ( ((IEFavoriteItem*)info.lParam)->pChildList == NULL )
        {
            icon = skin.iconNull;

            HICON hIcon = CDWFavIconMgt::Instance().GetFavIcon( 
                ((IEFavoriteItem*)info.lParam)->strURL, NULL, NULL, FALSE);
            
            if ( hIcon != NULL )
                icon = hIcon;
        }
        
        icon.DrawIconEx( dc, info.rcBtn.left + 4, rcText.top + 2, 16, 16 );

        dc.DrawText(info.strCaption, -1, &rcText, DT_LEFT | DT_SINGLELINE | DT_VCENTER );
        
        dc.SelectFont( hOldFont );
        dc.SetBkMode ( nBkMode );
    }

    LRESULT LoadFavorites()
    {
        DWORD dwThreadId = 0;

        CloseHandle( CreateThread(NULL, 0, LoadFavoritesThread, this, 0, &dwThreadId));
        
        return dwThreadId;
    }

    LRESULT OnLoadFavoritesOK(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        m_vtToolBtn.RemoveAll();

        CDWFavList& fList = CDWIEFavoritesMgt::Instance().GetFavoriteList();

        _SortFavMenuItem(fList);

        for ( size_t idx = 0; idx < fList.GetCount(); idx++ )
        {
            IEFavoriteItem& item = fList[idx];
            AddToolBtn2( item.strTitle, 0, 0, (LPARAM)&item );
        }

        RePositionBtns();
        CWindow::Invalidate();

        m_favMenu.DestroyMenu();


        return 1L;
    }


    LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 1L;
    }

    LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;
        
        if ( wParam == 1001 )
        {
            KillTimer(1001);

            POINT pt;
            GetCursorPos(&pt);
            
            PopupFavMenu();
        }

        return 0;
    }

    LRESULT OnMenuSelect(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        TCHAR szBuffer[MAX_PATH] = { 0 };

        WORD wFlags = HIWORD(wParam);
        WORD wID    = LOWORD(wParam);

        IEFavoriteItem* pFavItem = GetMenuFavItem( wID );

        if( (wFlags == 0xFFFF && lParam == NULL) || pFavItem == NULL )   // menu closing
        {
            CDWEventSvr::Instance().OnMessage( edi_status_bar );
        }
        else if ( !(wFlags & MF_POPUP) )
        {
            CDWEventSvr::Instance().OnMessage( edi_status_bar,
                (WPARAM)(LPCTSTR)pFavItem->strURL );
        }

        return 1;
    }

    
    BOOL IsHasSubMenu( int nIndex )
    {
        if  ( nIndex < 0 || nIndex > m_vtToolBtn.GetSize() )
            return FALSE;

        IEFavoriteItem* favItem = (IEFavoriteItem*)m_vtToolBtn[m_nClickIndex].lParam;
        ATLASSERT(favItem != NULL);

        return favItem->pChildList != NULL;
    }

    LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        int nOldHotIndex = m_nHotIndex;

        CDWToolbar::OnMouseMove( uMsg, wParam, lParam, bHandled );

        if (  m_bPopMenu && m_nClickIndex >= 0 && (m_nHotIndex < 0 || nOldHotIndex < 0) )
        {
            if ( IsHasSubMenu(m_nClickIndex) )
            {
                CClientDC dc(m_hWnd);
                DrawToolBtn( dc, m_vtToolBtn[m_nClickIndex], 2 );
            }
        }

        if ( m_bPopMenu && m_nHotIndex >= 0  )
        {
            if ( m_nHotIndex != m_nClickIndex && m_nHotIndex >= 0 )
            {
                m_nClickIndex = m_nHotIndex;

                if ( IsHasSubMenu(m_nClickIndex) )
                {
                    POINT pt;
                    GetCursorPos(&pt);

                    mouse_event( MOUSEEVENTF_LEFTDOWN, pt.x, pt.y, NULL, NULL);
                    mouse_event( MOUSEEVENTF_LEFTUP  , pt.x, pt.y, NULL, NULL);

                    SetTimer(1001, 10);
                }
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

    template <bool t_bManaged>
    class CDWFavMenuT : public CDWMenuT<t_bManaged>
    {
        ATL::CAtlMap<UINT, IEFavoriteItem*>& m_mapMenuId;
    public:
        CDWFavMenuT(ATL::CAtlMap<UINT, IEFavoriteItem*>& mapMenuId) 
            : m_mapMenuId(mapMenuId)
        { }

        virtual void _OnDrawMenuIcon( CDCHandle& dc, LPDRAWITEMSTRUCT lpDrawItem, int nSelected )
        {
            CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

            CIconHandle icon = skin.iconNull;
            
            MENUITEMINFO MenuItemInfo = { 0 };
            MenuItemInfo.cbSize = sizeof(MenuItemInfo);
            MenuItemInfo.fMask = MIIM_SUBMENU;

            CDWMenuHandle((HMENU)lpDrawItem->hwndItem).GetMenuItemInfo(
                lpDrawItem->itemID,
                FALSE, &MenuItemInfo);

            if ( MenuItemInfo.hSubMenu != NULL )
            {
                icon = skin.iconFavDir;
            }
            else
            {
                IEFavoriteItem* pitem =  GetMenuFavItem(lpDrawItem->itemID);
                ATLASSERT(pitem);

                if ( pitem == NULL || pitem->strURL.GetLength() <= 0 )
                    icon.m_hIcon = NULL;
                else
                {
                    HICON hIcon = CDWFavIconMgt::Instance().GetFavIcon( 
                        pitem->strURL, NULL, NULL, FALSE);
                    if ( hIcon != NULL )
                        icon = hIcon;
                }
            }

            if ( icon.m_hIcon != NULL )
                icon.DrawIconEx( dc,
                    lpDrawItem->rcItem.left + 5,
                    lpDrawItem->rcItem.top  + 2,
                    16, 16);
        }

        IEFavoriteItem* GetMenuFavItem( UINT uMenuId )
        {
            ATL::CAtlMap<UINT, IEFavoriteItem*>::CPair* 
                pFind = m_mapMenuId.Lookup( uMenuId );

            if ( pFind == NULL )
                return NULL;

            return pFind->m_value;
        }

    };

    void PopupFavMenu()
    {

        CDWFavMenuT<FALSE> menu(m_mapMenuId);
        menu.m_hMenu = CreateFavoriteMenu();
        if ( !menu.IsMenu() )
            return;

        TPMPARAMS tpmParams;
        tpmParams.cbSize = sizeof(tpmParams);
        GetWindowRect(&tpmParams.rcExclude);

        g_wndFav = m_hWnd;
        g_hFavPopupMenuHook = ::SetWindowsHookEx(WH_GETMESSAGE, 
            MsgHookFunc, 
            _Module.GetModuleInstance(), 
            GetCurrentThreadId());
        ATLASSERT(g_hFavPopupMenuHook);

        POINT pt;

        pt.x = m_vtToolBtn[m_nClickIndex].rcBtn.left,
        pt.y = m_vtToolBtn[m_nClickIndex].rcBtn.bottom,

        ClientToScreen(&pt);

        m_bPopMenu = TRUE;
        int nCmdId = menu.DWTrackPopupMenu( 
            TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, 
            pt.x, pt.y,
            m_hWnd, &tpmParams);
        m_bPopMenu = FALSE;

        UnhookWindowsHookEx(g_hFavPopupMenuHook);

        Invalidate();
        if ( nCmdId != NULL )
        {
            IEFavoriteItem* pitem = GetMenuFavItem(nCmdId);

            CDWEventSvr::Instance().OnMessage( edi_open_url, 
                (WPARAM)(LPCTSTR)pitem->strURL, TRUE );
        }

    }

    LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CDWToolbar::OnLButtonDown( uMsg, wParam, lParam, bHandled );
        
        if ( m_nClickIndex >= 0 && m_nHotIndex == m_nClickIndex )
        {
            IEFavoriteItem* favItem = (IEFavoriteItem*)m_vtToolBtn[m_nClickIndex].lParam;
            ATLASSERT(favItem != NULL);

            if ( favItem->pChildList == NULL )
            {
                CDWEventSvr::Instance().OnMessage( edi_open_url, 
                    (WPARAM)(LPCTSTR)favItem->strURL, TRUE );
            }
            else
            {
                PopupFavMenu();
            }
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

    static LRESULT CALLBACK MsgHookFunc(
        int nCode,
        WPARAM wParam,
        LPARAM lParam )
    {
        LPMSG pMsg = (LPMSG)lParam;


        LRESULT lRet = ::CallNextHookEx( g_hFavPopupMenuHook, nCode, wParam, lParam);

        if ( pMsg->message == WM_MOUSEMOVE )
        {
            int xPos = GET_X_LPARAM(pMsg->lParam); 
            int yPos = GET_Y_LPARAM(pMsg->lParam); 

            POINT pt = {xPos, yPos};

            RECT rcWindow = { 0 };
            ::GetWindowRect(g_wndFav, &rcWindow);

            xPos -= rcWindow.left;
            yPos -= rcWindow.top;

            if ( ::PtInRect(&rcWindow, pt) )
            {
                ::SendMessage( g_wndFav, WM_MOUSEMOVE, pMsg->wParam, MAKELPARAM(xPos,yPos) );
            }
        }
        
        return lRet;
    }


    LRESULT OnDestroy(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        m_favSysItem.RemoveAll();

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

        CDWFavList& fList = CDWIEFavoritesMgt::Instance().GetFavoriteList();

        if ( !m_favMenu.IsMenu() )
        {
            m_favMenu.CreatePopupMenu();
            UINT uMenuId = 100;
            _CreateFavoriteMenu( m_favMenu.m_hMenu, fList, uMenuId );
        }

        ATLASSERT( m_nClickIndex>= 0 );
        if ( m_nClickIndex < 0 )
            return NULL;

        IEFavoriteItem& Item = fList[m_nClickIndex];

        if ( Item.pChildList == NULL )
            return NULL;

        menu = m_favMenu.GetSubMenu( m_nClickIndex );
        
        return menu;
    }

    protected:

    BOOL LoadMenuOrder( CDWMenuOrder& odr, LPCTSTR pszFavPath )
    {
        __try
        {
            return odr.LoadMenuOrder(pszFavPath);
        }
        __except(1)
        {
            return FALSE;
        }
    }

    void _SortFavMenuItem( CDWFavList& fList, LPCTSTR pszFavPath = NULL )
    {
        CDWMenuOrder odr;
        
        if ( !LoadMenuOrder(odr, pszFavPath) )
            return;

        for ( size_t idx = 0; idx < fList.GetCount(); idx++ )
        {
            IEFavoriteItem& item = fList[idx];

            if ( item.pChildList != NULL )
            {
                ATL::CString strFavPath = pszFavPath;
                strFavPath += L"\\";
                strFavPath += item.strTitle;

                _SortFavMenuItem(*item.pChildList, strFavPath );
            }
        }

        if ( odr.len <= 0 || fList.GetCount() <= 0 )
            return;

        int *index = new int[fList.GetCount()];
        if ( index == 0 )
            return;

        for ( int i = 0; i < (int)fList.GetCount(); i++ )
        {
            index[i] = odr.GetOrder(fList[i].strTitle);
            if ( index[i] < 0 )
                index[i] = odr.len+1;
        }

        for ( int i = 0; i < (int)fList.GetCount() - 1; i++ )
        {
            int k = 0;

            int nsmall = odr.len+2;

            for ( int j = i; j < (int)fList.GetCount(); j++ )
            {
                if ( index[j] <= nsmall )
                {
                    k = j;
                    nsmall = index[j];
                }
            }

            if ( i == k )
                continue;

            index[k] = index[i];
            index[i] = odr.len+3;

            IEFavoriteItem item = fList[i];
            fList[i] = fList[k];
            fList[k] = item;
        }

        delete []index;
    }

    HMENU _CreateFavoriteMenu( CMenuHandle menu, CDWFavList& fList, UINT& uMenuId )
    {
        ATLASSERT( menu.IsMenu() );
        if ( !menu.IsMenu() )
            return NULL;



        
        if ( menu.m_hMenu != m_favMenu.m_hMenu )
        {
            IEFavoriteItem sysitem( L"添加到此文件夹...", 0 );

            POSITION pos = m_favSysItem.AddTail(sysitem);

            menu.AppendMenu( MF_STRING, uMenuId, sysitem.strTitle );  
            m_mapMenuId[uMenuId++] = &m_favSysItem.GetAt(pos);
            menu.AppendMenu( MF_SEPARATOR , (UINT_PTR)ID_SEPARATOR, L"" );
        }
        
        for ( size_t idx = 0; idx < fList.GetCount(); idx++ )
        {
            IEFavoriteItem& item = fList[idx];
            
            if ( item.pChildList != NULL )
            {
                CMenuHandle subMenu;
                subMenu.CreatePopupMenu();

                _CreateFavoriteMenu(subMenu, *item.pChildList, uMenuId );

                menu.AppendMenu( MF_STRING, subMenu, item.strTitle );
            }
            else
            {
                menu.AppendMenu( MF_STRING, uMenuId, item.strTitle );
                m_mapMenuId[uMenuId++] = &item;
            }
        }

        return menu;
    }

    IEFavoriteItem* GetMenuFavItem( UINT uMenuId )
    {
        ATL::CAtlMap<UINT, IEFavoriteItem*>::CPair* 
            pFind = m_mapMenuId.Lookup( uMenuId );

        if ( pFind == NULL )
            return NULL;

        return pFind->m_value;
    }

    static DWORD WINAPI LoadFavoritesThread( LPVOID p )
    {
        CDWFavoriteBar* pthis = (CDWFavoriteBar*)p;

        CDWIEFavoritesMgt::Instance().BuildIEFavorites();
        
        if ( pthis->IsWindow() )
            pthis->PostMessage( WM_LOADFAVORITE_OK );

        return 0;
    }

    BOOL  m_bPopMenu;
    CMenu m_favMenu;

    ATL::CAtlMap<UINT, IEFavoriteItem*> m_mapMenuId;
    ATL::CAtlList<IEFavoriteItem> m_favSysItem;
public:

    DECLARE_WND_CLASS(_T("DWExplorer_FavoriteBar"));

};