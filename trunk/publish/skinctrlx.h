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
    public ATL::CWindowImpl< T, TBase, TWinTraits >,
    public WTL::CDoubleBufferImpl<CSkinStaticExT<T> >
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


        CHAIN_MSG_MAP(CDoubleBufferImpl<CSkinStaticExT>)
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
                CBrushHandle hBrush = (HBRUSH)::SendMessage(GetParent(), WM_CTLCOLORSTATIC, (WPARAM)dc.m_hDC, (LPARAM)m_hWnd);
                dc.FillRect(&rcClient, hBrush);
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
    }

    public:

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

        ATOM atom = T::GetWndClassInfo().Register(&m_pfnSuperWindowProc);

        _AtlWinModule.AddCreateWndData(&m_thunk.cd, this);

        HWND hWndResult = TBase::SkinCreate(xmlElement, 
            hWndParent,              
            T::GetWndClassInfo().m_wc.lpszClassName,
            MenuOrID);

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


};