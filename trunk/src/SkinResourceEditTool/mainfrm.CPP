#include "StdAfx.h"
#include "MainFrm.h"

LRESULT CMainFrame::OnFileSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // TODO: Add your command handler code here

    CResDocument& document = CResDocument::Instance();
    
    
    std::vector<CStringTableResource::STRINGTABLE_ITEMINFO>& vtList =
        document.GetStringTableList();


    vtList.resize( m_wndRightListView.GetItemCount() - 1 );

    for (int i = 0; i < m_wndRightListView.GetItemCount() - 1; i++)
    {
        m_wndRightListView.GetItemText(i, 1, vtList[i].strIDName);
        m_wndRightListView.GetItemText(i, 2, vtList[i].strValue);
    }

    document.Save();

    return 0;
}

LRESULT CMainFrame::OnFileSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // TODO: Add your command handler code here

    return 0;
}

LRESULT CMainFrame::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    CResDocument& document = CResDocument::Instance();
    
    if (document.GetChanged())
    {
        WTL::CString strMsg;

        strMsg.Format(_T("文件 [%s] 以更改，确认要不保存退出吗？"), document.GetFileName());

        if (MessageBox(strMsg, _T("提示"), MB_YESNO | MB_ICONQUESTION) != IDYES)
            return 0;

    }

    return DefWindowProc();
}
