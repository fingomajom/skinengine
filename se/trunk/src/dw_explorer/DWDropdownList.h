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

typedef struct tag_DropListItem
{
    tag_DropListItem()
    {
        clrLeft  = -1;
        clrRight = -1;
    }

    CIconHandle icon;

    ATL::CString strLeft;
    ATL::CString strRight;

    COLORREF clrLeft;
    COLORREF clrRight;

} DROPLISTITEM, *LPDROPLISTITEM;

class CDWListBox : public CWindowImpl<CDWListBox, CListBox>
{
public:
    DECLARE_WND_CLASS(_T("DWExplorer_DWListBox"));

    BEGIN_MSG_MAP(CDWListBox)
        MESSAGE_HANDLER( WM_SETFOCUS   , OnSetFocus )
        MESSAGE_HANDLER( WM_ERASEBKGND , OnEraseBkGnd )
        MESSAGE_HANDLER( WM_MOUSEMOVE  , OnMouseMove  )
        MESSAGE_HANDLER( WM_LBUTTONUP  , OnLButtonUp  )
    END_MSG_MAP()

    LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        GetParent().SetFocus();

        return 0L;
    }

    LRESULT OnEraseBkGnd(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {
        return 1L;
    }

    LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

        BOOL bOutSide = FALSE;

        int nIndex = (int)ItemFromPoint( pt, bOutSide );

        if ( !bOutSide && nIndex != GetCurSel() )
            SetCurSel(nIndex);

        return DefWindowProc();
    }

    LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {
        LRESULT lResult = DefWindowProc();
        
        int nIndex = GetCurSel();

        if ( nIndex >= 0 )
        {
            ::SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(0,LBN_DBLCLK), 0);
        }

        return lResult;
    }

    void SetItemCount( int nCount )
    {
        int nItemCount = GetCount();

        while ( nItemCount != nCount )
        {
            if ( nCount < nItemCount )
                CListBox::DeleteString(0);
            else
                CListBox::AddString(L"");

            nItemCount = GetCount();
        }
    }
};

class CDWDropdownList : 
    public CWindowImpl<CDWDropdownList>,
    public COwnerDraw<CDWDropdownList>
{
public:
    CDWDropdownList(const ATL::CAtlArray<DROPLISTITEM>& vtDropList) :
        m_vtDropList(vtDropList)
    {
    }


    void SetItemCount( int nCount )
    {
        m_wndListBox.SetItemCount(nCount);
    }

    int GetSelIndex() const
    {
        if ( !m_wndListBox.IsWindow() )
            return -1;

        return m_wndListBox.GetCurSel();
    }

    BEGIN_MSG_MAP(CDWMainFrame)

        MESSAGE_HANDLER( WM_CREATE     , OnCreate   )
        MESSAGE_HANDLER( WM_KEYDOWN    , OnKeyDown  )
        MESSAGE_HANDLER( WM_SETFOCUS   , OnSetFocus )
        MESSAGE_HANDLER( WM_LBUTTONDOWN, OnLButtonDown )

        MESSAGE_HANDLER( WM_ERASEBKGND, OnEraseBkGnd )
        MESSAGE_HANDLER( WM_NCPAINT   , OnNcPaint    )
        MESSAGE_HANDLER( WM_NCCALCSIZE, OnNcCalcSize )
        MESSAGE_HANDLER( WM_SIZE      , OnSize       )

        COMMAND_CODE_HANDLER( LBN_DBLCLK, OnLbnDBlick)

        CHAIN_MSG_MAP(COwnerDraw<CDWDropdownList>)

    END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        RECT rcClient = {0};
        GetClientRect(&rcClient);

        m_wndListBox.Create(
            m_hWnd, rcClient, NULL,
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | 
            LBS_USETABSTOPS | LBS_OWNERDRAWFIXED | LBS_NODATA | LBS_NOTIFY,
            0);

        return DefWindowProc();
    }

    LRESULT OnLbnDBlick(WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/)
    {
        ShowWindow(SW_HIDE);

        int nIndex = m_wndListBox.GetCurSel();

        if ( nIndex >= 0 && nIndex < (int)m_vtDropList.GetCount())
        {
            m_wndEdit.SendMessage( WM_COMMAND, MAKEWPARAM(nIndex,LBN_DBLCLK) );
        }

        return 0L;
    }

    LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        LRESULT lResult = DefWindowProc();

        return lResult;
    }

    LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        m_wndEdit.SetFocus();
        return 0L;
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

    void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
    {
        CMemoryDC dc(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem);

        CDWSkinUIMgt* pskin = CDWSkinUIMgt::InstancePtr();
        if ( pskin == NULL )
            return;

        //COLORREF clrLine = HLS_TRANSFORM(pskin->clrFrameWindow, 30, 0);

        RECT rcBox = lpDrawItemStruct->rcItem;

        RECT rcText = rcBox;

        if ( (lpDrawItemStruct->itemState & ODS_SELECTED) )
        {
            dc.FillSolidRect(&rcText, HLS_TRANSFORM(pskin->clrFrameWindow, 60, 20));
        }
        else
        {
            CBrushHandle brush = (HBRUSH)m_wndParent.SendMessage( WM_CTLCOLOREDIT, 
                (WPARAM)dc.m_hDC, 
                (LPARAM)m_wndEdit.m_hWnd );

            if ( lpDrawItemStruct->itemID % 2 == 0 )
            {
                LOGBRUSH logBrush = { 0 };
                brush.GetLogBrush( &logBrush );

                CBrush brushNew;
                brushNew.CreateSolidBrush( HLS_TRANSFORM(logBrush.lbColor, -10, 0) );
                dc.FillRect(&rcText, brushNew);
            }
            else
            {
                dc.FillRect(&rcText, brush);
            }

            //CPen pen;
            //pen.CreatePen( PS_SOLID, 1, clrLine );
            //HPEN hPen = dc.SelectPen( pen );

            //dc.MoveTo( rcBox.left  + 5, rcBox.top );
            //dc.LineTo( rcBox.right - 5, rcBox.top);

            //dc.SelectPen( hPen );
        }


        if ( lpDrawItemStruct->itemID >= m_vtDropList.GetCount() )
        {
            return;
        }

        const DROPLISTITEM& item = m_vtDropList[lpDrawItemStruct->itemID];

        CIconHandle icon = item.icon;
        if ( icon.m_hIcon == NULL )
            icon = pskin->iconNull;

        icon.DrawIconEx(
            dc, rcText.left + 3, rcText.top + 3,
            16, 16 );

        rcText.left += 21;

        if ( m_wndEdit.GetDlgCtrlID() == ID_TOOL_SERACH_DROPDOWN )
            rcText.left += 5;

        HFONT hOldFont = dc.SelectFont(pskin->fontDefault);
        dc.SetBkMode(TRANSPARENT);

        if ( item.strRight.GetLength() > 0 )
            rcText.right = (rcText.right + rcText.left) / 5 * 3;

        dc.DrawText( item.strLeft, -1, &rcText,
            DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);

        if ( item.strRight.GetLength() > 0 )
        {
            rcText.left  = (rcText.right + 10);
            rcText.right = rcBox.right - 10;

            dc.DrawText( item.strRight, -1, &rcText,
                DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
        }

        dc.SelectFont(hOldFont);
    }

    void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
    {
        if(lpMeasureItemStruct->CtlType != ODT_MENU)
        {
            lpMeasureItemStruct->itemHeight = 20;
        }
        else
            lpMeasureItemStruct->itemHeight = ::GetSystemMetrics(SM_CYMENU);
    }

    LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CWindowDC dc(m_hWnd);

        CDWSkinUIMgt* pskin = CDWSkinUIMgt::InstancePtr();
        if ( pskin == NULL )
            return 1L;

        CRgn rgn;
        CRgn rgnNULL;

        RECT rcWindow;
        RECT rcBorder;
        GetWindowRect(&rcWindow);

        rcWindow.right  = rcWindow.right  - rcWindow.left;
        rcWindow.bottom = rcWindow.bottom - rcWindow.top;
        rcWindow.top    = rcWindow.left = 0;

        rcBorder = rcWindow;

        COLORREF clrBorder = HLS_TRANSFORM(pskin->clrFrameWindow, 60, 0);

        CPen pen;  pen .CreatePen( PS_SOLID, 1, clrBorder );
        HPEN   hOldPen   = dc.SelectPen( pen );
        HBRUSH hOldBrush = dc.SelectBrush( (HBRUSH)::GetStockObject(NULL_BRUSH) );

        dc.RoundRect( rcBorder.left, rcBorder.top-2, rcBorder.right, rcBorder.bottom, 3, 3);

        dc.SelectPen(hOldPen);
        dc.SelectBrush(hOldBrush);

        return 0L;
    }


    LRESULT OnEraseBkGnd(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {
        return 1L;
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
        LPMSG pMsg = ( LPMSG )lParam;

        LRESULT lRet = ::CallNextHookEx( s_hDropdownListHook, nCode, wParam, lParam);

        if ( pMsg->message == WM_LBUTTONDOWN || 
             pMsg->message == WM_LBUTTONDBLCLK ||
             pMsg->message == WM_RBUTTONDOWN ||
             pMsg->message == WM_RBUTTONDBLCLK ||
             pMsg->message == WM_NCLBUTTONDOWN || 
             pMsg->message == WM_NCLBUTTONDBLCLK ||
             pMsg->message == WM_NCRBUTTONDOWN ||
             pMsg->message == WM_NCRBUTTONDBLCLK )
        {
            POINT pt;
            GetCursorPos(&pt);

            RECT rcWindow;
            ::GetWindowRect(s_hWndDropdownList, &rcWindow);

            rcWindow.top -= 20;

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

    const ATL::CAtlArray<DROPLISTITEM>& m_vtDropList;

    static HHOOK s_hDropdownListHook;
    static HWND  s_hWndDropdownList;
};

__declspec(selectany) HHOOK CDWDropdownList::s_hDropdownListHook = NULL;
__declspec(selectany) HWND  CDWDropdownList::s_hWndDropdownList  = NULL;
