/********************************************************************
* CreatedOn: 2008-6-23   16:50
* FileName: CFindEditCtrl.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/


#pragma once

namespace KSGUI{


class CFindEditButton : 
    public KSGUI::SkinWindowImpl< CFindEditButton, KSGUI::CSkinButton>
{
    public:

    CFindEditButton()
    {
        m_bMouseHot = FALSE;
    }

    BOOL SubclassWindow(HWND hWnd)
    {
        ATLASSERT(m_hWnd == NULL);
        ATLASSERT(::IsWindow(hWnd));

        BOOL bRet = KSGUI::SkinWindowImpl< CFindEditButton, KSGUI::CSkinButton>::SubclassWindow(hWnd);
        if(bRet)
        {
            _Init();
        }
        return bRet;
    }

    //////////////////////////////////////////////////////////////////////////

    BEGIN_MSG_MAP(CSkinColorButtonImpl)

        MESSAGE_HANDLER( WM_CREATE    , OnCreate     )
        MESSAGE_HANDLER( WM_ERASEBKGND, OnEraseBKGnd )
        MESSAGE_HANDLER( WM_PAINT     , OnPaint      )

        MESSAGE_HANDLER( WM_MOUSEMOVE , OnMouseMove  )
        MESSAGE_HANDLER( WM_MOUSELEAVE, OnMouseLeave )

        MESSAGE_HANDLER( WM_LBUTTONDOWN, OnMouseMsg)
        MESSAGE_HANDLER( WM_LBUTTONUP  , OnMouseMsg)

        MESSAGE_HANDLER( WM_ENABLE    , OnMouseMsg)
        MESSAGE_HANDLER( WM_SETFOCUS  , OnMouseMsg)
        MESSAGE_HANDLER( WM_KILLFOCUS , OnMouseMsg)
        MESSAGE_HANDLER( WM_SETTEXT   , OnMouseMsg)

    END_MSG_MAP()

    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;
        _Init();
        return 1L;
    }


    LRESULT OnEraseBKGnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 1L;
    }

    LRESULT OnMouseMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LRESULT lResult = DefWindowProc();

        if (IsWindow())
            Invalidate(FALSE);

        return lResult;
    }


    LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LRESULT lResult = DefWindowProc();

        if (!m_bMouseHot)
        {
            TRACKMOUSEEVENT tme = { 0 };

            tme.cbSize  = sizeof(tme);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = m_hWnd;

            _TrackMouseEvent(&tme);

            m_bMouseHot = TRUE;

            Invalidate( FALSE );
        }

        return lResult;
    }

    LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LRESULT lResult = DefWindowProc();

        if (m_bMouseHot)
        {
            m_bMouseHot = FALSE;
            Invalidate( FALSE );
        }

        return lResult;
    }

    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CPaintDC dc(m_hWnd);

        DoPaint((HDC)dc);

        return 0;
    }

    void DoPaint(KSGUI::CSkinDCHandle dc)
    {
        BOOL bHot      = m_bMouseHot;
        BOOL bFocus    = ( GetFocus() == m_hWnd );
        BOOL bChecked  = GetState() & BST_PUSHED;
        BOOL bDisabled = GetStyle() & WS_DISABLED;

        COLORREF clrNormal = GetSysColor(COLOR_GRAYTEXT);
        COLORREF clrHot    = GetSysColor(COLOR_HOTLIGHT);

        COLORREF clrLine = bHot ? clrHot : clrNormal;

        RECT rcClient = { 0 };
        GetClientRect(&rcClient);

        RECT rcBox = rcClient;

        rcBox.left += 3;
        rcBox.top  += 3;
        rcBox.right  -= 3;
        rcBox.bottom -= 3;

        dc.SkinLine( rcBox.left - 1, rcBox.top,
            rcBox.right - 1, rcBox.bottom, clrLine, PS_SOLID, 1);
        dc.SkinLine( rcBox.left , rcBox.top,
            rcBox.right , rcBox.bottom, clrLine, PS_SOLID, 1);
        dc.SkinLine( rcBox.left + 1, rcBox.top,
            rcBox.right + 1, rcBox.bottom, clrLine, PS_SOLID, 1);

        dc.SkinLine( rcBox.right, rcBox.top,
            rcBox.left , rcBox.bottom, clrLine, PS_SOLID, 1);
        dc.SkinLine( rcBox.right - 1, rcBox.top,
            rcBox.left - 1, rcBox.bottom, clrLine, PS_SOLID, 1);
        dc.SkinLine( rcBox.right - 2, rcBox.top,
            rcBox.left - 2, rcBox.bottom, clrLine, PS_SOLID, 1);


    }

    void _Init()
    {
        int nRet = 0;

        ModifyStyle(0, BS_OWNERDRAW);
    }

public:

    BOOL     m_bMouseHot;
};


class CFindEditCtrl : 
    public SkinWindowImpl< CFindEditCtrl, KSGUI::CSkinStatic>

{
private:
    
    KSGUI::CString m_strLeftText;
    CBitmap m_bmpRight;

    KSGUI::CFindEditButton m_wndBtn;
    KSGUI::CSkinEdit       m_wndEdit;
public:

    CFindEditCtrl()
    {
        m_strLeftText = _T("Find...");
    }
    
    int SetFindLeftText( LPCTSTR pszText )
    {
        m_strLeftText = pszText;
        return m_strLeftText.GetLength();
    }

    BOOL SetRightBitmap(HBITMAP hBmp)
    {
        m_bmpRight.Attach( hBmp );

        return m_bmpRight.m_hBitmap != NULL;
    }

    BOOL SubclassWindow(HWND hWnd)
    {
        BOOL bResult = SkinWindowImpl< CFindEditCtrl, KSGUI::CSkinStatic>::SubclassWindow( hWnd );
        
        if (bResult)
            _Init();

        return bResult;
    }

    int GetFindText( LPTSTR szText, int nMax )
    {
        return m_wndEdit.GetWindowText( szText, nMax );
    }

    ATL::CString GetFindText()
    {
        TCHAR szBuffer[MAX_PATH] = { 0 };
        m_wndEdit.GetWindowText(szBuffer, MAX_PATH);
        
        return ATL::CString( szBuffer );
    }

    BEGIN_MSG_MAP(CFindEditCtrl)
        MESSAGE_HANDLER( WM_SIZE  , OnSize   )
        MESSAGE_HANDLER( WM_CREATE, OnCreate )

        MESSAGE_HANDLER( WM_PAINT, OnPaint )

        MESSAGE_HANDLER( WM_SETFOCUS , OnReflash )
        MESSAGE_HANDLER( WM_KILLFOCUS, OnReflash )
        
        MESSAGE_HANDLER( WM_LBUTTONDOWN, OnLButtonDown )

        COMMAND_CODE_HANDLER( EN_SETFOCUS , OnEditSetFocus  )
        COMMAND_CODE_HANDLER( EN_KILLFOCUS, OnEditKillFocus )
        COMMAND_CODE_HANDLER( EN_CHANGE   , OnEditChange    )

        COMMAND_CODE_HANDLER( BN_CLICKED  , OnBtnClicked    )

    END_MSG_MAP()

    void _Init()
    {
        RECT rcBtn  = { 0 };
        RECT rcEdit = { 0 };

        GetBtnRect(rcBtn);
        GetEditRect(rcEdit);

        ModifyStyle(0, SS_NOTIFY);

        m_wndBtn.Create( m_hWnd, &rcBtn, _T("X"), WS_CHILD);
        m_wndEdit.Create( m_hWnd, &rcEdit, _T(""), WS_CHILD | ES_AUTOVSCROLL);

        m_wndBtn.SetFont(GetFont());
        m_wndEdit.SetFont(GetFont());
    }

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        LRESULT lResult = DefWindowProc();

        _Init();

        return lResult; 
    }

    LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        LRESULT lResult = DefWindowProc();

        return lResult;
    }

    void GetBtnRect(RECT& rcBtn)
    {
        int nBtnBig = 14;

        GetClientRect(&rcBtn);

        rcBtn.left  = rcBtn.right - nBtnBig - 2;
        rcBtn.right -= 2;

        rcBtn.top = 2;
        rcBtn.bottom = rcBtn.top + nBtnBig;
    }

    void GetEditRect(RECT& rcBtn)
    {
        int nBtnBig = 14;

        GetClientRect(&rcBtn);

        rcBtn.right  = rcBtn.right - nBtnBig - 4;

        rcBtn.left   += 1;
        rcBtn.top    += 2;
        rcBtn.bottom -= 2;
    }

    LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {
        LRESULT lResult = DefWindowProc();

        if ( !m_wndEdit.IsWindowVisible() )
        {
            m_wndEdit.ShowWindow(SW_SHOWDEFAULT);
            m_wndEdit.SetFocus();

            CWindow::Invalidate();
        }

        return lResult;
    }

    LRESULT OnEditSetFocus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        return 1L;
    }

    LRESULT OnEditKillFocus(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        TCHAR szBuffer[MAX_PATH] = { 0 };
        int nLen = m_wndEdit.GetWindowText(szBuffer, MAX_PATH);
        
        if (nLen <= 0)
        {
            m_wndEdit.ShowWindow(SW_HIDE);
            CWindow::Invalidate();
        }


        return 1L;
    }

    LRESULT OnEditChange(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        TCHAR szBuffer[MAX_PATH] = { 0 };
        int nLen = m_wndEdit.GetWindowText(szBuffer, MAX_PATH);

        if (nLen > 0)
            m_wndBtn.ShowWindow(SW_SHOWDEFAULT);
        else
            m_wndBtn.ShowWindow(SW_HIDE);

        return ::SendMessage(GetParent(), WM_COMMAND, 
            MAKEWPARAM(GetDlgCtrlID(), wNotifyCode), LPARAM(szBuffer));


        return 1L;
    }

    LRESULT OnBtnClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        m_wndEdit.SetWindowText(_T(""));

        m_wndBtn.ShowWindow(SW_HIDE);
        m_wndEdit.ShowWindow(SW_HIDE);

        return 1L;
    }

    LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        CPaintDC dc(m_hWnd);
        
        TCHAR szBuffer[MAX_PATH] = { 0 };
        int nLen = m_wndEdit.GetWindowText(szBuffer, MAX_PATH);

        RECT rcClient = { 0 };

        GetClientRect(&rcClient);
        dc.FillSolidRect(&rcClient, GetSysColor(COLOR_WINDOW));

        if ( m_wndEdit.IsWindowVisible() || nLen > 0 )
        {
            LRESULT lResult = ::DefWindowProc(m_hWnd, WM_PAINT, (WPARAM)dc.m_hDC, NULL);

            return lResult;
        }

        COLORREF clrOld = dc.SetTextColor( GetSysColor( COLOR_GRAYTEXT ) );
        HFONT hOldFont = dc.SelectFont(GetFont());
        
        rcClient.left += 2;

        dc.DrawText( m_strLeftText, -1, &rcClient, 
            DT_SINGLELINE | DT_VCENTER);

        dc.SelectFont(hOldFont);
        dc.SetTextColor(clrOld);

        while ( m_bmpRight.m_hBitmap != NULL )
        {
            BITMAP bmp = { 0 };

            if (!m_bmpRight.GetBitmap(&bmp))
                break;

            CDC dcMem;
            dcMem.CreateCompatibleDC(dc);

            HBITMAP hOldBmp = dcMem.SelectBitmap( m_bmpRight );

            dc.BitBlt( rcClient.right - bmp.bmWidth - 2, 
                (rcClient.bottom - bmp.bmHeight) / 2,
                bmp.bmWidth, bmp.bmHeight, 
                dcMem, 0, 0, SRCCOPY);

            dcMem.SelectBitmap( hOldBmp );

            break;
        }


        return 1L;
    }

    LRESULT OnReflash(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        LRESULT lResult = DefWindowProc();

        CWindow::Invalidate();

        return lResult;
    }
};


};
