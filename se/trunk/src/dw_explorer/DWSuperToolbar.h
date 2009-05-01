
#pragma once

#include "DWToolbar.h"

class CDWSuperToolbar : public CDWToolbar
{
public:

    CFont m_font;

    BEGIN_MSG_MAP(CDWMainFrame)

        MESSAGE_HANDLER( WM_CREATE    , OnCreate     )
        MESSAGE_HANDLER( WM_ERASEBKGND, OnEraseBkGnd )
        MESSAGE_HANDLER( WM_SIZE      , OnSize       )

        CHAIN_MSG_MAP(CDWToolbar)

    END_MSG_MAP()


    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        m_font.CreatePointFont( 96, L"Courier New" );

        m_address_edit.Create(m_hWnd, &rcDefault, NULL, WS_CHILD | WS_VISIBLE );
        m_serach_edit .Create(m_hWnd, &rcDefault, NULL, WS_CHILD | WS_VISIBLE , WS_EX_CLIENTEDGE );

        m_address_edit.SetFont( m_font );
        m_serach_edit.SetFont( m_font );

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


public:

    CEdit m_address_edit;
    CEdit m_serach_edit;
};