#include "StdAfx.h"
#include "MainFrm.h"

LRESULT CMainFrame::OnFileSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // TODO: Add your command handler code here

    CResDocument& document = CResDocument::Instance();
    
    m_wndRightListView.SaveResToDocument();
    m_wndImageEditDlg.SaveResToDocument();


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

LRESULT CMainFrame::OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    //CFileDialog conDlg(TRUE, _T("bak"), _T("arpconfig"),
    //    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
    //    _T("备份文件(*.bak)\0*.bak; *.bak \0所有文件(*.*)\0*.*\0\0"));

    //conDlg.m_ofn.lpstrTitle = (_T("导入设置"));

    //if (IDOK != conDlg.DoModal())
    //{
    //    return 0;
    //}


    return 0;
}
