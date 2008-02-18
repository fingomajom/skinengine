/********************************************************************
* CreatedOn: 2008-2-17   12:25
* FileName: SkinResDialogListView.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#include "SkinControlsMgt.h"
#include "SkinResDialogView.h"

class SkinResDialogListView : 
    public CDialogImpl<SkinResDialogListView>,
    public SkinTreeItemControl
{
public:

    virtual void InitResult(HTREEITEM hTreeItem)
    {
        m_hTreeItem = hTreeItem;

        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        if (m_hWnd == NULL)
        {
            Create(ControlsMgt.m_piSkinFrame->GetResultParentWnd());
        }

        while (m_wndListBox.GetCount() > 0)
        {
            m_wndListBox.DeleteString(0);
        }

        m_wndEdit.SetWindowText(_T(""));

        std::vector<SkinDialogRes>& vtList =
            ControlsMgt.m_resDocument.m_resDialogDoc.m_vtDialogList;

        for (size_t i = 0; i < vtList.size(); i++)
        {
            m_wndListBox.AddString(vtList[i].m_dlgWndProperty.GetIdName());
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

    enum { IDD = IDD_EDITDLGLIST_DIALOG };

    CEdit    m_wndEdit;
    CListBox m_wndListBox;
    CButton  m_wndAddBtn;
    CButton  m_wndDelBtn;
    CButton  m_wndMotifyBtn;

    HTREEITEM m_hTreeItem;

    BEGIN_MSG_MAP(SkinResDialogListView)

        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_SIZE      , OnSize)

        COMMAND_HANDLER(IDC_DIALOG_LIST, LBN_SELCHANGE, OnSelChanged)
        COMMAND_HANDLER(IDC_ADD   , BN_CLICKED, OnAdd)
        COMMAND_HANDLER(IDC_DELETE, BN_CLICKED, OnDel)
        COMMAND_HANDLER(IDC_MODIFY, BN_CLICKED, OnModify)

    END_MSG_MAP()

    LRESULT OnSelChanged(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        int index = m_wndListBox.GetCurSel();

        return TRUE;
    }

    int FindIndex(LPCTSTR pszName)
    {
        TCHAR szBuffer[MAX_PATH] = { 0 };

        for (int i = 0; i < m_wndListBox.GetCount(); i++)
        {
            m_wndListBox.GetText(i, szBuffer);

            if (_tcscmp(pszName, szBuffer) == 0)
                return i;
        }

        return -1;
    }

    LRESULT OnAdd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        TCHAR szBuffer[MAX_PATH] = { 0 };

        m_wndEdit.GetWindowText(szBuffer, MAX_PATH);

        if (szBuffer[0] >= '0' && szBuffer[0] <= '9') // 不合法的项名
        {
            return TRUE;
        }
        
        int nindex = FindIndex(szBuffer);

        if (nindex >= 0)
        {
            m_wndListBox.SetCurSel(nindex);

            return TRUE;
        }

        m_wndListBox.AddString(szBuffer);

        {
            SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

            std::vector<SkinDialogRes>& vtList =
                ControlsMgt.m_resDocument.m_resDialogDoc.m_vtDialogList;

            ATLASSERT((m_wndListBox.GetCount() - 1) == vtList.size());

            SkinDialogRes dlgRes;

            dlgRes.m_dlgWndProperty.GetIdName() = szBuffer;

            vtList.push_back(dlgRes);

            SkinResDialogView* pResDialogView = 
                new SkinResDialogView( vtList[ m_wndListBox.GetCount() - 1] );

            m_wndListBox.SetItemDataPtr(m_wndListBox.GetCount() - 1, pResDialogView);

            HTREEITEM hTreeItem = ControlsMgt.m_skinTreeControlView.InsertControlItem(
                m_hTreeItem,
                szBuffer, 0, pResDialogView, 0);

            TVITEM tvItem;

            tvItem.mask  = TVIF_CHILDREN ;
            tvItem.hItem = m_hTreeItem;

            ControlsMgt.m_skinTreeControlView.GetItem(&tvItem);
            tvItem.cChildren = TRUE;
            ControlsMgt.m_skinTreeControlView.SetItem(&tvItem);


            ControlsMgt.m_skinTreeControlView.Expand(m_hTreeItem);
            ControlsMgt.m_skinTreeControlView.SelectItem(m_hTreeItem);

        }

        return TRUE;
    }

    LRESULT OnDel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        int nindex = m_wndListBox.GetCurSel();;

        if (nindex >= 0)
        {
            SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

            SkinResDialogView* pResDialogView = 
                (SkinResDialogView*)m_wndListBox.GetItemDataPtr(nindex);

            ControlsMgt.m_skinTreeControlView.DeleteItem(
                pResDialogView->m_hTreeItem);

            pResDialogView->DestroyWindow();

            delete pResDialogView;


            m_wndListBox.DeleteString(nindex);

            m_wndListBox.SetCurSel(nindex);

            {
                std::vector<SkinDialogRes>& vtList =
                    ControlsMgt.m_resDocument.m_resDialogDoc.m_vtDialogList;

                ATLASSERT(m_wndListBox.GetCount() == vtList.size() - 1);

                vtList.erase(vtList.begin() + nindex);
            }


            return TRUE;
        }

        return TRUE;
    }

    LRESULT OnModify(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        TCHAR szBuffer[MAX_PATH] = { 0 };

        m_wndEdit.GetWindowText(szBuffer, MAX_PATH);

        int nindex = m_wndListBox.GetCurSel();

        if (nindex >= 0)
        {
            int nfindindex = FindIndex(szBuffer);
            if (nfindindex >= 0)
                return TRUE;

            SkinResDialogView* pResDialogView = 
                (SkinResDialogView*)m_wndListBox.GetItemDataPtr(nindex);

            m_wndListBox.DeleteString(nindex);
            m_wndListBox.InsertString(nindex, szBuffer);

            m_wndListBox.SetItemDataPtr(nindex, pResDialogView);


            m_wndListBox.SetCurSel(nindex);

            {
                SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

                std::vector<SkinDialogRes>& vtList =
                    ControlsMgt.m_resDocument.m_resDialogDoc.m_vtDialogList;

                ATLASSERT(m_wndListBox.GetCount() == vtList.size());

                vtList[nindex].m_dlgWndProperty.GetIdName() = szBuffer;

                ControlsMgt.m_skinTreeControlView.SetItemText(
                    pResDialogView->m_hTreeItem, szBuffer);
            }


            return TRUE;
        }

        return TRUE;
    }


    LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        if (m_wndListBox.m_hWnd != NULL)
        {
            RECT rcClient  = { 0 };
            RECT rcListBox = { 0 };

            GetClientRect(&rcClient);

            m_wndListBox.GetClientRect(&rcListBox);
            m_wndListBox.ClientToScreen(&rcListBox);
            ScreenToClient(&rcListBox);

            rcListBox.bottom = rcClient.bottom - rcListBox.top ;

            m_wndListBox.MoveWindow(&rcListBox);
        }


        return DefWindowProc();
    }

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        m_wndEdit    = GetDlgItem(IDC_EDIT);
        m_wndListBox = GetDlgItem(IDC_DIALOG_LIST);
        m_wndAddBtn  = GetDlgItem(IDC_ADD);
        m_wndDelBtn  = GetDlgItem(IDC_DELETE);
        m_wndMotifyBtn = GetDlgItem(IDC_MODIFY);

        return TRUE;
    }




};