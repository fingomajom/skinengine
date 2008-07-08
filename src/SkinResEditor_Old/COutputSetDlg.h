// aboutdlg.h : interface of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "resource.h"
#include "SREConfigMgt.h"

class COutputSetDlg : public CDialogImpl<COutputSetDlg>
{
public:
    enum { IDD = IDD_OUTPUTSET_DIALOG };

    BEGIN_MSG_MAP(COutputSetDlg)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_ID_HANDLER(IDOK, OnOKCmd)
        COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
        COMMAND_HANDLER(IDC_SF_BUTTON1, BN_CLICKED, OnBnClickedSfButton1)
    END_MSG_MAP()

    // Handler prototypes (uncomment arguments if needed):
    //	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    //	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    //	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        CenterWindow(GetParent());

        SREConfigMgt config;
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        KSGUI::CString strHF;

        config.GetHeaderFileName( ControlsMgt.m_resDocument.GetFileName(),
            strHF);

        SetDlgItemText(IDC_RF_EDIT, ControlsMgt.m_resDocument.GetFileName());
        SetDlgItemText(IDC_HF_EDIT, strHF);

        return TRUE;
    }

    LRESULT OnOKCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        SREConfigMgt config;

        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        KSGUI::CString strHF;

        TCHAR pszBuffer[MAX_PATH] = { 0 };

        GetDlgItemText(IDC_HF_EDIT, pszBuffer, MAX_PATH);

        strHF = pszBuffer;

        config.SetHeaderFileName( ControlsMgt.m_resDocument.GetFileName(),
            strHF);

        EndDialog(wID);
        return 0;
    }

    LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        EndDialog(wID);
        return 0;
    }

    LRESULT OnBnClickedSfButton1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CFileDialog saveDlg(FALSE, _T("h"), _T(""),
            OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,// | OFN_ALLOWMULTISELECT, 
            _T("header file(*.h)\0*.h\0所有文件(*.*)\0*.*\0\0"));

        saveDlg.m_ofn.lpstrTitle = (_T("另存为"));

        if (saveDlg.DoModal() != IDOK)
            return 0;

        SetDlgItemText(IDC_HF_EDIT, saveDlg.m_ofn.lpstrFile);

        return 0;
    }
};
