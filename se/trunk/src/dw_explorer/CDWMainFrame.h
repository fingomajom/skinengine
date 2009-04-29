/********************************************************************
	created:	2009/04/28
	created:	28:4:2009   18:33
	filename: 	d:\Simple\DW_Explorer\DW_Explorer\DWFrame.h
	file path:	d:\Simple\DW_Explorer\DW_Explorer
	file base:	CDWMainFrame.h
	author:		lidengwang@sohu-rd.com
	
	purpose:	
*********************************************************************/

#pragma once

#include "resource.h"

class CDWMainFrame : 
    public CWindowImpl<CDWMainFrame, CWindow, CFrameWinTraits>, 
    public CMessageFilter
{
public:
    CDWMainFrame(void);
    ~CDWMainFrame(void);

    HWND CreateEx();


    BEGIN_MSG_MAP(CDWMainFrame)

        MESSAGE_HANDLER(WM_CREATE , OnCreate )
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)

        MESSAGE_HANDLER(WM_PAINT     , OnPaint     )
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkGnd)
        
        MESSAGE_HANDLER(WM_NCPAINT   , OnNcPaint   )


        MESSAGE_HANDLER(WM_SETFOCUS   , OnRefMsg    )
        MESSAGE_HANDLER(WM_KILLFOCUS  , OnRefMsg    )
        MESSAGE_HANDLER(WM_NCACTIVATE , OnRefMsg    )
        MESSAGE_HANDLER(WM_ACTIVATE   , OnRefMsg    )

        MESSAGE_HANDLER(WM_NCCALCSIZE, OnNcCalcSize)
        MESSAGE_HANDLER(WM_SIZE      , OnSize      )
        //MESSAGE_HANDLER(WM_NCHITTEST , OnNcHitTest )

        COMMAND_ID_HANDLER( ID_APP_EXIT          , OnFileExit      )
        COMMAND_ID_HANDLER( ID_FILE_NEW          , OnFileNew       )
        COMMAND_ID_HANDLER( ID_APP_ABOUT         , OnAppAbout      )

    END_MSG_MAP()

protected:

    virtual BOOL PreTranslateMessage(MSG* pMsg);

    void ReRgnWindow();

protected:

    LRESULT OnCreate    (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnDestroy   (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnPaint     (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnEraseBkGnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnNcPaint   (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnRefMsg    (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnSize      (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnNcHitTest (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnNcCalcSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnFileNew (WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

public:

    DECLARE_WND_CLASS(_T("DWExplorer_MainFrame"));

private:

    CRgn m_rgnWindow;

    CAxWindow m_wndAx;

};
