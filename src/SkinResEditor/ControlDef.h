/********************************************************************
* CreatedOn: 2008-2-17   12:25
* FileName: ControlDef.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

class SkinTreeItemControl
{
public:

    virtual void InitResult(HTREEITEM hTreeItem) = 0;

    virtual void ShowResult(HTREEITEM hTreeItem, LPARAM lParam) = 0;
    virtual void HideResult(HTREEITEM hTreeItem, LPARAM lParam) = 0;

};

class SkinFrame
{
public:

    virtual HWND SetActiveResultWindow(HWND hWndResult) = 0;

    virtual HWND GetResultParentWnd() = 0;

    virtual void OnReUpdateResView() = 0;

};

class SkinTreeControl
{
public:

    virtual HTREEITEM GetRootItem() = 0;

    virtual HTREEITEM InsertControlItem(HTREEITEM hParent,
        LPCTSTR pszDisName,
        int nImageIndex,
        SkinTreeItemControl* piTreeItemControl,
        LPARAM lParam
        ) = 0;

    virtual void DeleteControlItem(HTREEITEM hTreeItem) = 0;

};