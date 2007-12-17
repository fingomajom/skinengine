#pragma once

#include "resource.h"

class CEditStringDlg : public CDialogImpl<CEditStringDlg>
{
public:
    CEditStringDlg(void);
    ~CEditStringDlg(void);

public:
    enum { IDD = IDD_EDITSTRING_DIALOG };

    BEGIN_MSG_MAP(CAboutDlg)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_ID_HANDLER(IDOK, OnOk)
        COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
        COMMAND_HANDLER(IDC_ID_EDIT, EN_CHANGE, OnEnChangeIdEdit)
        COMMAND_HANDLER(IDC_CAPTION_EDIT, EN_CHANGE, OnEnChangeCaptionEdit)
    END_MSG_MAP()

    // Handler prototypes (uncomment arguments if needed):
    //	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    //	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    //	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
    
    WTL::CString m_strId;
    WTL::CString m_strCaption;

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        CenterWindow(GetParent());

        if (m_strId == _T(""))
            m_strId = _T("IDS_");

        SetDlgItemText(IDC_ID_EDIT, m_strId);
        SetDlgItemText(IDC_CAPTION_EDIT, m_strCaption);

        return TRUE;
    }

    LRESULT OnOk(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        TCHAR szBuffer[4096] = { 0 };

        GetDlgItemText( IDC_ID_EDIT, szBuffer, 4096 );       m_strId     = szBuffer;
        GetDlgItemText( IDC_CAPTION_EDIT, szBuffer, 4096 ); m_strCaption = szBuffer;
    
        EndDialog(wID);
        return 0;
    }

    LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        EndDialog(wID);
        return 0;
    }

    LRESULT OnEnChangeIdEdit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnEnChangeCaptionEdit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
