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


namespace KSGUI{


class CSkinDlgChildList : public KSGUI::skinxmldialog::enumchildwincallback
{
public:
    
    //KSGUI::skinxmldialog::enumchildwincallback

    BOOL onchildwin( skinxmlwin& xmlWin )
    {
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
    
    std::map<KSGUI::CString, SkinWindow*> m_mapIDS2Win;

    HWND m_hWndParent;
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

    void _InitXmlDlg()
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

};// namespace KSGUI
