
#pragma once

#include <atlutil.h>
#include "DWToolbar.h"
#include "DWEventSvr.h"


class CDWEdit : public CWindowImpl<CDWEdit, CEdit>
{
public:
    BEGIN_MSG_MAP(CDWMainFrame)

        MESSAGE_HANDLER( WM_KEYDOWN    , OnKeyDown  )
        MESSAGE_HANDLER( WM_SETFOCUS   , OnSetFocus )
        MESSAGE_HANDLER( WM_LBUTTONDOWN, OnLButtonDown )

    END_MSG_MAP()

    LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        LRESULT lResult = DefWindowProc();
        
        CWindow::Invalidate();

        return lResult;
    }

    LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        LRESULT lResult = DefWindowProc();

        SetSelAll();

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

};

class CDWSuperToolbar : 
    public CDWToolbar, 
    public CDWEventCallback
{
public:

    static CStringA EscapeUrl( LPCTSTR pszURL )
    {
        CStringA strURL;
        CStringA strResult;

        strURL = pszURL;

        DWORD dwLen = strURL.GetLength() * 8 + 1;

        LPSTR pszBuffer = strResult.GetBuffer( dwLen + 1 );
        if ( pszBuffer == NULL )
            return strResult;


        AtlEscapeUrl(strURL, pszBuffer, &dwLen, dwLen, ATL_URL_ENCODE_PERCENT);
        strResult.ReleaseBuffer();

        return strResult;
    }


    void OpenURL( LPCTSTR URL )
    {
        if ( StrStrI( URL, L"." ) == NULL )
        {
            OpenSerach(URL);
            return;
        }

        CDWEventSvr::Instance().OnMessage( edi_open_url, (WPARAM)URL, FALSE);
    }

    void OpenSerach( LPCTSTR pszKeyWord )
    {
        ATL::CString strURL;
        ATL::CString strKeyword = EscapeUrl(pszKeyWord);

        strURL.Format(L"http://www.baidu.com/s?wd=%s", strKeyword);

        CDWEventSvr::Instance().OnMessage( edi_open_url, (WPARAM)(LPCTSTR)strURL, FALSE);
    }

    BOOL PreTranslateMessage(MSG* pMsg)
    {
        if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN ) 
        {
            if ( pMsg->hwnd == m_address_edit )
            {
                int nTextLen = m_address_edit.GetWindowTextLength();
                if ( nTextLen <= 0)
                    return FALSE;

                WCHAR* URL = new WCHAR[ nTextLen + 1];
                m_address_edit.GetWindowText( URL, nTextLen + 1);

                OpenURL( URL );

                delete []URL;
            }
            else if ( pMsg->hwnd == m_serach_edit )
            {
                int nTextLen = m_serach_edit.GetWindowTextLength();
                if ( nTextLen <= 0)
                    return FALSE;

                WCHAR* URL = new WCHAR[ nTextLen + 1];
                m_serach_edit.GetWindowText( URL, nTextLen + 1);

                OpenSerach( URL );

                delete []URL;
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

        CHAIN_MSG_MAP(CDWToolbar)

    END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        m_address_edit.Create(m_hWnd, &rcDefault, NULL, WS_CHILD | WS_VISIBLE | ES_WANTRETURN | WS_TABSTOP );
        m_serach_edit .Create(m_hWnd, &rcDefault, NULL, WS_CHILD | WS_VISIBLE | ES_WANTRETURN | WS_TABSTOP );

        m_address_edit.SetFont( skin.fontDefault );
        m_serach_edit.SetFont( skin.fontDefault );

        CDWEventSvr::Instance().AddCallback( this );

        bHandled = FALSE;
        return 0L;
    }

    virtual void DoAfterPaint ( HDC hDC )
    {
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        RECT rcClient = { 0 };
        GetClientRect(&rcClient);

        CDCHandle dc(hDC);

        dc.FillSolidRect( &rcClient, skin.clrFrameWindow );

        if ( m_icon_addr.IsNull() )
            m_icon_addr = skin.iconNull;
        if ( m_icon_search.IsNull() )
            m_icon_search = skin.iconNull;

        COLORREF clrBorder = HLS_TRANSFORM(skin.clrFrameWindow, 60, 0);

        CPen pen;
        pen.CreatePen( PS_SOLID, 1, clrBorder );

        CBrush brush;
        brush.CreateSolidBrush( HLS_TRANSFORM(skin.clrFrameWindow, 20, 0) );

        HPEN   hOldPen   = dc.SelectPen( pen );
        HBRUSH hOldBrush = dc.SelectBrush( brush );

        m_address_edit.GetWindowRect(&rcClient);
        ScreenToClient(&rcClient);
        InflateRect(&rcClient, 2, 2);
        rcClient.top  -= 2;
        rcClient.left -= 18;

        POINT pt = { 5, 5 };

        dc.RoundRect(&rcClient, pt);
        m_icon_addr.DrawIconEx( hDC, rcClient.left + 4, rcClient.top + 3, 16, 16 );
        InflateRect(&rcClient, 1, 1);

        m_serach_edit.GetWindowRect(&rcClient);
        ScreenToClient(&rcClient);
        InflateRect(&rcClient, 2, 2);
        rcClient.top  -= 2;
        rcClient.left -= 18;

        dc.RoundRect(&rcClient, pt);
        m_icon_search.DrawIconEx( hDC, rcClient.left + 4, rcClient.top + 3, 16, 16 );

        dc.SelectPen(hOldPen);
        dc.SelectBrush(hOldBrush);
    }


    LRESULT OnEraseBkGnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {

        return 1L;
    }

    LRESULT OnCtlColor(UINT /* uMsg */, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
    {
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        CDCHandle dc = (HDC)wParam;
        dc.SetBkMode( TRANSPARENT );

        if ( m_bkBrush.m_hBrush == NULL )
            m_bkBrush.CreateSolidBrush( HLS_TRANSFORM(skin.clrFrameWindow, 20, 0) );

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
        if ( rcSerach.left > 250 )
            rcSerach.left = 250;
        else if ( rcSerach.left < 100 )
            rcSerach.left = 100;

        
        rcSerach.left = rcSerach.right - rcSerach.left;        

        rcAddr.right = rcSerach.left - 8;

        rcAddr.left += 22;
        rcSerach.left += 22;

        
        m_address_edit.MoveWindow(&rcAddr);
        m_serach_edit .MoveWindow(&rcSerach);

        return 1L;
    }

    LRESULT OnEventMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
    {
        if ( uMsg == eid_addr_changed )
        {
            m_address_edit.SetWindowText((LPCTSTR)wParam);
        }
        else if ( uMsg == edi_skin_changed )
        {
            if (!m_bkBrush.IsNull())
                m_bkBrush.DeleteObject();
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

    CDWEdit m_address_edit;
    CDWEdit m_serach_edit;

    CIconHandle m_icon_addr;
    CIconHandle m_icon_search;
};