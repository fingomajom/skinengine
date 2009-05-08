#pragma once

#include "DWSkinUIMgt.h"
#include "DWIEFavoritesMgt.h"


template <bool t_bManaged> class CDWMenuT;
typedef CDWMenuT<false>   CDWMenuHandle;
typedef CDWMenuT<true>    CDWMenu;


template <bool t_bManaged>
class CDWMenuT : public CMenuT<t_bManaged>
{
    static HHOOK   s_hMenuCreateHook;
    static WNDPROC s_pOldWindowProc;
    static HWND    s_hWndTrackMenu;
    static CDWMenuT<t_bManaged>* s_pThis;
    
    static ATL::CSimpleArray<ATL::CString> s_MenuItemString;

    enum{
        menu_max_wdith  = 250,
        menu_icon_width = 22,
        menu_text_space = 4
    };

public:

    CDWMenuT(HMENU hMenu = NULL) : CMenuT(hMenu)
    { }

    BOOL DWTrackPopupMenu( UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm = NULL)
    {
        ATLASSERT ( s_hMenuCreateHook == NULL );
        ATLASSERT ( s_pOldWindowProc  == NULL );
        ATLASSERT ( s_hWndTrackMenu   == NULL );
        ATLASSERT ( s_pThis           == NULL );

        s_hWndTrackMenu = hWnd;
        s_pThis         = this;

        s_pOldWindowProc = (WNDPROC)SetWindowLong( hWnd, GWL_WNDPROC, (LONG)NewMenuProc);

        s_hMenuCreateHook = ::SetWindowsHookEx(WH_CBT, 
            DWMenuCreateHookProc, 
            _Module.GetModuleInstance(), 
            GetCurrentThreadId());

        ATLASSERT(s_hMenuCreateHook);

        BOOL bRet = TrackPopupMenuEx( uFlags, x, y, hWnd, lptpm );
        s_MenuItemString.RemoveAll();

        UnhookWindowsHookEx(s_hMenuCreateHook);
        SetWindowLong( hWnd, GWL_WNDPROC, (LONG)s_pOldWindowProc);
        s_hMenuCreateHook = NULL;
        s_pOldWindowProc = NULL;
        s_hWndTrackMenu = NULL;
        s_pThis = NULL;
        
        return bRet;
    }

    static LRESULT WINAPI NewMenuProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
    {
        ATLASSERT ( hWnd == s_hWndTrackMenu  );
        ATLASSERT ( NULL != s_pOldWindowProc );
        ATLASSERT ( NULL != s_pThis );

        LRESULT lResult  = 0L;
        BOOL    bHandled = FALSE;

        switch ( uMsg )
        {
        case WM_INITMENU:
            bHandled = TRUE;
            lResult = s_pThis->OnInitMenu(uMsg, wParam, lParam, bHandled);
            break;
        case WM_MEASUREITEM:
            bHandled = TRUE;
            lResult = s_pThis->OnMeasureItem(uMsg, wParam, lParam, bHandled);
            break;
        case WM_DRAWITEM:
            bHandled = TRUE;
            lResult = s_pThis->OnDrawItem(uMsg, wParam, lParam, bHandled);
            break;
        }
        
        if ( bHandled )
            return lResult;

        return CallWindowProc(s_pOldWindowProc, hWnd, uMsg, wParam, lParam );
    }

    virtual LRESULT OnInitMenu(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
    {
        CMenuHandle menu = (HMENU)wParam;
        
        _InitMenu(menu);

        return 0L;    
    }

    virtual void _MeasureItemText( LPMEASUREITEMSTRUCT lpMeasureItem )
    {
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        CClientDC dc(s_hWndTrackMenu);

        SIZE sizeText = { 0 };
        HFONT hOld = dc.SelectFont(skin.fontDefault);

        dc.GetTextExtent( (LPCTSTR)lpMeasureItem->itemData, -1, &sizeText);

        dc.SelectFont(hOld);

        lpMeasureItem->itemWidth = sizeText.cx;
    }

    virtual LRESULT OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LPMEASUREITEMSTRUCT lpMeasureItem = (LPMEASUREITEMSTRUCT) lParam;

        if(lpMeasureItem->CtlType == ODT_MENU)
        {
            if(lpMeasureItem->itemID != ID_SEPARATOR)    
            {
                lpMeasureItem->itemHeight = 21;

                _MeasureItemText(lpMeasureItem);

                lpMeasureItem->itemWidth  = lpMeasureItem->itemWidth > menu_max_wdith ? 
                                            menu_max_wdith :  lpMeasureItem->itemWidth;
                lpMeasureItem->itemWidth += menu_icon_width;
                lpMeasureItem->itemWidth += menu_text_space;
            }	
            else	
            {		
                lpMeasureItem->itemHeight = 8;     
                lpMeasureItem->itemWidth  = 100;	
            }
        }

        return 1L;
    }

    virtual void _OnDrawMenuBkGnd( CDCHandle& dc, const RECT& rcBox, int nSelected )
    {
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        if ( nSelected )
            dc.FillSolidRect( &rcBox, skin.clrFrameWindow );
        else
            dc.FillSolidRect( &rcBox, HLS_TRANSFORM(skin.clrFrameWindow, 80, 0));
    }

    virtual void _OnDrawMenuIcon( CDCHandle& dc, LPDRAWITEMSTRUCT lpDrawItem, int nSelected )
    {
    }


    virtual LRESULT OnDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
    {
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        TCHAR szBuffer[MAX_PATH] = { 0 };

        LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT) lParam;

        if( lpDrawItem->CtlType != ODT_MENU )	
            return 0L;

        CDCHandle dc = lpDrawItem->hDC;

        if ( lpDrawItem->itemID != ID_SEPARATOR )
        {
            _OnDrawMenuBkGnd(dc, lpDrawItem->rcItem, 
                ((lpDrawItem->itemAction & ODA_SELECT) && (lpDrawItem->itemState & ODS_SELECTED)));  

            int nBkMode = dc.SetBkMode(TRANSPARENT);
            HFONT hOld = dc.SelectFont(skin.fontDefault);

            CMenuHandle menu = (HMENU)lpDrawItem->hwndItem;

            menu.GetMenuString( lpDrawItem->itemID, szBuffer, MAX_PATH, MF_BYCOMMAND);

            _OnDrawMenuIcon( dc, lpDrawItem, FALSE );

            RECT rcText   = lpDrawItem->rcItem;
            rcText.left  += menu_icon_width;
            rcText.left  += menu_text_space;
            rcText.right -= menu_text_space;

            dc.DrawText( szBuffer, -1, 
                &rcText, 
                DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS );

            dc.SetBkMode(nBkMode);
            dc.SelectFont(hOld);
        }
        else
        {
            _OnDrawMenuBkGnd(dc, lpDrawItem->rcItem, FALSE);  
            
            RECT rcSeparator = lpDrawItem->rcItem;
            rcSeparator.top = rcSeparator.bottom = ( rcSeparator.top + rcSeparator.bottom ) / 2;
            
            rcSeparator.left  += 5;
            rcSeparator.right -= 5;

            CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

            COLORREF clrL    = HLS_TRANSFORM( skin.clrFrameWindow, +50, 0 );
            COLORREF clrS    = HLS_TRANSFORM( skin.clrFrameWindow, -50, 0 );

            dc.Draw3dRect( &rcSeparator, clrL, clrS );
        }



        return 0L;    
    }

    virtual void _InitMenu( CMenuHandle menu ) 
    {
        TCHAR szBuffer[MAX_PATH] = { 0 };

        int nCount = menu.GetMenuItemCount();

        for( int i = 0; i < nCount; i++)             
        {
            CMenuHandle sub = menu.GetSubMenu( i );

            UINT id = menu.GetMenuItemID(i);
            
            menu.GetMenuString( i, szBuffer, MAX_PATH, MF_BYPOSITION);
            s_MenuItemString.Add(szBuffer);

            LPCTSTR lpszNewItem = (LPCTSTR)s_MenuItemString[s_MenuItemString.GetSize()-1];
            
            if ( sub.IsMenu() )
            {
                _InitMenu( sub );
                menu.ModifyMenu( i, MF_BYPOSITION | MF_OWNERDRAW , sub,  lpszNewItem);
            }
            else
            {
                menu.ModifyMenu( i, MF_BYPOSITION | MF_OWNERDRAW, id, lpszNewItem );
            }	
        }
    }

    class CDWMenuWindow : public CWindowImpl<CDWMenuWindow>
    {
    public:

        BOOL SubclassWindow(HWND hWnd)
        {
            BOOL bRet = CWindowImpl<CDWMenuWindow>::SubclassWindow(hWnd);
            if ( !bRet )
                return bRet;

            SetWindowLong( GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_TOPMOST );                

            return bRet;

        }

        BEGIN_MSG_MAP(CDWMenuWindow)

            MESSAGE_HANDLER(WM_NCPAINT   , OnNcPaint    )
            MESSAGE_HANDLER(WM_NCCALCSIZE, OnNcCalcSize )
        
        END_MSG_MAP();

        LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
        {
            CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

            CWindowDC wndDC(m_hWnd);

            RECT rcWindow;
            GetWindowRect(&rcWindow);

            rcWindow.right  = rcWindow.right  - rcWindow.left;
            rcWindow.bottom = rcWindow.bottom - rcWindow.top;
            rcWindow.top    = rcWindow.left = 0;

            COLORREF clrBorder = HLS_TRANSFORM(skin.clrFrameWindow, -40, 0);
            COLORREF clrMenu   = HLS_TRANSFORM(skin.clrFrameWindow, 80, 0);
            wndDC.Draw3dRect( &rcWindow, clrBorder, clrBorder);
            ::InflateRect(&rcWindow, -1, -1 );                
            wndDC.Draw3dRect( &rcWindow, clrMenu, clrMenu);
            ::InflateRect(&rcWindow, -1, -1 );                
            wndDC.Draw3dRect( &rcWindow, clrMenu, clrMenu);

            return 0;
        }

        void RepareMenuWindowSize()
        {
            RECT rcClient = { 0 };
            GetClientRect(&rcClient);

            SetWindowPos( NULL, 
                0, 0, 
                rcClient.right  - rcClient.left + 2,
                rcClient.bottom - rcClient.top + 2,
                SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_NOOWNERZORDER);

        }

        LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
        {
            LPNCCALCSIZE_PARAMS pNCParams = (LPNCCALCSIZE_PARAMS)lParam;

            RECT rcWindow = pNCParams->rgrc[0];

            LRESULT lResult = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
            if (wParam)
            {
                RECT& rcClient = pNCParams->rgrc[0];

                ::InflateRect(&rcClient, -2, -2 );                
            }

            return lResult;
        }


        virtual void OnFinalMessage(HWND /*hWnd*/)
        {
            delete this;
        }
    };

    static LRESULT CALLBACK DWMenuCreateHookProc(int nCode, WPARAM wParam, LPARAM lParam)
    {
        const int cchClassName = 7;
        TCHAR szClassName[cchClassName] = { 0 };

        LRESULT lResult = ::CallNextHookEx(s_hMenuCreateHook, nCode, wParam, lParam);

        if(nCode == HCBT_CREATEWND)
        {
            CWindow wndMenu = (HWND)wParam;
            
            ::GetClassName(wndMenu, szClassName, cchClassName);
            if(!StrCmpI(_T("#32768"), szClassName))
            {
                CDWMenuWindow* pWnd = new CDWMenuWindow();
                ATLASSERT(pWnd);
                if ( pWnd )
                    pWnd->SubclassWindow(wndMenu);
            
            }
        }
        else if(nCode == HCBT_DESTROYWND)
        {
            CWindow wndMenu = (HWND)wParam;

            ::GetClassName(wndMenu, szClassName, cchClassName);
            if(!StrCmpI(_T("#32768"), szClassName))
            {
            }
        }
        

        return lResult;
    }

};

template <bool t_bManaged> __declspec(selectany) HHOOK   CDWMenuT<t_bManaged>::s_hMenuCreateHook = NULL;
template <bool t_bManaged> __declspec(selectany) WNDPROC CDWMenuT<t_bManaged>::s_pOldWindowProc  = NULL;
template <bool t_bManaged> __declspec(selectany) HWND    CDWMenuT<t_bManaged>::s_hWndTrackMenu   = NULL;
template <bool t_bManaged> __declspec(selectany) CDWMenuT<t_bManaged>* CDWMenuT<t_bManaged>::s_pThis = NULL;
template <bool t_bManaged> __declspec(selectany) ATL::CSimpleArray<ATL::CString> CDWMenuT<t_bManaged>::s_MenuItemString;
