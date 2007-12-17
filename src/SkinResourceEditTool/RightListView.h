
#pragma once


#include "ResDocument.h"
#include "EditStringDlg.h"
#include "EditResBase.h"


class CRightListView : 
    public CWindowImpl<CRightListView, CListViewCtrl>,
    public CEditResBase
{
public:
    DECLARE_WND_SUPERCLASS(NULL, CListViewCtrl::GetWndClassName())

    BOOL PreTranslateMessage(MSG* pMsg)
    {
        pMsg;
        return FALSE;
    }

    BEGIN_MSG_MAP(CRightListView)
        MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
        REFLECTED_NOTIFY_CODE_HANDLER(NM_DBLCLK, OnRightListDbClk)
    END_MSG_MAP()

    // Handler prototypes (uncomment arguments if needed):
    //	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    //	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    //	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
    
    LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if (wParam == VK_DELETE)
        {
            int nIndex = GetSelectedIndex();
            
            if (nIndex >= 0 && nIndex < GetItemCount() - 1)
            {
                DeleteItem(nIndex);

                CResDocument& document = CResDocument::Instance();
                document.SetChanged();
            }    
        }

        return DefWindowProc();
    }


    virtual BOOL LoadResFromDocumnet()
    {
        if (m_hWnd == NULL)
            return FALSE;

        CResDocument& document = CResDocument::Instance();

        std::vector<CStringTableResource::STRINGTABLE_ITEMINFO>& vtList =
            document.GetStringTableResource()->GetStringTableList();

        if ( GetHeader().GetItemCount() == 0 )
        {
            InsertColumn(0, _T("Type")   , LVCFMT_LEFT, 60 );
            InsertColumn(2, _T("ID")     , LVCFMT_LEFT, 150);
            InsertColumn(3, _T("Caption"), LVCFMT_LEFT, 550);
        }


        for (size_t i = 0; i < vtList.size(); i++)
        {
            int nIndex = InsertItem(0xFFFFF, _T("String"));
            SetItemText(nIndex, 1, vtList[i].strIDName );
            SetItemText(nIndex, 2, vtList[i].strValue );
        }

        InsertItem(0xFFFFF, _T("New..."));

        return TRUE;
    }

    virtual BOOL SaveResToDocument()
    {
        if (m_hWnd == NULL)
            return FALSE;

        CResDocument& document = CResDocument::Instance();

        std::vector<CStringTableResource::STRINGTABLE_ITEMINFO>& vtList =
            document.GetStringTableResource()->GetStringTableList();


        vtList.resize( GetItemCount() - 1 );

        for (int i = 0; i < GetItemCount() - 1; i++)
        {
            GetItemText(i, 1, vtList[i].strIDName);
            GetItemText(i, 2, vtList[i].strValue);
        }

        return TRUE;
    }


    virtual BOOL ClearRes()
    {
        if (m_hWnd == NULL)
            return FALSE;

        DeleteAllItems();

        InsertItem(0xFFFFF, _T("New..."));

        return TRUE;

    }

    LRESULT OnRightListDbClk(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
    {
        int nIndex = GetSelectedIndex();

        if (nIndex < 0 )
            return DefWindowProc();

        CEditStringDlg dlg;

        GetItemText(nIndex, 1, dlg.m_strId);
        GetItemText(nIndex, 2, dlg.m_strCaption);

        while (dlg.DoModal() == IDOK)
        {
            for (int i = 0; i < GetItemCount() - 1; i++)
            {
                if (i == nIndex)
                    continue;

                WTL::CString strId;

                GetItemText(i, 1, strId);

                if (strId == dlg.m_strId)
                {
                    SelectItem(i);

                    return DefWindowProc();
                }
            }

            if ( nIndex == GetItemCount() - 1 )
            {
                SetItemText(nIndex, 0, _T("String"));
                InsertItem(0xFFFFF, _T("New..."));
            }

            SetItemText(nIndex, 1, dlg.m_strId );
            SetItemText(nIndex, 2, dlg.m_strCaption );

            CResDocument& document = CResDocument::Instance();
            document.SetChanged();

            break;
        }

        return DefWindowProc();
    }

};
