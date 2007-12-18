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

        strMsg.Format(_T("�ļ� [%s] �Ը��ģ�ȷ��Ҫ�������˳���"), document.GetFileName());

        if (MessageBox(strMsg, _T("��ʾ"), MB_YESNO | MB_ICONQUESTION) != IDYES)
            return 0;

    }

    return DefWindowProc();
}

LRESULT CMainFrame::OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    //CFileDialog conDlg(TRUE, _T("bak"), _T("arpconfig"),
    //    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
    //    _T("�����ļ�(*.bak)\0*.bak; *.bak \0�����ļ�(*.*)\0*.*\0\0"));

    //conDlg.m_ofn.lpstrTitle = (_T("��������"));

    //if (IDOK != conDlg.DoModal())
    //{
    //    return 0;
    //}


    return 0;
}