/********************************************************************
* CreatedOn: 2008-2-17   12:25
* FileName: SkinResDialogView.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#include "SkinControlsMgt.h"
#include "SkinResWndDefProperty.h"

class SkinResDialogView : 
    public CDialogImpl<SkinResDialogView>,
    public SkinTreeItemControl
{
public:
    
    SkinDialogRes& m_dialogRes;

    HTREEITEM      m_hTreeItem;

    CTreeViewCtrl  m_wndTree;
    CComboBox      m_wndComboBox;


    HTREEITEM      m_hLastSelItem;


    SkinResDialogView(SkinDialogRes& dialogRes)
        : m_dialogRes(dialogRes), m_hTreeItem(NULL)
    {
    }

public:

    virtual void InitResult(HTREEITEM hTreeItem)
    {
        m_hTreeItem = hTreeItem;

        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        if (m_hWnd == NULL)
        {
            Create(ControlsMgt.m_piSkinFrame->GetResultParentWnd());
        }

        m_wndTree.DeleteAllItems();

        
        SkinResWndDefProperty::GetResWndDefProperty(m_dialogRes.m_dlgWndProperty);

        m_wndTree.InsertItem( m_dialogRes.m_dlgWndProperty.GetIdName(),
            TVI_ROOT, TVI_LAST);

        for (size_t i = 0; i < m_dialogRes.m_vtChildWndList.size(); i++)
        {
            HTREEITEM hInsertItem = m_wndTree.InsertItem( 
                m_dialogRes.m_vtChildWndList[i].GetIdName(),
                m_wndTree.GetRootItem(), TVI_LAST);

            m_wndTree.SetItemData(hInsertItem, i);
        }

        std::vector<KSG::CString> vtClassName;

        SkinResWndDefProperty::GetDefClassNameList(vtClassName);

        for (size_t i = 0; i < vtClassName.size(); i++)
        {
            m_wndComboBox.AddString(vtClassName[i]);
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
        ShowWindow(SW_HIDE);
    }

    enum { IDD = IDD_EDITDLG_DIALOG };

    BEGIN_MSG_MAP(SkinResDialogListView)

        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_SIZE      , OnSize)

        COMMAND_HANDLER(IDC_ADD   , BN_CLICKED, OnAdd)
        COMMAND_HANDLER(IDC_DELETE, BN_CLICKED, OnDel)


        NOTIFY_CODE_HANDLER( TVN_SELCHANGED, OnSelChanged)

    END_MSG_MAP()

    LRESULT OnAdd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        TCHAR szBuffer[MAX_PATH] = { 0 };

        m_wndComboBox.GetWindowText(szBuffer, MAX_PATH);

        if (szBuffer[0] >= '0' && szBuffer[0] <= '9') // 不合法的项名
        {
            return TRUE;
        }

        SkinWndPropertyList WndProperty;
        WndProperty.GetIdName() = _T("Wnd");

        SkinResWndDefProperty::GetResWndDefProperty(WndProperty);
        WndProperty.SetProperty(_T("SkinClassName"), szBuffer);

        m_dialogRes.m_vtChildWndList.push_back(WndProperty);


        HTREEITEM hInsertItem = m_wndTree.InsertItem( 
            WndProperty.GetIdName(),
            m_wndTree.GetRootItem(), TVI_LAST);

        m_wndTree.SetItemData(hInsertItem, 
            m_dialogRes.m_vtChildWndList.size() - 1);


        return TRUE;
    }

    LRESULT OnDel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        int nindex = 0;// m_wndTree.GetCurSel();;

        if (nindex >= 0)
        {
            SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

             return TRUE;
        }

        return TRUE;
    }


    LRESULT OnSelChanged(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
    {
        LPNMTREEVIEW pNMTV = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

        LRESULT lResult = DefWindowProc();

        m_hLastSelItem = pNMTV->itemNew.hItem;
        
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();
        
        ControlsMgt.m_skinResPropertyView.Clear();

        if (m_hLastSelItem != NULL)
        {
            std::vector<SkinWndPropertyList::WndPropertyItem>* pPropertyList = NULL;
            
            if (m_hLastSelItem == m_wndTree.GetRootItem())
                pPropertyList = &m_dialogRes.m_dlgWndProperty.m_vtPropertyList;
            else
            {
                int nindex = (int)m_wndTree.GetItemData(m_hLastSelItem);

                ATLASSERT(nindex >= 0 && nindex < (int)m_dialogRes.m_vtChildWndList.size());
                
                pPropertyList = &m_dialogRes.m_vtChildWndList[nindex].m_vtPropertyList;
            }
            
            ATLASSERT(pPropertyList != NULL);

            for (size_t i = 0; i < pPropertyList->size(); i++)
            {
                ControlsMgt.m_skinResPropertyView.AppendProperty( 
                    (*pPropertyList)[i].strProperty, 
                    (*pPropertyList)[i].strValue, 
                    SkinResWndDefProperty::GetResWndPropertyEditType((*pPropertyList)[i].strProperty));
            }

        }

        return lResult;
    }


    LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {

        return DefWindowProc();
    }

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        m_wndTree     = GetDlgItem(IDC_WND_TREE);
        m_wndComboBox = GetDlgItem(IDC_COMBO);

        return TRUE;
    }

};