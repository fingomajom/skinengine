
#pragma once

#include "DWToolbar.h"
#include "DWEventSvr.h"


class CDWSuperToolbar : 
    public CDWToolbar, 
    public CDWEventCallback
{
public:


    BOOL PreTranslateMessage(MSG* pMsg)
    {
        if ( pMsg->hwnd == m_address_edit && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN )
        {
            int nTextLen = m_address_edit.GetWindowTextLength();
            if ( nTextLen <= 0)
                return FALSE;

            WCHAR* pAddr = new WCHAR[ nTextLen + 1];
            m_address_edit.GetWindowText( pAddr, nTextLen + 1);

            CDWEventSvr::Instance().OnMessage( edi_open_url, (WPARAM)pAddr, FALSE);

            delete []pAddr;
        }
        else if ( pMsg->hwnd == m_address_edit && pMsg->message == WM_SETFOCUS )
        {
            m_address_edit.SetSel( 0, -1 );
        }


        return FALSE;
    }

    BEGIN_MSG_MAP(CDWMainFrame)

        MESSAGE_HANDLER( WM_CREATE    , OnCreate     )
        MESSAGE_HANDLER( WM_ERASEBKGND, OnEraseBkGnd )
        MESSAGE_HANDLER( WM_SIZE      , OnSize       )

        CHAIN_MSG_MAP(CDWToolbar)

    END_MSG_MAP()


    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        m_address_edit.Create(m_hWnd, &rcDefault, NULL, WS_CHILD | WS_VISIBLE | ES_WANTRETURN | WS_TABSTOP );
        m_serach_edit .Create(m_hWnd, &rcDefault, NULL, WS_CHILD | WS_VISIBLE | ES_WANTRETURN | WS_TABSTOP , WS_EX_CLIENTEDGE );

        m_address_edit.SetFont( skin.fontDefault );
        m_serach_edit.SetFont( skin.fontDefault );

        CDWEventSvr::Instance().AddCallback( this );

        bHandled = FALSE;
        return 1L;
    }

    LRESULT OnEraseBkGnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        CDCHandle dc = (HDC)wParam;

        RECT rcClient = { 0 };
        GetClientRect(&rcClient);

        //dc.FillSolidRect( &rcClient, skin.clrFrameWindow );

        return DefWindowProc();
    }

    LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;

        RECT rcClient = { 0 };
        GetClientRect(&rcClient);
        
        ::InflateRect(&rcClient, -2, -2);
        m_address_edit.MoveWindow(&rcClient);

        return 1L;
    }

    LRESULT OnEventMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
    {
        if ( uMsg == eid_addr_changed )
            m_address_edit.SetWindowText((LPCTSTR)wParam);

        return 0;
    }

public:

    CEdit m_address_edit;
    CEdit m_serach_edit;
};