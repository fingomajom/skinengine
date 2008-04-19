
#pragma once

class SkinResMenuView : 
    public CDialogImpl<SkinResMenuView>,
    public SkinTreeItemControl
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


public:

    BEGIN_MSG_MAP(SkinResDialogListView)

        //MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)

    END_MSG_MAP()


};