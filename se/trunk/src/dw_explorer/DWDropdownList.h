/********************************************************************
	created:	2009/05/09
	created:	9:5:2009   22:03
	filename: 	d:\lidengwang\se\trunk\src\dw_explorer\DWDropdownList.h
	file path:	d:\lidengwang\se\trunk\src\dw_explorer
	file base:	DWDropdownList
	file ext:	h
	author:		lidengwang@s-zone.com
	
	purpose:	
*********************************************************************/

#pragma once


class CDWListBox : public CWindowImpl<CDWListBox, CListBox>
{
public:
    DECLARE_WND_CLASS(_T("DWExplorer_DWListBox"));

    BEGIN_MSG_MAP(CDWListBox)
        MESSAGE_HANDLER( WM_SETFOCUS  , OnSetFocus )
        MESSAGE_HANDLER( WM_ERASEBKGND, OnEraseBkGnd )
        MESSAGE_HANDLER( WM_LBUTTONDOWN, OnLButtonDown )
    END_MSG_MAP()

    LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        return 0L;
    }

    LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        return 0L;
    }


    LRESULT OnEraseBkGnd(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {
        GetParent().SendMessage(WM_ERASEBKGND, wParam, lParam);
        return 1L;
    }

};

class CDWDropdownList : 
    public CWindowImpl<CDWDropdownList>,
    public COwnerDraw<CDWDropdownList>
{
public:
    CDWDropdownList()
    {
    }

    BEGIN_MSG_MAP(CDWMainFrame)

        MESSAGE_HANDLER( WM_CREATE     , OnCreate  )
        MESSAGE_HANDLER( WM_KEYDOWN    , OnKeyDown  )
        MESSAGE_HANDLER( WM_SETFOCUS   , OnSetFocus )
        MESSAGE_HANDLER( WM_LBUTTONDOWN, OnLButtonDown )

        MESSAGE_HANDLER( WM_ERASEBKGND, OnEraseBkGnd )
        MESSAGE_HANDLER( WM_NCPAINT   , OnNcPaint    )
        MESSAGE_HANDLER( WM_NCCALCSIZE, OnNcCalcSize )
        MESSAGE_HANDLER( WM_SIZE      , OnSize       )

        CHAIN_MSG_MAP(COwnerDraw<CDWDropdownList>)

    END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        RECT rcClient = {0};
        GetClientRect(&rcClient);

        //m_wndListBox.Create(
        //    m_hWnd, rcClient, NULL,
        //    WS_CHILD | WS_VISIBLE | LBS_OWNERDRAWFIXED, 
        //    WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW );

        //m_wndListBox.AddString(L"");

        return DefWindowProc();
    }

    LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        LRESULT lResult = DefWindowProc();

        return lResult;
    }

    LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        return DefWindowProc();
    }

    LRESULT OnCommand(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        LRESULT lResult = DefWindowProc();

        return lResult;
    }

    LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        return 0L;
    }

    LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
    {
        CPaintDC dc(m_hWnd);

        return 0L;
    }

    LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CWindowDC dc(m_hWnd);

        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        CRgn rgn;
        CRgn rgnNULL;

        RECT rcWindow;
        RECT rcBorder;
        GetWindowRect(&rcWindow);

        rcWindow.right  = rcWindow.right  - rcWindow.left;
        rcWindow.bottom = rcWindow.bottom - rcWindow.top;
        rcWindow.top    = rcWindow.left = 0;

        rcBorder = rcWindow;

        COLORREF clrBorder = HLS_TRANSFORM(skin.clrFrameWindow, 60, 0);

        CPen pen;  pen .CreatePen( PS_SOLID, 1, clrBorder );
        HPEN   hOldPen   = dc.SelectPen( pen );
        HBRUSH hOldBrush = dc.SelectBrush( (HBRUSH)::GetStockObject(NULL_BRUSH) );

        dc.RoundRect( rcBorder.left, rcBorder.top-2, rcBorder.right, rcBorder.bottom, 3, 3);

        dc.SelectPen(hOldPen);
        dc.SelectBrush(hOldBrush);

        return 0L;
    }

    void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
    {
        int i = 0;
    }

    void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
    {
        if(lpMeasureItemStruct->CtlType != ODT_MENU)
        {
            CClientDC dc(m_hWnd);
            TEXTMETRIC tm = { 0 };
            dc.GetTextMetrics(&tm);

            lpMeasureItemStruct->itemHeight = tm.tmHeight;
        }
        else
            lpMeasureItemStruct->itemHeight = ::GetSystemMetrics(SM_CYMENU);
    }


    LRESULT OnEraseBkGnd(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {
        CDCHandle dc((HDC)wParam);

        RECT rcClient;
        GetClientRect(&rcClient);

        dc.FillRect(&rcClient, 
            (HBRUSH)m_wndParent.SendMessage( WM_CTLCOLOREDIT, (WPARAM)dc.m_hDC, (LPARAM)m_wndEdit.m_hWnd ) );
        
        return 0L;
    }

    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        ReRgnWindow();

        RECT rcClient = {0};
        GetClientRect(&rcClient);
        
        if ( m_wndListBox.IsWindow() )
            m_wndListBox.MoveWindow(&rcClient);

        return 0L;
    }

    LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LPNCCALCSIZE_PARAMS pNCParams = (LPNCCALCSIZE_PARAMS)lParam;

        RECT rcWindow = pNCParams->rgrc[0];

        LRESULT lResult = DefWindowProc();
        if (wParam)
        {
            pNCParams->rgrc[0] = rcWindow;
            ::InflateRect(&pNCParams->rgrc[0], -1, -1);
            pNCParams->rgrc[0].top--;
        }

        return lResult;
    }


    void ReRgnWindow()
    {
        CRgnHandle rgn;

        RECT rcWindow;
        GetWindowRect(&rcWindow);

        rcWindow.right  = rcWindow.right  - rcWindow.left;
        rcWindow.bottom = rcWindow.bottom - rcWindow.top;

        rcWindow.top = rcWindow.left = 0;

        rgn.CreateRoundRectRgn( rcWindow.left, rcWindow.top - 10, 
            rcWindow.right + 1, rcWindow.bottom + 1, 3, 3);

        SetWindowRgn(rgn, TRUE);

        if (m_rgnWindow.m_hRgn != NULL)
            m_rgnWindow.DeleteObject();

        m_rgnWindow.Attach( rgn );
    }


    BOOL ShowWindow(int nCmdShow) throw()
    {
        if ( nCmdShow != SW_HIDE )
        {
            ATLASSERT(s_hDropdownListHook == NULL);
            ATLASSERT(s_hWndDropdownList == NULL);

            s_hWndDropdownList  = m_hWnd;
            s_hDropdownListHook = ::SetWindowsHookEx(WH_GETMESSAGE, 
                DropdownListHookFunc, 
                _Module.GetModuleInstance(), 
                GetCurrentThreadId());
            ATLASSERT(s_hDropdownListHook);
        }
        else
        {
            if ( s_hDropdownListHook != NULL )
            {
                UnhookWindowsHookEx(s_hDropdownListHook);
                s_hDropdownListHook = NULL;
                s_hWndDropdownList  = NULL;
            }
        }

        return CWindow::ShowWindow( nCmdShow );
    }


    static LRESULT CALLBACK DropdownListHookFunc(
        int nCode,
        WPARAM wParam,
        LPARAM lParam )
    {
        LPMSG pMsg = (LPMSG)lParam;

        LRESULT lRet = ::CallNextHookEx( s_hDropdownListHook, nCode, wParam, lParam);

        if ( pMsg->message == WM_LBUTTONDOWN || 
             pMsg->message == WM_LBUTTONDBLCLK ||
             pMsg->message == WM_RBUTTONDOWN ||
             pMsg->message == WM_RBUTTONDBLCLK )
        {
            POINT pt;
            GetCursorPos(&pt);

            RECT rcWindow;
            ::GetWindowRect(s_hWndDropdownList, &rcWindow);

            if ( !::PtInRect( &rcWindow, pt ) )
            {
                UnhookWindowsHookEx( s_hDropdownListHook );
                ::ShowWindow(s_hWndDropdownList, SW_HIDE);
                s_hDropdownListHook = NULL;
                s_hWndDropdownList  = NULL;
            }
        }

        return lRet;
    }

    DECLARE_WND_CLASS(_T("DWExplorer_DropdownList"));

    CRgn m_rgnWindow;

    CWindow    m_wndParent;
    CWindow    m_wndEdit;
    CDWListBox m_wndListBox;

    static HHOOK s_hDropdownListHook;
    static HWND  s_hWndDropdownList;
};

__declspec(selectany) HHOOK CDWDropdownList::s_hDropdownListHook = NULL;
__declspec(selectany) HWND  CDWDropdownList::s_hWndDropdownList  = NULL;
