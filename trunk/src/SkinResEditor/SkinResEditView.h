
#pragma once


#include "SkinControlsMgt.h"

class SkinResEditView : 
    public CWindowImpl<SkinResEditView, CEdit>,
    public SkinTreeItemControl
{
public:

    virtual void InitResult(HTREEITEM hTreeItem)
    {

        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        if (m_hWnd == NULL)
        {
            Create(ControlsMgt.m_piSkinFrame->GetResultParentWnd(), 
                rcDefault, 
                NULL, 
                WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_WANTRETURN | ES_READONLY,
                WS_EX_CLIENTEDGE);

            SetFont( ControlsMgt.m_skinTreeControlView.GetFont() );
        }
    }

    virtual void ShowResult(HTREEITEM hTreeItem, LPARAM lParam)
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        ShowWindow(SW_SHOW);

        ControlsMgt.m_piSkinFrame->SetActiveResultWindow(m_hWnd);

        KSG::SkinXmlDocument doc;
        ControlsMgt.m_resDocument.SaveDocument(doc);

        KSG::CString strXmlText;

        doc.ToXMLString(strXmlText);

        SetWindowText( strXmlText );

    }

    virtual void HideResult(HTREEITEM hTreeItem, LPARAM lParam)
    {
        ShowWindow(SW_HIDE);

    }

    BEGIN_MSG_MAP(SkinResTreeView)
        MESSAGE_HANDLER( WM_ERASEBKGND, OnEraseBkgnd )
    END_MSG_MAP()

    LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        RECT rcClient = { 0 };

        GetClientRect(&rcClient);

        CClientDC dc(m_hWnd);
        
        dc.FillRect(&rcClient, COLOR_WINDOW);

        return 1L;
    }


};
