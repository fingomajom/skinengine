/********************************************************************
* CreatedOn: 2008-1-8   23:50
* FileName:  skinctrls.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include "skinxmlctrls.h"
#include "skinwin.h"
#include "skingdi.h"
#include "ksguicommon/drawer/StandardDrawer.h"
#include <atlctrls.h>


///////////////////////////////////////////////////////////////////////////////
// Classes in this file:
//
// CSkinStaticT<TBase> - CSkinStatic


namespace KSGUI{

//
//CSkinStatic;
//CSkinButton;
//
//CSkinListBox;
//CSkinComboBox;
//CSkinEdit;
//CSkinScrollBar;
//
//
//CSkinListViewCtrl;
//CSkinTreeViewCtrl;
//
//CSkinTabCtrl;
//CSkinUpDownCtrl;
//
//CSkinProgressBarCtrl;
//CSkinHotKeyCtrl;
//CSkinAnimateCtrl;
//CSkinRichEditCtrl;
//
//CSkinDateTimePickerCtrl;
//CSkinMonthCalendarCtrl;
//
//CSkinIPAddressCtrl;
//CSkinPagerCtrl;


template <class TBase>
class CSkinCtrlsT : public TBase
{
public:

    CSkinCtrlsT(HWND hWnd = NULL) : TBase(hWnd)
    { }

    CSkinCtrlsT< TBase >& operator =(HWND hWnd)
    {
        m_hWnd = hWnd;
        return *this;
    }

    HWND SkinCreate( 
        const SkinXmlElement& xmlElement,
        HWND hWndParent, _U_MENUorID MenuOrID = 0U ) throw()
    {
        HWND hWndResult = TBase::SkinCreate(xmlElement, 
            hWndParent,              
            GetWndClassName(),
            MenuOrID);

        if (hWndResult == NULL)
            return hWndResult;

        if (hWndResult != NULL && hWndParent != NULL)
            SetFont( TBase(hWndParent).GetFont() );

        return hWndResult;
    }

    HWND SkinCreate( 
        const SkinXmlElement& xmlElement,
        HWND hWndParent, LPCTSTR szClassName, _U_MENUorID MenuOrID = 0U ) throw()
    {
        HWND hWndResult = TBase::SkinCreate(xmlElement, 
            hWndParent,              
            szClassName,
            MenuOrID);

        if (hWndResult == NULL)
            return hWndResult;

        if (hWndResult != NULL && hWndParent != NULL)
            SetFont( TBase(hWndParent).GetFont() );

        return hWndResult;
    }

};


typedef CSkinCtrlsT< CStaticT<KSGUI::SkinWindow> >    CSkinStatic;
typedef CSkinCtrlsT< CButtonT<KSGUI::SkinWindow> >    CSkinButton;

typedef CSkinCtrlsT< CListBoxT<KSGUI::SkinWindow> >   CSkinListBox;
typedef CSkinCtrlsT< CComboBoxT<KSGUI::SkinWindow> >  CSkinComboBox;
typedef CSkinCtrlsT< CEditT<KSGUI::SkinWindow> >      CSkinEdit;
typedef CSkinCtrlsT< CScrollBarT<KSGUI::SkinWindow> > CSkinScrollBar;


typedef CSkinCtrlsT< CListViewCtrlT<KSGUI::SkinWindow> > CSkinListViewCtrl;
typedef CSkinCtrlsT< CTreeViewCtrlT<KSGUI::SkinWindow> > CSkinTreeViewCtrl;

typedef CSkinCtrlsT< CTabCtrlT<KSGUI::SkinWindow> >       CSkinTabCtrl;
typedef CSkinCtrlsT< CUpDownCtrlT<KSGUI::SkinWindow> >    CSkinUpDownCtrl;

typedef CSkinCtrlsT< CProgressBarCtrlT<KSGUI::SkinWindow> > CSkinProgressBarCtrl;
typedef CSkinCtrlsT< CHotKeyCtrlT<KSGUI::SkinWindow> >      CSkinHotKeyCtrl;
typedef CSkinCtrlsT< CAnimateCtrlT<KSGUI::SkinWindow> >     CSkinAnimateCtrl;
typedef CSkinCtrlsT< CRichEditCtrlT<KSGUI::SkinWindow> >    CSkinRichEditCtrl;

typedef CSkinCtrlsT< CDateTimePickerCtrlT<KSGUI::SkinWindow> > CSkinDateTimePickerCtrl;
typedef CSkinCtrlsT< CMonthCalendarCtrlT<KSGUI::SkinWindow> >  CSkinMonthCalendarCtrl;

typedef CSkinCtrlsT< CIPAddressCtrlT<KSGUI::SkinWindow> >    CSkinIPAddressCtrl;
typedef CSkinCtrlsT< CTrackBarCtrlT<KSGUI::SkinWindow> >     CSkinTrackBarCtrl;


class CSkinStaticPicture : public CSkinCtrlsT< CStaticT<KSGUI::SkinWindow> >
{
    typedef CSkinCtrlsT< CStaticT<KSGUI::SkinWindow> > theBase;
public:

    HWND SkinCreate( 
        const SkinXmlElement& xmlElement,
        HWND hWndParent, _U_MENUorID MenuOrID = 0U ) throw()
    {
        HWND hWndResult = theBase::SkinCreate(xmlElement, 
            hWndParent,              
            MenuOrID);

        skinxmlwin xmlwin(xmlElement);

        KSGUI::CString strBitbmpId;

        if ( xmlwin.GetObject(_T("Bitbmp"), strBitbmpId ) ) 
        {
            SetBitmap(strBitbmpId);
        }

        return hWndResult;
    }

    BOOL SetBitmap( LPCTSTR pszBmpResName )
    {
        if (m_bmp.m_hBitmap != NULL)
            m_bmp.DeleteObject();

        m_bmp.Attach( SkinLoadBitmap(pszBmpResName) );

        if (m_bmp.m_hBitmap != NULL)
        { 
            theBase::SetBitmap(m_bmp);
        }

        return TRUE;
    }

public:

    CBitmap m_bmp;
};


class CSkinCheckboxButton : public SkinWindowImpl<CSkinCheckboxButton, CButtonT<KSGUI::SkinWindow> >
{    
    typedef SkinWindowImpl<CSkinCheckboxButton, CButtonT<KSGUI::SkinWindow> > theBase;

public:

    CSkinCheckboxButton()
    {
    }

    HWND SkinCreate( 
        const SkinXmlElement& xmlElement,
        HWND hWndParent, _U_MENUorID MenuOrID = 0U ) throw()
    {
        HWND hWndResult = theBase::SkinCreate(xmlElement, 
            hWndParent,
            MenuOrID);
            
        if (hWndResult == NULL)
            return hWndResult;

        if (hWndResult != NULL && hWndParent != NULL)
            SetFont( CButtonT<KSGUI::SkinWindow>(hWndParent).GetFont() );
        
        return hWndResult;
    }


    BEGIN_MSG_MAP(CSkinCheckboxButton)

        MESSAGE_HANDLER(WM_PAINT     , OnPaint)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
        MESSAGE_HANDLER(WM_SETTEXT   , OnSetText)
        MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)

        MESSAGE_HANDLER(WM_LBUTTONDOWN    , OnRefWindowMsg)
        MESSAGE_HANDLER(WM_LBUTTONUP      , OnRefWindowMsg)
        MESSAGE_HANDLER(WM_NCLBUTTONDBLCLK, OnRefWindowMsg)
        MESSAGE_HANDLER(WM_KEYDOWN        , OnRefWindowMsg)
        MESSAGE_HANDLER(WM_KEYUP          , OnRefWindowMsg)

        MESSAGE_HANDLER(WM_MOUSEMOVE      , OnMouseMove   )

    END_MSG_MAP()

    LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 0L;
    }

    LRESULT OnRefWindowMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {        
        LRESULT lResult = DefWindowProc();

        InvalidateParentClient();

        return lResult;        
    }

    LRESULT OnSetText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        InvalidateParentClient();

        return DefWindowProc();
    }

    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if (wParam == 0)
        {
            CPaintDC dc(m_hWnd);

            DoPaint(dc.m_hDC);
        }
        else
        {
            DoPaint((HDC)wParam);
        }

        return 0L;
    }


    void DoPaint( CSkinDCHandle skinDC )
    {
        BOOL bCheck = GetCheck();

        RECT rcClient = { 0 };
        RECT rcCheck  = { 0 };

        GetClientRect(&rcClient);

        rcCheck = rcClient;

        const int nWidth = 13;

        rcCheck.top    = (rcCheck.bottom - rcCheck.top) / 2 - nWidth/2 - 1;
        rcCheck.bottom = rcCheck.top + nWidth;
        rcCheck.right  = nWidth;

        //skinDC.DrawFrameControl(&rcCheck, DFC_BUTTON, 
        //    DFCS_BUTTONCHECK | bCheck ? DFCS_CHECKED : 0);

        DrawCheckBox( skinDC, rcCheck.left, rcCheck.top,
            DFCS_BUTTONCHECK | bCheck ? CBS_CHECKED | CBS_NORMAL : CBS_NORMAL );

        rcClient.left += nWidth * 4 / 3;

        TCHAR szBuffer[MAX_PATH] = { 0 };
        GetWindowText(szBuffer, MAX_PATH);

        if ( _tcslen(szBuffer) > 0 )
        {
            skinDC.SkinDrawText(&rcClient, szBuffer, 
                DT_VCENTER | DT_SINGLELINE | DT_LEFT, 0L, GetFont());
        }

    }


    LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 1L;
    }
};




class SkinRichEditInit
{
public:

    SkinRichEditInit()
    {
        m_hRichEditModule = LoadLibrary(CRichEditCtrl::GetLibraryName());
    }
    
    ~SkinRichEditInit()
    {
        if (m_hRichEditModule != NULL)
            FreeLibrary(m_hRichEditModule);
    }

public:
    HMODULE m_hRichEditModule;
};
//////////////////////////////////////////////////////////////////////////

}; // namespace KSGUI