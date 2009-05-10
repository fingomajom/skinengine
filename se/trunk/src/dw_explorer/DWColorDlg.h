
#pragma once

#include "DWGDI.h"
#include "DWEventSvr.h"

static HHOOK s_hColorDialogHook = NULL;
static HWND  s_hWndColorDialog  = NULL;

class CDWTrackBarCtrl : public CWindowImpl<CDWTrackBarCtrl, CTrackBarCtrl>
{
public:
    DECLARE_WND_CLASS(_T("DWExplorer_DWTrackBarCtrl"));

    BEGIN_MSG_MAP(CDWTrackBarCtrl)
        //MESSAGE_HANDLER(WM_SETFOCUS  , OnSetFocus )
        MESSAGE_HANDLER(WM_KILLFOCUS , OnKillFocus )
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkGnd )
    END_MSG_MAP()

    LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        //::SetFocus(NULL);
        return 0L;
    }

    LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        LRESULT lResult = DefWindowProc();

        ::ShowWindow(GetParent(), SW_HIDE);

        return lResult;
    }
    LRESULT OnEraseBkGnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        CDWSkinUIMgt* pskin = CDWSkinUIMgt::InstancePtr();
        if ( pskin == NULL )
            return 1L;

        RECT rcClient;
        GetClientRect(&rcClient);
        
        CDCHandle((HDC)wParam).FillSolidRect(&rcClient, pskin->clrFrameWindow);

        return 1L;
    }

};


class CDWColorDialog : public CDialogImpl<CDWColorDialog>
{
public:
    enum { IDD = IDD_COLOR_DIALOG };


    BEGIN_MSG_MAP(CDWColorDialog)
        
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog )
        MESSAGE_HANDLER(WM_PAINT     , OnPaint      )
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkGnd )

        
        MESSAGE_HANDLER(WM_HSCROLL   , OnHScroll   )

        MESSAGE_HANDLER( WM_CTLCOLORBTN   , OnCtlColor )
        MESSAGE_HANDLER( WM_CTLCOLORSTATIC, OnCtlColor )

        MESSAGE_HANDLER( WM_CTLCOLOREDIT, OnCtlColor )
        MESSAGE_HANDLER( WM_CTLCOLORDLG , OnCtlColor )

    END_MSG_MAP()

    
    CDWTrackBarCtrl m_wndClrTrack;

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        m_wndClrTrack.SubclassWindow( GetDlgItem( IDC_COLOR_SLIDER ) );

        RECT rcClient;
        GetClientRect(&rcClient);

        m_wndClrTrack.SetRange( 0, rcClient.right-rcClient.left-20 );

        return TRUE;
    }

    LRESULT OnEraseBkGnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return 1L;
    }

    LRESULT OnHScroll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        CDWSkinUIMgt* pskin = CDWSkinUIMgt::InstancePtr();
        if ( pskin == NULL )
            return 1L;

        int nPos = m_wndClrTrack.GetPos();

        CClientDC dc(m_hWnd);

        COLORREF clrNew = dc.GetPixel(nPos+10, 10);
        
        if ( clrNew != pskin->clrFrameWindow )
        {
            pskin->clrFrameWindow = clrNew;

            CDWEventSvr::Instance().OnMessage( edi_skin_changed );
            ::RedrawWindow(GetParent(), NULL, NULL, RDW_FRAME | RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
        }

        return TRUE;
    }

    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CPaintDC dc(m_hWnd);

        RECT rcClient;
        GetClientRect(&rcClient);

        {
            CMemoryDC memDC(dc, rcClient);

            CDWSkinUIMgt* pskin = CDWSkinUIMgt::InstancePtr();
            if ( pskin == NULL )
                return 1L;

            memDC.FillRect(&rcClient, m_bkBrush);

            rcClient.top    += 5;
            rcClient.bottom -= 30;
            rcClient.left   += 10;
            rcClient.right  -= 9;

            if ( m_imageClr.IsNull() )
            {
                RECT rcImage   = rcClient;

                rcImage.right  = rcImage.right  - rcImage.left;
                rcImage.bottom = rcImage.bottom - rcImage.top;

                rcImage.top = rcImage.left = 0;

                m_imageClr.Create( rcImage.right, rcImage.bottom, 32 );

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

                CSkinDCHandle skinDC( m_imageClr.GetDC() );

                for ( int i = 0; i < nCount - 1; i++ )
                {
                    RECT rcBox = rcImage;
                    rcBox.left  += int(i * fwidth);
                    rcBox.right  = int(rcBox.left + fwidth)+1;

                    skinDC.SkinDrawGradualColorRect( rcBox,
                        clrs[i], clrs[i+1], FALSE );
                }

                m_imageClr.ReleaseDC();
            }

            m_imageClr.BitBlt( memDC, rcClient.left, rcClient.top);

        }


        return 0L;
    }

    LRESULT OnCtlColor(UINT /* uMsg */, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
    {
        CDWSkinUIMgt* pskin = CDWSkinUIMgt::InstancePtr();
        if ( pskin == NULL )
            return 1L;

        CDCHandle dc = (HDC)wParam;
        dc.SetBkMode( TRANSPARENT );

        if ( m_bkBrush.m_hBrush == NULL )
            m_bkBrush.CreateSolidBrush( pskin->clrFrameWindow );


        return (LRESULT)m_bkBrush.m_hBrush;
    }

    BOOL ShowWindow(int nCmdShow) throw()
    {
        if ( nCmdShow != SW_HIDE )
        {
            CDWSkinUIMgt* pskin = CDWSkinUIMgt::InstancePtr();
            if ( pskin == NULL )
                return 1L;

            if ( m_bkBrush.m_hBrush != NULL )
                m_bkBrush.DeleteObject();

            m_bkBrush.CreateSolidBrush( pskin->clrFrameWindow );


            ATLASSERT(s_hColorDialogHook == NULL);
            ATLASSERT(s_hWndColorDialog == NULL);

            s_hWndColorDialog = m_hWnd;
            s_hColorDialogHook = ::SetWindowsHookEx(WH_GETMESSAGE, 
                MsgHookFunc, 
                _Module.GetModuleInstance(), 
                GetCurrentThreadId());
            ATLASSERT(s_hColorDialogHook);
            SetFocus();
        }
        else
        {
            if ( s_hColorDialogHook != NULL )
            {
                UnhookWindowsHookEx( s_hColorDialogHook );
                s_hColorDialogHook = NULL;
                s_hWndColorDialog  = NULL;
            }

        }

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
            pMsg->message == WM_RBUTTONDBLCLK ||
            pMsg->message == WM_NCLBUTTONDOWN || 
            pMsg->message == WM_NCLBUTTONDBLCLK ||
            pMsg->message == WM_NCRBUTTONDOWN ||
            pMsg->message == WM_NCRBUTTONDBLCLK )
        {
            POINT pt;
            GetCursorPos(&pt);

            RECT rcWindow;
            ::GetWindowRect(s_hWndColorDialog, &rcWindow);

            if ( !::PtInRect( &rcWindow, pt ) )
            {
                UnhookWindowsHookEx( s_hColorDialogHook );
                ::ShowWindow(s_hWndColorDialog, SW_HIDE);
                s_hColorDialogHook = NULL;
                s_hWndColorDialog  = NULL;
            }
        }

        return lRet;
    }

    WTL::CBrush m_bkBrush;
    
    CDWImage m_imageClr;
};