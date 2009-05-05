#pragma once

#include "DWSkinUIMgt.h"
#include "DWIEFavoritesMgt.h"


static HHOOK s_hMenuCreateHook = NULL;

template<class T>
class CDWMenuImpl
{

public:

    enum{
        menu_max_wdith  = 250,
        menu_icon_width = 22,
        menu_text_space = 4
    };

    BOOL TrackPopupMenu( CMenuHandle menu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm = NULL)
    {
        s_hMenuCreateHook = ::SetWindowsHookEx(WH_CBT, 
            DWMenuCreateHookProc, 
            _Module.GetModuleInstance(), 
            GetCurrentThreadId());

        ATLASSERT(s_hMenuCreateHook);

        BOOL bRet = menu.TrackPopupMenuEx( uFlags, x, y, hWnd, lptpm);

        UnhookWindowsHookEx(s_hMenuCreateHook);
        
        return bRet;
    }

    BEGIN_MSG_MAP(CDWMenuImpl)

        MESSAGE_HANDLER(WM_INITMENU    , OnInitMenu    )
        MESSAGE_HANDLER(WM_MEASUREITEM , OnMeasureItem )
        MESSAGE_HANDLER(WM_DRAWITEM    , OnDrawItem    )

    END_MSG_MAP();

    LRESULT OnInitMenu(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
    {
        CMenuHandle menu = (HMENU)wParam;
        
        _InitMenu(menu);


        return 0L;    
    }

    LRESULT OnMeasureItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
    {
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        LPMEASUREITEMSTRUCT lpMeasureItem = (LPMEASUREITEMSTRUCT) lParam;

        if(lpMeasureItem->CtlType == ODT_MENU)
        {
            if(lpMeasureItem->itemID != ID_SEPARATOR)    
            {
                lpMeasureItem->itemHeight = 21;

                CClientDC dc(*(T*)this);
                SIZE sizeText = { 0 };
                HFONT hOld = dc.SelectFont(skin.fontDefault);

                ATL::CString strTitle;

                IEFavoriteItem* pitem = (IEFavoriteItem*)lpMeasureItem->itemID;
                if ( !::IsMenu( (HMENU)pitem ) )
                    strTitle = pitem->strTitle;

                dc.GetTextExtent( strTitle, -1, &sizeText);
                
                dc.SelectFont(hOld);

                lpMeasureItem->itemWidth  = sizeText.cx > menu_max_wdith ? menu_max_wdith :  sizeText.cx;
                lpMeasureItem->itemWidth += menu_icon_width;
                lpMeasureItem->itemWidth += menu_text_space;
            }	
            else	
            {		
                lpMeasureItem->itemHeight = 8;     
                lpMeasureItem->itemWidth  = 150;	
            }
        }

        return 1L;
    }

    virtual void OnDrawMenuBkGnd( CDCHandle& dc, const RECT& rcBox, int nSelected )
    {
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        if ( nSelected )
            dc.FillSolidRect( &rcBox, skin.clrFrameWindow );
        else
            dc.FillSolidRect( &rcBox, HLS_TRANSFORM(skin.clrFrameWindow, 80, 0));
    }

    LRESULT OnDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
    {
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        TCHAR szBuffer[MAX_PATH] = { 0 };

        LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT) lParam;

        if( lpDrawItem->CtlType != ODT_MENU )	
            return 0L;

        CDCHandle dc = lpDrawItem->hDC;

        OnDrawMenuBkGnd(dc, lpDrawItem->rcItem, 
            ((lpDrawItem->itemAction & ODA_SELECT) && (lpDrawItem->itemState & ODS_SELECTED)));  

        int nBkMode = dc.SetBkMode(TRANSPARENT);
        HFONT hOld = dc.SelectFont(skin.fontDefault);

        CMenuHandle menu = (HMENU)lpDrawItem->hwndItem;

        menu.GetMenuString( lpDrawItem->itemID, szBuffer, MAX_PATH, MF_BYCOMMAND);

        CIconHandle icon = skin.iconNull;
        if ( ::IsMenu((HMENU)lpDrawItem->itemID) )
            icon = skin.iconFavDir;

        icon.DrawIconEx( dc,
            lpDrawItem->rcItem.left + 5,
            lpDrawItem->rcItem.top  + 2,
            16, 16);

        RECT rcText   = lpDrawItem->rcItem;
        rcText.left  += menu_icon_width;
        rcText.left  += menu_text_space;
        rcText.right -= menu_text_space;

        dc.DrawText( szBuffer, -1, 
            &rcText, 
            DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS );

        dc.SetBkMode(nBkMode);
        dc.SelectFont(hOld);


        return 0L;    
    }


    void _InitMenu( CMenuHandle menu ) 
    {
        TCHAR szBuffer[MAX_PATH] = { 0 };

        int nCount = menu.GetMenuItemCount();

        for( int i = 0; i < nCount; i++)             
        {
            CMenuHandle sub = menu.GetSubMenu( i );

            UINT id = menu.GetMenuItemID(i);
            menu.GetMenuString( i, szBuffer, MAX_PATH, MF_BYPOSITION);
            
            if ( sub.IsMenu() )
            {
                _InitMenu( sub );
                menu.ModifyMenu( i, MF_BYPOSITION | MF_OWNERDRAW | MF_STRING, sub, szBuffer);
            }
            else
            {
                menu.ModifyMenu( i, MF_BYPOSITION | MF_OWNERDRAW| MF_STRING, id, szBuffer );
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

            //MESSAGE_HANDLER(WM_SIZE, OnSize  )
        
        END_MSG_MAP();

        LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
        {
            RECT rcClient = { 0 };
            RECT rcWindow = { 0 };

            GetClientRect(&rcClient);
            GetWindowRect(&rcWindow);

            if ( ( rcWindow.right - rcWindow.left - ( rcClient.right - rcWindow.left )) > 2 )
                RepareMenuWindowSize();


            return DefWindowProc();
        }

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
            if(!lstrcmp(_T("#32768"), szClassName))
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
            if(!lstrcmp(_T("#32768"), szClassName))
            {
            }
        }
        

        return lResult;
    }

};

