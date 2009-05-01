/********************************************************************
	created:	2009/05/01
	created:	1:5:2009   12:03
	filename: 	e:\lidengwang\gcode\se\trunk\src\dw_explorer\DWTableBar.h
	file path:	e:\lidengwang\gcode\se\trunk\src\dw_explorer
	file base:	DWTableBar.h
	author:		lidengwang@sohu-rd.com
	
	purpose:	
*********************************************************************/

#pragma once

class CDWTableBar : public CWindowImpl<CDWTableBar>
{
public:

    BEGIN_MSG_MAP(CDWTableBar)

        MESSAGE_HANDLER(WM_CREATE , OnCreate )
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)

        MESSAGE_HANDLER(WM_SIZE   , OnSize   )

        MESSAGE_HANDLER(WM_MOUSEMOVE , OnMouseMove)
        MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)

        MESSAGE_HANDLER(WM_LBUTTONDOWN , OnLButtonDown)
        MESSAGE_HANDLER(WM_LBUTTONUP   , OnLButtonUp)

        MESSAGE_HANDLER(WM_PAINT     , OnPaint      )
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkGnd )

    END_MSG_MAP()


    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return 1L;
    }

    LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return 1L;
    }

    LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
    {
        return 1L;
    }

    LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {

        return 1L;
    }

    LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return 1L;
    }
    LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return 1L;
    }

    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
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


    DECLARE_WND_CLASS(_T("DWExplorer_TableBar"));
};