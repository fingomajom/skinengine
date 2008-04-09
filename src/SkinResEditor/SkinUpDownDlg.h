/********************************************************************
* CreatedOn: 2008-4-9   10:37
* FileName: SkinUpDownDlg.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

class SkinUpDownDlg : public CDialogImpl<SkinUpDownDlg>
{
public:
    enum { IDD = IDD_UP_DOWN_DIALOG };

    CBitmap m_up_bmp;
    CBitmap m_down_bmp;

    HWND m_hWndParent;

    BEGIN_MSG_MAP(SkinUpDownDlg)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_COMMAND   , OnCommand)

        COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
        COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
    END_MSG_MAP()

    // Handler prototypes (uncomment arguments if needed):
    //	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    //	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    //	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

    LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {
        if (HIWORD(wParam) == BN_CLICKED &&
            ( LOWORD(wParam) == IDC_UP_BUTTON || LOWORD(wParam) == IDC_DOWN_BUTTON ))
            return ::SendMessage( m_hWndParent, uMsg, wParam, lParam );

        return DefWindowProc();
    }


    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        CButton up   = GetDlgItem(IDC_UP_BUTTON);
        CButton down = GetDlgItem(IDC_DOWN_BUTTON);

        m_up_bmp.LoadBitmap(IDB_UP_BITMAP);
        m_down_bmp.LoadBitmap(IDB_DOWN_BITMAP);

        up.SetBitmap(m_up_bmp);
        down.SetBitmap(m_down_bmp);

        const int width  = 20;
        const int height = 20;

        RECT rcClient = { 0, 0, width, height };

        MoveWindow(&rcClient);

        rcClient.bottom = height / 2;

        up.MoveWindow(&rcClient);

        rcClient.top = height / 2;
        rcClient.bottom = height;

        down.MoveWindow(&rcClient);

        return TRUE;
    }

    LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        EndDialog(wID);
        return 0;
    }
};
