
#pragma once

//#include <atlutil.h>
#include "DWComDef.h"
#include "DWToolbar.h"
#include "DWEventSvr.h"
#include "DWSearchMgt.h"
#include "DWFavIconMgt.h"
#include "DWMenu.h"

class CDWEdit : public CWindowImpl<CDWEdit, CEdit>
{
public:

    CDWEdit()
    {
    }
    
    BEGIN_MSG_MAP(CDWMainFrame)

        MESSAGE_HANDLER( WM_KEYDOWN    , OnKeyDown  )
        MESSAGE_HANDLER( WM_SETFOCUS   , OnSetFocus )
        MESSAGE_HANDLER( WM_LBUTTONDOWN, OnLButtonDown )

        MESSAGE_HANDLER( WM_PAINT      , OnPaint    )

    END_MSG_MAP()

    LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        LRESULT lResult = DefWindowProc();
        
        CWindow::Invalidate();
        CWindow(GetParent()).Invalidate();

        return lResult;
    }

    LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        LRESULT lResult = DefWindowProc();

        SetSelAll();
        CWindow::Invalidate();

        return lResult;
    }

    LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        BOOL bSelAll = FALSE;

        if ( GetFocus() != m_hWnd )
            bSelAll = TRUE;

        LRESULT lResult = DefWindowProc();

        if ( bSelAll )
            SetSelAll();

        return lResult;
    }

    LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
    {
        if ( GetWindowTextLength() > 0 ||
             ( m_strBkText.GetLength() <= 0 || GetFocus() == m_hWnd ) )
        {
            return DefWindowProc();
        }
        
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        CPaintDC dc(m_hWnd);


        COLORREF clrText = HLS_TRANSFORM(skin.clrFrameWindow, 60, 0);

        RECT rcClient;
        GetClientRect(&rcClient);

        dc.FillRect(&rcClient, 
            (HBRUSH)::SendMessage(GetParent(), WM_CTLCOLOREDIT, (WPARAM)dc.m_hDC, 0 ) );
        
        HFONT hOldFont = dc.SelectFont( GetFont() );
        
        rcClient.left  += 2;
        rcClient.right -= 2;
        dc.SetTextColor(clrText);
        dc.DrawText( m_strBkText, -1, &rcClient, DT_VCENTER | DT_SINGLELINE | DT_LEFT);
        
        dc.SelectFont( hOldFont );


        return 0L;
    }

    ATL::CString m_strBkText;
};

class CDWSuperToolbar : 
    public CDWToolbar, 
    public CDWEventCallback
{
public:

    void OpenURL( LPCTSTR URL )
    {
        if ( StrStrI( URL, L"." ) == NULL && 
             StrStrI( URL, L"://" ) == NULL &&
             StrStrI( URL, L":\\" ) == NULL )
        {
            OpenSerach(URL);
            return;
        }

        CDWEventSvr::Instance().OnMessage( edi_open_url, (WPARAM)URL, FALSE);
    }

    void OpenSerach( LPCTSTR pszKeyWord )
    {
        ATL::CString strURL = CDWSearchMgt::Instace().GetSearchURL( pszKeyWord );

        CDWEventSvr::Instance().OnMessage( edi_open_url, (WPARAM)(LPCTSTR)strURL, FALSE);
    }

    void GoOpenURL()
    {
        int nTextLen = m_address_edit.GetWindowTextLength();
        if ( nTextLen <= 0)
            return;

        WCHAR* URL = new WCHAR[ nTextLen + 1];
        m_address_edit.GetWindowText( URL, nTextLen + 1);

        OpenURL( URL );

        delete []URL;
    }

    void GoSerach()
    {
        int nTextLen = m_search_edit.GetWindowTextLength();
        if ( nTextLen <= 0)
            return;

        WCHAR* URL = new WCHAR[ nTextLen + 1];
        m_search_edit.GetWindowText( URL, nTextLen + 1);

        OpenSerach( URL );

        delete []URL;

    }

    BOOL PreTranslateMessage(MSG* pMsg)
    {
        if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN ) 
        {
            if ( pMsg->hwnd == m_address_edit )
            {
                GoOpenURL();
            }
            else if ( pMsg->hwnd == m_search_edit )
            {
                GoSerach();
            }
        }

        return FALSE;
    }

    BEGIN_MSG_MAP(CDWMainFrame)

        MESSAGE_HANDLER( WM_CREATE    , OnCreate     )
        MESSAGE_HANDLER( WM_ERASEBKGND, OnEraseBkGnd )
        MESSAGE_HANDLER( WM_SIZE      , OnSize       )

        MESSAGE_HANDLER( WM_CTLCOLORBTN   , OnCtlColor )
        MESSAGE_HANDLER( WM_CTLCOLORSTATIC, OnCtlColor )

        MESSAGE_HANDLER( WM_CTLCOLOREDIT, OnCtlColor )
        MESSAGE_HANDLER( WM_CTLCOLORDLG , OnCtlColor )

        MESSAGE_HANDLER(WM_LBUTTONDOWN      , OnLButtonDown)
        MESSAGE_HANDLER(WM_FAV_ICON_REFLASH , OnFavIconReflash )

        CHAIN_MSG_MAP(CDWToolbar)

    END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        m_address_edit.Create(m_hWnd, &rcDefault, NULL, 
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_WANTRETURN | WS_TABSTOP );
        m_search_edit .Create(m_hWnd, &rcDefault, NULL, 
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_WANTRETURN | WS_TABSTOP );

        m_address_edit.SetFont( skin.fontDefault );
        m_search_edit.SetFont( skin.fontDefault );

        m_address_edit.m_strBkText = L"��������������ַ";
        m_search_edit.m_strBkText  = CDWSearchMgt::Instace().GetSearchName();

        m_icon_search = CDWFavIconMgt::Instance().GetFavIcon( 
            CDWSearchMgt::Instace().GetSearchURL(), m_hWnd, NULL );


        CDWEventSvr::Instance().AddCallback( this );

        bHandled = FALSE;
        return 0L;
    }

    virtual void DoAfterPaint ( HDC hDC )
    {
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        RECT rcClient = { 0 };
        GetClientRect(&rcClient);

        CMemoryDC dc(hDC, rcClient);


        CreateBkBrush();

        dc.FillSolidRect( &rcClient, skin.clrFrameWindow );

        if ( m_icon_addr.IsNull() )
            m_icon_addr = skin.iconNull;
        if ( m_icon_search.IsNull() )
            m_icon_search = skin.iconNull;

        COLORREF clrBorder = HLS_TRANSFORM(skin.clrFrameWindow, 60, 0);

        CPen pen;
        pen.CreatePen( PS_SOLID, 1, clrBorder );

        HPEN   hOldPen   = dc.SelectPen( pen );
        HBRUSH hOldBrush = dc.SelectBrush( IsHttps() ? m_bkSBrush : m_bkBrush );

        m_address_edit.GetWindowRect(&rcClient);
        ScreenToClient(&rcClient);
        InflateRect(&rcClient, 2, 2);
        rcClient.top  -= 2;
        rcClient.left -= 18;

        POINT pt = { 5, 5 };

        dc.RoundRect(&rcClient, pt);
        m_icon_addr.DrawIconEx( dc, rcClient.left + 4, rcClient.top + 3, 16, 16 );

        InflateRect(&rcClient, 1, 1);

        dc.SelectBrush( m_bkBrush );
        m_search_edit.GetWindowRect(&rcClient);
        ScreenToClient(&rcClient);
        InflateRect(&rcClient, 2, 2);
        rcClient.top  -= 2;
        rcClient.left -= 23;

        dc.RoundRect(&rcClient, pt);
        m_icon_search.DrawIconEx( dc, rcClient.left + 4, rcClient.top + 3, 16, 16 );

        RECT rcImage = { 0, 0, 0, 0 };
        rcImage.right  = skin.png_dropdown.GetWidth() / 4;
        rcImage.bottom = skin.png_dropdown.GetHeight();
        skin.png_dropdown.AlphaDraw( dc, rcClient.left + 18, rcClient.top + 10, &rcImage, 1, 1.2f );

        dc.SelectPen(hOldPen);
        dc.SelectBrush(hOldBrush);
    }


    LRESULT OnEraseBkGnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return 1L;
    }

    LRESULT OnFavIconReflash(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        m_icon_search = (HICON)wParam;
        Invalidate();

        return 1L;
    }

    template <bool t_bManaged>
    class CDWSprMenuT : public CDWMenuT<t_bManaged>
    {
    public:
        CDWSprMenuT(HMENU hMenu = NULL) : CDWMenuT(hMenu)
        { }

        virtual void _OnDrawMenuIcon( CDCHandle& dc, LPDRAWITEMSTRUCT lpDrawItem, int nSelected )
        {
            CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

            CIconHandle icon;
            
            icon.m_hIcon = CDWFavIconMgt::Instance().GetFavIcon( 
                CDWSearchMgt::Instace().GetSearchURL( lpDrawItem->itemID - 10 ), NULL, NULL );

            if ( icon.m_hIcon == NULL )
                icon.m_hIcon = skin.iconNull;

            icon.DrawIconEx( dc,
                lpDrawItem->rcItem.left + 5,
                lpDrawItem->rcItem.top  + 2,
                16, 16);
        }
    };

    LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CDWToolbar::OnLButtonDown(uMsg, wParam, lParam, bHandled);
        bHandled = TRUE;

        RECT rcSearchIcon = { 0 };

        m_search_edit.GetWindowRect(&rcSearchIcon);
        ScreenToClient(&rcSearchIcon);
        rcSearchIcon.top  -= 2;
        rcSearchIcon.left -= 22;
        rcSearchIcon.right = rcSearchIcon.left + 26;
        
       
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

        if ( ::PtInRect(&rcSearchIcon, pt ) )
        {
            CDWSearchMgt& smgt = CDWSearchMgt::Instace();
            
            CDWSprMenuT<TRUE> menu;
            
            menu.Attach( smgt.CreatePopueMenu() );

            ClientToScreen(&pt);

            int nCmdId = menu.DWTrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD,
                pt.x, pt.y,
                m_hWnd);
            if ( nCmdId >= 10 )
            {
                smgt.m_nSelIndex = nCmdId - 10;
                m_search_edit.m_strBkText  = smgt.GetSearchName();

                m_icon_search = CDWFavIconMgt::Instance().GetFavIcon( smgt.GetSearchURL(), m_hWnd, NULL );
                m_search_edit.Invalidate();
                Invalidate();

                if ( m_search_edit.GetWindowTextLength() > 0 )
                {
                    GoSerach();
                }
            }
        }

        return 1L;
    }

    BOOL IsHttps()
    {
        TCHAR szText[10] = { 0 };

        m_address_edit.GetWindowText(szText, 9);

        return !StrCmpNI(szText, L"https://", 8 );
    }

    void CreateBkBrush()
    {
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        if ( m_bkBrush.m_hBrush == NULL )
            m_bkBrush.CreateSolidBrush( HLS_TRANSFORM(skin.clrFrameWindow, 20, 0) );
        
        if ( m_bkSBrush.m_hBrush == NULL )
        {
            COLORREF clrOld = HLS_TRANSFORM(skin.clrFrameWindow, 40, 0 );

            BYTE r,g,b;
            
            r = GetRValue(clrOld);
            g = GetGValue(clrOld);
            b = GetBValue(clrOld);
            
            r += 60;
            r = r < 60 ? 255 : r;

            m_bkSBrush.CreateSolidBrush( RGB(r,g,b) );
        }
    }

    LRESULT OnCtlColor(UINT /* uMsg */, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
    {
        CDCHandle dc = (HDC)wParam;
        dc.SetBkMode( TRANSPARENT );
        
        CreateBkBrush();

        if ( (HWND)lParam == m_address_edit.m_hWnd && IsHttps() )
            return (LRESULT)m_bkSBrush.m_hBrush;

        return (LRESULT)m_bkBrush.m_hBrush;
    }


    LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;

        RECT rcClient = { 0 };
        GetClientRect(&rcClient);
        
        ::InflateRect(&rcClient, -2, -2);
        rcClient.top+=3;
        rcClient.right -= 5 ;

        RECT rcAddr   = rcClient;
        RECT rcSerach = rcClient;

        rcSerach.left = ( rcClient.right / 4 );
        if ( rcSerach.left > 220 )
            rcSerach.left = 220;
        else if ( rcSerach.left < 100 )
            rcSerach.left = 100;
        
        rcSerach.left = rcSerach.right - rcSerach.left;        

        rcAddr.right = rcSerach.left - 8;

        rcAddr.left += 22;
        rcSerach.left += 22;

        
        m_address_edit.MoveWindow(&rcAddr);
        m_search_edit .MoveWindow(&rcSerach);

        return 1L;
    }

    LRESULT OnEventMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
    {
        if ( uMsg == eid_addr_changed )
        {
            m_address_edit.SetWindowText((LPCTSTR)wParam);
            CWindow::Invalidate();
        }
        else if ( uMsg == edi_skin_changed )
        {
            if (!m_bkBrush.IsNull())
                m_bkBrush.DeleteObject();
            if (!m_bkSBrush.IsNull())
                m_bkSBrush.DeleteObject();
        }
        else if ( uMsg == edi_spr_icon_changed )
        {
            if ( wParam != NULL )
                m_icon_addr = (HICON)wParam;
            if ( lParam != NULL )
                m_icon_search = (HICON)lParam;

            if ( wParam != NULL || lParam != NULL )
                Invalidate();
        }

        return 0;
    }

public:

    WTL::CBrush m_bkBrush;
    WTL::CBrush m_bkSBrush;

    CDWEdit m_address_edit;
    CDWEdit m_search_edit;

    CIconHandle m_icon_addr;
    CIconHandle m_icon_search;
};