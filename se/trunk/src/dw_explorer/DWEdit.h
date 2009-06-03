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


#define WM_EDIT_KEY_RETURN  (WM_USER + 1033)


class CDWEdit : 
    public CWindowImpl<CDWEdit, CEdit>,
    public IDropTarget
{
public:

    DECLARE_WND_CLASS(_T("DWExplorer_DWEdit"));

    CDWEdit() :
        m_wndDropList(m_vtDropList)
    {
    }

    void ClearDropdownList( BOOL bRePaint = FALSE )
    {
        m_vtDropList.RemoveAll();
        if ( bRePaint && m_wndDropList.IsWindowVisible() )
        {
            ResizeDropdownList();
            m_wndDropList.Invalidate();
        }
    }

    void AddDropdownList( const DROPLISTITEM& item, BOOL bRePaint = FALSE )
    {
        m_vtDropList.Add( item );

        if ( bRePaint && m_wndDropList.IsWindowVisible() )
        {
            ResizeDropdownList();
            m_wndDropList.Invalidate();
        }
    }

    void AddDropdownList( LPCTSTR pszLeft, 
        LPCTSTR pszRight = NULL, 
        HICON hIcon = NULL, 
        COLORREF clrLeft = -1, 
        COLORREF clrRight = -1,
        BOOL bRePaint = FALSE )
    {
        for ( size_t idx = 0; idx < m_vtDropList.GetCount(); idx++ )
        {
            if ( !StrCmpI(pszLeft, m_vtDropList[idx].strLeft) )
                return;
        }

        DROPLISTITEM item;

        item.strLeft  = pszLeft;
        item.strRight = pszRight;
        item.clrLeft  = clrLeft;
        item.clrRight = clrRight;
        item.icon     = hIcon;

        AddDropdownList(item, bRePaint);
    }

    BEGIN_MSG_MAP(CDWMainFrame)

        MESSAGE_HANDLER( WM_CREATE     , OnCreate  )
        MESSAGE_HANDLER( WM_DESTROY    , OnDestroy )
        MESSAGE_HANDLER( WM_KEYDOWN    , OnKeyDown  )
        MESSAGE_HANDLER( WM_SETFOCUS   , OnSetFocus )
        MESSAGE_HANDLER( WM_KILLFOCUS  , OnKillFocus )
        MESSAGE_HANDLER( WM_LBUTTONDOWN, OnLButtonDown )

        MESSAGE_HANDLER( WM_PAINT      , OnPaint    )

        COMMAND_CODE_HANDLER( LBN_DBLCLK, OnLbnDBlick)

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

        RegisterDragDrop(m_hWnd, this);


        return DefWindowProc();
    }

    LRESULT OnLbnDBlick(WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/)
    {
        int nIndex = wID;
        if ( nIndex >= 0 && nIndex < (int)m_vtDropList.GetCount())
        {
            SetWindowText( m_vtDropList[nIndex].strLeft );
            SetFocus();
            
            GetParent().PostMessage( WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), LBN_DBLCLK) );
        }

        return 0L;
    }


    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        if ( m_wndDropList.IsWindow() )
            m_wndDropList.DestroyWindow();

        RevokeDragDrop(m_hWnd);

        return DefWindowProc();
    }

    BOOL IsDropdownList() const
    {
        if ( !m_wndDropList.IsWindow() )
            return FALSE;

        return m_wndDropList.IsWindowVisible();
    }

    void ResizeDropdownList()
    {
        RECT rcWindow;
        GetWindowRect( &rcWindow );

        rcWindow.left  -= 20;
        rcWindow.right += 14;

        if ( GetDlgCtrlID() == ID_TOOL_SERACH_DROPDOWN )
            rcWindow.left -= 5;

        size_t uItemCount = m_vtDropList.GetCount();
        if ( uItemCount <= 0 )
            uItemCount = 1;

        m_wndDropList.SetItemCount(uItemCount);

        if ( uItemCount > 30 )
            uItemCount = 30;
        rcWindow.top    = rcWindow.bottom + 1;
        rcWindow.bottom = rcWindow.top + 1 + uItemCount * 20;

        m_wndDropList.MoveWindow( &rcWindow );
    }
    
    LRESULT ShowDropdownList( int nSel = -1 )
    {
        ResizeDropdownList();
        SetFocus();

        if ( nSel >= -1)
            m_wndDropList.m_wndListBox.SetCurSel(nSel);

        if ( !m_wndDropList.IsWindowVisible() )
        {
            m_wndDropList.ShowWindow( SW_SHOWNOACTIVATE );
        }
        else
            m_wndDropList.Invalidate();

        return TRUE;
    }

    LRESULT HideDropdownList()
    {
        if ( m_wndDropList.IsWindow() && m_wndDropList.IsWindowVisible() )
            m_wndDropList.ShowWindow( SW_HIDE );

        return TRUE;
    }

    LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;

        switch ( wParam )
        {
        case VK_ESCAPE:
            HideDropdownList();
            break;
        case VK_RETURN:
            if ( m_wndDropList.IsWindowVisible() && m_vtDropList.GetCount() > 0 )
            {
                if ( m_wndDropList.GetSelIndex() >= 0 )
                {
                    m_wndDropList.SendMessage(WM_COMMAND, MAKEWPARAM(0,LBN_DBLCLK), 0);
                    break;
                }
            }
            ::PostMessage( GetParent(), WM_EDIT_KEY_RETURN, GetDlgCtrlID(), 0 );
            break;
        case VK_UP:
        case VK_DOWN:
            if ( m_wndDropList.IsWindowVisible() )
            {
                if ( m_wndDropList.m_wndListBox.GetCurSel() < 0 )
                {
                    if ( wParam == VK_UP )
                        m_wndDropList.m_wndListBox.SetCurSel(m_wndDropList.m_wndListBox.GetCount()-1);
                    else
                        m_wndDropList.m_wndListBox.SetCurSel(0);
                }
                else
                    m_wndDropList.m_wndListBox.PostMessage(uMsg, wParam, lParam );
            }
            else
            {
                ShowDropdownList();
            }
            bHandled = TRUE;
        }

        return 0L;
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

        HWND hWnd = GetFocus();

        if ( (hWnd != m_wndDropList && hWnd != m_wndDropList.m_wndListBox) && IsDropdownList() )
        {
            HideDropdownList();
        }
        else
        {
            SetSelNone();
        }

        Invalidate();

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

        CDWSkinUIMgt* pskin = CDWSkinUIMgt::InstancePtr();
        if ( pskin == NULL )
            return 1L;

        CPaintDC dc(m_hWnd);


        COLORREF clrText = HLS_TRANSFORM(pskin->clrFrameWindow, 60, 0);

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

    ///////////////////////////////////////////////////////////////////////////
    // IDropTarget
    ULONG STDMETHODCALLTYPE AddRef()
    {
        return 1;
    }

    ULONG STDMETHODCALLTYPE Release()
    {
        return 1;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(const IID &riid, void **ppvObject)
    {
        if (riid == IID_IUnknown) 
        { 
            *ppvObject = (IUnknown*)(this); 
            return S_OK; 
        } 
        if (riid == IID_IDropTarget)
        {
            *ppvObject = (IDropTarget*)(this); 
            return S_OK; 
        }

        return E_NOINTERFACE ;
    }
    
    HRESULT STDMETHODCALLTYPE DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
    {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
    {
        *pdwEffect |= DROPEFFECT_COPY ;

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragLeave()
    {
        return S_OK ;
    }

    HRESULT STDMETHODCALLTYPE Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
    {
        *pdwEffect = DROPEFFECT_NONE;


        FORMATETC FormatEtc = { 0 };
        STGMEDIUM StgMedium = { 0 };

        FormatEtc.cfFormat = CF_UNICODETEXT;
        FormatEtc.tymed    = TYMED_HGLOBAL;
        pDataObj->GetData(&FormatEtc, &StgMedium);
        if (StgMedium.hGlobal)
        {
            LPCTSTR lpcszData = (LPCTSTR)GlobalLock(StgMedium.hGlobal);
            
            SetWindowText( lpcszData );

            GlobalUnlock(StgMedium.hGlobal);
            GlobalFree(StgMedium.hGlobal);
        }

        return S_OK ;
    }


    ATL::CString    m_strBkText;
    CDWDropdownList m_wndDropList;

    ATL::CAtlArray<DROPLISTITEM> m_vtDropList;
};
