

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

    enum{
        WS_FULLSCREEN = 1
    };

    CDWFrameUIImpl()
    {
        m_bActivete = FALSE;
        m_bMaximize = FALSE;
    }

    BEGIN_MSG_MAP(CDWFrameUIImpl)

        MESSAGE_HANDLER(WM_CREATE , OnCreate )

        MESSAGE_HANDLER(WM_PAINT     , OnPaint     )
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkGnd)

        MESSAGE_HANDLER(WM_NCPAINT   , OnNcPaint   )
       
        MESSAGE_HANDLER(WM_NCCALCSIZE, OnNcCalcSize  )
        MESSAGE_HANDLER(WM_SIZE      , OnSize        )

        MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnWindowPosChanging)

        MESSAGE_HANDLER(WM_COMMAND   , OnSysBarCmd)
        MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysBarCmd)

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

        return 0L;
    }

    void ReRgnWindow()
    {
        DWORD dwStyle = ((T*)this)->GetStyle();
        if ((dwStyle & WS_MINIMIZE) == WS_MINIMIZE)
            return;

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
        DWORD dwStyle = ((T*)this)->GetStyle();
        if ( (dwStyle & WS_MINIMIZE) == WS_MINIMIZE )
            return;

        RECT rcClient   = { 0 };
        RECT rcSysBar   = { 0 };
        RECT rcTitleBar = { 0 };


        ((T*)this)->GetClientRect(&rcClient);

        rcSysBar   = rcClient;

        rcSysBar.left   = rcClient.right - m_sys_bar.GetToolbarWidth() + 3;
        rcSysBar.bottom = rcClient.top + m_sys_bar.GetToolbarHeigth();

        if ( ((T*)this)->IsZoomed() )
        {
            rcSysBar.top    += 1;
            rcSysBar.bottom += 1;
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
            rcTitleBar.top    += 1;
            rcTitleBar.bottom += 1;

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

    LRESULT OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
    {
        T* pthis = (T*)this;

        bHandled = FALSE;
        LPWINDOWPOS lpWndPos = (LPWINDOWPOS)lParam;
        if (pthis->GetStyle() & WS_MAXIMIZE && (lpWndPos->flags & SWP_NOSIZE) == 0)
        {
            RECT rc;
            pthis->GetWindowRect(&rc);
            HMONITOR hMon = NULL;
            if (rc.right - rc.left < 320 || rc.bottom - rc.top < 300)
                hMon = ::MonitorFromRect(&m_rcLastWindowPos, MONITOR_DEFAULTTONEAREST);
            else
                hMon = ::MonitorFromWindow(pthis->m_hWnd, MONITOR_DEFAULTTONEAREST);

            RECT rcWork;
            MONITORINFO mi = { sizeof(MONITORINFO) };
            ::GetMonitorInfo(hMon, &mi);
            
            rcWork = (pthis->GetStyle() & WS_FULLSCREEN) ? mi.rcMonitor : mi.rcWork;

            rcWork.top    -= 2;
            rcWork.left   -= 4;
            rcWork.right  += 4;
            rcWork.bottom += 4;

            lpWndPos->x  = rcWork.left;
            lpWndPos->y  = rcWork.top;
            lpWndPos->cx = rcWork.right - rcWork.left;
            lpWndPos->cy = rcWork.bottom - rcWork.top;
        }
        return 0;
    }

    LRESULT OnSysBarCmd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;

        T* pthis = (T*)this;

        int nCmdId = 0;

        if ( uMsg == WM_SYSCOMMAND )
            nCmdId = wParam;
        else 
            nCmdId = LOWORD(wParam);

        switch ( nCmdId )
        {
        case SC_RESTORE:
            pthis->ModifyStyle(WS_FULLSCREEN, 0);
        case SC_MAXIMIZE:
        case SC_MINIMIZE:
            if (!( pthis->GetStyle() & WS_MAXIMIZE))
                 pthis->GetWindowRect(&m_rcLastWindowPos);
        case SC_CLOSE:

            bHandled = TRUE;
            pthis->DefWindowProc(WM_SYSCOMMAND, nCmdId | (wParam & 0xffff000f), lParam);

            break;
        }

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
        CDWSkinUIMgt* pskin = CDWSkinUIMgt::InstancePtr();
        if ( pskin == NULL )
            return 1L;

        CPaintDC dc(*(T*)this);

        CSkinDCHandle sdc = dc;

        RECT rcClient;
        ((T*)this)->GetClientRect(&rcClient);

        dc.FillSolidRect( &rcClient, pskin->clrFrameWindow );

        COLORREF clrTemp = HLS_TRANSFORM( pskin->clrFrameWindow, 60, 0 );
        sdc.SkinLine( rcClient.left, rcClient.top, rcClient.right, rcClient.top, clrTemp);

        return 0L;
    }

    LRESULT OnEraseBkGnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return 1L;
    }

    LRESULT OnSkipMsg(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        LRESULT lResult = ((T*)this)->DefWindowProc();

        return lResult;
    }


    LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CWindowDC wndDC(*(T*)this);

        CDWSkinUIMgt* pskin = CDWSkinUIMgt::InstancePtr();
        if ( pskin == NULL )
            return 1L;

        CRgn rgn;
        CRgn rgnNULL;

        RECT rcWindow;
        RECT rcBorder;
        ((T*)this)->GetWindowRect(&rcWindow);


        rcWindow.right  = rcWindow.right  - rcWindow.left;
        rcWindow.bottom = rcWindow.bottom - rcWindow.top;
        rcWindow.top    = rcWindow.left = 0;

        {
            CMemoryDC dc(wndDC, rcWindow);

            rgn.CreateRectRgnIndirect(&rcWindow);
            rcBorder = rcWindow;
            ::InflateRect( &rcBorder, -5, -5);
            rcBorder.top = 1;
            rgnNULL.CreateRectRgnIndirect( &rcBorder );

            rgn.CombineRgn(rgnNULL, RGN_DIFF);

            wndDC.SelectRgn( rgn );

            rcBorder = rcWindow;

            CPen pen;  pen .CreatePen( PS_SOLID, 1, pskin->clrFrameNcBorder );
            CPen pen1; pen1.CreatePen( PS_SOLID, 1, HLS_TRANSFORM( pskin->clrFrameWindow, -40, -40 ) );
            CPen pen2; pen2.CreatePen( PS_SOLID, 1, HLS_TRANSFORM( pskin->clrFrameWindow,  60, 0 ) );
            CPen pen3; pen3.CreatePen( PS_SOLID, 1, HLS_TRANSFORM( pskin->clrFrameWindow,  40, 0 ) );
            CPen pen4; pen4.CreatePen( PS_SOLID, 1, pskin->clrFrameWindow );

            HPEN   hOldPen   = dc.SelectPen( pen );
            HBRUSH hOldBrush = dc.SelectBrush( (HBRUSH)::GetStockObject(NULL_BRUSH) );

            dc.Rectangle(&rcBorder);
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
            dc.Draw3dRect( &rcBorder, pskin->clrFrameWindow, pskin->clrFrameWindow );
            ::InflateRect( &rcBorder, -1, -1);
            dc.Draw3dRect( &rcBorder, pskin->clrFrameWindow, pskin->clrFrameWindow );

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
        }

        return 0L;
    }

    LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return DefWindowProc();
    }

protected:
    
    RECT m_rcLastWindowPos;
    BOOL m_bMaximize;

    BOOL m_bActivete;
    CRgn m_rgnWindow;

    CDWTitleBar      m_sys_title;
    CDWSysBtnToolBar m_sys_bar;
};