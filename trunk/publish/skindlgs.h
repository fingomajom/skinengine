/********************************************************************
* CreatedOn: 2008-7-24   9:01
* FileName: skindlgs.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once 


#include <skindialog.h>

namespace KSGUI{


class SkinMessageBox : 
    public KSGUI::SkinCaptionDialogImpl<SkinMessageBox>
{
public:

    SkinMessageBox() :
        m_nMsgBoxRet(0)
    {
    }

    SkinMessageBox( 
        HWND     hWnd, 
        LPCTSTR  lpText, 
        LPCTSTR  lpCaption, 
        UINT     uType, 
        COLORREF clrMainColor = 0x007b4604 )
    {
        m_nMsgBoxRet = MessageBox( hWnd, lpText, lpCaption, uType, clrMainColor );
    }

    operator int ()
    {
        return m_nMsgBoxRet;
    }

    int MessageBox( HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, COLORREF clrMainColor = 0x007b4604 )
    {
        gethookid() = SetWindowsHookEx (WH_CALLWNDPROC,
            (HOOKPROC)HookMessageBoxInit,
            NULL,
            GetCurrentThreadId ());

        getpthis()     = this;
        m_clrMainColor = clrMainColor;

        int nRet = ::MessageBox(hWnd, lpText, lpCaption, uType);

        if ( gethookid() != NULL )
        {
            UnhookWindowsHookEx(gethookid());
            gethookid() = NULL;
        }

        return nRet;
    }

    BEGIN_MSG_MAP(SkinMessageBox)

        MESSAGE_HANDLER(WM_PAINT  , OnPaint   )
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy )

        CHAIN_MSG_MAP(KSGUI::SkinCaptionDialogImpl<SkinMessageBox>)

        MESSAGE_HANDLER(WM_INITDIALOG    , OnInitDialog)
        MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColor)

        CallWindowProc((WNDPROC)getoldproc(), hWnd, uMsg, wParam, lParam);

    END_MSG_MAP()


protected:

    static HHOOK& gethookid()
    {
        static HHOOK gs_hhook;
        return gs_hhook;
    }

    static DLGPROC& getoldproc()
    {
        static DLGPROC gs_proc;
        return gs_proc;        
    }

    static CDialogImplBaseT< SkinWindow >*& getpthis()
    {
        static CDialogImplBaseT< SkinWindow >* gs_pthis;
        return gs_pthis;
    }

    WTL::CStatic m_wndIconStatic;
    std::vector<KSGUI::CSkinColorButton*> m_vtCorBtn;

    int m_nMsgBoxRet;

protected:

    LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CenterWindow(GetParent());

        ::EnumChildWindows( m_hWnd, EnumChildProc, (LPARAM)this);

        bHandled = FALSE;

        return 1L;
    }

    LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        for ( size_t idx = 0; idx < m_vtCorBtn.size(); idx++)
        {
            KSGUI::CSkinColorButton* pClrBtn = m_vtCorBtn[idx];

            if (pClrBtn->IsWindow())
                pClrBtn->DestroyWindow();
            delete pClrBtn;
        }

        m_vtCorBtn.clear();

        bHandled = FALSE;
        return 1L;
    }


    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CPaintDC dc(m_hWnd);

        DrawDirectUI(dc);

        if ( m_wndIconStatic.IsWindow() )
        {
            RECT rcClient = { 0 };
            m_wndIconStatic.GetWindowRect( &rcClient );
            ScreenToClient(&rcClient);

            CIconHandle icon = m_wndIconStatic.GetIcon();
            if (!icon.IsNull())
                icon.DrawIcon(dc, rcClient.left, rcClient.top);
        }

        return 0;
    }


    LRESULT OnCtlColor(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        ::SetBkMode( (HDC)wParam, 0 );

        return (LRESULT)::AtlGetStockBrush(NULL_BRUSH);
    }

    static BOOL CALLBACK EnumChildProc( HWND hwnd,
        LPARAM lParam )
    {
        SkinMessageBox* pThis = (SkinMessageBox*)lParam;

        TCHAR szClassName[MAX_PATH] = { 0 };

        GetClassName(hwnd, szClassName, MAX_PATH);

        if ( !_tcsicmp(szClassName, CStatic::GetWndClassName()) )
        {
            WTL::CStatic m_wndStatic;

            m_wndStatic.Attach( hwnd );

            if (m_wndStatic.GetStyle() & SS_ICON)
            {                
                m_wndStatic.ShowWindow(SW_HIDE);
                pThis->m_wndIconStatic.Attach(hwnd);
            }
        }
        else if ( !_tcsicmp(szClassName, CButton::GetWndClassName()) )
        {
            KSGUI::CSkinColorButton* pClrBtn = new KSGUI::CSkinColorButton;
            if (pClrBtn != NULL)
            {
                RECT rcClient = { 0 };
                ::GetWindowRect(hwnd, &rcClient);
                pThis->ScreenToClient(&rcClient);

                ::SetWindowPos( hwnd, NULL, rcClient.left, rcClient.top + 1, 
                    rcClient.right - rcClient.left,
                    rcClient.bottom - rcClient.top - 2, 
                    SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);


                pClrBtn->SubclassWindow( hwnd );
                pThis->m_vtCorBtn.push_back( pClrBtn );
            }

        }

        return TRUE;
    }


private:


    static LRESULT CALLBACK HookMessageBoxInit(int nCode,WPARAM wParam,LPARAM lParam)
    {
        if (nCode == HC_ACTION)
        {
            CWPSTRUCT* pwp = (CWPSTRUCT*)lParam;

            if (pwp->message == WM_INITDIALOG)
            {
                BOOL result;

                getpthis()->m_hWnd = pwp->hwnd;
                getpthis()->ModifyStyle(WS_CAPTION | WS_CLIPCHILDREN, 0);

                result = getpthis()->m_thunk.Init((WNDPROC)getpthis()->GetDialogProc(), 
                    getpthis());
                if (result == FALSE) 
                {
                    SetLastError(ERROR_OUTOFMEMORY);
                    return -1;
                }

                DLGPROC pProc = (DLGPROC)getpthis()->m_thunk.GetWNDPROC();
                getoldproc() = (DLGPROC)::SetWindowLongPtr(pwp->hwnd, DWLP_DLGPROC, (LONG_PTR)pProc);

                UnhookWindowsHookEx(gethookid());
                gethookid() = NULL;
            }

        }

        return CallNextHookEx(gethookid(),nCode,wParam,lParam);
    }
};


};
