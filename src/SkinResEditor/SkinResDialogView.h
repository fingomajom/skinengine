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

    int m_ndialogindex;
    

    HTREEITEM      m_hTreeItem;

    CTreeViewCtrl  m_wndTree;
    CComboBox      m_wndComboBox;


    HTREEITEM      m_hLastSelItem;

    int m_nNewIdIndex;

    SkinDialogRes& GetResDialog()
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();


        return ControlsMgt.m_resDocument.m_resDialogDoc.m_vtDialogList[m_ndialogindex];
    }


    SkinResDialogView(int ndialogindex)
        : m_ndialogindex(ndialogindex), m_hTreeItem(NULL)
    {
        m_nNewIdIndex = 0;
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

        SkinDialogRes& dialogRes = GetResDialog();

        
        SkinResWndDefProperty::GetResClassWndDefProperty(
            KSG::CString(), dialogRes.m_dlgWndProperty);

        dialogRes.m_dlgWndProperty.SetProperty(_T("Width") , _T("300"));
        dialogRes.m_dlgWndProperty.SetProperty(_T("Height"), _T("400"));


        m_wndTree.InsertItem( dialogRes.m_dlgWndProperty.GetIdName(),
            TVI_ROOT, TVI_LAST);

        for (size_t i = 0; i < dialogRes.m_vtChildWndList.size(); i++)
        {
            HTREEITEM hInsertItem = m_wndTree.InsertItem( 
                dialogRes.m_vtChildWndList[i].GetIdName(),
                m_wndTree.GetRootItem(), TVI_LAST);

            m_wndTree.SetItemData(hInsertItem, i);
        }

        std::vector<KSG::CString> vtClassName;

        SkinResWndDefProperty::GetDefClassNameList(vtClassName);



        for (size_t i = 0; i < vtClassName.size(); i++)
        {
            m_wndComboBox.AddString(vtClassName[i]);
        }

        m_wndTree.Expand(m_wndTree.GetRootItem());
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

        if ( ( szBuffer[0] >= '0' && szBuffer[0] <= '9' ) || _tcslen(szBuffer) <= 0) // 不合法的项名
        {
            return TRUE;
        }

        SkinDialogRes& dialogRes = GetResDialog();

        SkinWndPropertyList WndProperty;

        SkinResWndDefProperty::GetResClassWndDefProperty(
            szBuffer, WndProperty);

        WndProperty.GetIdName().Format(_T("IDC_%d"), m_nNewIdIndex++);

        WndProperty.SetProperty(_T("IdName"), WndProperty.GetIdName());
        WndProperty.SetProperty(_T("SkinClassName"), szBuffer);

        dialogRes.m_vtChildWndList.push_back(WndProperty);


        HTREEITEM hInsertItem = m_wndTree.InsertItem( 
            WndProperty.GetIdName(),
            m_wndTree.GetRootItem(), TVI_LAST);

        m_wndTree.SetItemData(hInsertItem, 
            dialogRes.m_vtChildWndList.size() - 1);

        TVITEM tvItem;

        tvItem.mask  = TVIF_CHILDREN ;
        tvItem.hItem = m_wndTree.GetRootItem();

        m_wndTree.GetItem(&tvItem);
        tvItem.cChildren = TRUE;
        m_wndTree.SetItem(&tvItem);


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
        SkinDialogRes& dialogRes = GetResDialog();


        m_hLastSelItem = pNMTV->itemNew.hItem;
        
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();
        
        ControlsMgt.m_skinResPropertyView.Clear();

        if (m_hLastSelItem != NULL)
        {
            std::vector<SkinWndPropertyList::WndPropertyItem>* pPropertyList = NULL;
            
            if (m_hLastSelItem == m_wndTree.GetRootItem())
                pPropertyList = &dialogRes.m_dlgWndProperty.m_vtPropertyList;
            else
            {
                int nindex = (int)m_wndTree.GetItemData(m_hLastSelItem);

                ATLASSERT(nindex >= 0 && nindex < (int)dialogRes.m_vtChildWndList.size());
                
                pPropertyList = &dialogRes.m_vtChildWndList[nindex].m_vtPropertyList;
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