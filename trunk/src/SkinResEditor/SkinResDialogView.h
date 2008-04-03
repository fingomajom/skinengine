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
#include "SkinDialgPreviewWindow.h"


class SkinResDialogView : 
    public CDialogImpl<SkinResDialogView>,
    public SkinTreeItemControl,
    public SkinPropertyView::PropertyEditNotify
{
public:

    enum
    {
        em_itemid_begin = 1000
    };

    int m_ndialogindex;
    int m_nNewItemId;
    

    HTREEITEM      m_hTreeItem;

    CTreeViewCtrl  m_wndTree;
    CComboBox      m_wndComboBox;
    CButton        m_wndAddBtn;
    CButton        m_wndDelBtn;

    SkinDialgPreviewWindow m_wndPreView;

    HTREEITEM      m_hLastSelItem;


    SkinDialogRes& GetResDialog()
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();


        return ControlsMgt.m_resDocument.m_resDialogDoc.m_vtDialogList[m_ndialogindex];
    }


    SkinResDialogView(int ndialogindex)
        : m_ndialogindex(ndialogindex), m_hTreeItem(NULL)
    {
        m_nNewItemId = em_itemid_begin;
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

        if (dialogRes.m_dlgWndProperty.m_vtPropertyList.size() == 0)
        {
            SkinResWndDefProperty::GetResClassWndDefProperty(
                KSG::CString(), dialogRes.m_dlgWndProperty);

            dialogRes.m_dlgWndProperty.SetProperty(_T("IdName"), dialogRes.m_dlgWndProperty.GetIdName());
            dialogRes.m_dlgWndProperty.SetProperty(_T("Width") , _T("300"));
            dialogRes.m_dlgWndProperty.SetProperty(_T("Height"), _T("200"));

        }

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

        CreatePreviewWindow();
    }

    virtual void ShowResult(HTREEITEM hTreeItem, LPARAM lParam)
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        ShowWindow(SW_SHOW);

        m_wndTree.SetItemText(m_wndTree.GetRootItem(), GetResDialog().m_dlgWndProperty.GetIdName());

        ControlsMgt.m_piSkinFrame->SetActiveResultWindow(m_hWnd);

    }

    virtual void HideResult(HTREEITEM hTreeItem, LPARAM lParam)
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        ControlsMgt.m_skinResPropertyView.Clear();

        ShowWindow(SW_HIDE);
    }

    enum { IDD = IDD_EDITDLG_DIALOG };

    
    void CreatePreviewWindow()
    {
        SkinXmlElement xmlElement;
        m_wndPreView.ReCreatePreviewWindow(GetDlgItem(IDC_REVIEW_STATIC), GetResDialog());

        //RECT rcClient   = { 0 };
        //RECT rcTree     = { 0 };
        //RECT rcStatic   = { 0 };

        //GetClientRect(&rcClient);

        //m_wndTree.GetClientRect(&rcTree);
        //m_wndPreView.GetWindowRect(&rcStatic);
        //    
        //rcStatic.right = rcStatic.right - rcStatic.left + rcTree.right + 2;
        //rcStatic.left  = rcTree.right + 2;

        //rcStatic.bottom = rcStatic.bottom - rcStatic.top;
        //rcStatic.top = 0;

        //m_wndPreView.MoveWindow(&rcStatic);

        //GetDlgItem(IDC_REVIEW_STATIC).ShowWindow(SW_HIDE);

    }

    BEGIN_MSG_MAP(SkinResDialogListView)

        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_SIZE      , OnSize)

        COMMAND_HANDLER(IDC_ADD   , BN_CLICKED, OnAdd)
        COMMAND_HANDLER(IDC_DELETE, BN_CLICKED, OnDel)


        NOTIFY_CODE_HANDLER( TVN_SELCHANGED, OnSelChanged)

    END_MSG_MAP()

    int GetNextItemId()
    {
        int nResult = 0;

        SkinDialogRes& dialogRes = GetResDialog();
        
        std::vector<SkinWndPropertyList>& vtChildWndList = dialogRes.m_vtChildWndList;

        KSG::CString strItemIdNew;
        KSG::CString strItemId;

        for ( ; true; m_nNewItemId++ )
        {
            strItemIdNew.Format(_T("%d"), m_nNewItemId);
                        
            BOOL bUsed = FALSE;

            for ( size_t i = 0; i < vtChildWndList.size(); i++ )
            {
                if (!vtChildWndList[i].GetProperty(_T("ItemId"), strItemId) )
                    continue;
                
                if (!strItemIdNew.CompareNoCase(strItemId))
                {
                    bUsed = TRUE;

                    break;
                }
            }

            if (!bUsed)
            {
                nResult = m_nNewItemId;

                break;
            }
        }

        return nResult;
    }

    int GetItemIndex(HTREEITEM hTreeItem)
    {
        int nResult = -1;

        while (hTreeItem != NULL)
        {
            hTreeItem = m_wndTree.GetPrevSiblingItem(hTreeItem);

            nResult++;
        }

        return nResult;
    }

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

        int nNewItemId = GetNextItemId();

        WndProperty.GetIdName().Format(_T("IDC_%d"), nNewItemId);

        WndProperty.SetProperty(_T("IdName"), WndProperty.GetIdName());
        WndProperty.SetProperty(_T("SkinClassName"), szBuffer);
        WndProperty.SetProperty(_T("ItemId"), nNewItemId);


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

        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();
        ControlsMgt.m_resDocument.Modify(TRUE);

        CreatePreviewWindow();

        return TRUE;
    }

    LRESULT OnDel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        HTREEITEM hTreeItem = m_wndTree.GetSelectedItem();

        int nindex = GetItemIndex(hTreeItem);

        if (nindex >= 0)
        {
            SkinDialogRes& dialogRes = GetResDialog();

            ATLASSERT((size_t)nindex < dialogRes.m_vtChildWndList.size());

            dialogRes.m_vtChildWndList.erase(
                dialogRes.m_vtChildWndList.begin() + nindex);

            m_wndTree.DeleteItem(hTreeItem);

            SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();
            ControlsMgt.m_resDocument.Modify(TRUE);

            CreatePreviewWindow();

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
        ControlsMgt.m_skinResPropertyView.SetPropertyEditNotify(this);

        if (m_hLastSelItem != NULL)
        {
            std::vector<SkinWndPropertyList::WndPropertyItem>* pPropertyList = NULL;
            
            if (m_hLastSelItem == m_wndTree.GetRootItem())
                pPropertyList = &dialogRes.m_dlgWndProperty.m_vtPropertyList;
            else
            {
                int nindex = (int)GetItemIndex(m_hLastSelItem);

                ATLASSERT(nindex >= 0 && nindex < (int)dialogRes.m_vtChildWndList.size());
                
                pPropertyList = &dialogRes.m_vtChildWndList[nindex].m_vtPropertyList;

                KSG::CString strClassName;

                dialogRes.m_vtChildWndList[nindex].GetProperty(_T("SkinClassName") , strClassName);

                m_wndComboBox.SetWindowText(strClassName);
            }
            
            ATLASSERT(pPropertyList != NULL);

            for (size_t i = 0; i < pPropertyList->size(); i++)
            {
                int ntype = SkinResWndDefProperty::GetResWndPropertyEditType((*pPropertyList)[i].strProperty);

                if (m_hLastSelItem == m_wndTree.GetRootItem() &&
                    !_tcscmp((*pPropertyList)[i].strProperty, _T("IdName")))
                {
                    ntype = SkinPropertyView::it_readonly;
                }

                ControlsMgt.m_skinResPropertyView.AppendProperty( 
                    (*pPropertyList)[i].strProperty, 
                    (*pPropertyList)[i].strValue, 
                    ntype);
            }

        }

        return lResult;
    }


    LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        if (m_wndTree.m_hWnd != NULL)
        {
            RECT rcClient   = { 0 };
            RECT rcTree     = { 0 };
            RECT rcComboBox = { 0 };
            RECT rcAddBtn   = { 0 };
            RECT rcDelBtn   = { 0 };
            RECT rcStatic   = { 0 };

            GetClientRect(&rcClient);

            m_wndTree.GetClientRect(&rcTree);

            rcTree.bottom = rcClient.bottom - 50;;
            
            rcComboBox = rcTree;
            rcComboBox.top = rcComboBox.bottom + 2;
            rcComboBox.bottom = rcComboBox.top + 22;

            rcAddBtn = rcComboBox;
            rcAddBtn.top = rcAddBtn.bottom + 2;
            rcAddBtn.bottom = rcAddBtn.top + 22;

            rcAddBtn.left += 2;
            rcAddBtn.right = (rcAddBtn.left + rcAddBtn.right) / 2 + 1;
            
            rcDelBtn = rcAddBtn;
            rcDelBtn.left = rcAddBtn.right + 2;
            rcDelBtn.right = rcTree.right - 2;

            rcStatic = rcClient;
            rcStatic.left = rcTree.right + 2;

            m_wndTree.MoveWindow(&rcTree);
            m_wndComboBox.MoveWindow(&rcComboBox);
            m_wndAddBtn.MoveWindow(&rcAddBtn);
            m_wndDelBtn.MoveWindow(&rcDelBtn);

            GetDlgItem(IDC_REVIEW_STATIC).MoveWindow(&rcStatic);
        }

        return DefWindowProc();
    }

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        m_wndTree     = GetDlgItem(IDC_WND_TREE);
        m_wndComboBox = GetDlgItem(IDC_COMBO);

        m_wndAddBtn  = GetDlgItem(IDC_ADD);
        m_wndDelBtn  = GetDlgItem(IDC_DELETE);

        return TRUE;
    }

    void OnValueChange (
        LPCTSTR pszPropertyName,
        LPCTSTR pszOldValue,
        LPCTSTR pszNewValue)
    {
        SkinDialogRes& dialogRes = GetResDialog();

        HTREEITEM hTreeItem = m_wndTree.GetSelectedItem();


        if (hTreeItem == m_wndTree.GetRootItem())
        {
            UpdateWndProperty(dialogRes.m_dlgWndProperty,
                pszPropertyName,
                pszOldValue, pszNewValue);
        }
        else
        {
            int nindex = GetItemIndex(hTreeItem);
            
            UpdateWndProperty(dialogRes.m_vtChildWndList[nindex],
                pszPropertyName,
                pszOldValue, pszNewValue);
        }
        
    }

    void UpdateWndProperty( SkinWndPropertyList& WndPropertyList, 
        LPCTSTR pszPropertyName,
        LPCTSTR pszOldValue,
        LPCTSTR pszNewValue)
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();
        SkinDialogRes&   dialogRes   = GetResDialog();


        if ( !_tcscmp(pszPropertyName, _T("IdName") ) )
        {
            if (pszNewValue[0] >= '0' && pszNewValue[0] <= '9') // 不合法的项名
            {
                ControlsMgt.m_skinResPropertyView.SetProperty(_T("IdName"),
                    pszOldValue);

                return;
            }

            if (&WndPropertyList != &dialogRes.m_dlgWndProperty)
            {

                for ( size_t i = 0; i < dialogRes.m_vtChildWndList.size(); i++ )
                {
                    if ( !dialogRes.m_vtChildWndList[i].GetIdName().CollateNoCase(pszNewValue) ) // 不合法的项名
                    {
                        ControlsMgt.m_skinResPropertyView.SetProperty(_T("IdName"),
                            pszOldValue);

                        return;
                    }
                }

            }

            m_wndTree.SetItemText( m_wndTree.GetSelectedItem(), pszNewValue);

            WndPropertyList.GetIdName() = pszNewValue;
            WndPropertyList.SetProperty( pszPropertyName, pszNewValue );
        }

        WndPropertyList.SetProperty( pszPropertyName, pszNewValue );

        CreatePreviewWindow();

    }


    void OnButtonClieck(LPCTSTR pszPropertyName)
    {

    }
};