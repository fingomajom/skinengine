#pragma once

#include "DWSkinUIMgt.h"
#include "DWIEFavoritesMgt.h"

#include <atlcrack.h>

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
    
    static ATL::CAtlList<ATL::CString> s_MenuItemString;

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

        uFlags |= TPM_NOANIMATION;

        s_hMenuCreateHook = ::SetWindowsHookEx(WH_CALLWNDPROC, 
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
        CDWSkinUIMgt* pskin = CDWSkinUIMgt::InstancePtr();
        if ( pskin == NULL )
            return;

        CClientDC dc(s_hWndTrackMenu);

        SIZE sizeText = { 0 };
        HFONT hOld = dc.SelectFont(pskin->fontDefault);

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
        CDWSkinUIMgt* pskin = CDWSkinUIMgt::InstancePtr();
        if ( pskin == NULL )
            return;

        if ( nSelected )
            dc.FillSolidRect( &rcBox, pskin->clrFrameWindow );
        else
            dc.FillSolidRect( &rcBox, HLS_TRANSFORM(pskin->clrFrameWindow, 80, 0));
    }

    virtual void _OnDrawMenuIcon( CDCHandle& dc, LPDRAWITEMSTRUCT lpDrawItem, int nSelected )
    {
    }


    virtual LRESULT OnDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
    {
        CDWSkinUIMgt* pskin = CDWSkinUIMgt::InstancePtr();
        if ( pskin == NULL )
            return 1L;

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
            HFONT hOld = dc.SelectFont(pskin->fontDefault);

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

            COLORREF clrL    = HLS_TRANSFORM( pskin->clrFrameWindow, +50, 0 );
            COLORREF clrS    = HLS_TRANSFORM( pskin->clrFrameWindow, -50, 0 );

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
            POSITION pos = s_MenuItemString.AddTail(szBuffer);

            LPCTSTR lpszNewItem = (LPCTSTR)s_MenuItemString.GetAt(pos);
            
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

            //SetWindowLong( GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_TOPMOST );   

            return bRet;

        }


        BEGIN_MSG_MAP_EX(CDWMenuWindow)
            MESSAGE_HANDLER(WM_PRINT     , OnPrint      )
            MESSAGE_HANDLER(WM_NCPAINT   , OnNcPaint    )
            MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkGnd )
        END_MSG_MAP()

        //////////////////////////////////////////////////////////////////////////

        LRESULT OnPrint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
        {
            DefWindowProc();

            CDCHandle dc(((HDC)wParam));

            CDWSkinUIMgt* pskin = CDWSkinUIMgt::InstancePtr();
            if ( pskin == NULL )
                return 1L;

            RECT rcWindow;
            GetWindowRect(&rcWindow);

            rcWindow.right  = rcWindow.right  - rcWindow.left;
            rcWindow.bottom = rcWindow.bottom - rcWindow.top;
            rcWindow.top    = rcWindow.left = 0;

            COLORREF clrBorder = HLS_TRANSFORM(pskin->clrFrameWindow, -40, 0);
            COLORREF clrMenu   = HLS_TRANSFORM(pskin->clrFrameWindow, 80, 0);
            dc.Draw3dRect( &rcWindow, clrBorder, clrBorder);
            ::InflateRect(&rcWindow, -1, -1 );                
            dc.Draw3dRect( &rcWindow, clrMenu, clrMenu);
            ::InflateRect(&rcWindow, -1, -1 );                
            dc.Draw3dRect( &rcWindow, clrMenu, clrMenu);

            return 1L;
        }

        LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
        {
            CDWSkinUIMgt* pskin = CDWSkinUIMgt::InstancePtr();
            if ( pskin == NULL )
                return 1L;

            CWindowDC wndDC(m_hWnd);

            RECT rcWindow;
            GetWindowRect(&rcWindow);

            rcWindow.right  = rcWindow.right  - rcWindow.left;
            rcWindow.bottom = rcWindow.bottom - rcWindow.top;
            rcWindow.top    = rcWindow.left = 0;

            COLORREF clrBorder = HLS_TRANSFORM(pskin->clrFrameWindow, -40, 0);
            COLORREF clrMenu   = HLS_TRANSFORM(pskin->clrFrameWindow, 80, 0);
            wndDC.Draw3dRect( &rcWindow, clrBorder, clrBorder);
            ::InflateRect(&rcWindow, -1, -1 );                
            wndDC.Draw3dRect( &rcWindow, clrMenu, clrMenu);
            ::InflateRect(&rcWindow, -1, -1 );                
            wndDC.Draw3dRect( &rcWindow, clrMenu, clrMenu);

            return 0;
        }

        LRESULT OnEraseBkGnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
        {
            return 1L;
        }

        virtual void OnFinalMessage(HWND /*hWnd*/)
        {
            delete this;
        }
    };

    static LRESULT CALLBACK DWMenuCreateHookProc(int nCode, WPARAM wParam, LPARAM lParam)
    {
        if (nCode == HC_ACTION)
        {
            CWPSTRUCT *pwp = (CWPSTRUCT*)lParam;

            switch (pwp->message)
            {
            case 0x1e2:
                {
                    CWindow wndMenu = (HWND)pwp->hwnd;

                    TCHAR szClassName[128];

                    ::GetClassName(wndMenu, szClassName, 127);
                    if(!StrCmpI(_T("#32768"), szClassName))
                    {
                        CDWMenuWindow* pWnd = new CDWMenuWindow();
                        ATLASSERT(pWnd);
                        if ( pWnd )
                            pWnd->SubclassWindow(wndMenu);            
                    }
                }
            }
        }

        return CallNextHookEx(s_hMenuCreateHook, nCode, wParam, lParam);
    }

};

template <bool t_bManaged> __declspec(selectany) HHOOK   CDWMenuT<t_bManaged>::s_hMenuCreateHook = NULL;
template <bool t_bManaged> __declspec(selectany) WNDPROC CDWMenuT<t_bManaged>::s_pOldWindowProc  = NULL;
template <bool t_bManaged> __declspec(selectany) HWND    CDWMenuT<t_bManaged>::s_hWndTrackMenu   = NULL;
template <bool t_bManaged> __declspec(selectany) CDWMenuT<t_bManaged>* CDWMenuT<t_bManaged>::s_pThis = NULL;
template <bool t_bManaged> __declspec(selectany) ATL::CAtlList<ATL::CString> CDWMenuT<t_bManaged>::s_MenuItemString;
