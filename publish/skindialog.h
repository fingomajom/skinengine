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

    KSGUI::duidrawbase* GetDxUIItem(const KSGUI::CString& strIdName)
    {
        KSGUI::duidrawbase* pRet = NULL;

        for (size_t idx = 0; idx < m_directui.vtdrawlist.size(); idx++)
        {
            if ( !strIdName.CompareNoCase( m_directui.vtdrawlist[idx]->GetIdName() ) )
            {
                pRet = m_directui.vtdrawlist[idx];
                break;
            }
        }
        
        return pRet;
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
        MESSAGE_HANDLER(WM_PAINT     , OnPaint      )
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkGnd )
    END_MSG_MAP()

    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CPaintDC dc(m_hWnd);

        DrawDirectUI(dc);
        
        return 0;
    }

    LRESULT OnEraseBkGnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CDCHandle dc( (HDC)wParam );
        if (dc.m_hDC == NULL)
            return DefWindowProc();

        COLORREF clrBkGnd = GetSysColor(COLOR_BTNFACE);


        skinxmlwin xmlWin(m_xmlDlgElement);
        xmlWin.GetBkColor(clrBkGnd);

        RECT rcClient = { 0 };
        GetClientRect(&rcClient);

        dc.FillSolidRect(&rcClient, clrBkGnd);

        return 1L;
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

class SkinNCButtonBase
{
public:

    enum{
        em_align_left   = 0x01,
        em_align_top    = 0x02,
        em_align_right  = 0x04,
        em_align_bottom = 0x08
    };

    enum{
        em_bs_normal      = 0x00,
        em_bs_mouseover   = 0x01,
        em_bs_lbuttondown = 0x02
    };

public:

    SkinNCButtonBase()
    {
        memset(&m_rcButton, 0, sizeof(m_rcButton));
        memset(&m_sizeBtn, 0, sizeof(m_sizeBtn));
        memset(&m_ptOffset, 0, sizeof(m_ptOffset));
        m_uNcHitTestCode = 0;
        m_uSysCmdId = 0;
        m_uStatus = 0;
    }


    virtual void DoPaint( CSkinDCHandle dc )
    {
    }

    void ChangedBtnPos( const RECT& rcWindow )
    {
        RECT rcWndParent;

        rcWndParent.left   = 0;
        rcWndParent.top    = 0;
        rcWndParent.right  = rcWindow.right  - rcWindow.left;
        rcWndParent.bottom = rcWindow.bottom - rcWindow.top;

        if((m_uAlign & em_align_right) == em_align_right)
        {
            m_rcButton.right = rcWndParent.right - m_ptOffset.x;
            m_rcButton.left = m_rcButton.right - m_sizeBtn.cx;
        }
        else
        {
            if((m_uAlign & em_align_left) == em_align_left)
            {
                m_rcButton.left = rcWndParent.left + m_ptOffset.x;
                m_rcButton.right = m_rcButton.left + m_sizeBtn.cx;
            }
        }
        if((m_uAlign & em_align_bottom) == em_align_bottom)
        {
            m_rcButton.bottom = rcWndParent.bottom - m_ptOffset.y;
            m_rcButton.top = m_rcButton.bottom - m_sizeBtn.cy;
        }
        else
        {
            if((m_uAlign & em_align_top) == em_align_top)
            {
                m_rcButton.top = rcWndParent.top + m_ptOffset.y;
                m_rcButton.bottom = m_rcButton.top + m_sizeBtn.cy;
            }
        }
    }

public:

    UINT NcHitTestCode() const
    {
        return m_uNcHitTestCode;
    }

    void NcHitTestCode( UINT uCode )
    {
        m_uNcHitTestCode = uCode;
    }

    UINT SysCmdId() const
    {
        return m_uSysCmdId;
    }

    void SysCmdId( UINT uId )
    {
        m_uSysCmdId = uId;
    }
    
    SIZE SizeBtn() const
    {
        return m_sizeBtn;
    }

    void SizeBtn( SIZE size )
    {
        m_sizeBtn = size;
    }

    POINT Offset() const
    {
        return m_ptOffset;
    }

    void Offset( POINT pt )
    {
        m_ptOffset = pt;
    }

    UINT Align() const
    {
        return m_uAlign;
    }

    void Align( UINT uAlign )
    {
        m_uAlign = uAlign;
    }

    UINT Status() const
    {
        return m_uStatus;
    }

    void Status( UINT uStatus )
    {
        m_uStatus = uStatus;
    }

    RECT ButtonRect() const
    {
        return m_rcButton;
    }

protected:
    
    RECT  m_rcButton;
    UINT  m_uNcHitTestCode;
    UINT  m_uSysCmdId;
    
    UINT  m_uAlign;
    SIZE  m_sizeBtn;
    POINT m_ptOffset;

    UINT  m_uStatus;

};

template <class T>
class SkinCaptionDialogT
{
public:

    class SkinCaptionButton : public SkinNCButtonBase
    {
    public:

        SkinCaptionButton( 
            UINT  uNcHitTestCode, 
            UINT  uSysCmdId,
            UINT  uAlign,
            SIZE  sizeBtn,
            POINT ptOffset,
            COLORREF clr = 0) :
            clrMainColor(clr)
        {
            m_uNcHitTestCode = (uNcHitTestCode);
            m_uSysCmdId      = (uSysCmdId);
            m_uAlign         = (uAlign);
            m_sizeBtn        = (sizeBtn);
            m_ptOffset       = (ptOffset);
        }
 
        virtual void DoPaint( CSkinDCHandle dc )
        {
            COLORREF clrbkgnd = m_uStatus == em_bs_normal ? 
                HLS_TRANSFORM(clrMainColor, 5, 0) :
                HLS_TRANSFORM(clrMainColor, 25, 30);

            dc.SkinDrawRectangle(m_rcButton, 
                clrbkgnd, HLS_TRANSFORM(clrMainColor, 60, 30));

            COLORREF clrLine = m_uStatus == em_bs_normal ? 0xFFFFFF : 0xFFFFFF;

            if ( NcHitTestCode() == HTCLOSE )
            {
                RECT rcBox = m_rcButton;

                rcBox.left += 3;
                rcBox.top  += 3;
                rcBox.right  -= 4;
                rcBox.bottom -= 3;

                dc.SkinLine( rcBox.left , rcBox.top,
                    rcBox.right , rcBox.bottom, clrLine, PS_SOLID, 1);
                dc.SkinLine( rcBox.left + 1, rcBox.top,
                    rcBox.right + 1, rcBox.bottom, clrLine, PS_SOLID, 1);
                dc.SkinLine( rcBox.right, rcBox.top,
                    rcBox.left , rcBox.bottom, clrLine, PS_SOLID, 1);
                dc.SkinLine( rcBox.right - 1, rcBox.top,
                    rcBox.left - 1, rcBox.bottom, clrLine, PS_SOLID, 1);

            }
            else if ( NcHitTestCode() == HTMAXBUTTON )
            {
                RECT rcBox = m_rcButton;

                rcBox.left += 3;
                rcBox.top  += 3;
                rcBox.right  -= 3;
                rcBox.bottom -= 3;

                //dc.SkinDrawBorder(rcBox, clrLine);
                dc.Draw3dRect(&rcBox, clrLine, clrLine);

                dc.SkinLine( rcBox.left , rcBox.top + 1,
                    rcBox.right, rcBox.top + 1, clrLine);
            }
            else if (NcHitTestCode() == HTMINBUTTON )
            {
                dc.SkinLine( m_rcButton.left + 3, m_rcButton.top + 8,
                    m_rcButton.right - 5, m_rcButton.top + 8, clrLine, PS_SOLID, 1);
                dc.SkinLine( m_rcButton.left + 3, m_rcButton.top + 9,
                    m_rcButton.right - 5, m_rcButton.top + 9, clrLine, PS_SOLID, 1);
            }
        }

    public:

        COLORREF MainColor() const
        {
            return clrMainColor;
        }

        void MainColor( COLORREF clr )
        {
            clrMainColor = clr;
        }

    public:       

        COLORREF clrMainColor;
    };


    SkinCaptionDialogT() : 
        m_pBtnPress(NULL),
        m_pLastHotBtn(NULL)
    {
        m_bMaxed       = FALSE;
        m_clrMainColor = RGB(87,117,159);
    }

    void _InitXmlDlg()
    {
        T* pskindlg = static_cast<T*>(this);

        skinxmlwin xmlwin = pskindlg->m_xmlDlgElement;

        xmlwin.GetObject(_T("MainColor"), m_clrMainColor);
    }

    SkinNCButtonBase* m_pBtnPress;
    SkinNCButtonBase* m_pLastHotBtn;

    std::vector<SkinNCButtonBase*> m_vtNcBtn;

    BOOL m_bMaxed;
    RECT m_rcLastWindow;

    CRgn m_rgnWindow;

    COLORREF m_clrMainColor;

    enum {
        em_border_width    = 3,
        em_titlebar_height = 21
    };

    BEGIN_MSG_MAP(SkinCaptionDialogT)

        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)

        MESSAGE_HANDLER(WM_NCPAINT   , OnNcPaint)
        MESSAGE_HANDLER(WM_NCCALCSIZE, OnNcCalcSize)

        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
        MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)

        MESSAGE_HANDLER(WM_NCLBUTTONDOWN, OnNcLButtonDown)
        MESSAGE_HANDLER(WM_NCLBUTTONUP  , OnNcLButtonUp  )
        MESSAGE_HANDLER(WM_NCMOUSEMOVE  , OnNcMouseMove  )
        MESSAGE_HANDLER(WM_NCMOUSELEAVE , OnNcMouseLeave )

        MESSAGE_HANDLER(WM_NCLBUTTONDBLCLK, OnNcLButtonDblclk)

        MESSAGE_HANDLER(WM_LBUTTONUP    , OnLButtonUp)

        MESSAGE_HANDLER(WM_NCHITTEST    , OnNcHitTest    )

        MESSAGE_HANDLER(WM_SETTEXT, OnSetText)

        MESSAGE_HANDLER(WM_SIZE   , OnSize)

    END_MSG_MAP()

    LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {       
        CWindow wndThis = static_cast<T*>(this)->m_hWnd;


        RECT rcClient = { 0 };

        wndThis.GetClientRect(&rcClient);

        int width  = rcClient.right - rcClient.left + em_border_width * 2;
        int height = rcClient.bottom - rcClient.top + em_titlebar_height + em_border_width * 2;

        InitDefaultCaptionBtn();

        ::SetWindowPos(wndThis, NULL, 0, 0,
             width, height,
            SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

        m_rcLastWindow = rcClient;
        wndThis.ClientToScreen(&m_rcLastWindow);

        m_rcLastWindow.right = m_rcLastWindow.left + width;
        m_rcLastWindow.bottom = m_rcLastWindow.top + height;

        ReRgnWindow();

        bHandled = FALSE;

        return 0;
    }

    LRESULT InitDefaultCaptionBtn( )
    {
        CWindow wndThis = static_cast<T*>(this)->m_hWnd;

        BOOL bMinimized = wndThis.GetStyle() & WS_MINIMIZEBOX;
        BOOL bMaximized = wndThis.GetStyle() & WS_MAXIMIZEBOX;

        SkinNCButtonBase* pNewCaptionBtn = NULL;

        int nrightoffset = em_border_width * 2;

        SIZE  sizeBtn  = { em_titlebar_height * 3 / 4, em_titlebar_height * 2 / 3 };
        POINT ptOffset = { nrightoffset , (em_border_width + em_titlebar_height) / 5};

        bool bfirst = true;

        if ( (wndThis.GetStyle() & WS_SYSMENU) || bMaximized || bMinimized )
        {
            pNewCaptionBtn = new SkinCaptionButton( HTCLOSE, 
                SC_CLOSE,
                SkinNCButtonBase::em_align_top | SkinNCButtonBase::em_align_right,
                sizeBtn, 
                ptOffset,
                m_clrMainColor);

            if (pNewCaptionBtn != NULL)
                m_vtNcBtn.push_back( pNewCaptionBtn );

            bfirst = false;
        }

        if (bMaximized)
        {
            if (!bfirst)
            {
                ptOffset.x += (( em_titlebar_height * 3 / 4 ) + em_border_width ) ;
            }

            pNewCaptionBtn = new SkinCaptionButton( HTMAXBUTTON, 
                SC_MAXIMIZE,
                SkinNCButtonBase::em_align_top | SkinNCButtonBase::em_align_right,
                sizeBtn, 
                ptOffset,
                m_clrMainColor);

            if (pNewCaptionBtn != NULL)
                m_vtNcBtn.push_back( pNewCaptionBtn );

            bfirst = false;
        }

        if (bMinimized)
        {
            if (!bfirst)
            {
                ptOffset.x += (( em_titlebar_height * 3 / 4 ) + em_border_width ) ;
            }

            pNewCaptionBtn = new SkinCaptionButton( HTMINBUTTON, 
                SC_MINIMIZE,
                SkinNCButtonBase::em_align_top | SkinNCButtonBase::em_align_right,
                sizeBtn, 
                ptOffset,
                m_clrMainColor);

            if (pNewCaptionBtn != NULL)
                m_vtNcBtn.push_back( pNewCaptionBtn );

            bfirst = false;
        }

        return 1L;
    }

    void ReRgnWindow()
    {
        CWindow wndThis = static_cast<T*>(this)->m_hWnd;

        CRgnHandle rgn;

        RECT rcWindow;
        wndThis.GetWindowRect(&rcWindow);

        rcWindow.right  = rcWindow.right  - rcWindow.left;
        rcWindow.bottom = rcWindow.bottom - rcWindow.top;

        rcWindow.top = rcWindow.left = 0;

        rgn.CreateRoundRectRgn( rcWindow.left, rcWindow.top, 
            rcWindow.right + 1, rcWindow.bottom + 6, 5, 5);

        wndThis.SetWindowRgn(rgn, TRUE);

        if (m_rgnWindow.m_hRgn != NULL)
            m_rgnWindow.DeleteObject();

        m_rgnWindow.Attach( rgn );

    }

    LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CWindow wndThis = static_cast<T*>(this)->m_hWnd;

        CWindowDC dc(wndThis);

        CSkinDCHandle skinDC(dc.m_hDC);

        RECT rcWindow = { 0 };
        RECT rcDraw   = { 0 };

        wndThis.GetWindowRect(&rcWindow);

        rcWindow.right  = rcWindow.right  - rcWindow.left;
        rcWindow.bottom = rcWindow.bottom - rcWindow.top;

        rcWindow.left = 0;
        rcWindow.top  = 0;

        {
            RECT rcBorder = rcWindow;
            

            COLORREF clrLine1 = HLS_TRANSFORM(m_clrMainColor, 80, 0);

            InflateRect(&rcBorder, -1, -1);
            rcBorder.top = rcWindow.top + em_titlebar_height + em_border_width - 1;
            skinDC.SkinFrameRect(rcBorder, clrLine1);
            InflateRect(&rcBorder, -1, -1);
            rcBorder.top = rcWindow.top + em_titlebar_height + em_border_width - 1;
            skinDC.SkinFrameRect(rcBorder, clrLine1);

            rcBorder = rcWindow;
            rcBorder.top = rcWindow.top + em_titlebar_height + em_border_width - 1;
            skinDC.SkinFrameRect(rcBorder, m_clrMainColor);

        }
        

        if (true)
        {
            //rcWindow.top   += 1;
            //rcWindow.left  += 1;
            //rcWindow.right -= 1;
            rcWindow.bottom = rcWindow.top + em_titlebar_height + em_border_width;

            CMemoryDC memDC(dc, rcWindow);

            CSkinDCHandle skinDC(memDC.m_hDC);

            RECT rcGradual = rcWindow;
            rcGradual.bottom = (rcGradual.top + rcGradual.bottom) / 2;

            skinDC.SkinDrawGradualColorRect(rcGradual, 
                HLS_TRANSFORM(m_clrMainColor, 80, -20), 
                HLS_TRANSFORM(m_clrMainColor, 45, 10));

            rcGradual = rcWindow;
            rcGradual.top = (rcGradual.top + rcGradual.bottom) / 2;

            skinDC.SkinDrawGradualColorRect(rcGradual, 
                HLS_TRANSFORM(m_clrMainColor, 35, -10), 
                HLS_TRANSFORM(m_clrMainColor, 05, 10));


            skinDC.SkinLine( rcWindow.left + 1, rcWindow.bottom - 2,
                rcWindow.right - 1, rcWindow.bottom - 2, HLS_TRANSFORM(m_clrMainColor, 40, 20));

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
                LOGFONT logFont;

                CFont font;
            
                CFontHandle fontWnd = wndThis.GetFont();
                fontWnd.GetLogFont(&logFont);
                logFont.lfWeight = 800;

                font.CreateFontIndirect(&logFont);

                HFONT hOldFont = skinDC.SelectFont(font);

                skinDC.SetTextColor(0xFFFFFF);
                skinDC.SetBkMode(TRANSPARENT);

                RECT rcText = { npaintx + 4, em_border_width, rcWindow.right, em_titlebar_height };    
                skinDC.DrawText( szCaption, -1, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE );

                skinDC.SelectFont(hOldFont);

                //skinDC.SkinDrawText(npaintx + 4, em_border_width * 2, szCaption, 0xffffff, font);
            }

            for (size_t idx = 0; idx < m_vtNcBtn.size(); idx++)
            {
                m_vtNcBtn[idx]->DoPaint( skinDC );
            }
        }

        return 0L;
    }

    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;

        ReRgnWindow();

        return 0L;
    }

    LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CWindow wndThis = static_cast<T*>(this)->m_hWnd;

        RECT rcClient = { 0 };
        wndThis.GetClientRect(&rcClient);

        CMemoryDC memDC( (HDC)wParam, rcClient );

        CSkinDCHandle skinDC((HDC)memDC);

        skinDC.SkinDrawGradualColorRect(rcClient, 
            HLS_TRANSFORM(m_clrMainColor, 99, 20),
            HLS_TRANSFORM(m_clrMainColor, 85, 30)); 


        bHandled = TRUE;

        return 1L;
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

        for (size_t idx = 0; idx < m_vtNcBtn.size(); idx++)
        {
            m_vtNcBtn[idx]->ChangedBtnPos(rcWindow);
        }
    }

    
    LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CWindow wndThis = static_cast<T*>(this)->m_hWnd;
        
        if (wndThis.GetStyle() & WS_MINIMIZE)
            return DefWindowProc(wndThis, uMsg, wParam, lParam);

        switch(wParam)
        {
        case SC_MAXIMIZE:
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

            return 0L;

            break;
        }


        return DefWindowProc(wndThis, uMsg, wParam, lParam);
    }

    
    LRESULT OnNcLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CWindow wndThis = static_cast<T*>(this)->m_hWnd;

        int xPos = GET_X_LPARAM(lParam); 
        int yPos = GET_Y_LPARAM(lParam); 

        RECT rcWindow = { 0 };
        wndThis.GetWindowRect(&rcWindow);

        m_pBtnPress = NULL;

        for (size_t idx = 0; idx < m_vtNcBtn.size(); idx++)
        {
            RECT rcButton = m_vtNcBtn[idx]->ButtonRect();

            if ( ::PtInRect( &rcButton, CPoint(xPos - rcWindow.left, yPos - rcWindow.top) ) )                 
            {
                CWindowDC dc(wndThis);
                CSkinDCHandle skinDC(dc.m_hDC);

                m_pBtnPress = m_vtNcBtn[idx];
                m_pBtnPress->Status( SkinNCButtonBase::em_bs_lbuttondown );
                m_pBtnPress->DoPaint( skinDC );
            }
        }

        if (m_pBtnPress != NULL)
        {

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

        RECT rcWindow = { 0 };
        wndThis.GetWindowRect(&rcWindow);
        
        SkinNCButtonBase* pBtnPos = NULL;

        for (size_t idx = 0; idx < m_vtNcBtn.size(); idx++)
        {
            RECT rcButton = m_vtNcBtn[idx]->ButtonRect();

            if ( ::PtInRect( &rcButton, CPoint(xPos - rcWindow.left, yPos - rcWindow.top) ) )                 
            {
                CWindowDC dc(wndThis);
                CSkinDCHandle skinDC(dc.m_hDC);

                pBtnPos = m_vtNcBtn[idx];
                pBtnPos->Status( SkinNCButtonBase::em_bs_normal );
                pBtnPos->DoPaint( skinDC );
            }
        }


        if ( pBtnPos != NULL && pBtnPos == m_pBtnPress)
        {
            switch (m_pBtnPress->NcHitTestCode())
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
                
                if ( m_pLastHotBtn->NcHitTestCode() != NULL )
                {
                    bHandled = FALSE;
                }
                else
                {
                    wndThis.PostMessage(WM_SYSCOMMAND, m_pLastHotBtn->SysCmdId());
                }

            }
        }

        m_pBtnPress = NULL;

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

        if (wParam == HTCAPTION && (wndThis.GetStyle() & WS_MAXIMIZEBOX))
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

        if ( m_pLastHotBtn != NULL )
        {
            CWindowDC dc(wndThis);
            CSkinDCHandle skinDC(dc.m_hDC);
            
            m_pLastHotBtn->Status( SkinNCButtonBase::em_bs_normal );
            m_pLastHotBtn->DoPaint( skinDC );
        }

        SkinNCButtonBase* pLastBtn = m_pLastHotBtn;

        m_pLastHotBtn = NULL;

        for (size_t idx = 0; idx < m_vtNcBtn.size(); idx++)
        {
            RECT rcButton = m_vtNcBtn[idx]->ButtonRect();

            if ( ::PtInRect( &rcButton, CPoint(xPos - rcWindow.left, yPos - rcWindow.top) ) )                 
            {
                CWindowDC dc(wndThis);
                CSkinDCHandle skinDC(dc.m_hDC);
                    
                m_pLastHotBtn = m_vtNcBtn[idx];
                m_pLastHotBtn->Status( SkinNCButtonBase::em_bs_mouseover );
                m_pLastHotBtn->DoPaint( skinDC );
            }
        }

        if (pLastBtn == NULL &&
            m_pLastHotBtn != NULL)
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

        if ( m_pLastHotBtn != NULL )
        {
            CWindowDC dc(wndThis);
            CSkinDCHandle skinDC(dc.m_hDC);

            m_pLastHotBtn->Status( SkinNCButtonBase::em_bs_normal );
            m_pLastHotBtn->DoPaint( skinDC );

            m_pLastHotBtn = NULL;
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

        rcTitlebar.bottom = rcTitlebar.top + em_titlebar_height + em_border_width;

        for (size_t idx = 0; idx < m_vtNcBtn.size(); idx++)
        {
            RECT rcButton = m_vtNcBtn[idx]->ButtonRect();

            if ( m_vtNcBtn[idx]->NcHitTestCode() != 0 &&
                 ::PtInRect( &rcButton, CPoint(xPos - rcTitlebar.left, yPos - rcTitlebar.top) ) )                 
            {
                return m_vtNcBtn[idx]->NcHitTestCode();
            }
        }


        if (::PtInRect( &rcTitlebar, CPoint(xPos, yPos) ))
            return HTCAPTION;

        return lResult;
    }

    LRESULT OnSetText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CWindow wndThis = static_cast<T*>(this)->m_hWnd;

        LRESULT lResult = ::DefWindowProc(wndThis, uMsg, wParam, lParam);

        ::SetWindowPos(wndThis, NULL, 0, 0, 0, 0,
            SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOZORDER | SWP_NOSIZE );

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
        
        if ( uMsg == WM_PAINT )
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
