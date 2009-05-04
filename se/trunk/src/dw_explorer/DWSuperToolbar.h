
#pragma once

#include "DWToolbar.h"
#include "DWEventSvr.h"


class CDWEdit : public CWindowImpl<CDWEdit, CEdit>
{
public:
    BEGIN_MSG_MAP(CDWMainFrame)

        MESSAGE_HANDLER( WM_KEYDOWN , OnKeyDown )

    END_MSG_MAP()

    LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        LRESULT lResult = DefWindowProc();
        
        CWindow::Invalidate();

        return lResult;
    }
};

class CDWSuperToolbar : 
    public CDWToolbar, 
    public CDWEventCallback
{
public:


    BOOL PreTranslateMessage(MSG* pMsg)
    {
        if ( pMsg->hwnd == m_address_edit && pMsg->message == WM_KEYDOWN ) 
        {
            if ( pMsg->wParam == VK_RETURN )
            {
                int nTextLen = m_address_edit.GetWindowTextLength();
                if ( nTextLen <= 0)
                    return FALSE;

                WCHAR* pAddr = new WCHAR[ nTextLen + 1];
                m_address_edit.GetWindowText( pAddr, nTextLen + 1);

                CDWEventSvr::Instance().OnMessage( edi_open_url, (WPARAM)pAddr, FALSE);

                delete []pAddr;
            }
        }
        else if ( pMsg->hwnd == m_address_edit && pMsg->message == WM_SETFOCUS )
        {
            m_address_edit.SetSel( 0, -1 );
        }


        return FALSE;
    }

    BEGIN_MSG_MAP(CDWMainFrame)

        MESSAGE_HANDLER( WM_CREATE    , OnCreate     )
        //MESSAGE_HANDLER( WM_ERASEBKGND, OnEraseBkGnd )
        MESSAGE_HANDLER( WM_SIZE      , OnSize       )

        MESSAGE_HANDLER( WM_CTLCOLORBTN   , OnCtlColor )
        MESSAGE_HANDLER( WM_CTLCOLORSTATIC, OnCtlColor )

        MESSAGE_HANDLER( WM_CTLCOLOREDIT, OnCtlColor )
        //MESSAGE_HANDLER( WM_CTLCOLORDLG , OnCtlColor )

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
        return 1L;
    }

    virtual void DoAfterPaint ( HDC hDC )
    {
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        RECT rcClient = { 0 };
        GetClientRect(&rcClient);

        CDCHandle dc(hDC);

        dc.FillSolidRect( &rcClient, skin.clrFrameWindow );


        COLORREF clrBorder = HLS_TRANSFORM(skin.clrFrameWindow, 30, 20);

        CPen pen;
        pen.CreatePen( PS_SOLID, 1, clrBorder );

        HPEN   hOldPen   = dc.SelectPen( pen );
        HBRUSH hOldBrush = dc.SelectBrush( (HBRUSH)::GetStockObject(NULL_BRUSH) );

        m_address_edit.GetWindowRect(&rcClient);
        ScreenToClient(&rcClient);
        InflateRect(&rcClient, 2, 2);
        rcClient.bottom-=1;

        POINT pt = { 5, 5 };

        dc.RoundRect(&rcClient, pt);

        m_serach_edit.GetWindowRect(&rcClient);
        ScreenToClient(&rcClient);
        InflateRect(&rcClient, 2, 2);
        rcClient.bottom-=1;

        dc.RoundRect(&rcClient, pt);

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

        static CBrush s_bkBrush;
        if ( s_bkBrush.m_hBrush == NULL )
            s_bkBrush.CreateSolidBrush( skin.clrFrameWindow );

        return (LRESULT)s_bkBrush.m_hBrush;
    }


    LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;

        RECT rcClient = { 0 };
        GetClientRect(&rcClient);
        
        ::InflateRect(&rcClient, -2, -2);
        rcClient.top++;

        RECT rcAddr   = rcClient;
        RECT rcSerach = rcClient;

        rcSerach.left = ( rcClient.right / 4 );
        if ( rcSerach.left > 250 )
            rcSerach.left = 250;
        else if ( rcSerach.left < 100 )
            rcSerach.left = 100;

        rcSerach.left = rcSerach.right - rcSerach.left;
        rcAddr.right = rcSerach.left - 8;
        
        m_address_edit.MoveWindow(&rcAddr);
        m_serach_edit .MoveWindow(&rcSerach);

        return 1L;
    }

    LRESULT OnEventMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
    {
        if ( uMsg == eid_addr_changed )
            m_address_edit.SetWindowText((LPCTSTR)wParam);

        return 0;
    }

public:

    CDWEdit m_address_edit;
    CDWEdit m_serach_edit;
};