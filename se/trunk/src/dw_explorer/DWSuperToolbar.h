
#pragma once

#include "DWComDef.h"
#include "DWToolbar.h"
#include "DWEventSvr.h"
#include "DWSearchMgt.h"
#include "DWFavIconMgt.h"
#include "DWMenu.h"
#include "DWEdit.h"

#include "DWSmartAddrMgt.h"

class CDWSuperToolbar : 
    public CDWToolbar, 
    public CDWEventCallback
{
public:

    void OpenURL( LPCTSTR URL )
    {
        if ( StrStrI( URL, L"." ) == NULL && 
             StrStrI( URL, L"://" ) == NULL &&
             StrStrI( URL, L":\\" ) == NULL )
        {
            OpenSerach(URL);
            return;
        }

        CDWEventSvr::Instance().OnMessage( edi_open_url, (WPARAM)URL, FALSE);
    }

    void OpenSerach( LPCTSTR pszKeyWord )
    {
        CDWSearchMgt* psrhMgt = CDWSearchMgt::InstancePtr();
        if ( psrhMgt == NULL )
            return;

        ATL::CString strURL = psrhMgt->GetSearchURL( pszKeyWord );

        CDWEventSvr::Instance().OnMessage( edi_open_url, (WPARAM)(LPCTSTR)strURL, FALSE);
    }

    void GoOpenURL()
    {
        int nTextLen = m_address_edit.GetWindowTextLength();
        if ( nTextLen <= 0)
            return;

        WCHAR* URL = new WCHAR[ nTextLen + 1];
        m_address_edit.GetWindowText( URL, nTextLen + 1);

        OpenURL( URL );

        delete []URL;
    }

    void GoSerach()
    {
        int nTextLen = m_search_edit.GetWindowTextLength();
        if ( nTextLen <= 0)
            return;

        WCHAR* URL = new WCHAR[ nTextLen + 1];
        m_search_edit.GetWindowText( URL, nTextLen + 1);

        m_search_edit.AddDropdownList( URL );

        OpenSerach( URL );

        delete []URL;
    }

    BOOL PreTranslateMessage(MSG* pMsg)
    {
        if ( pMsg->message == WM_KEYDOWN )
        {
            switch( pMsg->wParam )
            {
            case VK_F2:
                m_address_edit.SetFocus();
                break;
            case VK_F3:
                m_address_edit.SetFocus();
                m_address_edit.ShowDropdownList();
                break;
            case VK_F4:
                m_search_edit.SetFocus();
                break;
            }
        }

        return FALSE;
    }

    BEGIN_MSG_MAP(CDWMainFrame)

        MESSAGE_HANDLER( WM_CREATE    , OnCreate     )
        MESSAGE_HANDLER( WM_ERASEBKGND, OnEraseBkGnd )
        MESSAGE_HANDLER( WM_SIZE      , OnSize       )

        MESSAGE_HANDLER( WM_CTLCOLORBTN   , OnCtlColor )
        MESSAGE_HANDLER( WM_CTLCOLORSTATIC, OnCtlColor )

        MESSAGE_HANDLER( WM_CTLCOLOREDIT, OnCtlColor )
        MESSAGE_HANDLER( WM_CTLCOLORDLG , OnCtlColor )

        MESSAGE_HANDLER(WM_LBUTTONDOWN      , OnLButtonDown)
        MESSAGE_HANDLER(WM_FAV_ICON_REFLASH , OnFavIconReflash )

        MESSAGE_HANDLER(WM_MENUSELECT       , OnMenuSelect)

        MESSAGE_HANDLER(WM_EDIT_KEY_RETURN  , OnEditKeyReturn)

        COMMAND_CODE_HANDLER( EN_CHANGE  , OnEnChange )
        COMMAND_CODE_HANDLER( LBN_DBLCLK , OnLbnDBlick)

        CHAIN_MSG_MAP(CDWToolbar)

    END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        CDWSkinUIMgt* pskin = CDWSkinUIMgt::InstancePtr();
        if ( pskin == NULL )
            return 1L;

        m_address_edit.Create(m_hWnd, &rcDefault, NULL, 
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_WANTRETURN | ES_NOHIDESEL | WS_TABSTOP,
            0, ID_TOOL_ADDR_DROPDOWN);
        m_search_edit .Create(m_hWnd, &rcDefault, NULL, 
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_WANTRETURN | ES_NOHIDESEL | WS_TABSTOP,
            0, ID_TOOL_SERACH_DROPDOWN);

        m_address_edit.SetFont( pskin->fontDefault );
        m_search_edit.SetFont( pskin->fontDefault );

        m_search_edit.AddDropdownList(L"中国人");
        m_search_edit.AddDropdownList(L"中国人1");
        m_search_edit.AddDropdownList(L"中国人2");

        m_address_edit.AddDropdownList(L"http://www.sogou.com");
        m_address_edit.AddDropdownList(L"http://www.baidu.com");
        m_address_edit.AddDropdownList(L"http://www.google.com");
        m_address_edit.AddDropdownList(L"http://www.sina.com");
        m_address_edit.AddDropdownList(L"http://www.sohu.com");

        m_address_edit.m_strBkText = L"请在这里输入网址";
        m_search_edit.m_strBkText  = CDWSearchMgt::Instance().GetSearchName();

        m_icon_search = CDWFavIconMgt::Instance().GetFavIcon( 
            CDWSearchMgt::Instance().GetSearchURL(), m_hWnd, NULL );


        CDWEventSvr::Instance().AddCallback( this );

        AddToolBtn2( _T(""), ID_TOOL_ADDR_DROPDOWN  , &pskin->png_dropdown);
        AddToolBtn2( _T(""), ID_TOOL_SERACH_DROPDOWN, &pskin->png_dropdown);

        bHandled = FALSE;
        return 0L;
    }

    virtual void OnFinalMessage(HWND /*hWnd*/)
    {
        m_vtToolBtn.RemoveAll();
    }

    virtual void RePositionBtns()
    {
        RECT rcEdit = { 0 };

        m_address_edit.GetWindowRect(&rcEdit);
        ScreenToClient(&rcEdit);
        rcEdit.left  = rcEdit.right+2;
        rcEdit.right = rcEdit.left + 11;

        m_vtToolBtn[0].rcBtn = rcEdit;

        m_search_edit.GetWindowRect(&rcEdit);
        ScreenToClient(&rcEdit);
        rcEdit.left  = rcEdit.right+2;
        rcEdit.right = rcEdit.left + 11;

        m_vtToolBtn[1].rcBtn = rcEdit;
    }

    virtual void DrawToolBtn( HDC hDC, ToolBtnInfo& info, int nIdex )
    {
        CDWSkinUIMgt* pskin = CDWSkinUIMgt::InstancePtr();
        if ( pskin == NULL )
            return;

        ATLASSERT(info.image);
        if ( info.image == NULL )
            return;

        RECT rcSrcImage = { 0 };

        rcSrcImage.left   = info.image->GetWidth() / 4;
        rcSrcImage.bottom = info.image->GetHeight();

        rcSrcImage.right = rcSrcImage.left * (nIdex + 1);
        rcSrcImage.left  = rcSrcImage.left * nIdex;

        if ( info.uID == ID_TOOL_ADDR_DROPDOWN && IsHttps() )
            CDCHandle(hDC).FillRect(&info.rcBtn, m_bkSBrush);
        else
            CDCHandle(hDC).FillRect(&info.rcBtn, m_bkBrush);


        info.image->AlphaDraw( hDC, info.rcBtn.left, info.rcBtn.top, &rcSrcImage );
    }


    LRESULT OnEnChange(WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/)
    {
        Invalidate();

        ::InvalidateRect(hWndCtl, NULL, TRUE);

        if ( wID == ID_TOOL_ADDR_DROPDOWN && GetFocus() == m_address_edit )
        {
            int nTextLen = m_address_edit.GetWindowTextLength();

            WCHAR* pszIText = new WCHAR[ nTextLen + 1];
            m_address_edit.GetWindowText( pszIText, nTextLen + 1);


            CDWSmartAddrMgt& sa = CDWSmartAddrMgt::Instance();
            
            AddrDropItemList aList;
            sa.QueryAddrDropList(m_hWnd, pszIText, aList, 20 );
            m_address_edit.ClearDropdownList();
            for ( POSITION pos = aList.GetHeadPosition(); pos != NULL ; )
            {
                ADDRDROPLISTITEM& item = aList.GetNext(pos);
                
                m_address_edit.AddDropdownList( item.strURL, item.strTitle );
            }

            if ( m_address_edit.m_vtDropList.GetCount() > 0 || 1 )
                m_address_edit.ShowDropdownList();

            delete []pszIText;
        }
        
        return 0;
    }

    LRESULT OnLbnDBlick(WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/)
    {
        if ( wID == ID_TOOL_ADDR_DROPDOWN )
        {
            GoOpenURL();
        }
        else if ( wID == ID_TOOL_SERACH_DROPDOWN )
        {
            GoSerach();
        }

        return 0L;
    }

    LRESULT OnMenuSelect(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        TCHAR szBuffer[MAX_PATH] = { 0 };

        WORD wFlags = HIWORD(wParam);
        WORD wID    = LOWORD(wParam);

        if( (wFlags == 0xFFFF && lParam == NULL) || wID < 10 )   // menu closing
        {
            CDWEventSvr::Instance().OnMessage( edi_status_bar );
        }
        else if ( !(wFlags & MF_POPUP) )
        {
            CMenuHandle menu = HMENU(lParam);

            CDWEventSvr::Instance().OnMessage( edi_status_bar, 
                (WPARAM)(LPCTSTR)CDWSearchMgt::Instance().GetSearchName(wID-10) );
        }

        return 1;
    }

    LRESULT OnEditKeyReturn(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if ( wParam == ID_TOOL_ADDR_DROPDOWN )
        {
            GoOpenURL();
        }
        else if ( wParam == ID_TOOL_SERACH_DROPDOWN )
        {
            GoSerach();
        }

        return 0L;
    }

    
    virtual void DoBeforePaint( HDC hDC, const RECT& rcClient ) 
    {
        CDWSkinUIMgt* pskin = CDWSkinUIMgt::InstancePtr();
        if ( pskin == NULL )
            return;

        CDCHandle dc(hDC);

        RECT rcEdit = { 0 };

        CreateBkBrush();

        dc.FillSolidRect( &rcClient, pskin->clrFrameWindow );

        if ( m_icon_addr.IsNull() )
            m_icon_addr = pskin->iconNull;
        if ( m_icon_search.IsNull() )
            m_icon_search = pskin->iconNull;

        COLORREF clrBorder = HLS_TRANSFORM(pskin->clrFrameWindow, 60, 0);

        CPen pen;
        pen.CreatePen( PS_SOLID, 1, clrBorder );

        HPEN   hOldPen   = dc.SelectPen( pen );
        HBRUSH hOldBrush = dc.SelectBrush( IsHttps() ? m_bkSBrush : m_bkBrush );

        m_address_edit.GetWindowRect(&rcEdit);
        ScreenToClient(&rcEdit);
        InflateRect(&rcEdit, 2, 2);
        rcEdit.top   -= 2;
        rcEdit.left  -= 18;
        rcEdit.right += 12;


        RECT rcImage = { 0, 0, 0, 0 };
        rcImage.right  = pskin->png_dropdown.GetWidth() / 4;
        rcImage.bottom = pskin->png_dropdown.GetHeight();

        POINT pt = { 3, 3 };

        dc.RoundRect(&rcEdit, pt);
        m_icon_addr.DrawIconEx( dc, rcEdit.left + 4, rcEdit.top + 3, 16, 16 );

        dc.SelectBrush( m_bkBrush );
        m_search_edit.GetWindowRect(&rcEdit);
        ScreenToClient(&rcEdit);
        InflateRect(&rcEdit, 2, 2);
        rcEdit.top   -= 2;
        rcEdit.left  -= 23;
        rcEdit.right += 12;

        dc.RoundRect(&rcEdit, pt);
        m_icon_search.DrawIconEx( dc, rcEdit.left + 4, rcEdit.top + 3, 16, 16 );

        pskin->png_dropdown.AlphaDraw( dc, rcEdit.left + 18, rcEdit.top + 10, &rcImage, 0, 1.0f );

        dc.SelectPen(hOldPen);
        dc.SelectBrush(hOldBrush);
    }


    LRESULT OnEraseBkGnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return 1L;
    }

    LRESULT OnFavIconReflash(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        m_icon_search = (HICON)wParam;
        Invalidate();

        return 1L;
    }

    template <bool t_bManaged>
    class CDWSprMenuT : public CDWMenuT<t_bManaged>
    {
    public:
        CDWSprMenuT(HMENU hMenu = NULL) : CDWMenuT(hMenu)
        { }

        virtual void _OnDrawMenuIcon( CDCHandle& dc, LPDRAWITEMSTRUCT lpDrawItem, int nSelected )
        {
            CDWSkinUIMgt* pskin = CDWSkinUIMgt::InstancePtr();
            if ( pskin == NULL )
                return;

            CIconHandle icon;
            
            icon.m_hIcon = CDWFavIconMgt::Instance().GetFavIcon( 
                CDWSearchMgt::Instance().GetSearchURL( lpDrawItem->itemID - 10 ), NULL, NULL, FALSE);

            if ( icon.m_hIcon == NULL )
                icon.m_hIcon = pskin->iconNull;

            icon.DrawIconEx( dc,
                lpDrawItem->rcItem.left + 5,
                lpDrawItem->rcItem.top  + 2,
                16, 16);
        }
    };

    LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CDWToolbar::OnLButtonDown(uMsg, wParam, lParam, bHandled);
        
        if ( m_nClickIndex >= 0 )
        {
            switch ( m_vtToolBtn[m_nClickIndex].uID )
            {
            case ID_TOOL_ADDR_DROPDOWN:
                if ( m_address_edit.IsDropdownList() )
                    m_address_edit.HideDropdownList();
                else
                    m_address_edit.ShowDropdownList();
                break;
            case ID_TOOL_SERACH_DROPDOWN:
                if ( m_search_edit.IsDropdownList() )
                    m_search_edit.HideDropdownList();
                else
                    m_search_edit.ShowDropdownList();
                break;
            }
        }

        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

        RECT rcSearchIcon = { 0 };

        m_search_edit.GetWindowRect(&rcSearchIcon);
        ScreenToClient(&rcSearchIcon);
        rcSearchIcon.top  -= 2;
        rcSearchIcon.left -= 22;
        rcSearchIcon.right = rcSearchIcon.left + 26;
        

        if ( ::PtInRect(&rcSearchIcon, pt ) )
        {
            CDWSearchMgt& smgt = CDWSearchMgt::Instance();
            
            CDWSprMenuT<TRUE> menu;
            
            menu.Attach( smgt.CreatePopueMenu() );

            ClientToScreen(&pt);

            int nCmdId = menu.DWTrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD,
                pt.x, pt.y,
                m_hWnd);
            if ( nCmdId >= 10 )
            {
                smgt.m_nSelIndex = nCmdId - 10;
                m_search_edit.m_strBkText  = smgt.GetSearchName();

                m_icon_search = CDWFavIconMgt::Instance().GetFavIcon( smgt.GetSearchURL(), m_hWnd, NULL );
                m_search_edit.Invalidate();
                Invalidate();

                if ( m_search_edit.GetWindowTextLength() > 0 )
                {
                    GoSerach();
                }
            }
        }

        return 1L;
    }

    BOOL IsHttps()
    {
        TCHAR szText[10] = { 0 };

        m_address_edit.GetWindowText(szText, 9);

        return !StrCmpNI(szText, L"https://", 8 );
    }

    void CreateBkBrush()
    {
        CDWSkinUIMgt* pskin = CDWSkinUIMgt::InstancePtr();
        if ( pskin == NULL )
            return;

        if ( m_bkBrush.m_hBrush == NULL )
            m_bkBrush.CreateSolidBrush( HLS_TRANSFORM(pskin->clrFrameWindow, 20, 0) );
        
        if ( m_bkSBrush.m_hBrush == NULL )
        {
            COLORREF clrOld = HLS_TRANSFORM(pskin->clrFrameWindow, 40, 0 );

            BYTE r,g,b;
            
            r = GetRValue(clrOld);
            g = GetGValue(clrOld);
            b = GetBValue(clrOld);
            
            r += 60;
            r = r < 60 ? 255 : r;

            m_bkSBrush.CreateSolidBrush( RGB(r,g,b) );
        }
    }

    LRESULT OnCtlColor(UINT /* uMsg */, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
    {
        CDCHandle dc = (HDC)wParam;
        dc.SetBkMode( TRANSPARENT );
        
        CreateBkBrush();

        if ( (HWND)lParam == m_address_edit.m_hWnd && IsHttps() )
            return (LRESULT)m_bkSBrush.m_hBrush;

        return (LRESULT)m_bkBrush.m_hBrush;
    }


    LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;

        RECT rcClient = { 0 };
        GetClientRect(&rcClient);
        
        ::InflateRect(&rcClient, -2, -2);
        rcClient.top+=3;
        rcClient.right -= 5 ;

        RECT rcAddr   = rcClient;
        RECT rcSerach = rcClient;

        rcSerach.left = ( rcClient.right / 4 );
        if ( rcSerach.left > 220 )
            rcSerach.left = 220;
        else if ( rcSerach.left < 100 )
            rcSerach.left = 100;
        
        rcSerach.left = rcSerach.right - rcSerach.left;        

        rcAddr.right = rcSerach.left - 8;

        rcAddr.left += 22;
        rcAddr.right -= 12;

        rcSerach.left += 22;
        rcSerach.right -= 12;

        
        m_address_edit.MoveWindow(&rcAddr);
        m_search_edit .MoveWindow(&rcSerach);

        RePositionBtns();

        return 1L;
    }

    LRESULT OnEventMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
    {
        if ( uMsg == eid_addr_changed )
        {
            if ( StrCmpI((LPCTSTR)wParam, BLANK_URL) )
                m_address_edit.AddDropdownList( (LPCTSTR)wParam );
            m_address_edit.SetWindowText((LPCTSTR)wParam);
        }
        else if ( uMsg == edi_skin_changed )
        {
            if (!m_bkBrush.IsNull())
                m_bkBrush.DeleteObject();
            if (!m_bkSBrush.IsNull())
                m_bkSBrush.DeleteObject();
        }
        else if ( uMsg == edi_spr_icon_changed )
        {
            if ( wParam != NULL )
                m_icon_addr = (HICON)wParam;
            if ( lParam != NULL )
                m_icon_search = (HICON)lParam;

            if ( wParam != NULL || lParam != NULL )
                Invalidate();
        }
        else if ( uMsg == edi_addr_setfocus )
        {
            m_address_edit.SetFocus();
        }

        return 0;
    }

    DECLARE_WND_CLASS(_T("DWExplorer_DWSuperToolbar"));

public:

    WTL::CBrush m_bkBrush;
    WTL::CBrush m_bkSBrush;

    CDWEdit m_address_edit;
    CDWEdit m_search_edit;

    CIconHandle m_icon_addr;
    CIconHandle m_icon_search;
};