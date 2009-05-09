/********************************************************************
	created:	2009/05/09
	created:	9:5:2009   22:02
	filename: 	d:\lidengwang\se\trunk\src\dw_explorer\DWEdit.h
	file path:	d:\lidengwang\se\trunk\src\dw_explorer
	file base:	DWEdit
	file ext:	h
	author:		lidengwang@s-zone.com
	
	purpose:	
*********************************************************************/

#pragma once

#include "DWDropdownList.h"

#define ID_TOOL_ADDR_DROPDOWN       10
#define ID_TOOL_SERACH_DROPDOWN     11

class CDWEdit : public CWindowImpl<CDWEdit, CEdit>
{
public:

    DECLARE_WND_CLASS(_T("DWExplorer_DWEdit"));

    CDWEdit()
    {
    }

    BEGIN_MSG_MAP(CDWMainFrame)

        MESSAGE_HANDLER( WM_CREATE     , OnCreate  )
        MESSAGE_HANDLER( WM_DESTROY    , OnDestroy )
        MESSAGE_HANDLER( WM_KEYDOWN    , OnKeyDown  )
        MESSAGE_HANDLER( WM_SETFOCUS   , OnSetFocus )
        MESSAGE_HANDLER( WM_KILLFOCUS  , OnKillFocus )
        MESSAGE_HANDLER( WM_LBUTTONDOWN, OnLButtonDown )
        MESSAGE_HANDLER( WM_COMMAND    , OnCommand  )

        MESSAGE_HANDLER( WM_PAINT      , OnPaint    )

        REFLECT_NOTIFICATIONS();

    END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        m_wndDropList.m_wndEdit   = m_hWnd;
        m_wndDropList.m_wndParent = GetParent();
        m_wndDropList.Create(
            m_hWnd, rcDefault, NULL,
            WS_POPUP | WS_BORDER, 
            WS_EX_TOOLWINDOW);

        return DefWindowProc();
    }

    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        if ( m_wndDropList.IsWindow() )
            m_wndDropList.DestroyWindow();

        return DefWindowProc();
    }

    BOOL IsDropdownList() const
    {
        if ( !m_wndDropList.IsWindow() )
            return FALSE;

        return m_wndDropList.IsWindowVisible();
    }
    
    LRESULT ShowDropdownList()
    {
        RECT rcWindow;
        GetWindowRect( &rcWindow );

        rcWindow.left  -= 20;
        rcWindow.right += 14;

        if ( GetDlgCtrlID() == ID_TOOL_SERACH_DROPDOWN )
            rcWindow.left -= 5;

        rcWindow.top    = rcWindow.bottom;
        rcWindow.bottom = rcWindow.top + 300;

        SetFocus();
        m_wndDropList.MoveWindow( &rcWindow );
        return m_wndDropList.ShowWindow( SW_SHOWNOACTIVATE );
    }

    LRESULT HideDropdownList()
    {
        return m_wndDropList.ShowWindow( SW_HIDE );
    }

    LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        LRESULT lResult = DefWindowProc();

        return lResult;
    }

    LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        LRESULT lResult = DefWindowProc();

        SetSelAll();
        CWindow::Invalidate();

        return lResult;
    }

    LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        LRESULT lResult = DefWindowProc();

        //if ( IsDropdownList() )
        //    HideDropdownList();

        return lResult;
    }

    LRESULT OnCommand(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        LRESULT lResult = DefWindowProc();

        return lResult;
    }

    LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        if ( GetFocus() != m_hWnd )
        {
            SetFocus();
            SetSelAll();
            
            return TRUE;
        }

        return DefWindowProc();
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

    ATL::CString    m_strBkText;
    CDWDropdownList m_wndDropList;
};
