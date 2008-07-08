
#pragma once

#include "SkinItemIdMgt.h"

class SkinResMenuView : 
    public CDialogImpl<SkinResMenuView>,
    public SkinTreeItemControl,
    public SkinPropertyView::PropertyEditNotify
{
public:

    enum { IDD = IDD_EDITMENU_DIALOG };

    SkinResMenuView()
    {
    }

    ~SkinResMenuView()
    {
    }

    virtual void InitResult(HTREEITEM hTreeItem)
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        if (m_hWnd == NULL)
        {
            Create(ControlsMgt.m_piSkinFrame->GetResultParentWnd());
        }

        m_wndMenuTree.DeleteAllItems();
        m_wndMenuList.DeleteAllItems();

        std::vector<SkinMenuItem>& vtMenuList = ControlsMgt.m_resDocument.m_resMenuDoc.m_vtMenuList;

        for (size_t idx = 0; idx < vtMenuList.size(); idx++)
        {
            m_wndMenuList.InsertItem(idx, vtMenuList[idx].strIdName, 4);
        }

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
        ControlsMgt.m_skinResPropertyView.SetPropertyEditNotify(NULL);

        ShowWindow(SW_HIDE);
    }


public:

    CListViewCtrl m_wndMenuList;
    CTreeViewCtrl m_wndMenuTree;
    CImageList    m_imagelist;

    BEGIN_MSG_MAP(SkinResDialogListView)

        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)

        COMMAND_HANDLER(IDC_ADD_MENU   , BN_CLICKED, OnAddMenu)
        COMMAND_HANDLER(IDC_DEL_MENU   , BN_CLICKED, OnDelMenu)

        COMMAND_HANDLER(IDC_ADD_MENUITEM   , BN_CLICKED, OnAddMenuItem)
        COMMAND_HANDLER(IDC_DEL_MENUITEM   , BN_CLICKED, OnDelMenuItem)

        NOTIFY_HANDLER(IDC_MENU_LIST, LVN_ENDLABELEDIT, OnLvnEndlabeleditMenuList)
        NOTIFY_HANDLER(IDC_MENU_LIST, LVN_ITEMCHANGED, OnLvnItemchangedMenuList)
        NOTIFY_HANDLER(IDC_MENU_TREE, TVN_SELCHANGED, OnTvnSelchangedMenuTree)
    END_MSG_MAP()

    BOOL IsExistMenuName( LPCTSTR pszName )
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        std::vector<SkinMenuItem>& vtMenuList = 
            ControlsMgt.m_resDocument.m_resMenuDoc.m_vtMenuList;

        for (size_t idx = 0; idx < vtMenuList.size(); idx++)
        {
            if (!vtMenuList[idx].strIdName.CompareNoCase(pszName))
                return TRUE;
        }

        return FALSE;
    }

    LRESULT OnAddMenu(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        std::vector<SkinMenuItem>& vtMenuList = 
            ControlsMgt.m_resDocument.m_resMenuDoc.m_vtMenuList;

        SkinMenuItem newMenuItem;

        newMenuItem.strIsPopupMenu = _T("False");

        static int newMenuId = 1;

        while (true)
        {
            newMenuItem.strIdName.Format(_T("IDM_NEWMENU%d"), newMenuId++);

            if (!IsExistMenuName(newMenuItem.strIdName))
                break;

        }

        m_wndMenuList.InsertItem(0xFFFFF, newMenuItem.strIdName, 4);
        vtMenuList.push_back(newMenuItem);
        
        ControlsMgt.m_resDocument.Modify(TRUE);

        return 1L;
    }

    LRESULT OnDelMenu(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        int nIdx = m_wndMenuList.GetSelectedIndex();
        if (nIdx < 0)
            return 1L;

        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        std::vector<SkinMenuItem>& vtMenuList = 
            ControlsMgt.m_resDocument.m_resMenuDoc.m_vtMenuList;

        m_wndMenuList.DeleteItem( nIdx );
        vtMenuList.erase(vtMenuList.begin() + nIdx);

        m_wndMenuList.SelectItem( nIdx == m_wndMenuList.GetItemCount() ? nIdx - 1 : nIdx);

        ControlsMgt.m_resDocument.Modify(TRUE);

        return 1L;
    }

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        m_wndMenuList = GetDlgItem(IDC_MENU_LIST);
        m_wndMenuTree = GetDlgItem(IDC_MENU_TREE);

        m_wndMenuList.InsertColumn(0, _T(""), 0, 150);

        CBitmap bmp;

        bmp.LoadBitmap(IDB_RESTYPE_BITMAP);

        m_imagelist.Create(16, 16, ILC_COLOR24 | ILC_MASK, 3, 1);
        m_imagelist.Add(bmp, RGB(255, 0, 255));

        m_wndMenuList.SetImageList(m_imagelist, LVSIL_SMALL);
        m_wndMenuTree.SetImageList(m_imagelist);

        m_wndMenuList.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);

        return TRUE;
    }


    LRESULT SkinResMenuView::OnLvnEndlabeleditMenuList(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
    {
        NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

        KSGUI::CString strNewIdName = pDispInfo->item.pszText;
        if (strNewIdName.GetLength() <= 0)
            return 0L;

        int idx = pDispInfo->item.iItem;

        if (idx < 0)
            return 0L;


        if ( _tcslen(strNewIdName) <= _tcslen(_T("IDM_")) ||
            _tcsncmp(strNewIdName, _T("IDM_"), _tcslen(_T("IDM_")) ) ) // 不合法的项名
        {
            KSGUI::CString strMsg;

            strMsg.Format(
                _T("[%s]不是合法的项名\n必顺以 IDM_ 开头的字符串。"),
                strNewIdName);

            MessageBox(strMsg, _T("错误"));

            return 0L;
        }

        ATLASSERT( idx < m_wndMenuList.GetItemCount() );

        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        std::vector<SkinMenuItem>& vtMenuList = 
            ControlsMgt.m_resDocument.m_resMenuDoc.m_vtMenuList;

        m_wndMenuList.SetItemText(idx, 0, strNewIdName);
        vtMenuList[idx].strIdName = strNewIdName;

        m_wndMenuTree.SetItemText(m_wndMenuTree.GetRootItem(),
            strNewIdName);

        ControlsMgt.m_resDocument.Modify(TRUE);

        return 1L;
    }

    void ShowMenu( int idx )
    {
        m_wndMenuTree.DeleteAllItems();

        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        std::vector<SkinMenuItem>& vtMenuList = 
            ControlsMgt.m_resDocument.m_resMenuDoc.m_vtMenuList;

        if (idx >= 0 && idx < (int)vtMenuList.size())
        {
            HTREEITEM hItemInsert = m_wndMenuTree.InsertItem( vtMenuList[idx].strIdName,
                4, 4, TVI_ROOT, TVI_LAST);

            ShowMenu(hItemInsert, vtMenuList[idx]);

            m_wndMenuTree.Expand(hItemInsert);
        }

        m_wndMenuTree.SelectItem(m_wndMenuTree.GetRootItem());
    }

    void ShowMenu( HTREEITEM hTreeItemParent, SkinMenuItem& MenuItemInfo )
    {
        for (size_t idx = 0; idx < MenuItemInfo.m_vtChildPopupMenu.size(); idx++)
        {
            HTREEITEM hItemInsert = m_wndMenuTree.InsertItem( MenuItemInfo.m_vtChildPopupMenu[idx].strItemText,
                4, 4, hTreeItemParent, TVI_LAST);

            if (MenuItemInfo.m_vtChildPopupMenu[idx].m_vtChildPopupMenu.size() > 0)
                ShowMenu(hItemInsert, MenuItemInfo.m_vtChildPopupMenu[idx]);
        }
    }


    LRESULT OnLvnItemchangedMenuList(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
    {
        LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
        
        int idx = m_wndMenuList.GetSelectedIndex();

        if (idx < 0)
            m_wndMenuTree.DeleteAllItems();
        else
            ShowMenu(idx);


        return 0;
    }

    BOOL GetMenuItem(HTREEITEM hTreeItem, 
        SkinMenuItem*& pParentItem, SkinMenuItem*& pMenuItemInfo, int* pidx = NULL )
    {
        std::vector<int> vtpos;

        HTREEITEM hTreeItemParent = hTreeItem; 

        while (hTreeItemParent != NULL && hTreeItemParent != m_wndMenuTree.GetRootItem())
        {
            hTreeItemParent = m_wndMenuTree.GetParentItem(hTreeItem);
            
            int npos = -1;

            while ( hTreeItem != NULL )
            {
                hTreeItem = m_wndMenuTree.GetPrevSiblingItem(hTreeItem);

                npos++;
            }
            
            vtpos.insert(vtpos.begin(), npos);

            hTreeItem = hTreeItemParent;
        }

        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        std::vector<SkinMenuItem>& vtMenuList = 
            ControlsMgt.m_resDocument.m_resMenuDoc.m_vtMenuList;

        int nIdx = m_wndMenuList.GetSelectedIndex();
        if (nIdx < 0)
            return FALSE;

        pParentItem = &vtMenuList[nIdx];
        pMenuItemInfo = pParentItem;

        for (size_t idx = 0; idx < vtpos.size(); idx++)
        {
            pParentItem = pMenuItemInfo;
            pMenuItemInfo = &pMenuItemInfo->m_vtChildPopupMenu[vtpos[idx]];

            if (pidx != NULL)
                *pidx = vtpos[idx];
        }

        return TRUE;
    }

    LRESULT OnTvnSelchangedMenuTree(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
    {
        LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        HTREEITEM hSelItem = m_wndMenuTree.GetSelectedItem();

        ControlsMgt.m_skinResPropertyView.Clear();
        ControlsMgt.m_skinResPropertyView.SetPropertyEditNotify(NULL);

        if (hSelItem == NULL || hSelItem == m_wndMenuTree.GetRootItem())
        {
        }
        else
        {

            SkinMenuItem* pParentItem   = NULL;
            SkinMenuItem* pMenuItemInfo = NULL;

            GetMenuItem(hSelItem, pParentItem, pMenuItemInfo);

            ATLASSERT(pParentItem != NULL);
            ATLASSERT(pMenuItemInfo != NULL);

            if (pMenuItemInfo != NULL)
            {

                ControlsMgt.m_skinResPropertyView.SetPropertyEditNotify(this);

                ControlsMgt.m_skinResPropertyView.AppendProperty(
                    _T("Caption"), pMenuItemInfo->strItemText );
                ControlsMgt.m_skinResPropertyView.AppendProperty(
                    _T("IdName"), pMenuItemInfo->strIdName );
                ControlsMgt.m_skinResPropertyView.AppendProperty(
                    _T("ItemId"), pMenuItemInfo->strItemId );
            }

        }


        return 0;
    }


    int GetNextMenuIdName()
    {
        static int nnextid = 5000;

        return nnextid++;
    }

    LRESULT OnAddMenuItem(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        HTREEITEM hSelItem = m_wndMenuTree.GetSelectedItem();
        if (hSelItem == NULL)
            return 0L;

        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        SkinMenuItem* pParentItem   = NULL;
        SkinMenuItem* pMenuItemInfo = NULL;

        GetMenuItem(hSelItem, pParentItem, pMenuItemInfo);

        ATLASSERT(pParentItem != NULL);
        ATLASSERT(pMenuItemInfo != NULL);

        if (pMenuItemInfo != NULL)
        {
            SkinMenuItem newMenuItem;

            newMenuItem.strItemText = _T("NewMenuItem");
            newMenuItem.strItemId.Format(_T("%d"), GetNextMenuIdName());
            newMenuItem.strIdName.Format(_T("IDC_MENUID_%s"), newMenuItem.strItemId);

            SkinItemIdMgt::instance().UsedItemId( newMenuItem.strIdName, newMenuItem.strItemId );

            pMenuItemInfo->m_vtChildPopupMenu.push_back(newMenuItem);

            m_wndMenuTree.InsertItem( newMenuItem.strItemText,
                4, 4, hSelItem, TVI_LAST);

            m_wndMenuTree.Expand(hSelItem);

            ControlsMgt.m_resDocument.Modify(TRUE);

        }

        return 0;
    }

    void OnDelUsedItemId( SkinMenuItem& MenuItemInfo )
    {
        SkinItemIdMgt::instance().DelItemId( MenuItemInfo.strIdName );

        for (size_t idx = 0; idx < MenuItemInfo.m_vtChildPopupMenu.size(); idx++)
        {
            OnDelUsedItemId( MenuItemInfo.m_vtChildPopupMenu[idx] );
        }
    }

    LRESULT OnDelMenuItem(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        HTREEITEM hSelItem = m_wndMenuTree.GetSelectedItem();
        if (hSelItem == NULL || hSelItem == m_wndMenuTree.GetRootItem())
            return 0L;

        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        SkinMenuItem* pParentItem   = NULL;
        SkinMenuItem* pMenuItemInfo = NULL;

        int idx = 0;

        GetMenuItem(hSelItem, pParentItem, pMenuItemInfo, &idx);

        ATLASSERT(pParentItem != NULL);
        ATLASSERT(pMenuItemInfo != NULL);

        if (pMenuItemInfo != NULL)
        {
            OnDelUsedItemId(*pMenuItemInfo);
            
            pParentItem->m_vtChildPopupMenu.erase(
                pParentItem->m_vtChildPopupMenu.begin() + idx);

            m_wndMenuTree.DeleteItem(hSelItem);

            ControlsMgt.m_resDocument.Modify(TRUE);

        }

        return 0;
    }

    virtual void OnValueChange (
        LPCTSTR pszPropertyName,
        LPCTSTR pszOldValue,
        LPCTSTR pszNewValue)
    {
        HTREEITEM hSelItem = m_wndMenuTree.GetSelectedItem();
        if (hSelItem == NULL || hSelItem == m_wndMenuTree.GetRootItem())
            return;

        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        SkinMenuItem* pParentItem   = NULL;
        SkinMenuItem* pMenuItemInfo = NULL;

        GetMenuItem(hSelItem, pParentItem, pMenuItemInfo);

        ATLASSERT(pParentItem != NULL);
        ATLASSERT(pMenuItemInfo != NULL);

        if (!_tcscmp(pszPropertyName, _T("Caption")))
        {
            m_wndMenuTree.SetItemText(hSelItem, pszNewValue);
            pMenuItemInfo->strItemText = pszNewValue;
        }
        else if (!_tcscmp(pszPropertyName, _T("IdName")))
        {
            if ( _tcslen(pszNewValue) <= _tcslen(_T("IDC_")) ||
                _tcsncmp(pszNewValue, _T("IDC_"), _tcslen(_T("IDC_")) ) ) // 不合法的项名
            {

                ControlsMgt.m_skinResPropertyView.SetProperty(_T("IdName"),
                    pszOldValue);

                KSGUI::CString strMsg;

                strMsg.Format(
                    _T("[%s]不是合法的项名\n必顺以 IDC_ 开头的字符串。"),
                    pszNewValue);

                MessageBox(strMsg, _T("错误"));

                return ;
            }

            pMenuItemInfo->strIdName = pszNewValue;

            SkinItemIdMgt::instance().UsedItemId( pMenuItemInfo->strIdName, pMenuItemInfo->strItemId );
            SkinItemIdMgt::instance().DelItemId( pszOldValue );

            ControlsMgt.m_skinResPropertyView.SetProperty(_T("ItemId"),
                pMenuItemInfo->strItemId);

        }
        else if (!_tcscmp(pszPropertyName, _T("ItemId")))
        {
            pMenuItemInfo->strItemId = pszNewValue;
            SkinItemIdMgt::instance().ChangeItemId( pMenuItemInfo->strIdName, pMenuItemInfo->strItemId );
        }


    }

    virtual void OnButtonClieck(LPCTSTR pszPropertyName) 
    {

    }

};