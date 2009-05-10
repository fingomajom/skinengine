/********************************************************************
	created:	2009/05/01
	created:	1:5:2009   12:17
	filename: 	e:\lidengwang\gcode\se\trunk\src\dw_explorer\DWTitleBar.h
	file path:	e:\lidengwang\gcode\se\trunk\src\dw_explorer
	file base:	DWTitleBar.h
	author:		lidengwang@sohu-rd.com
	
	purpose:	
*********************************************************************/

#pragma once

#include "DWToolbar.h"
#include "DWEventSvr.h"
#include "DWColorDlg.h"


class CDWTitleBar : public CDWToolbar
{

public:
    CDWTitleBar()
    {
    }

    BEGIN_MSG_MAP(CDWMainFrame)

        MESSAGE_HANDLER(WM_CREATE        , OnCreate )
        MESSAGE_HANDLER(WM_LBUTTONDOWN   , OnLButtonDown )
        MESSAGE_HANDLER(WM_LBUTTONDBLCLK , OnLButtonDBClk)
        MESSAGE_HANDLER(WM_RBUTTONUP     , OnRButtonUp  )

        CHAIN_MSG_MAP(CDWToolbar)

    END_MSG_MAP()

    virtual void RePositionBtns()
    {   
        RECT rcBtn = { m_nLeftSpace, m_nTopSpace, m_nLeftSpace, m_nTopSpace};

        RECT rcClient = { 0 };
        GetClientRect(&rcClient);

        ATLASSERT( m_vtToolBtn.GetSize() == 1);
        if ( m_vtToolBtn.GetSize() <= 0 )
            return;
        ToolBtnInfo& info = m_vtToolBtn[0];

        ATLASSERT(info.image);
        if ( info.image == NULL )
            return;
        rcBtn.right  = rcClient.right;
        rcBtn.left   = rcBtn.right - (info.image->GetWidth()) / 4;
        rcBtn.bottom = m_nTopSpace + info.image->GetHeight();

        info.rcBtn = rcBtn;

        m_nToolbarWidth  = rcClient.right - rcClient.left;
        m_nToolbarHeigth = rcClient.bottom - rcClient.top;
    }

    virtual void DrawToolBtn( HDC hDC, ToolBtnInfo& info, int nIdex )
    {
        CDWSkinUIMgt* pskin = CDWSkinUIMgt::InstancePtr();
        if ( pskin == NULL )
            return;

        RECT rcSrcImage = { 0 };

        rcSrcImage.left   = info.image->GetWidth() / 4;
        rcSrcImage.bottom = info.image->GetHeight();

        rcSrcImage.right = rcSrcImage.left * (nIdex + 1);
        rcSrcImage.left  = rcSrcImage.left * nIdex;

        CDCHandle(hDC).FillSolidRect( &info.rcBtn, pskin->clrFrameWindow );

        info.image->AlphaDraw( hDC, info.rcBtn.left, info.rcBtn.top, &rcSrcImage, 1, 1.5 );
    }


    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        AddToolBtn( _T(""), SC_MINIMIZE, IDR_PNG_BTN_TITLE_CUS);

        bHandled = FALSE;
        return 0L;
    }

    virtual void DoAfterPaint ( HDC hDC, const RECT& rcClient ) 
    {
        CDWSkinUIMgt* pskin = CDWSkinUIMgt::InstancePtr();
        if ( pskin == NULL )
            return;
        
        RECT rcText = rcClient;

        rcText.top   += 2;
        rcText.left  += 2;
        rcText.right -= 40;

        TCHAR szText[MAX_PATH] = { 0 };
        ::GetWindowText(GetParent(), szText, MAX_PATH);

        CIconHandle icon = (HICON)::SendMessage(GetParent(), WM_GETICON, FALSE, 0);
        if ( icon.m_hIcon != NULL )
        {
            icon.DrawIconEx( hDC, rcText.left, rcText.top , 16, 16 );
            rcText.left += 20;
        }
        
        rcText.top += 1;

        int nBkMode = ::SetBkMode( hDC, TRANSPARENT );
        HGDIOBJ hOldObj = ::SelectObject( hDC, (HGDIOBJ)pskin->fontDefault );
        ::SetTextColor(hDC, HLS_TRANSFORM(pskin->clrFrameWindow, -60, 0));

        DrawText( hDC, szText, -1, &rcText, 
            DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);

        ::SelectObject( hDC, hOldObj );
        ::SetBkMode( hDC, nBkMode );

    }

    LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;
        
        int xPos = GET_X_LPARAM(lParam); 
        int yPos = GET_Y_LPARAM(lParam); 

        POINT pt = { xPos, yPos };

        ATLASSERT( m_vtToolBtn.GetSize() == 1);
        if ( m_vtToolBtn.GetSize() <= 0 )
            return 1L;
        ToolBtnInfo& info = m_vtToolBtn[0];
        
        if ( !PtInRect(&info.rcBtn, pt) && !::IsZoomed(GetParent()) )
        {
            ::PostMessage(GetParent(), WM_NCLBUTTONDOWN, HTCAPTION, lParam );
        }

        return 1L;
    }

    LRESULT OnLButtonDBClk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;

        int xPos = GET_X_LPARAM(lParam); 
        int yPos = GET_Y_LPARAM(lParam); 

        POINT pt = { xPos, yPos };

        ATLASSERT( m_vtToolBtn.GetSize() == 1);
        if ( m_vtToolBtn.GetSize() <= 0 )
            return 1L;
        ToolBtnInfo& info = m_vtToolBtn[0];

        if ( !PtInRect(&info.rcBtn, pt) )
        {
             if ( !::IsZoomed(GetParent() ) )
                ::PostMessage(GetParent(), WM_SYSCOMMAND, SC_MAXIMIZE, lParam );
             else
                 ::PostMessage(GetParent(), WM_SYSCOMMAND, SC_RESTORE, lParam );
        }
        
        return 1L;
    }

    LRESULT OnRButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        POINT pt;

        CWindow wndParent = GetParent();
        if ( !::IsWindow(wndParent) )
            return 0L;

        CMenuHandle menuSys = wndParent.GetSystemMenu( FALSE );
        if ( !menuSys.IsMenu() )
            return 0L;

        GetCursorPos(&pt);
        int nRetCmd = menuSys.TrackPopupMenu( TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, 
            pt.x, pt.y, wndParent );
        if ( nRetCmd != 0 )
        {
            wndParent.PostMessage( WM_SYSCOMMAND, nRetCmd, 0 );
        }

        return 1L;
    }

    CDWColorDialog m_clr_dlg;

    virtual BOOL SendClickMsg( UINT nID )
    {
        CDWSkinUIMgt* pskin = CDWSkinUIMgt::InstancePtr();
        if ( pskin == NULL )
            return FALSE;
        
        if ( !m_clr_dlg.IsWindow() )
            m_clr_dlg.Create( m_hWnd );

        RECT rcWindow = { 0 };
        m_clr_dlg.GetWindowRect(&rcWindow);

        RECT rcThisWindow;
        GetWindowRect(&rcThisWindow);
        POINT pt;
        GetCursorPos(&pt);

        m_clr_dlg.SetWindowPos(NULL, 
            pt.x - (rcWindow.right-rcWindow.left)/2, rcThisWindow.bottom, 0, 0, 
            SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);

        m_clr_dlg.ShowWindow( SW_SHOW );
        

        return 0;
    }

};