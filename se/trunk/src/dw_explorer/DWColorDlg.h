
#pragma once

#include "DWGDI.h"
#include "DWEventSvr.h"

static HHOOK s_hColorDialogHook = NULL;
static HWND  s_hWndColorDialog  = NULL;

class CDWColorDialog : public CDialogImpl<CDWColorDialog>
{
public:
    enum { IDD = IDD_COLOR_DIALOG };


    BEGIN_MSG_MAP(CDWColorDialog)
        
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_PAINT     , OnPaint     )
        
        MESSAGE_HANDLER(WM_HSCROLL   , OnHScroll   )

    END_MSG_MAP()

    
    CTrackBarCtrl m_wndClrTrack;

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        m_wndClrTrack = GetDlgItem( IDC_COLOR_SLIDER );

        RECT rcClient;
        GetClientRect(&rcClient);

        m_wndClrTrack.SetRange( 0, rcClient.right-rcClient.left-20 );

        return TRUE;
    }

    LRESULT OnHScroll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        int nPos = m_wndClrTrack.GetPos();

        CClientDC dc(m_hWnd);

        COLORREF clrNew = dc.GetPixel(nPos+10, 10);
        
        if ( clrNew != skin.clrFrameWindow )
        {
            skin.clrFrameWindow = clrNew;

            CDWEventSvr::Instance().OnMessage( edi_skin_changed );
            ::RedrawWindow(GetParent(), NULL, NULL, RDW_FRAME | RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
        }

        return TRUE;
    }


    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CPaintDC dc(m_hWnd);

        CSkinDCHandle skinDC(dc);

        RECT rcClient;
        GetClientRect(&rcClient);

        rcClient.top += 5;
        rcClient.bottom -= 30;

        rcClient.left  += 10;
        rcClient.right -= 9;

        COLORREF clrs[] = 
        {
            RGB(0,0,255),
            RGB(0,255,0),
            RGB(255,0,0),
            RGB(0,255,255),
            RGB(255,255,0),
            RGB(255,0,255),
            RGB(0,255,255),
            RGB(0,128,255),
            RGB(128,0,128),
            RGB(255,0,255)
        };

        int nCount = sizeof(clrs) / sizeof(clrs[0]);

        float fwidth = (float)(rcClient.right - rcClient.left) / (nCount - 1);

        for ( int i = 0; i < nCount - 1; i++ )
        {
            RECT rcBox = rcClient;
            rcBox.left  += int(i * fwidth);
            rcBox.right  = int(rcBox.left + fwidth)+1;

            skinDC.SkinDrawGradualColorRect( rcBox,
                clrs[i], clrs[i+1], FALSE );
        }


        return 0L;
    }

    BOOL ShowWindow(int nCmdShow) throw()
    {
        s_hWndColorDialog = m_hWnd;
        s_hColorDialogHook = ::SetWindowsHookEx(WH_GETMESSAGE, 
            MsgHookFunc, 
            _Module.GetModuleInstance(), 
            GetCurrentThreadId());
        ATLASSERT(s_hColorDialogHook);

        return CDialogImpl<CDWColorDialog>::ShowWindow( nCmdShow );
    }

    static LRESULT CALLBACK MsgHookFunc(
        int nCode,
        WPARAM wParam,
        LPARAM lParam )
    {
        LPMSG pMsg = (LPMSG)lParam;

        LRESULT lRet = ::CallNextHookEx( s_hColorDialogHook, nCode, wParam, lParam);

        if ( pMsg->message == WM_LBUTTONDOWN || 
             pMsg->message == WM_LBUTTONDBLCLK ||
             pMsg->message == WM_RBUTTONDOWN ||
             pMsg->message == WM_RBUTTONDBLCLK )
        {
            POINT pt;
            GetCursorPos(&pt);

            RECT rcWindow;
            ::GetWindowRect(s_hWndColorDialog, &rcWindow);

            if ( !::PtInRect( &rcWindow, pt ) )
            {
                UnhookWindowsHookEx( s_hColorDialogHook );
                ::ShowWindow(s_hWndColorDialog, SW_HIDE);
            }
        }

        return lRet;
    }


};