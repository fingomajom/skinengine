

#pragma once

#include "DWGDI.h"
#include "DWImage.h"
#include "DWSkinUIMgt.h"
#include "DWTitleBar.h"
#include "DWSysBtnToolbar.h"

template<class T>
class CDWFrameUIImpl
{
public:

    BEGIN_MSG_MAP(CDWMainFrame)

        MESSAGE_HANDLER(WM_CREATE , OnCreate )

        MESSAGE_HANDLER(WM_PAINT     , OnPaint     )
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkGnd)

        MESSAGE_HANDLER(WM_NCPAINT   , OnNcPaint   )

        MESSAGE_HANDLER(WM_SETFOCUS   , OnSkipMsg    )
        MESSAGE_HANDLER(WM_KILLFOCUS  , OnSkipMsg    )
        MESSAGE_HANDLER(WM_NCACTIVATE , OnSkipMsg    )
        MESSAGE_HANDLER(WM_ACTIVATE   , OnSkipMsg    )
        MESSAGE_HANDLER(WM_NCCALCSIZE, OnNcCalcSize  )
        MESSAGE_HANDLER(WM_SIZE      , OnSize        )

        COMMAND_ID_HANDLER( SC_MINIMIZE         , OnSysBarCmd )
        COMMAND_ID_HANDLER( SC_MAXIMIZE         , OnSysBarCmd )
        COMMAND_ID_HANDLER( SC_RESTORE          , OnSysBarCmd )
        COMMAND_ID_HANDLER( SC_CLOSE            , OnSysBarCmd )

    END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;

        ::SetWindowPos(*(T*)this, NULL, 0, 0,
            0, 0,
            SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

        ReRgnWindow();

        m_sys_bar  .Create( *(T*)this, &m_sys_bar.rcDefault);
        m_sys_title.Create( *(T*)this, &m_sys_title.rcDefault);


        return 0;
    }

    void ReRgnWindow()
    {
        CRgnHandle rgn;

        RECT rcWindow;
        ::GetWindowRect(*(T*)this, &rcWindow);

        rcWindow.right  = rcWindow.right  - rcWindow.left;
        rcWindow.bottom = rcWindow.bottom - rcWindow.top;

        rcWindow.top = rcWindow.left = 0;

        rgn.CreateRoundRectRgn( rcWindow.left, rcWindow.top, 
            rcWindow.right + 1, rcWindow.bottom + 12, 9, 9);

        ::SetWindowRgn(*(T*)this, rgn, TRUE);

        if (m_rgnWindow.m_hRgn != NULL)
            m_rgnWindow.DeleteObject();

        m_rgnWindow.Attach( rgn );
    }

    void RePositionCtrls()
    {
        RECT rcClient   = { 0 };
        RECT rcSysBar   = { 0 };
        RECT rcTitleBar = { 0 };


        ((T*)this)->GetClientRect(&rcClient);

        rcSysBar   = rcClient;

        rcSysBar.left   = rcClient.right - m_sys_bar.GetToolbarWidth();
        rcSysBar.bottom = rcClient.top + m_sys_bar.GetToolbarHeigth();

        if ( ((T*)this)->IsZoomed() )
        {
            rcSysBar.top    += GetSystemMetrics(SM_CYSIZEFRAME);
            rcSysBar.bottom += GetSystemMetrics(SM_CYSIZEFRAME);
        }

        if ( ::IsWindow( m_sys_bar ) )
        {
            m_sys_bar.OnSwitchBtn();
            m_sys_bar.MoveWindow(&rcSysBar);
        }
        if ( ::IsWindow( m_sys_title ) )
        {
            rcTitleBar = rcSysBar;
            rcTitleBar.left  = rcClient.left;
            rcTitleBar.right = rcSysBar.left;
            if ( !((T*)this)->IsZoomed() )
                rcTitleBar.top++;

            m_sys_title.MoveWindow(&rcTitleBar);
        }
    }

    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;

        ReRgnWindow();
        RePositionCtrls();

        return 0L;
    }

    LRESULT OnSysBarCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        PostMessage(*(T*)this, WM_SYSCOMMAND, wID, 0);

        return 1L;
    }

    LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        BOOL bMinimized = ((T*)this)->GetStyle() & WS_MINIMIZE;

        LPNCCALCSIZE_PARAMS pNCParams = (LPNCCALCSIZE_PARAMS)lParam;

        RECT rcWindow = pNCParams->rgrc[0];

        LRESULT lResult = ::DefWindowProc(*(T*)this, uMsg, wParam, lParam);
        if (wParam && !bMinimized)
        {
            pNCParams->rgrc[0] = rcWindow;
            ::InflateRect(&pNCParams->rgrc[0], -5, -1);
            pNCParams->rgrc[0].bottom -= 4;
        }

        return lResult;
    }


    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        CPaintDC dc(*(T*)this);

        CSkinDCHandle sdc = dc;

        RECT rcClient;
        ((T*)this)->GetClientRect(&rcClient);

        dc.FillSolidRect( &rcClient, skin.clrFrameWindow );

        COLORREF clrTemp = HLS_TRANSFORM( skin.clrFrameWindow, 60, 0 );
        sdc.SkinLine( rcClient.left, rcClient.top, rcClient.right, rcClient.top, clrTemp);

        return 0L;
    }

    LRESULT OnEraseBkGnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return 1L;
    }

    LRESULT OnSkipMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return 1L;
    }


    LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CWindowDC dc(*(T*)this);
            
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        CRgn rgn;
        CRgn rgnNULL;

        RECT rcWindow;
        RECT rcBorder;
        ((T*)this)->GetWindowRect(&rcWindow);


        rcWindow.right  = rcWindow.right  - rcWindow.left;
        rcWindow.bottom = rcWindow.bottom - rcWindow.top;
        rcWindow.top    = rcWindow.left = 0;

        rgn.CreateRectRgnIndirect(&rcWindow);
        rcBorder = rcWindow;
        ::InflateRect( &rcBorder, -5, -5);
        rcBorder.top = 1;
        rgnNULL.CreateRectRgnIndirect( &rcBorder );

        rgn.CombineRgn(rgnNULL, RGN_DIFF);

        dc.SelectRgn( rgn );

        rcBorder = rcWindow;

        dc.Draw3dRect( &rcBorder, 
            skin.clrFrameNcBorder,
            skin.clrFrameWindow);

        COLORREF clrTemp = HLS_TRANSFORM( skin.clrFrameWindow, 60, 0 );

        CPen pen;  pen .CreatePen( PS_SOLID, 1, skin.clrFrameNcBorder );
        CPen pen1; pen1.CreatePen( PS_SOLID, 1, HLS_TRANSFORM( skin.clrFrameWindow, -40, -40 ) );
        CPen pen2; pen2.CreatePen( PS_SOLID, 1, HLS_TRANSFORM( skin.clrFrameWindow,  60, 0 ) );
        CPen pen3; pen3.CreatePen( PS_SOLID, 1, HLS_TRANSFORM( skin.clrFrameWindow,  40, 0 ) );
        CPen pen4; pen4.CreatePen( PS_SOLID, 1, skin.clrFrameWindow );
        CPen pen5; pen5.CreatePen( PS_SOLID, 1, HLS_TRANSFORM( skin.clrFrameWindow,  60, 0 ) );

        HPEN   hOldPen   = dc.SelectPen( pen );
        HBRUSH hOldBrush = dc.SelectBrush( (HBRUSH)::GetStockObject(NULL_BRUSH) );

        ::InflateRect( &rcBorder, -1, -1);
        dc.RoundRect( rcBorder.left, rcBorder.top, rcBorder.right, rcBorder.bottom, 3, 3);
        dc.SelectPen( pen1 );
        dc.RoundRect( rcBorder.left, rcBorder.top, rcBorder.right, rcBorder.bottom, 7, 7);
        dc.SelectPen( pen2 );
        dc.RoundRect( rcBorder.left, rcBorder.top, rcBorder.right, rcBorder.bottom, 9, 9);

        ::InflateRect( &rcBorder, -1, -1);
        dc.SelectPen( pen3 );
        dc.RoundRect( rcBorder.left, rcBorder.top, rcBorder.right, rcBorder.bottom, 5, 5);
        dc.SelectPen( pen4 );
        dc.RoundRect( rcBorder.left, rcBorder.top, rcBorder.right, rcBorder.bottom, 7, 7);

        ::InflateRect( &rcBorder, -1, -1);
        dc.Draw3dRect( &rcBorder, skin.clrFrameWindow, skin.clrFrameWindow );
        ::InflateRect( &rcBorder, -1, -1);
        dc.Draw3dRect( &rcBorder, skin.clrFrameWindow, skin.clrFrameWindow );

        rcBorder = rcWindow;
        rcBorder.top = rcBorder.bottom - 6;
        ::InflateRect( &rcBorder, -1, -1);
        dc.SelectPen( pen2 );
        dc.Rectangle( &rcBorder );
        ::InflateRect( &rcBorder, -1, -1); rcBorder.top--;
        dc.SelectPen( pen4 );
        dc.Rectangle( &rcBorder );

        dc.SelectPen(hOldPen);
        dc.SelectBrush(hOldBrush);


        return 0L;
    }

    LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return DefWindowProc();
    }

private:
    CRgn m_rgnWindow;

    CDWTitleBar      m_sys_title;
    CDWSysBtnToolBar m_sys_bar;
};