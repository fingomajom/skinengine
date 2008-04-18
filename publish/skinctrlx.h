/********************************************************************
* CreatedOn: 2008-4-3   11:44
* FileName: skinctrlx.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include <skingdi.h>
#include <skinwin.h>
#include <atlctrlx.h>
#include <skinxmlctrlx.h>


namespace KSGUI{

//class CSkinStaticEx;

//////////////////////////////////////////////////////////////////////////

enum {
    KTLS_NORMAL     = 0,
    KTLS_USEPARENTBKBRUSH = 1,
};

template <class T, class TBase = KSGUI::SkinWindow, class TWinTraits = ATL::CControlWinTraits>
class  __declspec(novtable) CSkinStaticExT : 
    public SkinWindowImpl< T, TBase, TWinTraits >
{

public:

    int          m_nPaintStyle;
    COLORREF     m_clrBkGnd;
    COLORREF     m_clrTextColor;
    CFont        m_hTextFont;   
    KSGUI::CString m_strCaption;
    DWORD        m_uDrawTextFlags;

public:
    CSkinStaticExT()
    {
        m_clrBkGnd     = 0;
        m_clrTextColor = 0;

        m_uDrawTextFlags = DT_LEFT | DT_TOP;
        m_nPaintStyle    = KTLS_NORMAL;
    }

    ~CSkinStaticExT()
    {
    }

    //////////////////////////////////////////////////////////////////////////

    BOOL SubclassWindow(HWND hWnd)
    {
        ATLASSERT(m_hWnd == NULL);
        ATLASSERT(::IsWindow(hWnd));

        BOOL bRet = ATL::CWindowImpl< T, TBase, TWinTraits>::SubclassWindow(hWnd);
        if(bRet)
        {
            T* pT = static_cast<T*>(this);
            pT->_Init();
        }
        return bRet;
    }

    //////////////////////////////////////////////////////////////////////////

    BEGIN_MSG_MAP(CSkinStaticExT)
        MESSAGE_HANDLER(WM_CREATE , OnCreate)
        MESSAGE_HANDLER(WM_COMMAND, OnCommand)
        MESSAGE_HANDLER(WM_SETTEXT, OnSetText)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBKGnd)

        MESSAGE_HANDLER(WM_PAINT  , OnPaint)

    END_MSG_MAP()

    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;
        _Init();
        return 1L;
    }


    LRESULT OnEraseBKGnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 1L;
    }


    LRESULT OnSetText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        m_strCaption = (LPCTSTR)lParam;

        InvalidateParentClient();

        return DefWindowProc();
    }

    LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return SendMessage(GetParent(), uMsg, wParam, lParam);
    }

    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CPaintDC dc(m_hWnd);

        T* pT = static_cast<T*>(this);

        pT->DoPaint((HDC)dc);

        return 0;
    }

    void DoPaint(CDCHandle dc)
    {
        RECT rcClient = {0};
        GetClientRect(&rcClient);


        switch(m_nPaintStyle)
        {
        case KTLS_USEPARENTBKBRUSH:
            {
                //CBrushHandle hBrush = (HBRUSH)::SendMessage(GetParent(), WM_CTLCOLORSTATIC, (WPARAM)dc.m_hDC, (LPARAM)m_hWnd);
                //dc.FillRect(&rcClient, hBrush);
            }

            break;
        case KTLS_NORMAL:
        default:
            {
                CBrush brush;
                brush.CreateSolidBrush(m_clrBkGnd);

                dc.FillRect(&rcClient, brush);
            }
            break;
        }

        //rcClient.left++;
        //rcClient.top++;
        rcClient.right--;
        rcClient.bottom--;

        int   nOldBkMode = dc.SetBkMode(TRANSPARENT);
        HFONT hOldFont   = dc.SelectFont(m_hTextFont.m_hFont == NULL ? 
            GetParent().GetFont() : m_hTextFont );

        COLORREF clrOldColor = dc.SetTextColor(m_clrTextColor);

        dc.DrawText(m_strCaption, -1, &rcClient, m_uDrawTextFlags);

        dc.SelectFont(hOldFont);
        dc.SetBkMode(nOldBkMode);
        dc.SetTextColor(clrOldColor);
    }

    void _Init()
    {
        int nRet = 0;

        HDC hDC = GetDC();
        CBrushHandle hBrush = (HBRUSH)::SendMessage(GetParent(), WM_CTLCOLORSTATIC, (WPARAM)hDC, (LPARAM)m_hWnd);
        LOGBRUSH logBrush = {0};

        nRet = hBrush.GetLogBrush(&logBrush);
        if (nRet != 0)
        {
            m_clrBkGnd = logBrush.lbColor;
        }
        else
        {
            m_clrBkGnd = GetSysColor(COLOR_BTNFACE);
        }   
        ReleaseDC(hDC);

        m_clrTextColor = GetSysColor(COLOR_WINDOWTEXT);

        TCHAR pszText[1024] = {0};
        GetWindowText(pszText, 1023);

        m_strCaption = pszText;

        CWindow::ModifyStyleEx(0, WS_EX_TRANSPARENT);
    }

    public:

    HWND SkinCreate( 
        const SkinXmlElement& xmlElement,
        HWND hWndParent, _U_MENUorID MenuOrID = 0U ) throw()
    {
        HWND hWndResult = SkinWindowImpl< T, TBase, TWinTraits >::
            SkinCreate(xmlElement, hWndParent, MenuOrID);

        if (hWndResult == NULL)
            return hWndResult;

        skinxmlstaticex xmlWin(xmlElement);
        
        xmlWin.GetFont(m_hTextFont.m_hFont);

        xmlWin.GetPaintStyle(m_nPaintStyle);
        xmlWin.GetDrawTextFlags(m_uDrawTextFlags);

        xmlWin.GetTextColor(m_clrTextColor);
        xmlWin.GetBkColor(m_clrBkGnd);

        return hWndResult;
    }

};

class CSkinStaticEx : public CSkinStaticExT<CSkinStaticEx>
{
public:
    DECLARE_WND_CLASS(_T("KIS_SkinStaticEx"))


    static SkinWindow* SkinCreate_Static(
        const SkinXmlElement& xmlElement,
        HWND hWndParent, _U_MENUorID MenuOrID = 0U )
    {
        CSkinStaticEx * pSkinWindow = new CSkinStaticEx;

        if (pSkinWindow == NULL)
            return pSkinWindow;


        if (pSkinWindow->SkinCreate(xmlElement, 
            hWndParent, 
            MenuOrID) == NULL)
        {
            delete pSkinWindow;

            pSkinWindow = NULL;
        }

        return pSkinWindow;
    }
};


class CSkinBitmapButton : public CBitmapButtonImpl<CSkinBitmapButton, SkinWindow>
{
public:

    DECLARE_WND_SUPERCLASS(_T("KSG_SkinBitmapButton"), GetWndClassName())

    CSkinBitmapButton(DWORD dwExtendedStyle = BMPBTN_AUTOSIZE | BMPBTN_HOVER | BMPBTN_SHAREIMAGELISTS, HIMAGELIST hImageList = NULL) : 
        CBitmapButtonImpl<CSkinBitmapButton, SkinWindow>(dwExtendedStyle, hImageList)
    { 
    }

    BOOL LoadBitmap(LPCTSTR pszBmpResName, UINT uBmpCount = 3)
    {
        if (m_imagelist.m_hImageList != NULL)
            m_imagelist.Destroy();

        CBitmap bmp;

        bmp.Attach( SkinLoadBitmap(pszBmpResName) );
        if (bmp.m_hBitmap == NULL)
            return FALSE;

        BITMAP bm = { 0 };

        if(!bmp.GetBitmap(&bm))
            return FALSE;

        m_imagelist.Create(bm.bmWidth / uBmpCount, bm.bmHeight, ILC_COLOR32, 3, 1);
        m_imagelist.Add(bmp);

        SetImageList(m_imagelist);

        SetImages( 0, 2, 1, uBmpCount == 4 ? 3 : -1);

        return TRUE;
    }

    HWND SkinCreate( 
        const SkinXmlElement& xmlElement,
        HWND hWndParent, _U_MENUorID MenuOrID = 0U ) throw()
    {
        BOOL result;

        ATLASSUME(m_hWnd == NULL);

        // Allocate the thunk structure here, where we can fail gracefully.
        result = m_thunk.Init(NULL, NULL);
        if (result == FALSE) {
            SetLastError(ERROR_OUTOFMEMORY);
            return NULL;
        }

        ATOM atom = GetWndClassInfo().Register(&m_pfnSuperWindowProc);

        _AtlWinModule.AddCreateWndData(&m_thunk.cd, this);

        HWND hWndResult = SkinWindow::SkinCreate(xmlElement, 
            hWndParent,              
            GetWndClassInfo().m_wc.lpszClassName,
            MenuOrID);

        skinxmlbmpbtn xmlWin(xmlElement);
        
        KSGUI::CString strBmpResName;
        int nBmpResCount = 3;
        
        xmlWin.GetBmpResName(strBmpResName);
        xmlWin.GetBmpCount(nBmpResCount);

        if (!LoadBitmap( strBmpResName, nBmpResCount ))
        {
            DestroyWindow();

            ATLASSERT(FALSE); // Load BmpRes Failed;

            hWndResult = NULL;
        }

        return hWndResult;
    }

    static SkinWindow* SkinCreate_Static(
        const SkinXmlElement& xmlElement,
        HWND hWndParent, _U_MENUorID MenuOrID = 0U )
    {
        CSkinBitmapButton * pSkinWindow = new CSkinBitmapButton;

        if (pSkinWindow == NULL)
            return pSkinWindow;


        if (pSkinWindow->SkinCreate(xmlElement, 
            hWndParent, 
            MenuOrID) == NULL)
        {
            delete pSkinWindow;

            pSkinWindow = NULL;
        }

        return pSkinWindow;
    }

private:

    CImageList m_imagelist;
};


class SkinHyperLink : public CHyperLinkImpl<SkinHyperLink, SkinWindow>
{
    typedef CHyperLinkImpl<SkinHyperLink, SkinWindow> theBase;

public:
    DECLARE_WND_CLASS(_T("Skin_HyperLink"))

    CFont m_NormalFont;

    BEGIN_MSG_MAP(CSkinCheckboxButton)

        MESSAGE_HANDLER(WM_LBUTTONDOWN    , OnRefWindowMsg)
        MESSAGE_HANDLER(WM_LBUTTONUP      , OnRefWindowMsg)
        MESSAGE_HANDLER(WM_NCLBUTTONDBLCLK, OnRefWindowMsg)
        MESSAGE_HANDLER(WM_KEYDOWN        , OnRefWindowMsg)
        MESSAGE_HANDLER(WM_KEYUP          , OnRefWindowMsg)
        MESSAGE_HANDLER(WM_SETFOCUS       , OnRefWindowMsg)
        MESSAGE_HANDLER(WM_KILLFOCUS      , OnRefWindowMsg)
        MESSAGE_HANDLER(WM_MOUSEMOVE      , OnRefWindowMsg)
        MESSAGE_HANDLER(WM_MOUSELEAVE     , OnRefWindowMsg)

        CHAIN_MSG_MAP(theBase)

    END_MSG_MAP()

    LRESULT OnRefWindowMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {        
        LRESULT lResult = DefWindowProc();
        bHandled = FALSE;

        if ( uMsg == WM_MOUSEMOVE )
        {
            bool bHover = m_bHover;

            theBase::ProcessWindowMessage( m_hWnd, uMsg, wParam, lParam, lResult, 0 );
            bHandled = TRUE;

            if ( bHover != m_bHover )
                InvalidateParentClient();

            return lResult;
        }
        else if ( uMsg == WM_MOUSELEAVE )
        {
            theBase::ProcessWindowMessage( m_hWnd, uMsg, wParam, lParam, lResult, 0 );
            bHandled = TRUE;
        }

        InvalidateParentClient();


        return lResult;        
    }

    void DoEraseBackground(CDCHandle dc)
    {
    }



    HWND SkinCreate( 
        const SkinXmlElement& xmlElement,
        HWND hWndParent, _U_MENUorID MenuOrID = 0U ) throw()
    {
        BOOL result;

        ATLASSUME(m_hWnd == NULL);

        // Allocate the thunk structure here, where we can fail gracefully.
        result = m_thunk.Init(NULL, NULL);
        if (result == FALSE) {
            SetLastError(ERROR_OUTOFMEMORY);
            return NULL;
        }

        ATOM atom = GetWndClassInfo().Register(&m_pfnSuperWindowProc);

        _AtlWinModule.AddCreateWndData(&m_thunk.cd, this);

        HWND hWndResult = SkinWindow::SkinCreate(xmlElement, 
            hWndParent,              
            GetWndClassInfo().m_wc.lpszClassName,
            MenuOrID);

        SetHyperLinkExtendedStyle(HLINK_COMMANDBUTTON | HLINK_UNDERLINED | HLINK_UNDERLINEHOVER);
        
        skinxmlhyperlink xmlwin(xmlElement);
        

        HFONT hLinkFont = NULL;
        KSGUI::CString strTooltipText;

        xmlwin.GetFont( m_NormalFont.m_hFont );

        if (m_NormalFont.m_hFont != NULL)
            SetFont(m_NormalFont);

        xmlwin.GetLinkFont( hLinkFont );
        if (hLinkFont != NULL)
            SetLinkFont(hLinkFont);

        xmlwin.GetToolTipText(strTooltipText);
        if (strTooltipText.GetLength() > 0)
            SetToolTipText(strTooltipText);

        CalcLabelRect();

        return hWndResult;
    }

    static SkinWindow* SkinCreate_Static(
        const SkinXmlElement& xmlElement,
        HWND hWndParent, _U_MENUorID MenuOrID = 0U )
    {
        SkinHyperLink * pSkinWindow = new SkinHyperLink;

        if (pSkinWindow == NULL)
            return pSkinWindow;


        if (pSkinWindow->SkinCreate(xmlElement, 
            hWndParent, 
            MenuOrID) == NULL)
        {
            delete pSkinWindow;

            pSkinWindow = NULL;
        }

        return pSkinWindow;
    }

};


template <class T, class TBase = KSGUI::SkinWindow, class TWinTraits = ATL::CControlWinTraits>
class  __declspec(novtable) SkinTableCtrlExImpl : 
    public SkinWindowImpl< T, TBase, TWinTraits >
{
public:

    class DrawItemCallBack
    {
    public:
        virtual void OnMeasureItem(HDC hdc, int nidx, RECT& rcItem, LPCTSTR pszItemText ) = 0;
        virtual void OnDrawItem(HDC hdc, int nidx, RECT& rcItem, LPCTSTR pszItemText ) = 0;
    };


    class DefDrawItemCallBack : public DrawItemCallBack
    {
    public:

        virtual void OnMeasureItem(HDC hdc, int nidx, RECT& rcItem, LPCTSTR pszItemText )
        {
            ::DrawText(hdc, pszItemText, -1, &rcItem, DT_CALCRECT);
        }
        virtual void OnDrawItem(HDC hdc, int nidx, RECT& rcItem, LPCTSTR pszItemText )
        {
            ::DrawText(hdc, pszItemText, -1, &rcItem, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
        }

    } m_defDrawItemCallBack;


    DrawItemCallBack* m_piDrawCallback;

    enum{
        em_left_space = 10,
        em_between_space = 5
    };

public:

    SkinTableCtrlExImpl(void) :
        m_piDrawCallback(&m_defDrawItemCallBack)
    {
        m_clrBkGnd = RGB(255 ,255, 255);
        m_clrLine  = RGB(138, 148, 190);
        m_clrText  = RGB(0, 0, 0);

        m_nselectedindex = -1;
        m_ntrackindex    = -1;
    }

    ~SkinTableCtrlExImpl(void)
    {
    }

    int InsertItem(int nPos, LPCTSTR pszCaption)
    {
        ATLASSERT(pszCaption != NULL);
        if (pszCaption == NULL)
            return -1;

        if (m_nselectedindex < 0)
            m_nselectedindex = 0;

        TableItemInfo tableItem;

        tableItem.strItemText = pszCaption;

        if (nPos <= 0)
        {
            m_vtItems.insert(m_vtItems.begin(), tableItem);

            return 0;
        }

        if (nPos >= (int)m_vtItems.size())
        {
            m_vtItems.insert(m_vtItems.end(), tableItem);

            return int(m_vtItems.size() - 1);
        }

        m_vtItems.insert(m_vtItems.begin() + nPos, tableItem);

        return nPos;
    }

    void CalcAllItemRect(HDC hdc)
    {
        int nleftpos = em_left_space;

        RECT rcTemp = { 0 };
        RECT rcClient;
        GetClientRect(&rcClient);

        for (int idx = 0; idx < int(m_vtItems.size()); idx++)
        {
            ATLASSERT(m_piDrawCallback != NULL);
            if (m_piDrawCallback == NULL)
                break;
            m_piDrawCallback->OnMeasureItem(hdc, idx, rcTemp, m_vtItems[idx].strItemText);

            m_vtItems[idx].rcItem.left   = nleftpos;
            m_vtItems[idx].rcItem.right  = nleftpos + rcTemp.right + em_between_space * 2;
            m_vtItems[idx].rcItem.top    = 0;
            m_vtItems[idx].rcItem.bottom = rcClient.bottom;

            nleftpos =  m_vtItems[idx].rcItem.right + em_between_space;
        }
    }

    void SetColors(COLORREF line, COLORREF bk)
    {
        m_clrBkGnd = bk;
        m_clrLine  = line;
    }

    //////////////////////////////////////////////////////////////////////////

    int GetCurFocus() const
    {
        return m_nselectedindex;
    }

    BEGIN_MSG_MAP(KanTableCtrlImpl)

        MESSAGE_HANDLER(WM_PAINT      , OnPaint)
        MESSAGE_HANDLER(WM_ERASEBKGND , OnEraseBKGnd)


        MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
        //MESSAGE_HANDLER(WM_MOUSEMOVE  , OnMouseMove  )
        //MESSAGE_HANDLER(WM_MOUSELEAVE , OnMouseLeave )

        MESSAGE_HANDLER(WM_KEYDOWN    , OnKeyDown    )

        MESSAGE_HANDLER(WM_GETDLGCODE , OnGetDlgCode )

        MESSAGE_HANDLER(WM_SETFOCUS   , OnSetFocus   )
        MESSAGE_HANDLER(WM_KILLFOCUS  , OnKillFocus  )

    END_MSG_MAP()

    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CPaintDC dc(m_hWnd);

        T* pT = static_cast<T*>(this);

        pT->DoPaint((HDC)dc);

        return 0;
    }

    LRESULT OnEraseBKGnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 1L;
    }


    LRESULT OnGetDlgCode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LRESULT lDlgCode = DLGC_WANTALLKEYS | DLGC_WANTARROWS;

        return lDlgCode;
    }

    LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LRESULT lResult = DefWindowProc(uMsg, wParam, lParam);

        Invalidate(FALSE);

        return lResult;
    }

    LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LRESULT lResult = DefWindowProc(uMsg, wParam, lParam);

        Invalidate(FALSE);

        return lResult;
    }

    LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LRESULT lResult = DefWindowProc(uMsg, wParam, lParam);

        Invalidate(FALSE);

        return lResult;
    }

    LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LRESULT lResult = DefWindowProc(uMsg, wParam, lParam);

        int nx = LOWORD(lParam);
        int ny = HIWORD(lParam);

        int nHovIndex = -1;

        for (int idx = 0; idx < int(m_vtItems.size()); idx++)
        {
            if (::PtInRect(&m_vtItems[idx].rcItem, CPoint(nx, ny)))
            {
                nHovIndex = idx;
                break;
            }
        }

        if (nHovIndex < 0)
            return lResult;

        m_nselectedindex = nHovIndex;

//        RedrawWindow();
        InvalidateParentClient();


        NMHDR nmhdr = {0};

        nmhdr.hwndFrom = m_hWnd;
        nmhdr.idFrom   = ::GetDlgCtrlID(m_hWnd);
        nmhdr.code     = TCN_SELCHANGE; 

        HWND hWnd = GetParent();

        ::SendMessage(hWnd, WM_NOTIFY, 
            (WPARAM)nmhdr.idFrom, (LPARAM)&nmhdr);

        return lResult;
    }

    LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LRESULT lResult = DefWindowProc(uMsg, wParam, lParam);

        int nx = LOWORD(lParam);
        int ny = HIWORD(lParam);

        int nHovIndex = -1;

        //if (ny < m_nheaderheight && nx < int(m_nheaderwidth * m_vtItems.size()))
        //{
        //    nHovIndex = nx / m_nheaderwidth;
        //}

        if (nHovIndex >= 0)
        {
            if (m_ntrackindex >= 0)
            {
                if (m_ntrackindex == nHovIndex)
                {
                }
                else
                {
                    m_ntrackindex = nHovIndex;
                    RedrawWindow();
                }
            }
            else
            {
                TRACKMOUSEEVENT tme;
                tme.cbSize = sizeof(tme);
                tme.hwndTrack = m_hWnd;
                tme.dwFlags = TME_LEAVE;
                tme.dwHoverTime = 1;
                _TrackMouseEvent(&tme);

                m_ntrackindex = nHovIndex;

                RedrawWindow();
            }
        }
        else
        {
            if (m_ntrackindex >= 0)
            {
                m_ntrackindex = -1;

                RedrawWindow();
            }
        }

        return lResult;
    }

    LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LRESULT lResult = DefWindowProc(uMsg, wParam, lParam);

        if (m_ntrackindex >= 0)
        {
            m_ntrackindex = -1;
            RedrawWindow();
        }

        return lResult;
    }

    void DoPaint(CDCHandle dc)
    {
        RECT rcClient;
        GetClientRect(&rcClient);

        //CBrush brush;
        //brush.CreateSolidBrush(m_clrBkGnd);
        //dc.FillRect(&rcClient, brush);

        RECT rcBox = rcClient;
        rcBox.top = rcClient.bottom - 1;

        rcBox.bottom = rcBox.top + 1;
        if (m_vtItems.size() != 0)
            rcBox.right  = em_left_space;

        dc.Draw3dRect(&rcBox, m_clrLine, m_clrLine);

        CWindow wndParent = GetParent();

        HFONT hDefaultFont = wndParent.m_hWnd != NULL ? wndParent.GetFont() : GetFont();

        int nOldMode = dc.SetBkMode(TRANSPARENT);

        HFONT hOldFont = dc.SelectFont( m_hHotFont.m_hFont != NULL ? m_hHotFont : hDefaultFont);

        CalcAllItemRect(dc);

        dc.SelectFont(hOldFont);

        for (int i = 0; i < int(m_vtItems.size()); i++)
        {
            RECT rcPaint = m_vtItems[i].rcItem ;

            int nflag = 0;

            if (m_nselectedindex == i)
            {
                nflag = 1;
            }
            else if (m_ntrackindex == i)
            {
                nflag = 2;
            }

            DrawBkHeaderItem(dc, rcPaint, nflag);

            if ( i == m_vtItems.size()- 1 )
            {
                rcBox.left = rcPaint.right;
                rcBox.right = rcClient.right;
            }
            else
            {
                rcBox.left = rcPaint.right;
                rcBox.right = rcBox.left + em_between_space;
            }

            dc.Draw3dRect(&rcBox, m_clrLine, m_clrLine);

            rcPaint.top    += em_between_space;
            rcPaint.bottom -= em_between_space;
            rcPaint.left   += em_between_space;
            rcPaint.right  -= em_between_space;

            HFONT hOldFont = dc.SelectFont(hDefaultFont);

            if (nflag == 1 && m_hHotFont.m_hFont != NULL)
                dc.SelectFont(m_hHotFont);

            dc.SetTextColor( nflag == 0 ? m_clrText : m_clrHotText );


            if ( m_piDrawCallback != NULL)
                m_piDrawCallback->OnDrawItem(dc, i, rcPaint, m_vtItems[i].strItemText);
            
            dc.SelectFont(hOldFont);

        }

        dc.SetBkMode(nOldMode);
    }

    void DrawBkHeaderItem( HDC hdc, const RECT& rcItem, int nFlag = 0 )
    {
        CSkinDCHandle dc(hdc);

        RECT rcBox = rcItem;

        if (nFlag == 0)
        {
            dc.SkinDrawRoundRect(rcItem, 3 , 0, m_clrLine);
            dc.SkinLine(rcItem.left , rcItem.bottom - 1, 
                rcItem.right - 1, rcItem.bottom - 1, m_clrLine);

            rcBox.left++;
            rcBox.top++;
            rcBox.right--;
            rcBox.bottom--;
            dc.SkinDrawGradualColorRect(rcBox, m_clrBkGnd, HLS_TRANSFORM(m_clrBkGnd, -10, 0));
        }
        else if (nFlag == 1)
        {
            dc.SkinLine(rcItem.left + 1, rcItem.top, rcItem.right - 2, rcItem.top    , m_clrLine);
            dc.SkinLine(rcItem.left , rcItem.top + 1, rcItem.left , rcItem.bottom - 1 , m_clrLine);
            dc.SkinLine(rcItem.right - 1, rcItem.top + 1, rcItem.right - 1, rcItem.bottom - 1 , m_clrLine);

            rcBox.left++;
            rcBox.top++;
            rcBox.right--;
            rcBox.bottom += 10;

            dc.SkinDrawRoundRect(rcItem, 3 , m_clrBkGnd, m_clrLine);
           
        }
    }

    HWND SkinCreate( 
        const SkinXmlElement& xmlElement,
        HWND hWndParent, _U_MENUorID MenuOrID = 0U ) throw()
    {
        HWND hWndResult = SkinWindowImpl< T, TBase, TWinTraits >::
            SkinCreate(xmlElement, hWndParent, MenuOrID);

        if (hWndResult == NULL)
            return hWndResult;

        if (hWndResult != NULL && hWndParent != NULL)
            SetFont( TBase(hWndParent).GetFont() );

#ifdef _SKINRESEDITOR_MODE_   // for SkinResEditor
        InsertItem(0, _T("KSGUI"));
        InsertItem(1, _T("SkinResEditor Test"));
        InsertItem(2, _T("..."));
#endif

        skinxmltablectrlex xmlWin(xmlElement);

        xmlWin.GetTextColor(m_clrText);
        xmlWin.GetBkColor(m_clrBkGnd);
        xmlWin.GetMainColor(m_clrLine);
        xmlWin.GetHotTextColor(m_clrHotText);
        xmlWin.GetHotTextFont(m_hHotFont.m_hFont);

        return hWndResult;
    }


private:

    RECT GetHeaderRect(int nindex)
    {
        return m_vtItems[nindex].rcItem;
    }

private:

    struct TableItemInfo
    {
        KSGUI::CString strItemText;
        RECT rcItem;
    };

    std::vector<TableItemInfo> m_vtItems;

    int m_nselectedindex;
    int m_ntrackindex;

    CFont m_hHotFont;

    COLORREF m_clrLine;
    COLORREF m_clrBkGnd;
    COLORREF m_clrText;
    COLORREF m_clrHotText;
};


class SkinTableCtrlEx : public SkinTableCtrlExImpl<SkinTableCtrlEx>
{
public:
    DECLARE_WND_CLASS(_T("KIS_TableCtrl"))

public:
    SkinTableCtrlEx()
    {
    }

    static SkinWindow* SkinCreate_Static(
        const SkinXmlElement& xmlElement,
        HWND hWndParent, _U_MENUorID MenuOrID = 0U )
    {
        SkinTableCtrlEx * pSkinWindow = new SkinTableCtrlEx;

        if (pSkinWindow == NULL)
            return pSkinWindow;


        if (pSkinWindow->SkinCreate(xmlElement, 
            hWndParent, 
            MenuOrID) == NULL)
        {
            delete pSkinWindow;

            pSkinWindow = NULL;
        }

        return pSkinWindow;
    }

};

};