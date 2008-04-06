/********************************************************************
* CreatedOn: 2007-12-19   17:40
* FileName: SkinPropertyView.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/
#pragma once

#define WM_EDITKILLFOCUS (WM_USER  +0xFF2)

#include <skinxmlgdi.h>

class SkinPropertyView : 
    public CWindowImpl<SkinPropertyView, CListViewCtrl>,
    public COwnerDraw<SkinPropertyView>
{
    typedef CWindowImpl<SkinPropertyView, CListViewCtrl> theBaseClass;

public:

    class PropertyEditNotify
    {
    public:
        virtual void OnValueChange (
            LPCTSTR pszPropertyName,
            LPCTSTR pszOldValue,
            LPCTSTR pszNewValue) = 0;

        virtual void OnButtonClieck(LPCTSTR pszPropertyName) = 0;
    };

    //////////////////////////////////////////////////////////////////////////
    CImageList m_imagelist;

    PropertyEditNotify* m_pEditNotify;
    //////////////////////////////////////////////////////////////////////////
    CEdit   m_edit;
    CButton m_button;

    int m_neditindex;
    int m_nbtnindex;
    //////////////////////////////////////////////////////////////////////////
public:

    enum item_type
    {
        it_readonly,
        it_text,
        it_number,
        it_color,
        it_font,
        it_button
    };

    SkinPropertyView() :
        m_neditindex(-1),
        m_pEditNotify(0)
    {

    }

    void SetPropertyEditNotify(PropertyEditNotify* pNewNotify)
    {
        m_pEditNotify = pNewNotify;
    }

    BOOL Clear()
    {
        DeleteAllItems();

        m_edit.ShowWindow(SW_HIDE);
        m_button.ShowWindow(SW_HIDE);

        return TRUE;
    }

    BOOL AppendProperty( 
        LPCTSTR pszPropertyName,
        LPCTSTR pszValue,
        int ntype = it_text)
    {            
        int nAppendIndex = FindItem(pszPropertyName);
        if (nAppendIndex < 0)
            nAppendIndex = CListViewCtrl::InsertItem(0xFFFFF, pszPropertyName);

        if (nAppendIndex < 0)
            return FALSE;

        CListViewCtrl::SetItemText(nAppendIndex, 1, pszValue);
        CListViewCtrl::SetItemData(nAppendIndex, ntype);

        return TRUE;
    }

    void DeleteProperty(LPCTSTR pszPropertyName)
    {
        int nFindIndex = FindItem(pszPropertyName);

        if (nFindIndex >= 0)
            CListViewCtrl::DeleteItem(nFindIndex);            
    }

    BOOL SetProperty(
        LPCTSTR pszPropertyName,
        LPCTSTR pszNewValue)
    {
        int nFindIndex = FindItem(pszPropertyName);
        
        if (nFindIndex < 0)
            return FALSE;

        m_edit.ShowWindow(SW_HIDE);

        CListViewCtrl::SetItemText(nFindIndex, 1, pszNewValue);

        return TRUE;
    }

    BOOL GetProperty(LPCTSTR pszPropertyName,
        LPTSTR pszOutBuffer,
        int nBufferSize)
    {
        int nFindIndex = FindItem(pszPropertyName);

        if (nFindIndex < 0)
            return FALSE;

        CListViewCtrl::GetItemText(nFindIndex, 1, pszOutBuffer, nBufferSize);

        return TRUE;
    }

protected:

    int FindItem(LPCTSTR pszPropertyName)
    {
        LVFINDINFO findInfo = { 0 };

        findInfo.flags = LVFI_STRING;
        findInfo.psz   = pszPropertyName;

        return CListViewCtrl::FindItem(&findInfo, -1);
    }

public:

    DECLARE_WND_SUPERCLASS(NULL, CListViewCtrl::GetWndClassName())


    BOOL SubclassWindow(HWND hWnd)
    {
        BOOL bResult = theBaseClass::SubclassWindow(hWnd);
        
        _InitPropertyView();

        return bResult;
    }

    //////////////////////////////////////////////////////////////////////////

    BOOL PreTranslateMessage(MSG* pMsg)
    {
        pMsg;
        return FALSE;
    }

    BOOL _InitPropertyView()
    {
        ModifyStyle( 0, LVS_REPORT | LVS_NOSORTHEADER | LVS_OWNERDRAWFIXED);

        SetExtendedListViewStyle(
            LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP | LVS_EX_LABELTIP);

        m_imagelist.Create( 1, 16, 0, ILC_COLOR, 0 );

        SetImageList( m_imagelist, LVSIL_SMALL );

        InsertColumn( 0, _T("Property"), LVCFMT_LEFT, 100 );
        InsertColumn( 1, _T("Value"), LVCFMT_LEFT, 130 );

        m_edit.Create(m_hWnd, 
            rcDefault, 
            NULL, 
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_WANTRETURN);

        m_button.Create(m_hWnd, 
            rcDefault, 
            _T("..."), 
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON);

        m_edit.SetFont(GetFont());
        m_button.SetFont(GetFont());

        GetEditProc(m_edit) = (WNDPROC)::SetWindowLong(m_edit, GWL_WNDPROC, (LONG) HookEditMsg );

        //m_edit.SetWindowLong(GWL_WNDPROC, (LONG) HookEditMsg );
        
        m_neditindex = -1;

        return TRUE;
    }

    BEGIN_MSG_MAP(SkinPropertyView)

        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
        MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
        MESSAGE_HANDLER(WM_EDITKILLFOCUS, OnEditKillFocus)
        COMMAND_CODE_HANDLER(BN_CLICKED, OnBtnClicked)

        REFLECTED_NOTIFY_CODE_HANDLER(LVN_DELETEITEM, OnDeleteItem)

        CHAIN_MSG_MAP_ALT(COwnerDraw<SkinPropertyView>, 1)

    END_MSG_MAP()


    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        LRESULT lResult = DefWindowProc();
        
        _InitPropertyView();

        return lResult;
    }

    LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {
        LRESULT lResult = DefWindowProc();

        LVHITTESTINFO info = { 0 };

        info.flags = LVHT_ONITEMLABEL;

        info.pt.x = GET_X_LPARAM(lParam);
        info.pt.y = GET_Y_LPARAM(lParam);

        int nIndex = CListViewCtrl::HitTest(&info);

        if (nIndex < 0)
            return lResult;

        RECT rcSubItem = { 0 };

        CListViewCtrl::GetSubItemRect(nIndex, 1, LVIR_LABEL, &rcSubItem);

        if (::PtInRect(&rcSubItem, info.pt))
        {
            int ntype = CListViewCtrl::GetItemData(nIndex);

            m_edit.ShowWindow(SW_HIDE);
            m_button.ShowWindow(SW_HIDE);

            switch(ntype)
            {
            case it_readonly:
                break;
            case it_button:
            case it_color:
            case it_font:
                {
                    rcSubItem.left = rcSubItem.right - 20;
                    rcSubItem.top+=0;
                    rcSubItem.right--;
                    rcSubItem.bottom--;

                    m_button.MoveWindow(&rcSubItem);
                    m_button.ShowWindow(SW_SHOW);
                    //m_button.SetFocus();

                    m_nbtnindex = nIndex;

                    CListViewCtrl::GetSubItemRect(nIndex, 1, LVIR_LABEL, &rcSubItem);

                    rcSubItem.right -= 20;

                }

                //break;
            case it_text:
            case it_number:
            default:
                {
                    TCHAR szBuffer[1024] = { 0 };

                    CListViewCtrl::GetItemText(nIndex, 1,
                        szBuffer, 1024);

                    rcSubItem.left += 2;
                    rcSubItem.top+=2;
                    rcSubItem.right--;
                    rcSubItem.bottom--;
                    
                    m_edit.SetWindowText(szBuffer);
                    m_edit.MoveWindow(&rcSubItem);
                    m_edit.ShowWindow(SW_SHOW);
                    m_edit.SetFocus();

                    lParam = MAKELPARAM(info.pt.x - rcSubItem.left,
                        info.pt.y - rcSubItem.top);

                    m_edit.SendMessage(uMsg, wParam, lParam);
                    m_edit.SendMessage(WM_LBUTTONUP, wParam, lParam);
                    m_edit.SendMessage(WM_MOUSEMOVE, wParam, lParam);

                    if (ntype == it_number)
                        m_edit.ModifyStyle(0, ES_NUMBER);
                    else
                        m_edit.ModifyStyle(ES_NUMBER, 0);


                    m_neditindex = nIndex;
                }

                break;
            }
        }

        return lResult;
    }

    LRESULT OnEditKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {
        if (m_neditindex >= 0)
        {
            TCHAR szOldBuffer[1024] = { 0 };
            TCHAR szNewBuffer[1024] = { 0 };
            TCHAR szPropertyName[1024] = { 0 };

            CListViewCtrl::GetItemText(m_neditindex, 1,
                szOldBuffer, 1024);

            m_edit.GetWindowText(szNewBuffer, 1024);

            if (!_tcscmp(szOldBuffer, szNewBuffer))
                return 0;

            CListViewCtrl::SetItemText(m_neditindex, 1,
                szNewBuffer);

            CListViewCtrl::GetItemText(m_neditindex, 0,
                szPropertyName, 1024);

            if (m_pEditNotify != NULL && _tcslen(szPropertyName) > 0)
            {

                m_pEditNotify->OnValueChange(szPropertyName,
                    szOldBuffer, szNewBuffer);
            }
        }

        return 0;
    }

    LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LRESULT lResult = DefWindowProc();
        
        //if (GetFocus() != m_button.m_hWnd)
        //    m_button.ShowWindow(SW_HIDE);
            
        bHandled = TRUE;

        return lResult;
    }

    LRESULT OnBtnClicked(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        if (m_nbtnindex >= 0)
        {
            TCHAR szPropertyName[1024] = { 0 };

            if (CListViewCtrl::GetItemData(m_nbtnindex) == it_color)
            {
                TCHAR szOldBuffer[1024] = { 0 };
                TCHAR szNewBuffer[1024] = { 0 };

                COLORREF clr = 0;


                CListViewCtrl::GetItemText(m_nbtnindex, 1,
                    szOldBuffer, 1024);

                KSG::skinxmlcolor xmlcolor;
                
                (KSG::CString&)xmlcolor = (szOldBuffer);

                xmlcolor >> clr;
                
                CColorDialog clrdlg(clr);

                if (clrdlg.DoModal() != IDOK)
                    return 0;

                clr = clrdlg.GetColor();

                xmlcolor << clr;

                if (!_tcscmp(szOldBuffer, xmlcolor))
                    return 0;

                CListViewCtrl::SetItemText(m_nbtnindex, 1,
                    xmlcolor);
                m_edit.SetWindowText(xmlcolor);

                if (m_pEditNotify != NULL)
                {
                    CListViewCtrl::GetItemText(m_nbtnindex, 0,
                        szPropertyName, 1024);

                    m_pEditNotify->OnValueChange(szPropertyName,
                        szOldBuffer, xmlcolor);
                }

                return 0;
            }
            else if (CListViewCtrl::GetItemData(m_nbtnindex) == it_font)
            {
                TCHAR szOldBuffer[1024] = { 0 };
                TCHAR szNewBuffer[1024] = { 0 };

                LOGFONT logFont = { 0 };

                CListViewCtrl::GetItemText(m_nbtnindex, 1,
                    szOldBuffer, 1024);

                KSG::skinxmlfont xmlfont;

                (KSG::CString&)xmlfont = (szOldBuffer);

                xmlfont >> logFont;

                CFontDialog fontdlg(&logFont);

                if (fontdlg.DoModal() != IDOK)
                    return 0;

                //clr = fontdlg.GetFont();

                xmlfont << logFont;

                if (!_tcscmp(szOldBuffer, xmlfont))
                    return 0;

                CListViewCtrl::SetItemText(m_nbtnindex, 1,
                    xmlfont);
                m_edit.SetWindowText(xmlfont);

                if (m_pEditNotify != NULL)
                {
                    CListViewCtrl::GetItemText(m_nbtnindex, 0,
                        szPropertyName, 1024);

                    m_pEditNotify->OnValueChange(szPropertyName,
                        szOldBuffer, xmlfont);
                }

                return 0;
            }

            if (m_pEditNotify != NULL)
            {
                CListViewCtrl::GetItemText(m_nbtnindex, 0,
                    szPropertyName, 1024);

                m_pEditNotify->OnButtonClieck(szPropertyName);
            }
        }

        return 0;
    }


    static LRESULT CALLBACK HookEditMsg( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
    {
        WNDPROC pfnOldProc = GetEditProc(hWnd);

        LRESULT lResult = CallWindowProc( pfnOldProc, hWnd, uMsg, wParam, lParam);

        if (uMsg == WM_KILLFOCUS)
            ::SendMessage(::GetParent(hWnd), WM_EDITKILLFOCUS, 0, 0);

        if (uMsg == WM_KEYDOWN && wParam == VK_RETURN)
            ::SendMessage(::GetParent(hWnd), WM_EDITKILLFOCUS, 0, 0);

        return lResult;
    }

    static WNDPROC& GetEditProc(HWND hWnd)
    {
        static WNDPROC gs_wndEditProc;
        return gs_wndEditProc;
    }

    void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
    {

        CDCHandle dc = lpDrawItemStruct->hDC;

        RECT rcItem = lpDrawItemStruct->rcItem;
        int  nItem  = lpDrawItemStruct->itemID;

        bool bSelected = (lpDrawItemStruct->itemState & ODS_SELECTED) != 0;
        bool bFocus    = GetFocus() == m_hWnd;

        CBrush brushBkgnd;
        brushBkgnd.CreateSolidBrush( GetSysColor(COLOR_WINDOW) );

        dc.FillRect(&rcItem, brushBkgnd);


        RECT    rcSubItem;
        TCHAR   szBuffer[MAX_PATH] = { 0 };

        int nColumnCount = 2;

        for (int nColumn = 0; nColumn < nColumnCount; nColumn++)
        {
            GetSubItemRect( nItem, nColumn, LVIR_LABEL, &rcSubItem );

            GetItemText( nItem, nColumn, szBuffer, MAX_PATH);

            if (nColumn == 0 && bSelected)
            {
                COLORREF clrBkGnd = 
                    GetSysColor( COLOR_HIGHLIGHT);

                CBrush brushBkgnd;
                brushBkgnd.CreateSolidBrush( clrBkGnd );
                
                //rcSubItem.left = 1;

                dc.FillRect(&rcSubItem, brushBkgnd);

                dc.SetTextColor(
                    GetSysColor(COLOR_HIGHLIGHTTEXT) );
            }
            else
            {
                if (GetItemData(nItem) == it_readonly) // && nColumn == 1)
                    dc.SetTextColor(GetSysColor(COLOR_GRAYTEXT));
                else
                    dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));
            }


            if (nColumn > 0)
                rcSubItem.left += 2;
            else 
                rcSubItem.left += 2;

            dc.DrawText(szBuffer, -1 , &rcSubItem, DT_VCENTER | DT_SINGLELINE );
        }

    }

    void DeleteItem(LPDELETEITEMSTRUCT /*lpDeleteItemStruct*/)
    {
    }

    LRESULT OnDeleteItem(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
    {
        LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

        return 0;
    }

};
