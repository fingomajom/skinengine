// aboutdlg.h : interface of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <atlctrlx.h>
#include "resource.h"
#include "QuickSearchMgr.h"


static WNDPROC  g_oldProc = NULL;

class CAboutDlg : public CDialogImpl<CAboutDlg>
{
public:
	enum { IDD = IDD_ADDR_BAND };

	BEGIN_MSG_MAP(CAboutDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)

        COMMAND_HANDLER(IDC_QS_EDIT, EN_CHANGE, OnQSEditChange)
	END_MSG_MAP()

    LRESULT OnQSEditChange(WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/)
    {
        CQuickSearchMgr& mgt = CQuickSearchMgr::Instance();

        TCHAR szText[MAX_PATH] = { 0 };
        ::GetWindowText(hWndCtl, szText, MAX_PATH);

        mgt.m_wndQSPopWnd.ClearResult();
        mgt.m_wndQSPopWnd.QSearchKeyword( szText );

        if ( !mgt.m_wndQSPopWnd.IsWindowVisible() )
        {
            mgt.m_wndQSPopWnd.ShowWindow( SW_SHOWNOACTIVATE );
        }

        return 1L;
    }


    static LRESULT CALLBACK WindowProc(
        HWND hwnd,
        UINT uMsg,
        WPARAM wParam,
        LPARAM lParam)
    {
        CQuickSearchMgr& mgt = CQuickSearchMgr::Instance();

        switch( uMsg )
        {
        case WM_SETFOCUS:
            {
                RECT rcWindow = { 0 };
                ::GetWindowRect(hwnd, &rcWindow);

                POINT pt = { rcWindow.right, rcWindow.bottom };

                mgt.m_wndQSPopWnd.ShowQSPopWnd_ForQSBar( pt );
            }
            break;
        case WM_KILLFOCUS:
            if ( GetFocus() == mgt.m_wndQSPopWnd ||
                ::GetParent(GetFocus()) == mgt.m_wndQSPopWnd )
                break;

            if ( ::IsWindow(mgt.m_wndQSPopWnd) )
                mgt.m_wndQSPopWnd.ShowWindow(SW_HIDE);
            break;
        }

        return CallWindowProc( g_oldProc, hwnd, uMsg, wParam, lParam );
    }

    BOOL IsDialogMessage(LPMSG lpMsg)
    {
        BOOL bResult = CDialogImpl<CAboutDlg>::IsDialogMessage(lpMsg);
        
        return bResult;
    }


	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
        CEdit edit = GetDlgItem(IDC_QS_EDIT);
        
        g_oldProc = (WNDPROC)edit.SetWindowLong( GWL_WNDPROC, (LONG)WindowProc );

		return TRUE;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		return 0;
	}

};
