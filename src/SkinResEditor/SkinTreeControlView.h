/********************************************************************
* CreatedOn: 2008-2-17   12:25
* FileName: SkinTreeControlView.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include "ControlDef.h"

class SkinTreeControlView : 
    public CWindowImpl<SkinTreeControlView, CTreeViewCtrl>,
    public SkinTreeControl
{
    struct TreeItemData
    {
        SkinTreeItemControl* piTreeItemControl;
        LPARAM lParam;
    };

    std::map<HTREEITEM, TreeItemData> m_mapItemData;
    
    TreeItemData m_lastResult;

public:

    SkinTreeControlView()
    {
        m_lastResult.lParam = 0;
        m_lastResult.piTreeItemControl = NULL;
    }

public:

    virtual HTREEITEM GetRootItem()
    {
        return CTreeViewCtrl::GetRootItem();
    }

    virtual HTREEITEM InsertControlItem(HTREEITEM hParent,
        LPCTSTR pszDisName,
        int nImageIndex,
        SkinTreeItemControl* piTreeItemControl,
        LPARAM lParam)
    {
        HTREEITEM hResult = CTreeViewCtrl::InsertItem(pszDisName, nImageIndex,
            nImageIndex, hParent, TVI_LAST);

        if (hResult == NULL)
            return hResult;
        
        if (piTreeItemControl != NULL)
        {
            m_mapItemData[hResult].piTreeItemControl = piTreeItemControl;
            m_mapItemData[hResult].lParam = lParam;

            piTreeItemControl->InitResult(hResult);
        }


        return hResult;
    }

    virtual void DeleteControlItem(HTREEITEM hTreeItem)
    {
        CTreeViewCtrl::DeleteItem(hTreeItem);
    }

    //////////////////////////////////////////////////////////////////////////

    DECLARE_WND_SUPERCLASS(NULL, CTreeViewCtrl::GetWndClassName())

    BOOL PreTranslateMessage(MSG* pMsg)
    {
        pMsg;
        return FALSE;
    }

    BEGIN_MSG_MAP(SkinResTreeView)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        REFLECTED_NOTIFY_CODE_HANDLER(TVN_SELCHANGED, OnSelChanged)
    END_MSG_MAP()


    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        LRESULT lResult = DefWindowProc();


        return lResult;
    }

    LRESULT OnSelChanged(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
    {
        LPNMTREEVIEW pNMTV = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

        TreeItemData newResult = { 0 };
        
        std::map<HTREEITEM, TreeItemData>::const_iterator itfind = 
            m_mapItemData.find(pNMTV->itemNew.hItem);

        if (itfind != m_mapItemData.end())
            newResult = itfind->second;

        if (m_lastResult.piTreeItemControl != NULL)   
        {
            m_lastResult.piTreeItemControl->HideResult(
                pNMTV->itemOld.hItem,
                m_lastResult.lParam);
        }

        m_lastResult = newResult;

        if (newResult.piTreeItemControl != NULL)
        {
            newResult.piTreeItemControl->ShowResult(
                pNMTV->itemNew.hItem,
                newResult.lParam);
        }
        else
        {

        }

        return 0;
    }

    LRESULT OnDeleteItem(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
    {
        LPNMTREEVIEW pNMTV = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
        
        m_mapItemData.erase(pNMTV->itemNew.hItem);

        return 0;
    }


};