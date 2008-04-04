/********************************************************************
* CreatedOn: 2008-4-4   12:25
* FileName: SkinDialgPreviewWindow.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/
#pragma once

#include <skindialog.h>

#define WM_SELECTCHILDWINDOW (WM_USER + 0x222)

class SkinHookMouse
{
public:
    SkinHookMouse()
    {
        m_hHookMouse  = NULL;
        m_hHookWindow = NULL;
    }

    BOOL InitHookMouse()
    {
        BOOL bResult = TRUE;

        if (m_hHookMouse == NULL)
        {
            m_hHookMouse = SetWindowsHookEx( WH_MOUSE, 
                HookMouse, 
                0, 
                GetCurrentThreadId());

            if (m_hHookMouse == NULL)
                bResult = FALSE;
        }

        return bResult;
    }

    BOOL UnitHookMouse()
    {
        BOOL bResult = TRUE;

        if ( m_hHookMouse != NULL )
        {
            UnhookWindowsHookEx( m_hHookMouse );
            m_hHookMouse = NULL;
        }

        return bResult;
    }

    static LRESULT CALLBACK HookMouse( int iCode, WPARAM wParam, LPARAM lParam )
    {
        LRESULT lResult = CallNextHookEx( instance().m_hHookMouse, iCode, wParam, lParam );

        if (wParam == WM_LBUTTONDOWN)
        {
            LPMOUSEHOOKSTRUCT lpMouseInfo = (LPMOUSEHOOKSTRUCT)lParam;
            
            if (instance().m_hHookWindow == NULL || !::IsWindow(instance().m_hHookWindow))
                return lResult;

            CWindow wndHook = instance().m_hHookWindow;

            CRect rcWindow;

            wndHook.GetWindowRect(&rcWindow);

            if (rcWindow.PtInRect(lpMouseInfo->pt))
            {
                wndHook.SendMessage(WM_LBUTTONDOWN);
            }
        }
        
        return lResult;
    }

    static SkinHookMouse& instance()
    {
        static SkinHookMouse gs_instance;
        return gs_instance;
    }

public:
    HHOOK m_hHookMouse;
    HWND  m_hHookWindow;
};

class SkinSelectFlagWindow : 
    public CWindowImpl<SkinSelectFlagWindow, SkinWindow, CControlWinTraits>
{
public:
    DECLARE_WND_CLASS(_T("Skin_SkinSelectFlagWindow"))

    enum
    {
        line_space = 2,
        line_color = 0x0000FF00
    };

    BEGIN_MSG_MAP(SkinSelectFlagWindow)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
    END_MSG_MAP()

    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {
        CPaintDC dc(m_hWnd);

        CSkinDCHandle skinDC(dc.m_hDC);

        RECT rcClient;
        RECT rcWindow;
        GetClientRect(&rcClient);
        GetWindowRect(&rcWindow);

        skinDC.SkinDrawBorder(rcClient, line_color, PS_SOLID, line_space );

        //CClientDC destdc(::GetDesktopWindow());
        //
        //dc.BitBlt( 0, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
        //    destdc,
        //    rcWindow.left, rcWindow.top, 
        //    SRCCOPY);

        
        return TRUE;
    }

    void RedrawFlag()
    {
        CRgnHandle newRgn;
        CRgnHandle rgnTemp;


        RECT rcClient = { 0 };
        RECT rcTemp   = { 0 };
        GetClientRect(&rcClient);

        newRgn.CreateRectRgnIndirect(&rcClient);
        
        rcTemp = rcClient;

        rcTemp.left += line_space;
        rcTemp.top  += line_space;
        rcTemp.right  -= line_space;
        rcTemp.bottom -= line_space;

        rgnTemp.CreateRectRgnIndirect(&rcTemp);
        newRgn.CombineRgn(rgnTemp, RGN_DIFF);
        rgnTemp.DeleteObject();


        int nwidth = rcClient.right  - rcClient.left;
        int nhigth = rcClient.bottom - rcClient.top;
        
        int nlinew = (nwidth > nhigth) ? nhigth / 4 : nwidth / 4;

        rcTemp = rcClient;

        rcTemp.left += nlinew;
        rcTemp.right  -= nlinew;

        rgnTemp.CreateRectRgnIndirect(&rcTemp);
        newRgn.CombineRgn(rgnTemp, RGN_DIFF);
        rgnTemp.DeleteObject();

        rcTemp = rcClient;

        rcTemp.top  += nlinew;
        rcTemp.bottom -= nlinew;

        rgnTemp.CreateRectRgnIndirect(&rcTemp);
        newRgn.CombineRgn(rgnTemp, RGN_DIFF);
        rgnTemp.DeleteObject();

        SetWindowRgn(newRgn, TRUE);

        if (m_rgn.m_hRgn != NULL)
        {
            m_rgn.DeleteObject();            
        }

        m_rgn.Attach(newRgn);

    }

    CRgn m_rgn;
};

class SkinDialgPreviewWindow : 
    public KSG::SkinDialogImpl<SkinDialgPreviewWindow>
{
public:
    
    BEGIN_MSG_MAP(SkinDialgPreviewWindow)
        //MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
    END_MSG_MAP()

    LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {
        LRESULT lResult = DefWindowProc();
        
        POINT pt = { 0 };

        ::GetCursorPos(&pt);

        ScreenToClient(&pt);

        HWND hWndFromPt = ::ChildWindowFromPoint(m_hWnd, pt);

        if (hWndFromPt == NULL || hWndFromPt == m_hWnd)
        {
            ::SendMessage(::GetParent(m_hWndParent), WM_SELECTCHILDWINDOW, 0, 0);
        }
        else
        {
            std::map<KSG::CString, SkinWindow*>::iterator iter = 
                m_childList.m_mapIDS2Win.begin();

            KSG::SkinWindow* pSkinWindow = NULL;

            for ( ; iter != m_childList.m_mapIDS2Win.end(); iter++)
            {
                pSkinWindow = iter->second;

                if (pSkinWindow == NULL)
                    continue;

                if (pSkinWindow->m_hWnd == hWndFromPt)
                {
                    ::SendMessage(::GetParent(m_hWndParent), 
                        WM_SELECTCHILDWINDOW, (WPARAM)(LPCTSTR)iter->first, 0);

                    return DefWindowProc();
                }
            }

            ::SendMessage(::GetParent(m_hWndParent), WM_SELECTCHILDWINDOW, 0, 0);
        }

        return DefWindowProc();
    }

    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {
        CPaintDC dc(m_hWnd);

        CSkinDCHandle skinDC(dc.m_hDC);

        SkinWindow* pSelWindow = m_childList.GetDlgItem(m_strSelIdName);

        if (pSelWindow != NULL)
        {
            RECT rcBox = { 0 };

            pSelWindow->GetWindowRect(&rcBox);
            ScreenToClient(&rcBox);

            const int nspace = 3;
            COLORREF clrLine = 0x0000ff00;

            rcBox.left -= 3;
            rcBox.top  -= 3;

            rcBox.right  += 3;
            rcBox.bottom += 3;

            skinDC.SkinDrawBorder(rcBox, clrLine, PS_SOLID, nspace);
        }

        return TRUE;
    }


    DWORD GetStyle( DWORD dwStyle )
    {
        return WS_CHILD | WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_DISABLED | WS_CLIPSIBLINGS | WS_CLIPCHILDREN ;
    }


    DWORD GetExStyle( DWORD dwExtyle )
    {
        return WS_EX_CONTROLPARENT;
    }


    HWND ReCreatePreviewWindow(HWND hWndParent,
        SkinDialogRes& xmlDlgRes)
    {
        m_hWndParent = hWndParent;

        if (m_wndSelectFlag.m_hWnd != NULL)
            m_wndSelectFlag.DestroyWindow();

        if (m_hWnd != NULL)
            DestroyWindow();
        
        SkinXmlDocument doc;

        if (!doc.LoadXML(_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?> <KSG />")))
            return NULL;

        xmlDlgRes.SaveResDoc(doc.RootElement());

        m_xmlDlgElement.CopyFrom(doc.RootElement().FirstChildElement());

        Create(hWndParent);

        ShowWindow(SW_SHOWNOACTIVATE);


        m_wndSelectFlag.Create(m_hWnd, rcDefault, NULL, WS_VISIBLE | WS_POPUP);

        return m_hWnd;
    }

    BOOL AddSkinWindow( SkinWndPropertyList& WndProperty )
    {
        SkinXmlDocument doc;

        if (!doc.LoadXML(_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?> <KSG />")))
            return NULL;

        WndProperty.SaveResDoc(doc.RootElement());

        skinxmlwin winxml;

        winxml = doc.RootElement().FirstChildElement();

        m_childList.AddChildWindow(winxml);

        return TRUE;
    }
    BOOL DelSkinWindow( SkinWndPropertyList& WndProperty )
    {
        m_childList.DestroyChildWindow(WndProperty.m_strIdName);

        Invalidate();

        return TRUE;
    }
    BOOL UpdateSkinWindow( SkinWndPropertyList& WndProperty )
    {
        DelSkinWindow(WndProperty);
        AddSkinWindow(WndProperty);

        UpdateSelectFlag();

        return TRUE;
    }

    void UpdateSelectFlag()
    {
        SkinWindow* pSelWindow = m_childList.GetDlgItem(m_strSelIdName);

        if (pSelWindow != NULL)
        {
            RECT rcBox = { 0 };

            pSelWindow->GetWindowRect(&rcBox);
            //ScreenToClient(&rcBox);

            rcBox.left -= SkinSelectFlagWindow::line_space/2;
            rcBox.top  -= SkinSelectFlagWindow::line_space/2;

            rcBox.right  += SkinSelectFlagWindow::line_space/2;
            rcBox.bottom += SkinSelectFlagWindow::line_space/2;

            ;

            m_wndSelectFlag.MoveWindow(&rcBox);
            m_wndSelectFlag.RedrawFlag();
            m_wndSelectFlag.ShowWindow(SW_SHOWNOACTIVATE);

        }
        else
            m_wndSelectFlag.ShowWindow(SW_HIDE);

    }

    BOOL SelectWindow( SkinWndPropertyList& WndProperty )
    {
        m_strSelIdName = WndProperty.GetIdName();
        
        UpdateSelectFlag();

        return TRUE;
    }
    BOOL ClearSelectWindow( )
    {
        m_strSelIdName = _T("");

        m_wndSelectFlag.ShowWindow(SW_HIDE);

        //Invalidate();

        return TRUE;
    }

public:



public:

    KSG::CString m_strSelIdName;

    HWND m_hWndParent;

    SkinSelectFlagWindow m_wndSelectFlag;
};

