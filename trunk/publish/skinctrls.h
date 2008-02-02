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


namespace KSG{

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
        return TBase::SkinCreate(xmlElement, 
            hWndParent,              
            GetWndClassName(),
            MenuOrID);
    }

};


typedef CSkinCtrlsT< CStaticT<KSG::SkinWindow> >    CSkinStatic;
typedef CSkinCtrlsT< CButtonT<KSG::SkinWindow> >    CSkinButton;

typedef CSkinCtrlsT< CListBoxT<KSG::SkinWindow> >   CSkinListBox;
typedef CSkinCtrlsT< CComboBoxT<KSG::SkinWindow> >  CSkinComboBox;
typedef CSkinCtrlsT< CEditT<KSG::SkinWindow> >      CSkinEdit;
typedef CSkinCtrlsT< CScrollBarT<KSG::SkinWindow> > CSkinScrollBar;


typedef CSkinCtrlsT< CListViewCtrlT<KSG::SkinWindow> > CSkinListViewCtrl;
typedef CSkinCtrlsT< CTreeViewCtrlT<KSG::SkinWindow> > CSkinTreeViewCtrl;

typedef CSkinCtrlsT< CTabCtrlT<KSG::SkinWindow> >       CSkinTabCtrl;
typedef CSkinCtrlsT< CUpDownCtrlT<KSG::SkinWindow> >    CSkinUpDownCtrl;

typedef CSkinCtrlsT< CProgressBarCtrlT<KSG::SkinWindow> > CSkinProgressBarCtrl;
typedef CSkinCtrlsT< CHotKeyCtrlT<KSG::SkinWindow> >      CSkinHotKeyCtrl;
typedef CSkinCtrlsT< CAnimateCtrlT<KSG::SkinWindow> >     CSkinAnimateCtrl;
typedef CSkinCtrlsT< CRichEditCtrlT<KSG::SkinWindow> >    CSkinRichEditCtrl;

typedef CSkinCtrlsT< CDateTimePickerCtrlT<KSG::SkinWindow> > CSkinDateTimePickerCtrl;
typedef CSkinCtrlsT< CMonthCalendarCtrlT<KSG::SkinWindow> >  CSkinMonthCalendarCtrl;

typedef CSkinCtrlsT< CIPAddressCtrlT<KSG::SkinWindow> > CSkinIPAddressCtrl;
typedef CSkinCtrlsT< CTrackBarCtrlT<KSG::SkinWindow> >     CSkinTrackBarCtrl;



//////////////////////////////////////////////////////////////////////////

}; // namespace KSG