/********************************************************************
* CreatedOn: 2007-12-19   17:40
* FileName:  SkinResStringListView.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/
#pragma once

#include "SkinControlsMgt.h"

class SkinResStringListView : 
    public CWindowImpl<SkinResStringListView, CListViewCtrl>,
    public SkinTreeItemControl
{

    CEdit   m_edit;
    int     m_neditindex;
    int     m_neditsubitem;
public:

    SkinResStringListView()
    {
        m_neditindex   = -1;
        m_neditsubitem = 0;
    }

    ~SkinResStringListView()
    {
        m_hWnd = (0);
    }

    DECLARE_WND_SUPERCLASS(NULL, CListViewCtrl::GetWndClassName())

    BOOL PreTranslateMessage(MSG* pMsg)
    {
        pMsg;
        return FALSE;
    }

    virtual void InitResult(HTREEITEM hTreeItem)
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        if (m_hWnd == NULL)
        {

            Create(ControlsMgt.m_piSkinFrame->GetResultParentWnd(), 
                rcDefault, NULL, 
                WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_SINGLESEL | LVS_NOSORTHEADER,
                WS_EX_CLIENTEDGE);

            InsertColumn(0, _T("Type")   , LVCFMT_LEFT, 60 );
            InsertColumn(2, _T("ID")     , LVCFMT_LEFT, 150);
            InsertColumn(3, _T("Caption"), LVCFMT_LEFT, 550);

            ModifyStyle( 0, LVS_REPORT | LVS_NOSORTHEADER | LVS_SINGLESEL | LVS_SHOWSELALWAYS);

            SetExtendedListViewStyle(
                LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP | LVS_EX_LABELTIP);
            

            m_edit.Create(m_hWnd, 
                rcDefault, 
                NULL, 
                WS_CHILD | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_WANTRETURN );

            m_edit.SetFont(GetFont());

            GetEditProc(m_edit) = (WNDPROC)::SetWindowLong(m_edit, GWL_WNDPROC, (LONG) HookEditMsg );

        }

        CListViewCtrl::DeleteAllItems();

        std::vector<SkinStrResDoc::STRRESITEMINFO>& vtList =
            ControlsMgt.m_resDocument.m_resStrDoc.m_vtItemList;

        for (size_t i = 0; i < vtList.size(); i++)
        {
            int nIndex = InsertItem(0xFFFFF, _T("String"));
            SetItemText(nIndex, 1, vtList[i].strIDName );
            SetItemText(nIndex, 2, vtList[i].strValue );
        }

        SetItemText(InsertItem(0xFFFFF, _T("New...")), 1, _T("IDS_"));

    }

    static LRESULT CALLBACK HookEditMsg( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
    {
        WNDPROC pfnOldProc = GetEditProc(hWnd);

        LRESULT lResult = CallWindowProc( pfnOldProc, hWnd, uMsg, wParam, lParam);

        if (uMsg == WM_KILLFOCUS)
            ::SendMessage(::GetParent(hWnd), WM_EDITKILLFOCUS, 0, 0);

        return lResult;
    }

    static WNDPROC& GetEditProc(HWND hWnd)
    {
        static WNDPROC gs_wndEditProc;
        return gs_wndEditProc;
    }


    virtual void ShowResult(HTREEITEM hTreeItem, LPARAM lParam)
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();
        
        ShowWindow(SW_SHOW);

        ControlsMgt.m_piSkinFrame->SetActiveResultWindow(m_hWnd);
    }

    virtual void HideResult(HTREEITEM hTreeItem, LPARAM lParam)
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        ControlsMgt.m_skinResPropertyView.Clear();

        ShowWindow(SW_HIDE);

    }


    BEGIN_MSG_MAP(SkinResStringListView)

        MESSAGE_HANDLER(WM_KEYDOWN      , OnKeyDown)
        MESSAGE_HANDLER(WM_LBUTTONDOWN  , OnLButtonDown)
        MESSAGE_HANDLER(WM_EDITKILLFOCUS, OnEditKillFocus)
        
        REFLECTED_NOTIFY_CODE_HANDLER(LVN_ITEMCHANGED, OnItemChanged)

    END_MSG_MAP()

    // Handler prototypes (uncomment arguments if needed):
    //	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    //	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    //	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
    
    LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {
        LRESULT lResult = DefWindowProc();

        LVHITTESTINFO info = { 0 };

        info.flags = LVHT_ONITEMLABEL;

        info.pt.x = GET_X_LPARAM(lParam);
        info.pt.y = GET_Y_LPARAM(lParam);

        int nIndex = CListViewCtrl::HitTest(&info);

        if (nIndex < 0)
        {
            m_edit.ShowWindow(SW_HIDE);

            return lResult;
        }

        RECT rcSubItem = { 0 };

        for (int sub = 1; sub <= 2; sub++)
        {
            CListViewCtrl::GetSubItemRect(nIndex, sub, LVIR_LABEL, &rcSubItem);

            if (::PtInRect(&rcSubItem, info.pt))
            {
                TCHAR szBuffer[1024] = { 0 };

                CListViewCtrl::GetItemText(nIndex, sub,
                    szBuffer, 1024);

                rcSubItem.left += 5;
                rcSubItem.right--;

                //rcSubItem.top;
                //rcSubItem.bottom;

                if (sub == 1)
                {
                    //::SetWindowLong(m_edit, GWL_STYLE , 
                    //    WS_CHILD | WS_BORDER | ES_AUTOHSCROLL );

                    //::SetWindowPos(m_edit, NULL, 0, 0, 0, 0,
                    //    SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_DEFERERASE);

                    //m_edit.ModifyStyle(WS_VSCROLL | ES_MULTILINE | ES_WANTRETURN,
                    //    ES_AUTOHSCROLL, SWP_FRAMECHANGED | SWP_DRAWFRAME);
                }
                else if (sub == 2)
                {
                    //m_edit.ModifyStyle(ES_AUTOHSCROLL,
                    //    WS_VSCROLL | ES_MULTILINE | ES_WANTRETURN, SWP_FRAMECHANGED | SWP_DRAWFRAME);
                    //::SetWindowLong(m_edit, GWL_STYLE , 
                    //    WS_CHILD | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_WANTRETURN );
                    
                    //::SetWindowPos(m_edit, NULL, 0, 0, 0, 0,
                    //    SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_DEFERERASE);

                    //rcSubItem.left   += 15;
                    //rcSubItem.bottom += (160);
                }

                m_edit.SetWindowText(szBuffer);

                if ( sub == 2 )
                {
                    int nHeight = m_edit.GetLineCount() * 12 ;
                    if (nHeight < 150)
                        nHeight = 150;
                    else if (nHeight > 500)
                        nHeight = 500;

                    rcSubItem.bottom += nHeight;
                }

                m_edit.MoveWindow(&rcSubItem);
                m_edit.ShowWindow(SW_SHOW);
                m_edit.SetFocus();

                lParam = MAKELPARAM(info.pt.x - rcSubItem.left,
                    info.pt.y - rcSubItem.top);

                m_edit.SendMessage(uMsg, wParam, lParam);
                m_edit.SendMessage(WM_LBUTTONUP, wParam, lParam);
                m_edit.SendMessage(WM_MOUSEMOVE, wParam, lParam);

                m_neditindex = nIndex;
                m_neditsubitem = sub;

                return lResult;
            }
        }
        m_edit.ShowWindow(SW_HIDE);

        return lResult;
    }

    LRESULT OnEditKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {
        if (m_neditindex >= 0)
        {
            SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

            std::vector<SkinStrResDoc::STRRESITEMINFO>& vtList =
                ControlsMgt.m_resDocument.m_resStrDoc.m_vtItemList;

            TCHAR szNewBuffer[1024] = { 0 };

            m_edit.GetWindowText(szNewBuffer, 1024);

            if ( m_neditsubitem == 1 )
            {
                if ( _tcslen(szNewBuffer) <= _tcslen(_T("IDS_")) ||
                    _tcsncmp(szNewBuffer, _T("IDS_"), _tcslen(_T("IDS_")) ) ) // 不合法的项名
                {
                    KSGUI::CString strMsg;

                    strMsg.Format(
                        _T("[%s]不是合法的项名\n必顺以 IDS_ 开头的字符串。"),
                        szNewBuffer);

                    MessageBox(strMsg, _T("错误"));

                    return TRUE;
                }


                if ( FindSameKeyItem(szNewBuffer, m_neditindex) >= 0)
                {
                    KSGUI::CString strMsg;
                    strMsg.Format(_T("重复的项[%s]"), szNewBuffer);
                    MessageBox(strMsg, _T("错误"), MB_OK);
                    return TRUE;
                }

                if (m_neditindex == (GetItemCount() - 1))
                {
                    if (_tcscmp(szNewBuffer, _T("IDS_")))
                    {
                        SkinStrResDoc::STRRESITEMINFO item;

                        item.strIDName = szNewBuffer;

                        CListViewCtrl::GetItemText(m_neditindex, 2,
                            item.strValue.GetBuffer(1025), 1024);
                        
                        item.strValue.ReleaseBuffer();

                        vtList.push_back(item);

                        ATLASSERT(vtList.size() == GetItemCount());

                        SetItemText(InsertItem(0xFFFFF, _T("New...")), 1, _T("IDS_"));
                        
                    }
                }
            }

            CListViewCtrl::SetItemText(m_neditindex, m_neditsubitem,
                szNewBuffer);
            
            ATLASSERT(vtList.size() == GetItemCount() - 1);

            if (m_neditindex < ( GetItemCount() - 1) )
            {
                if (m_neditsubitem == 1)
                {
                    vtList[m_neditindex].strIDName = szNewBuffer;
                }
                else if (m_neditsubitem == 2)
                {
                    vtList[m_neditindex].strValue = szNewBuffer;
                }
            }

            ControlsMgt.m_resDocument.Modify(TRUE);
        }
        
        m_edit.ShowWindow(SW_HIDE);

        return 0;
    }

    int FindSameKeyItem(LPCTSTR pszKeyName, int nExceptIndex)
    {
        TCHAR szNewBuffer[1024] = { 0 };

        for (int i = 0; i < GetItemCount() - 1; i++)
        {
            if (i == nExceptIndex)
                continue;

            CListViewCtrl::GetItemText(i, 1,
                szNewBuffer, 1024);

            if (!_tcscmp(pszKeyName, szNewBuffer))
                return i;
        }

        return -1;
    }


    LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if (wParam == VK_DELETE)
        {
            int nIndex = GetSelectedIndex();
            
            if (nIndex >= 0 && nIndex < GetItemCount() - 1)
            {
                DeleteItem(nIndex);

                SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();
                ControlsMgt.m_resDocument.m_resStrDoc.m_vtItemList.erase(
                    ControlsMgt.m_resDocument.m_resStrDoc.m_vtItemList.begin() + nIndex);
                
                ControlsMgt.m_resDocument.Modify(TRUE);

                CListViewCtrl::SelectItem(nIndex);
            }    
        }

        return DefWindowProc();
    }

    LRESULT OnItemChanged(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
    {
        LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

        int index = CListViewCtrl::GetSelectedIndex();

        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();
        
        ControlsMgt.m_skinResPropertyView.Clear();
        ControlsMgt.m_skinResPropertyView.SetPropertyEditNotify(NULL);

        if (index >= 0 )
        {
            std::vector<SkinStrResDoc::STRRESITEMINFO>& vtList =
                ControlsMgt.m_resDocument.m_resStrDoc.m_vtItemList;

            ControlsMgt.m_skinResPropertyView.AppendProperty(
                _T("Type"), (index < GetItemCount() - 1) ? _T("String") : _T("NEW..."), SkinPropertyView::it_readonly);
            ControlsMgt.m_skinResPropertyView.AppendProperty(
                _T("ID"), (index < GetItemCount() - 1) ? vtList[index].strIDName : _T(""), SkinPropertyView::it_readonly);
            ControlsMgt.m_skinResPropertyView.AppendProperty(
                _T("Caption"), (index < GetItemCount() - 1) ? vtList[index].strValue : _T(""), SkinPropertyView::it_readonly);
        }



        return 0;
    }

};
