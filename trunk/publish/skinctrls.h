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

#include "skinxmlctrls.h"
#include "skinwin.h"
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

//////////////////////////////////////////////////////////////////////////

}; // namespace KSGUI