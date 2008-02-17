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

class SkinResDialogListView : 
    public CDialogImpl<SkinResDialogListView>,
    public SkinTreeItemControl
{
public:

    virtual void InitResult(HTREEITEM hTreeItem)
    {

        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        if (m_hWnd == NULL)
        {
            Create(ControlsMgt.m_piSkinFrame->GetResultParentWnd());
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

    BEGIN_MSG_MAP(SkinResDialogListView)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_SIZE      , OnSize)
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

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