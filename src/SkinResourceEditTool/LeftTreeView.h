
#pragma once

#include "ResDocument.h"

enum{
    em_rvt_none = 0,
    em_rvt_string_table
};

class CLeftTreeView : public CWindowImpl<CLeftTreeView, CTreeViewCtrl>
{
public:
    DECLARE_WND_SUPERCLASS(NULL, CTreeViewCtrl::GetWndClassName())

    BOOL PreTranslateMessage(MSG* pMsg)
    {
        pMsg;
        return FALSE;
    }

    BEGIN_MSG_MAP(CLeftTreeView)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        //REFLECTED_NOTIFY_CODE_HANDLER(TVN_SELCHANGED, OnSelChanged)
    END_MSG_MAP()

    // Handler prototypes (uncomment arguments if needed):
    //	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    //	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    //	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        LRESULT lResult = DefWindowProc();

        CResDocument& document = CResDocument::Instance();
        
        HTREEITEM hItem = CTreeViewCtrl::InsertItem(
            _tcslen(document.GetFileName()) > 0 ? document.GetFileName() : _T("Œﬁ±ÍÃ‚"),
            TVI_ROOT, TVI_LAST);
        CTreeViewCtrl::SetItemData(hItem, em_rvt_none);
        

        InitStringTable(document);

        CTreeViewCtrl::Expand(CTreeViewCtrl::GetRootItem());

        return lResult;
    }

    void InitStringTable( CResDocument& document )
    {
        HTREEITEM hItem = CTreeViewCtrl::InsertItem(
            _T("StringTable[11]"),
            CTreeViewCtrl::GetRootItem(), TVI_LAST);
        
        CTreeViewCtrl::SetItemData(hItem, em_rvt_string_table);
                
    }


};
