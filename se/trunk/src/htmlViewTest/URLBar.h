// aboutdlg.h : interface of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CURLBar : public CDialogImpl<CURLBar>
{
public:
    enum { IDD = IDD_URL_DIALOG };

    BEGIN_MSG_MAP(CAboutDlg)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_ID_HANDLER(IDOK, OnOKCmd)
    END_MSG_MAP()

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {

        return TRUE;
    }

    LRESULT OnOKCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        SendMessage( GetParent(), WM_COMMAND, MAKEWPARAM(IDOK,BN_CLICKED), 0 );

        return 0;
    }
};
