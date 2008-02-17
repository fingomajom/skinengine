/********************************************************************
* CreatedOn: 2008-2-17   12:25
* FileName: SkinResEditView.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once


#include "SkinControlsMgt.h"
#include <skindialog.h>

class SkinResEditView : 
    public KSG::SkinDialogImpl<SkinResEditView>, //CWindowImpl<SkinResEditView, CPaneContainer>,
    public SkinTreeItemControl
{

public:

    CEdit m_wndEdit;

    virtual void InitResult(HTREEITEM hTreeItem)
    {

        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        if (m_hWnd == NULL)
        {
            Create(ControlsMgt.m_piSkinFrame->GetResultParentWnd());

            m_wndEdit.Create(m_hWnd, 
                rcDefault, 
                NULL, 
                WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_WANTRETURN | ES_READONLY,
                WS_EX_CLIENTEDGE);

            m_wndEdit.SetFont( ControlsMgt.m_skinTreeControlView.GetFont() );

        }
    }

    virtual void ShowResult(HTREEITEM hTreeItem, LPARAM lParam)
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        ShowWindow(SW_SHOW);

        ControlsMgt.m_piSkinFrame->SetActiveResultWindow(m_hWnd);

        //if (ControlsMgt.m_resDocument.Modify())
        {
            KSG::SkinXmlDocument doc;
            ControlsMgt.m_resDocument.SaveDocument(doc);

            KSG::CString strXmlText;

            doc.ToXMLString(strXmlText);

            m_wndEdit.SetWindowText( strXmlText );
        }

    }

    virtual void HideResult(HTREEITEM hTreeItem, LPARAM lParam)
    {
        ShowWindow(SW_HIDE);

    }

    DWORD GetDefaultStyle()
    {
        return DS_SETFONT | WS_CHILD | WS_SYSMENU;
    }


    BEGIN_MSG_MAP(SkinResTreeView)
        MESSAGE_HANDLER(WM_SIZE , OnSize)
        //MESSAGE_HANDLER( WM_ERASEBKGND, OnEraseBkgnd )
    END_MSG_MAP()

    LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {

        if (m_wndEdit.m_hWnd != NULL)
        {
            RECT rcClient = { 0 };

            GetClientRect(&rcClient);

            m_wndEdit.MoveWindow(&rcClient);

        }

        return DefWindowProc();
    }


    LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        RECT rcClient = { 0 };

        GetClientRect(&rcClient);

        CClientDC dc(m_hWnd);
        
        dc.FillRect(&rcClient, COLOR_WINDOW);

        return 1L;
    }


};
