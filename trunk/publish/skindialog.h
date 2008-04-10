/********************************************************************
* CreatedOn: 2008-2-2   9:02
* FileName: skindialog.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include <atlwin.h>

#include "skinxmldialog.h"
#include "skinwincreator.h"
#include "skindirectui.h"

namespace KSGUI{


class CSkinDlgChildList : public KSGUI::skinxmldialog::enumchildwincallback
{
public:
    
    BOOL onchildwin( skinxmlwin& xmlWin )
    {
        if ( m_directui.createdirectui(xmlWin) )
            return TRUE;

        return AddChildWindow(xmlWin);
    }

    void DestroyChilds()
    {
        std::map<KSGUI::CString, SkinWindow*>::iterator iter = 
            m_mapIDS2Win.begin();

        for ( ; iter != m_mapIDS2Win.end(); iter++)
        {
            KSGUI::SkinWindow* pSkinWindow = NULL;
            
            pSkinWindow = iter->second;

            if (pSkinWindow == NULL)
                continue;

            if (::IsWindow(pSkinWindow->m_hWnd))
                pSkinWindow->DestroyWindow();

            delete pSkinWindow;
        }
        
        m_mapIDS2Win.clear();
    }

    KSGUI::SkinWindow* GetDlgItem(const KSGUI::CString& strIdName)
    {
        KSGUI::SkinWindow* pSkinWindow = NULL;
        

        std::map<KSGUI::CString, SkinWindow*>::iterator iter = 
            m_mapIDS2Win.find(strIdName);

        if (iter != m_mapIDS2Win.end())
            pSkinWindow = iter->second;

        return pSkinWindow;
    }

    BOOL AddChildWindow( skinxmlwin& xmlWin )
    {
        KSGUI::SkinWindow* pSinWindow = NULL;

        pSinWindow = KSGUI::SkinWindowCreator::Instance().SkinCreate(xmlWin, m_hWndParent);

        if (pSinWindow == NULL)
            return FALSE;

        KSGUI::CString strIdName;
        DWORD dwId = 0;

        HFONT hFont = NULL;

        if ( xmlWin.GetIdName(strIdName) )
        {
            m_mapIDS2Win[strIdName] = pSinWindow;
        }
        else
        {
            if (::IsWindow(pSinWindow->m_hWnd))
                pSinWindow->DestroyWindow();

            delete pSinWindow;

            return FALSE;
        }

        return TRUE;
    }


    BOOL DestroyChildWindow( LPCTSTR pszIdName )
    {
        std::map<KSGUI::CString, SkinWindow*>::iterator iter = 
            m_mapIDS2Win.find(pszIdName);

        if (iter != m_mapIDS2Win.end())
        {
            KSGUI::SkinWindow* pSkinWindow = NULL;

            pSkinWindow = iter->second;

            if (pSkinWindow == NULL)
                return FALSE;

            if (::IsWindow(pSkinWindow->m_hWnd))
                pSkinWindow->DestroyWindow();

            delete pSkinWindow;

            m_mapIDS2Win.erase(iter);
        }
        
        return TRUE;
    }

public:
    
    HWND m_hWndParent;

    skindirectui m_directui;

    std::map<KSGUI::CString, SkinWindow*> m_mapIDS2Win;
};

#define DEFINE_DLGXMLRES_NAME(ResName)  \
    LPCTSTR GetDlgXMLResName()\
    {\
    return ResName;\
    }


template <class T, class TBase = SkinWindow >
class ATL_NO_VTABLE SkinDialogImpl : public  CDialogImpl<T, TBase>
{
    typedef CDialogImpl<T, TBase> theBase;

public:

    SkinDialogImpl()
    {

    }
    
    ~SkinDialogImpl()
    {
        m_childList.DestroyChilds();
    }

        
    BOOL InitDlgXMLElement( LPCTSTR pszResName )
    {
        if (m_xmlDlgElement.IsValid() || pszResName == NULL || _tcslen(pszResName) == 0 )
            return TRUE;

        skinresbase* pskinres = skinresloader::instance().get_skinres();
        ATLASSERT(pskinres != NULL);
        if (!pskinres)
            return FALSE;

        skindlgresbase* pskindlgres = pskinres->get_skindlgres();
        ATLASSERT(pskindlgres != NULL);
        if (!pskindlgres)
            return FALSE;

        skinxmldialog xmldlg;

        if(!pskindlgres->LoadDialog(pszResName, xmldlg))
        {
            ATLASSERT(FALSE);
            return FALSE;
        }

        m_xmlDlgElement = xmldlg;

        return TRUE;
    }

    void InitDlgXMLElement( SkinXmlElement xmlDlgElement  )
    {
        m_xmlDlgElement.CopyFrom(xmlDlgElement);
    }

    void DrawDirectUI( HDC hDC )
    {
        m_childList.m_directui.drawdirectui(hDC);
    }

    BEGIN_MSG_MAP(SkinDialogImpl)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
    END_MSG_MAP()

    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CPaintDC dc(m_hWnd);

        DrawDirectUI(dc);
        
        return 0;
    }

public:

    LPCTSTR GetDlgXMLResName()
    {
        return _T("");
    }

    INT_PTR DoModal(HWND hWndParent = ::GetActiveWindow(), LPARAM dwInitParam = NULL)
    {
        BOOL result;

        ATLASSUME(m_hWnd == NULL);

        result = m_thunk.Init(NULL,NULL);
        if (result == FALSE) 
        {
            SetLastError(ERROR_OUTOFMEMORY);
            return -1;
        }

        _AtlWinModule.AddCreateWndData(&m_thunk.cd, (CDialogImplBaseT< TBase >*)this);

#ifdef _DEBUG
        m_bModal = true;
#endif //_DEBUG

        InitDlgXMLElement(static_cast<T*>(this)->GetDlgXMLResName());

        DLGTEMPLATEEX* pDlgTemplate = (DLGTEMPLATEEX*)_MakeDlgTemplateBuffer( -1 );
        if( pDlgTemplate == NULL )
            return 0;

        INT_PTR nRet = ::DialogBoxIndirectParam( 
            _AtlBaseModule.GetResourceInstance(), 
            (LPCDLGTEMPLATE)pDlgTemplate, 
            hWndParent, 
            T::StartDialogProc,
            dwInitParam
            );

        _ReleaseDlgTemplateBuffer( pDlgTemplate );
        
        return nRet;
    }

    BOOL EndDialog(int nRetCode)
    {

        ATLASSERT(::IsWindow(m_hWnd));
#ifdef _DEBUG
        ATLASSUME(m_bModal);	// must be a modal dialog
#endif //_DEBUG

        return ::EndDialog(m_hWnd, nRetCode);

    }
    // modeless dialogs
    HWND Create(HWND hWndParent, LPARAM dwInitParam = NULL)
    {
        BOOL result;

        ATLASSUME(m_hWnd == NULL);

        // Allocate the thunk structure here, where we can fail
        // gracefully.

        result = m_thunk.Init(NULL,NULL);
        if (result == FALSE) 
        {
            SetLastError(ERROR_OUTOFMEMORY);
            return NULL;
        }

        _AtlWinModule.AddCreateWndData(&m_thunk.cd, (CDialogImplBaseT< TBase >*)this);
#ifdef _DEBUG
        m_bModal = false;
#endif //_DEBUG

        InitDlgXMLElement(static_cast<T*>(this)->GetDlgXMLResName());

        ::DLGTEMPLATEEX* pDlgTemplate = (::DLGTEMPLATEEX*)_MakeDlgTemplateBuffer( -1 );
        if( pDlgTemplate == NULL )
            return NULL;

        HWND hWnd = ::CreateDialogIndirectParam(
            _AtlBaseModule.GetResourceInstance(), 
            (LPCDLGTEMPLATE)pDlgTemplate,
            hWndParent,
            T::StartDialogProc,
            dwInitParam
            );

        ATLASSUME(m_hWnd == hWnd);

        _ReleaseDlgTemplateBuffer( pDlgTemplate );

        return hWnd;
    }

    HWND Create(HWND hWndParent, RECT&, LPARAM dwInitParam = NULL)
    {
        return Create(hWndParent, dwInitParam);
    }


    BOOL DestroyWindow()
    {
        ATLASSERT(::IsWindow(m_hWnd));
#ifdef _DEBUG
        ATLASSERT(!m_bModal);	// must not be a modal dialog
#endif //_DEBUG

        m_xmlDlgElement.ClearThis();

        return ::DestroyWindow(m_hWnd);
    }


    static INT_PTR CALLBACK StartDialogProc(HWND hWnd, UINT uMsg,
        WPARAM wParam, LPARAM lParam)
    {        
        CDialogImplBaseT< TBase >* pThis = (CDialogImplBaseT< TBase >*)_AtlWinModule.ExtractCreateWndData();
        ATLASSERT(pThis != NULL);
        if(!pThis)
        {
            return 0;
        }
        pThis->m_hWnd = hWnd;
        // Initialize the thunk.  This was allocated in CDialogImpl::DoModal or
        // CDialogImpl::Create, so failure is unexpected here.

        pThis->m_thunk.Init((WNDPROC)pThis->GetDialogProc(), pThis);
        DLGPROC pProc = (DLGPROC)pThis->m_thunk.GetWNDPROC();
        DLGPROC pOldProc = (DLGPROC)::SetWindowLongPtr(hWnd, DWLP_DLGPROC, (LONG_PTR)pProc);

#ifdef _DEBUG
        // check if somebody has subclassed us already since we discard it
        if(pOldProc != StartDialogProc)
            ATLTRACE(atlTraceWindowing, 0, _T("Subclassing through a hook discarded.\n"));
#else
        pOldProc;	// avoid unused warning
#endif
        INT_PTR nResult = pProc(hWnd, uMsg, wParam, lParam);

        SkinDialogImpl<T, TBase>* pSinThis = static_cast<SkinDialogImpl<T, TBase>*>(pThis);

        pSinThis->_InitXmlDlg();

        return nResult;
    }   


    DWORD GetStyle( DWORD dwStyle )
    {
        if (dwStyle == 0)
            dwStyle =  DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE;

        return dwStyle;
    }

    DWORD GetExStyle( DWORD dwExStyle )
    {
        return dwExStyle;
    }


    void* _MakeDlgTemplateBuffer( int nIDD )
    {
        BYTE*    pBuffer  = NULL;
        WORD*    pdwIter  = NULL;
        //wchar_t* pwszIter = NULL;
        size_t   nSize    = 2048;

        skinxmldialog xmldialog = m_xmlDlgElement;

        DWORD dwStyle   = 0;
        DWORD dwExStyle = 0;
        
        RECT rcClient = rcDefault;

        KSGUI::CString strCaption;

        xmldialog.GetStyle(dwStyle);
        xmldialog.GetExStyle(dwExStyle);
        xmldialog.GetCaption(strCaption);

        GetRealRect(NULL, xmldialog, rcClient);

        dwStyle =  static_cast<T*>(this)->GetStyle(dwStyle);
        dwExStyle =  static_cast<T*>(this)->GetExStyle(dwExStyle);

        dwStyle |= (DS_SETFONT);

        pBuffer = new BYTE[nSize];
        if( pBuffer == NULL )
            return NULL;

        ::memset( pBuffer, 0, nSize );

        DLGTEMPLATEEX* pDlgTemplate = (DLGTEMPLATEEX*)pBuffer;

        // 以下数据从皮肤获取
        pDlgTemplate->dlgVer    = 1;
        pDlgTemplate->signature = 0xFFFF;
        pDlgTemplate->helpID    = 0;
        pDlgTemplate->exStyle   = dwExStyle;
        pDlgTemplate->style     = dwStyle;
        pDlgTemplate->cDlgItems = 0;

        pDlgTemplate->x      = (short)rcClient.left;
        pDlgTemplate->y      = (short)rcClient.top;
        pDlgTemplate->cx     = (short)rcClient.right;
        pDlgTemplate->cy     = (short)rcClient.bottom;

        pdwIter = (WORD*)( pDlgTemplate + 1 );
        
        //pdwIter++;

        int a = sizeof(DLGTEMPLATEEX);

        *pdwIter++ = 0; // No menu.
        *pdwIter++ = 0; // Default window class.
        
        wcsncpy_s((wchar_t*)pdwIter, 255, CT2W(strCaption).m_psz, 255); // caption

        pdwIter += (strCaption.GetLength() + 1);
        
        skinxmlfont xmlfont;

        if ( xmldialog.GetObject(_T("Font"), xmlfont) )
        {
            LOGFONT logFont = { 0 };
            
            xmlfont >> logFont;

            *pdwIter++ = (WORD)logFont.lfHeight; // font size

            *pdwIter++ = (WORD) logFont.lfWeight;
            *pdwIter++ = (WORD) ((logFont.lfCharSet << 8 ) | logFont.lfItalic );

            wcsncpy_s((wchar_t*)pdwIter, 255, CT2W(logFont.lfFaceName).m_psz, 255); // font name

            pdwIter += (wcslen(CT2W(logFont.lfFaceName).m_psz) + 1);

            //*pwszIter++ = 800;

        }
        return pBuffer;
    }

    void _ReleaseDlgTemplateBuffer( void* pBuffer )
    {
        if( pBuffer != NULL )
        {
            delete [] pBuffer;
        }

    }

    virtual void _InitXmlDlg()
    {
        ATLASSERT(m_hWnd != NULL);

        skinxmldialog xmldialog = m_xmlDlgElement;

        m_childList.m_hWndParent = m_hWnd;
        xmldialog.EnumChildWindows(&m_childList);

    }

    virtual BOOL GetRealRect( HWND hWndParent, const SkinXmlElement& xmlElement, RECT& rcClient )
    {
        if ( !xmlElement.IsValid() )
            return FALSE;

        skinxmlwin xmlWin(xmlElement);

        rcClient.left   = 0;
        rcClient.top    = 0;
        rcClient.right  = 0;
        rcClient.bottom = 0;
        
        xmlWin.GetLeft((int&)rcClient.left);
        xmlWin.GetTop((int&)rcClient.top);

        xmlWin.GetWidth((int&)rcClient.right);
        xmlWin.GetHeight((int&)rcClient.bottom);
       

        return TRUE;
    }

    
public:

    SkinXmlElement    m_xmlDlgElement;
    CSkinDlgChildList m_childList;
};


template <class T>
class SkinCaptionDialogT
{
public:

    class SkinCaptionButton
    {
    public:

        void Paint( CSkinDCHandle& dc , int nflags )
        {
            COLORREF clrbkgnd = nflags == 0 ? 
                HLS_TRANSFORM(pT->m_clrMainColor, 85, 30) :
                HLS_TRANSFORM(pT->m_clrMainColor, 100, 30);

            dc.SkinDrawRectangle(rcButton, 
                clrbkgnd, HLS_TRANSFORM(pT->m_clrMainColor, 50, 30));

            if ( uNcHitTestCode == HTCLOSE )
            {
                RECT rcBox = rcButton;

                rcBox.left += 3;
                rcBox.top  += 3;
                rcBox.right  -= 4;
                rcBox.bottom -= 3;

                dc.SkinLine( rcBox.left , rcBox.top,
                    rcBox.right , rcBox.bottom, pT->m_clrMainColor, PS_SOLID, 1);
                dc.SkinLine( rcBox.left + 1, rcBox.top,
                    rcBox.right + 1, rcBox.bottom, pT->m_clrMainColor, PS_SOLID, 1);
                dc.SkinLine( rcBox.right, rcBox.top,
                    rcBox.left , rcBox.bottom, pT->m_clrMainColor, PS_SOLID, 1);
                dc.SkinLine( rcBox.right - 1, rcBox.top,
                    rcBox.left - 1, rcBox.bottom, pT->m_clrMainColor, PS_SOLID, 1);

            }
            else if ( uNcHitTestCode == HTMAXBUTTON )
            {
                RECT rcBox = rcButton;

                rcBox.left += 3;
                rcBox.top  += 3;
                rcBox.right  -= 3;
                rcBox.bottom -= 3;

                dc.SkinDrawBorder(rcBox, pT->m_clrMainColor);
                dc.SkinLine( rcBox.left , rcBox.top + 1,
                    rcBox.right, rcBox.top + 1, pT->m_clrMainColor);
            }
            else if (uNcHitTestCode == HTMINBUTTON )
            {
               dc.SkinLine( rcButton.left + 3, rcButton.top + 8,
                    rcButton.right - 5, rcButton.top + 8, pT->m_clrMainColor, PS_SOLID, 1);
               dc.SkinLine( rcButton.left + 3, rcButton.top + 9,
                   rcButton.right - 5, rcButton.top + 9, pT->m_clrMainColor, PS_SOLID, 1);
            }
        }

    public:
        SkinCaptionButton( SkinCaptionDialogT<T>* p ) :
            bUsed(FALSE),
            uNcHitTestCode(HTCLOSE),
            pT(p)
        {
        }
        
        BOOL bUsed;
        RECT rcButton;
        UINT uNcHitTestCode;

        SkinCaptionDialogT<T>* pT;
    };


    SkinCaptionDialogT() :
        m_closebtn(this), m_maxbtn(this), m_minbtn(this)
    {
        m_closebtn.uNcHitTestCode = HTCLOSE;
        m_minbtn.uNcHitTestCode   = HTMINBUTTON;
        m_maxbtn.uNcHitTestCode   = HTMAXBUTTON;

        m_bBtnPressed = FALSE;
        m_uBtnPress   = HTNOWHERE;
        m_uLastHotBtn = HTNOWHERE;

        m_bMaxed      = FALSE;

        m_clrMainColor = RGB(87,117,159);
    }

    void _InitXmlDlg()
    {
        T* pskindlg = static_cast<T*>(this);

        skinxmlwin xmlwin = pskindlg->m_xmlDlgElement;

        xmlwin.GetObject(_T("MainColor"), m_clrMainColor);
    }


    SkinCaptionButton m_closebtn;
    SkinCaptionButton m_minbtn;
    SkinCaptionButton m_maxbtn;

    BOOL m_bBtnPressed;
    BOOL m_bMaxed;
    UINT m_uBtnPress;

    UINT m_uLastHotBtn;

    RECT m_rcLastWindow;

    COLORREF m_clrMainColor;

    enum {
        em_border_width = 3,
        em_titlebar_height = 21,

        //em_clr_border = RGB(87,117,159),
        //em_clr_border = RGB(128, 0 , 128),
    };

    BEGIN_MSG_MAP(SkinCaptionDialogT)

        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)

        MESSAGE_HANDLER(WM_NCPAINT   , OnNcPaint)
        MESSAGE_HANDLER(WM_NCCALCSIZE, OnNcCalcSize)

        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)

        MESSAGE_HANDLER(WM_NCLBUTTONDOWN, OnNcLButtonDown)
        MESSAGE_HANDLER(WM_NCLBUTTONUP  , OnNcLButtonUp  )
        MESSAGE_HANDLER(WM_NCMOUSEMOVE  , OnNcMouseMove  )
        MESSAGE_HANDLER(WM_NCMOUSELEAVE , OnNcMouseLeave )

        MESSAGE_HANDLER(WM_NCLBUTTONDBLCLK, OnNcLButtonDblclk)

        MESSAGE_HANDLER(WM_LBUTTONUP    , OnLButtonUp)

        MESSAGE_HANDLER(WM_NCHITTEST    , OnNcHitTest    )

    END_MSG_MAP()

    LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {       
        CWindow wndThis = static_cast<T*>(this)->m_hWnd;

        RECT rcClient = { 0 };

        wndThis.GetClientRect(&rcClient);

        int width  = rcClient.right - rcClient.left + em_border_width * 2;
        int height = rcClient.bottom - rcClient.top + em_titlebar_height + em_border_width * 2;

        ::SetWindowPos(wndThis, NULL, 0, 0,
             width, height,
            SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

        m_rcLastWindow = rcClient;
        wndThis.ClientToScreen(&m_rcLastWindow);

        m_rcLastWindow.right = m_rcLastWindow.left + width;
        m_rcLastWindow.bottom = m_rcLastWindow.top + height;

        bHandled = FALSE;


        return 0;
    }

    LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CWindow wndThis = static_cast<T*>(this)->m_hWnd;

        CWindowDC dc(wndThis);

        CSkinDCHandle skinDC(dc.m_hDC);

        //LRESULT lResult = ::DefWindowProc(wndThis, uMsg, wParam, lParam);

        RECT rcWindow = { 0 };
        RECT rcDraw   = { 0 };

        wndThis.GetWindowRect(&rcWindow);

        rcWindow.right  = rcWindow.right  - rcWindow.left;
        rcWindow.bottom = rcWindow.bottom - rcWindow.top;

        rcWindow.left = 0;
        rcWindow.top  = 0;

        skinDC.SkinDrawBorder(rcWindow, HLS_TRANSFORM(m_clrMainColor, 80, 0), PS_SOLID, em_border_width);
        
        rcDraw = rcWindow;
        rcDraw.bottom = rcDraw.top;

        skinDC.SkinDrawBorder(rcDraw.left, rcDraw.top, 
            rcDraw.right, rcDraw.bottom, HLS_TRANSFORM(m_clrMainColor, 95, 0), PS_SOLID, em_border_width);

        skinDC.SkinDrawBorder(rcWindow, m_clrMainColor);

        rcWindow.top   += em_border_width;
        rcWindow.left  += em_border_width;
        rcWindow.right -= em_border_width;

        rcWindow.bottom = rcWindow.top + em_titlebar_height;

        skinDC.SkinDrawGradualColorRect(rcWindow, 
            HLS_TRANSFORM(m_clrMainColor, 85, 30), 
            HLS_TRANSFORM(m_clrMainColor, 99, 20));

        skinDC.SkinLine( rcWindow.left, rcWindow.bottom - 1,
            rcWindow.right, rcWindow.bottom - 1, HLS_TRANSFORM(m_clrMainColor, 70, 0));

        int npaintx =  em_border_width * 2;

        HICON hicon = wndThis.GetIcon(FALSE);
        if (hicon != NULL)
        {
            skinDC.DrawIconEx(npaintx, em_border_width , hicon, 
                ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));

            npaintx += (::GetSystemMetrics(SM_CXSMICON) + em_border_width);
        }


        TCHAR szCaption[MAX_PATH] = { 0 };
        wndThis.GetWindowText(szCaption, MAX_PATH);

        if (_tcslen(szCaption) > 0)
        {
            skinDC.SkinDrawText(npaintx, em_border_width * 2, szCaption, 0, wndThis.GetFont());
        }

        if (m_closebtn.bUsed)
            m_closebtn.Paint(skinDC, 0);
        if (m_minbtn.bUsed)
            m_minbtn.Paint(skinDC, 0);
        if (m_maxbtn.bUsed)
            m_maxbtn.Paint(skinDC, 0);

        return 0;
    }

    LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CWindow wndThis = static_cast<T*>(this)->m_hWnd;

        CSkinDCHandle skinDC((HDC)wParam);

        RECT rcClient = { 0 };

        wndThis.GetClientRect(&rcClient);

        skinDC.SkinDrawGradualColorRect(rcClient, 
            HLS_TRANSFORM(m_clrMainColor, 99, 20),
            HLS_TRANSFORM(m_clrMainColor, 85, 30)); 


        return 0L;
    }

    
    LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CWindow wndThis = static_cast<T*>(this)->m_hWnd;
        
        BOOL bMinimized = wndThis.GetStyle() & WS_MINIMIZE;
        BOOL bMaximized = wndThis.GetStyle() & WS_MAXIMIZE;

        LPNCCALCSIZE_PARAMS pNCParams = (LPNCCALCSIZE_PARAMS)lParam;
        
        LPRECT lpRect = (LPRECT)lParam;

        RECT rcWindow = pNCParams->rgrc[0];

        LRESULT lResult = ::DefWindowProc(wndThis, uMsg, wParam, lParam);

        if (wParam && !bMinimized)
        {
            CalcCaptionBtnPlace(rcWindow);

            pNCParams->rgrc[0] = rcWindow;

            pNCParams->rgrc[0].top    += (em_border_width + em_titlebar_height);
            pNCParams->rgrc[0].left   += em_border_width;
            pNCParams->rgrc[0].right  -= em_border_width;
            pNCParams->rgrc[0].bottom -= em_border_width;

        }

        return lResult;
    }

    void CalcCaptionBtnPlace(RECT rcWindow)
    {
        CWindow wndThis = static_cast<T*>(this)->m_hWnd;

        BOOL bMinimized = wndThis.GetStyle() & WS_MINIMIZEBOX;
        BOOL bMaximized = wndThis.GetStyle() & WS_MAXIMIZEBOX;

        int nrightpos = rcWindow.right - rcWindow.left - em_border_width * 2;

        m_closebtn.bUsed = TRUE;

        m_closebtn.rcButton.right  = nrightpos;
        m_closebtn.rcButton.left   = nrightpos - ( em_titlebar_height * 3 / 4 );

        m_closebtn.rcButton.top    = (em_border_width + em_titlebar_height) / 4;
        m_closebtn.rcButton.bottom = m_closebtn.rcButton.top + em_titlebar_height * 2 / 3;

        if (bMaximized)
        {
            nrightpos -= (( em_titlebar_height * 3 / 4 ) + em_border_width ) ;

            m_maxbtn.bUsed = TRUE;

            m_maxbtn.rcButton.right  = nrightpos;
            m_maxbtn.rcButton.left   = nrightpos - ( em_titlebar_height * 3 / 4 );

            m_maxbtn.rcButton.top    = (em_border_width + em_titlebar_height) / 4;
            m_maxbtn.rcButton.bottom = m_maxbtn.rcButton.top + em_titlebar_height * 2 / 3;
        }

        if (bMinimized)
        {
            nrightpos -= (( em_titlebar_height * 3 / 4 ) + em_border_width );

            m_minbtn.bUsed = TRUE;

            m_minbtn.rcButton.right  = nrightpos;
            m_minbtn.rcButton.left   = nrightpos - ( em_titlebar_height * 3 / 4 );

            m_minbtn.rcButton.top    = (em_border_width + em_titlebar_height) / 4;
            m_minbtn.rcButton.bottom = m_minbtn.rcButton.top + em_titlebar_height * 2 / 3;
        }

    }

    
    LRESULT OnNcLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CWindow wndThis = static_cast<T*>(this)->m_hWnd;

        if ( wParam == HTMINBUTTON ||
             wParam == HTMAXBUTTON ||
             wParam == HTCLOSE )
        {
            m_bBtnPressed = TRUE;
            m_uBtnPress   = wParam;

            CWindowDC dc(wndThis);
            CSkinDCHandle skinDC(dc.m_hDC);

            if (m_uBtnPress == HTCLOSE)
                m_closebtn.Paint(skinDC, 0);
            else if (m_uBtnPress == HTMINBUTTON)
                m_minbtn.Paint(skinDC, 0);
            else if (m_uBtnPress == HTMAXBUTTON)
                m_maxbtn.Paint(skinDC, 0);

        }
        else if ( wParam == HTCAPTION && !m_bMaxed )
        {
            ::DefWindowProc(wndThis, WM_NCLBUTTONDOWN, HTCAPTION, 
                MAKELPARAM(0, 0));
        }

        return 0;
    }

    LRESULT OnNcLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CWindow wndThis = static_cast<T*>(this)->m_hWnd;

        int xPos = GET_X_LPARAM(lParam); 
        int yPos = GET_Y_LPARAM(lParam); 

        if (wParam == m_uBtnPress)
        {
            POINT pt =  { xPos, yPos };

            switch (wParam)
            {
            case HTCLOSE:
                wndThis.PostMessage(WM_SYSCOMMAND, SC_CLOSE, 0);
                break;
            case HTMINBUTTON:
                wndThis.PostMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
                break;
            case HTMAXBUTTON:
                if ( m_bMaxed )
                {
                    wndThis.MoveWindow(&m_rcLastWindow);
                    m_bMaxed = FALSE;
                }
                else
                {
                    wndThis.GetWindowRect(&m_rcLastWindow);
                    m_bMaxed = TRUE;

                    RECT rt;   
                    SystemParametersInfo(SPI_GETWORKAREA,0,&rt,0);   
                    wndThis.MoveWindow(&rt, TRUE);
                    wndThis.Invalidate();
                }
                break;
            default:
                bHandled = FALSE;
            }
        }

        return 0;
    }

    LRESULT OnLButtonUp(UINT /**//*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;

        return 0;
    }

    LRESULT OnNcLButtonDblclk(UINT /**//*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CWindow wndThis = static_cast<T*>(this)->m_hWnd;

        if (wParam == HTCAPTION)
        {
            if ( m_bMaxed )
            {
                wndThis.MoveWindow(&m_rcLastWindow);
                m_bMaxed = FALSE;
            }
            else
            {
                wndThis.GetWindowRect(&m_rcLastWindow);
                m_bMaxed = TRUE;

                RECT rt;   
                SystemParametersInfo(SPI_GETWORKAREA,0,&rt,0);   
                wndThis.MoveWindow(&rt);
                wndThis.Invalidate();
            }
        }

        return 0;
    }


    LRESULT OnNcMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CWindow wndThis = static_cast<T*>(this)->m_hWnd;

        LRESULT lResult = ::DefWindowProc(wndThis, uMsg, wParam, lParam);
        
        int xPos = GET_X_LPARAM(lParam); 
        int yPos = GET_Y_LPARAM(lParam); 

        RECT rcWindow = { 0 };
        RECT rcDraw   = { 0 };

        wndThis.GetWindowRect(&rcWindow);

        switch(m_uLastHotBtn)
        {
        case HTCLOSE:
        case HTMINBUTTON:
        case HTMAXBUTTON:
            {
                CWindowDC dc(wndThis);
                CSkinDCHandle skinDC(dc.m_hDC);

                if (m_uLastHotBtn == HTCLOSE)
                    m_closebtn.Paint(skinDC, 0);
                else if (m_uLastHotBtn == HTMINBUTTON)
                    m_minbtn.Paint(skinDC, 0);
                else if (m_uLastHotBtn == HTMAXBUTTON)
                    m_maxbtn.Paint(skinDC, 0);
               
                m_uLastHotBtn = HTNOWHERE;
            }
            break;
        }

        UINT uLastHotBtn = m_uLastHotBtn;

        if ( m_closebtn.bUsed && 
             ::PtInRect( &m_closebtn.rcButton, 
                CPoint(xPos - rcWindow.left, yPos - rcWindow.top )))
        {
            CWindowDC dc(wndThis);
            CSkinDCHandle skinDC(dc.m_hDC);
            
            m_closebtn.Paint(skinDC, 1);

            m_uLastHotBtn = HTCLOSE;
        }

        if ( m_minbtn.bUsed && 
            ::PtInRect( &m_minbtn.rcButton, 
            CPoint(xPos - rcWindow.left, yPos - rcWindow.top )))
        {
            CWindowDC dc(wndThis);
            CSkinDCHandle skinDC(dc.m_hDC);

            m_minbtn.Paint(skinDC, 1);

            m_uLastHotBtn = HTMINBUTTON;
        }

        if ( m_maxbtn.bUsed && 
            ::PtInRect( &m_maxbtn.rcButton, 
            CPoint(xPos - rcWindow.left, yPos - rcWindow.top )))
        {
            CWindowDC dc(wndThis);
            CSkinDCHandle skinDC(dc.m_hDC);

            m_maxbtn.Paint(skinDC, 1);

            m_uLastHotBtn = HTMAXBUTTON;
        }

        if (uLastHotBtn == HTNOWHERE &&
            m_uLastHotBtn != HTNOWHERE)
        {
            TRACKMOUSEEVENT tme = { 0 };

            tme.cbSize  = sizeof(tme);
            tme.dwFlags = TME_LEAVE | TME_NONCLIENT;
            tme.hwndTrack = wndThis;

            _TrackMouseEvent(&tme);
        }

        return lResult;
    }

    LRESULT OnNcMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CWindow wndThis = static_cast<T*>(this)->m_hWnd;

        switch(m_uLastHotBtn)
        {
        case HTCLOSE:
        case HTMINBUTTON:
        case HTMAXBUTTON:
            {
                CWindowDC dc(wndThis);
                CSkinDCHandle skinDC(dc.m_hDC);

                if (m_uLastHotBtn == HTCLOSE)
                    m_closebtn.Paint(skinDC, 0);
                else if (m_uLastHotBtn == HTMINBUTTON)
                    m_minbtn.Paint(skinDC, 0);
                else if (m_uLastHotBtn == HTMAXBUTTON)
                    m_maxbtn.Paint(skinDC, 0);

                m_uLastHotBtn = HTNOWHERE;
            }
            break;
        }

        return 0L;
    }


    LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CWindow wndThis = static_cast<T*>(this)->m_hWnd;

        LRESULT lResult = ::DefWindowProc(wndThis, uMsg, wParam, lParam);

        int xPos = GET_X_LPARAM(lParam); 
        int yPos = GET_Y_LPARAM(lParam); 

        RECT rcTitlebar = { 0 };

        wndThis.GetWindowRect(&rcTitlebar);

        rcTitlebar.bottom = rcTitlebar.top + em_titlebar_height;

        if (m_closebtn.bUsed)
            if (::PtInRect( &m_closebtn.rcButton, 
                CPoint(xPos - rcTitlebar.left, yPos - rcTitlebar.top) ))
                return m_closebtn.uNcHitTestCode;
        if (m_minbtn.bUsed)
            if (::PtInRect( &m_minbtn.rcButton, 
                CPoint(xPos - rcTitlebar.left, yPos - rcTitlebar.top) ))
                return m_minbtn.uNcHitTestCode;
        if (m_maxbtn.bUsed)
            if (::PtInRect( &m_maxbtn.rcButton, 
                CPoint(xPos - rcTitlebar.left, yPos - rcTitlebar.top) ))
                return m_maxbtn.uNcHitTestCode;


        if (::PtInRect( &rcTitlebar, CPoint(xPos, yPos) ))
            return HTCAPTION;

        return lResult;
    }
};


template<class T, class CaptionDialogT = SkinCaptionDialogT<T> >
class SkinCaptionDialogImpl : 
    public SkinDialogImpl<T>,
    public CaptionDialogT
{
public:

    BEGIN_MSG_MAP(SkinCaptionDialogImpl)
        CHAIN_MSG_MAP(SkinDialogImpl<T>)
        CHAIN_MSG_MAP(CaptionDialogT)
    END_MSG_MAP()

    void _InitXmlDlg()
    {
        SkinDialogImpl<T>::_InitXmlDlg();
        CaptionDialogT::_InitXmlDlg();
    }
};


};// namespace KSGUI
