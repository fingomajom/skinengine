#include "StdAfx.h"
#include "CDWMainFrame.h"
#include "DWGDI.h"
#include "DWImage.h"


COLORREF g_clrNcBorder = RGB(90, 90, 90);
COLORREF g_clrWindow   = RGB( 196, 80, 220 );



CDWMainFrame::CDWMainFrame(void)
{
}

CDWMainFrame::~CDWMainFrame(void)
{
}


HWND CDWMainFrame::CreateEx()
{
    RECT rcClient = { 0 };
    SystemParametersInfo( SPI_GETWORKAREA, 0, &rcClient, 0 );

    rcClient.left   = ( rcClient.right - rcClient.left ) / 8;
    rcClient.right  = rcClient.left * 7;
    rcClient.top    = ( rcClient.bottom - rcClient.top ) / 8;
    rcClient.bottom = rcClient.top * 7;

    return Create( NULL, &rcClient, _T("DW_Explorer"), 
         WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN );
}

BOOL CDWMainFrame::PreTranslateMessage(MSG* pMsg)
{
    return FALSE;
}

LRESULT CDWMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->AddMessageFilter(this);

    ::SetWindowPos(m_hWnd, NULL, 0, 0,
        0, 0,
        SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

    ReRgnWindow();

    RECT rcClient;
    GetClientRect(&rcClient);

    rcClient.top += 60;
    m_wndAx.Create( m_hWnd, &rcClient, L"http://www.baidu.com", WS_CHILD );
    m_wndAx.ShowWindow( SW_SHOWDEFAULT );

    GetClientRect(&rcClient);
    rcClient.left += 40;
    rcClient.right = 300;
    rcClient.bottom = 40;
    

    m_sys_bar.AddToolBtn( L"", 1, IDR_PNG_BTN_SYS_MIN);
    m_sys_bar.AddToolBtn( L"", 1, IDR_PNG_BTN_SYS_MAX);
    m_sys_bar.AddToolBtn( L"", 1, IDR_PNG_BTN_SYS_MAX2);
    m_sys_bar.AddToolBtn( L"", 1, IDR_PNG_BTN_SYS_CLOSE);
    m_sys_bar.Create( m_hWnd, &rcClient );


    return 0;
}

void CDWMainFrame::ReRgnWindow()
{
    CRgnHandle rgn;

    RECT rcWindow;
    GetWindowRect(&rcWindow);

    rcWindow.right  = rcWindow.right  - rcWindow.left;
    rcWindow.bottom = rcWindow.bottom - rcWindow.top;

    rcWindow.top = rcWindow.left = 0;

    rgn.CreateRoundRectRgn( rcWindow.left, rcWindow.top, 
        rcWindow.right + 1, rcWindow.bottom + 12, 9, 9);

    SetWindowRgn(rgn, TRUE);

    if (m_rgnWindow.m_hRgn != NULL)
        m_rgnWindow.DeleteObject();

    m_rgnWindow.Attach( rgn );
}

void CDWMainFrame::RePositionCtrls()
{
    RECT rcClient;
    GetClientRect(&rcClient);
    
    rcClient.left = rcClient.right - m_sys_bar.GetToolbarWidth();
    rcClient.bottom = rcClient.top + m_sys_bar.GetToolbarHeigth();

    if ( ::IsWindow( m_sys_bar ) )
        m_sys_bar.MoveWindow(&rcClient);
}

LRESULT CDWMainFrame::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;

    ReRgnWindow();
    RePositionCtrls();

    return 0L;
}



LRESULT CDWMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    PostMessage(WM_CLOSE);
    return 0;
}

LRESULT CDWMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    return 0;
}


LRESULT CDWMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    return 0;
}

LRESULT CDWMainFrame::OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
    if((GetStyle() & (WS_CHILD)) == 0)
        ::PostQuitMessage(1);

    bHandled = FALSE;
    return 1;
}

LRESULT CDWMainFrame::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    BOOL bMinimized = GetStyle() & WS_MINIMIZE;

    LPNCCALCSIZE_PARAMS pNCParams = (LPNCCALCSIZE_PARAMS)lParam;

    RECT rcWindow = pNCParams->rgrc[0];

    LRESULT lResult = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
    if (wParam && !bMinimized)
    {
        pNCParams->rgrc[0] = rcWindow;
        ::InflateRect(&pNCParams->rgrc[0], -5, -1);
        pNCParams->rgrc[0].bottom -= 4;
    }

    return lResult;
}


LRESULT CDWMainFrame::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    CPaintDC dc(m_hWnd);

    CSkinDCHandle sdc = dc;

    RECT rcClient;
    GetClientRect(&rcClient);

    dc.FillSolidRect( &rcClient, g_clrWindow );
    
    COLORREF clrTemp = HLS_TRANSFORM( g_clrWindow, 60, 0 );
    sdc.SkinLine( rcClient.left, rcClient.top, rcClient.right, rcClient.top, clrTemp);

    return 0L;
}

LRESULT CDWMainFrame::OnEraseBkGnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    return 1L;
}

LRESULT CDWMainFrame::OnSkipMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    return 1L;
}


LRESULT CDWMainFrame::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    CWindowDC dc(m_hWnd);

    
    CRgn rgn;
    CRgn rgnNULL;

    RECT rcWindow;
    RECT rcBorder;
    GetWindowRect(&rcWindow);


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

    dc.Draw3dRect( &rcBorder, g_clrNcBorder, g_clrNcBorder );

    COLORREF clrTemp = HLS_TRANSFORM( g_clrWindow, 60, 0 );

    CPen pen;  pen .CreatePen( PS_SOLID, 1, g_clrNcBorder );
    CPen pen1; pen1.CreatePen( PS_SOLID, 1, HLS_TRANSFORM( g_clrWindow, -40, -40 ) );
    CPen pen2; pen2.CreatePen( PS_SOLID, 1, HLS_TRANSFORM( g_clrWindow,  60, 0 ) );
    CPen pen3; pen3.CreatePen( PS_SOLID, 1, HLS_TRANSFORM( g_clrWindow,  40, 0 ) );
    CPen pen4; pen4.CreatePen( PS_SOLID, 1, g_clrWindow );
    CPen pen5; pen5.CreatePen( PS_SOLID, 1, HLS_TRANSFORM( g_clrWindow,  60, 0 ) );

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
    dc.Draw3dRect( &rcBorder, g_clrWindow, g_clrWindow );
    ::InflateRect( &rcBorder, -1, -1);
    dc.Draw3dRect( &rcBorder, g_clrWindow, g_clrWindow );

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

LRESULT CDWMainFrame::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    return DefWindowProc();
}
