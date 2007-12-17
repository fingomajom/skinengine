#include "StdAfx.h"
#include "EditStringDlg.h"

CEditStringDlg::CEditStringDlg(void)
{
}

CEditStringDlg::~CEditStringDlg(void)
{
}

LRESULT CEditStringDlg::OnEnChangeIdEdit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    TCHAR szBuffer[4096] = { 0 };

    GetDlgItemText( IDC_ID_EDIT, szBuffer, 4096 );

    if (_tcslen(szBuffer) == 0)
    {
        ::EnableWindow(GetDlgItem(IDOK), FALSE);
        return TRUE;
    }
    if (szBuffer[0] >= '0' && szBuffer[0] <= '9')
    {
        ::EnableWindow(GetDlgItem(IDOK), FALSE);
        return TRUE;
    }

    GetDlgItemText( IDC_CAPTION_EDIT, szBuffer, 4096 );
    if (_tcslen(szBuffer) == 0)
    {
        ::EnableWindow(GetDlgItem(IDOK), FALSE);
        return TRUE;
    }

    ::EnableWindow(GetDlgItem(IDOK), TRUE);

    return 0;
}

LRESULT CEditStringDlg::OnEnChangeCaptionEdit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    TCHAR szBuffer[4096] = { 0 };

    GetDlgItemText( IDC_ID_EDIT, szBuffer, 4096 );

    if (_tcslen(szBuffer) == 0)
    {
        ::EnableWindow(GetDlgItem(IDOK), FALSE);
        return TRUE;
    }

    if (szBuffer[0] >= '0' && szBuffer[0] <= '9')
    {
        ::EnableWindow(GetDlgItem(IDOK), FALSE);
        return TRUE;
    }

    GetDlgItemText( IDC_CAPTION_EDIT, szBuffer, 4096 );
    if (_tcslen(szBuffer) == 0)
    {
        ::EnableWindow(GetDlgItem(IDOK), FALSE);
        return TRUE;
    }

    ::EnableWindow(GetDlgItem(IDOK), TRUE);

    return 0;
}
