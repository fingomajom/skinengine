/********************************************************************
* CreatedOn: 2007-12-19   17:40
* FileName: SkinResTreeView.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/
#pragma once

#include "SkinResDocument.h"

enum{
    em_rvt_none = 0,
    em_rvt_image,
    em_rvt_string_table,
    em_rvt_dialogs,
    em_rvt_dialog,
    em_rvt_dialog_item
};

class SkinResTreeView : public CWindowImpl<SkinResTreeView, CTreeViewCtrl>
{
public:
    DECLARE_WND_SUPERCLASS(NULL, CTreeViewCtrl::GetWndClassName())

    BOOL PreTranslateMessage(MSG* pMsg)
    {
        pMsg;
        return FALSE;
    }

    BEGIN_MSG_MAP(SkinResTreeView)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        //REFLECTED_NOTIFY_CODE_HANDLER(TVN_SELCHANGED, OnSelChanged)
    END_MSG_MAP()


    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        LRESULT lResult = DefWindowProc();

        //CResDocument& document = CResDocument::Instance();
        //
        //HTREEITEM hItem = CTreeViewCtrl::InsertItem(
        //    _tcslen(document.GetFileName()) > 0 ? document.GetFileName() : _T("Œﬁ±ÍÃ‚"),
        //    TVI_ROOT, TVI_LAST);
        //CTreeViewCtrl::SetItemData(hItem, em_rvt_none);
        //

        //InitStringTable(document);

        //CTreeViewCtrl::Expand(CTreeViewCtrl::GetRootItem());

        return lResult;
    }

    //void InitStringTable( CResDocument& document )
    //{
    //    HTREEITEM hItem = CTreeViewCtrl::InsertItem(
    //        _T("Images"),
    //        CTreeViewCtrl::GetRootItem(), TVI_LAST);

    //    CTreeViewCtrl::SetItemData(hItem, em_rvt_image);

    //    hItem = CTreeViewCtrl::InsertItem(
    //        _T("StringTable[11]"),
    //        CTreeViewCtrl::GetRootItem(), TVI_LAST);

    //    CTreeViewCtrl::SetItemData(hItem, em_rvt_string_table);
    //            
    //}


};
