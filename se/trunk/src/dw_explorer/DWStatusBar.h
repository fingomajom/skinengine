
#pragma once



#include "DWToolbar.h"
#include "DWEventSvr.h"
#include "DWMenu.h"


class CDWStatusBar : 
    public CDWToolbar,
    public CDWEventCallback

{
public:
    CDWStatusBar()
    {
    }

    BEGIN_MSG_MAP(CDWStatusBar)

        MESSAGE_HANDLER(WM_CREATE , OnCreate )
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)

        MESSAGE_HANDLER(WM_SIZE   , OnSize   )

        MESSAGE_HANDLER(WM_MOUSEMOVE , OnMouseMove)
        MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)

        MESSAGE_HANDLER(WM_LBUTTONDOWN , OnLButtonDown)
        MESSAGE_HANDLER(WM_LBUTTONUP   , OnLButtonUp)

        MESSAGE_HANDLER(WM_TIMER       , OnTimer )

        CHAIN_MSG_MAP(CDWToolbar)

    END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;

        CDWEventSvr::Instance().AddCallback(this);
        SetStatusText(NULL, FALSE);

        return 0L;    
    }

    virtual void RePositionBtns()
    {   
    }

    virtual void DrawToolBtn( HDC hDC, ToolBtnInfo& info, int nIndex )
    {
    }

    LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 1L;
    }

    LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;


        return 0;
    }

    LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        int nOldHotIndex = m_nHotIndex;

        CDWToolbar::OnMouseMove( uMsg, wParam, lParam, bHandled );

        return 1L;
    }

    LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CDWToolbar::OnMouseLeave( uMsg, wParam, lParam, bHandled );

        return 1L;
    }

    LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CDWToolbar::OnLButtonDown( uMsg, wParam, lParam, bHandled );

        return 1L;
    }
    LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CDWToolbar::OnLButtonUp( uMsg, wParam, lParam, bHandled );

        return 1L;
    }

    LRESULT OnDestroy(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return 1L;
    }

    LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        CPaintDC dc(m_hWnd);

        return 1L;
    }

    LRESULT OnEraseBkGnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return DefWindowProc();
    }

    virtual void DoBeforePaint( HDC hDC, const RECT& rcClient) 
    {
        CDWSkinUIMgt* pskin = CDWSkinUIMgt::InstancePtr();
        if ( pskin == NULL )
            return;

        COLORREF clrL = HLS_TRANSFORM( pskin->clrFrameWindow, +50, 0 );

        CDCHandle(hDC).FillSolidRect( &rcClient, pskin->clrFrameWindow );

        CIconHandle icon = m_iconLeft;
        if ( icon == NULL )
            icon = pskin->iconNull;

        icon.DrawIconEx( hDC, rcClient.left + 2, rcClient.top + 4,
            16, 16);

        RECT rcText = rcClient;

        rcText.top  += 4;
        rcText.left += 20;

        int nBkMode = ::SetBkMode( hDC, TRANSPARENT );
        HGDIOBJ hOldObj = ::SelectObject( hDC, (HGDIOBJ)pskin->fontDefault );
        ::SetTextColor( hDC, HLS_TRANSFORM( pskin->clrFrameWindow, -50, 0 ));

        DrawText( hDC, m_strStatus, -1, &rcText, 
            DT_LEFT | DT_VCENTER | DT_SINGLELINE );

        ::SelectObject( hDC, hOldObj );
        ::SetBkMode( hDC, nBkMode );

    }
    
    void SetStatusText( LPCTSTR pszText, BOOL bRePaint = TRUE )
    {
        if ( pszText == NULL || lstrlenW(pszText) <= 0 )
            m_strStatus = L"Íê³É";
        else
            m_strStatus = pszText;
        if ( bRePaint && IsWindow() )
            CWindow::Invalidate();
    }

    virtual LRESULT OnEventMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
    {
        if ( uMsg == edi_status_bar )
        {
            SetStatusText( (LPCTSTR)wParam );
        }

        return 0;
    }

    DECLARE_WND_CLASS(_T("DWExplorer_StatusBar"));

public:
    
    CIconHandle  m_iconLeft;
    ATL::CString m_strStatus;

};
