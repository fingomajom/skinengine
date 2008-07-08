/********************************************************************
* CreatedOn: 2008-1-11   03:08
* FileName: skinwincreator.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include <map>
#include "skinctrls.h"
#include "skinxmlctrls.h"
#include "skinctrlx.h"

namespace KSGUI{


template<class SkinWindowType>
SkinWindow* SkinCreate_Static(
    const SkinXmlElement& xmlElement,
    HWND hWndParent, _U_MENUorID MenuOrID = 0U )
{
    SkinWindowType * pSkinWindow = new SkinWindowType;

    if (pSkinWindow == NULL)
        return pSkinWindow;

    if (pSkinWindow->SkinCreate(xmlElement, hWndParent, MenuOrID) == NULL)
    {
        delete pSkinWindow;

        pSkinWindow = NULL;
    }

    return pSkinWindow;
}


class SkinWindowCreator
{

protected:

    typedef SkinWindow* pfnSkinCreateFunc(const SkinXmlElement& xmlElement,
        HWND hWndParent, _U_MENUorID MenuOrID );

public:

    SkinWindow* SkinCreate(
        const SkinXmlElement& xmlElement,
        HWND hWndParent, _U_MENUorID MenuOrID = 0U )
    {
        SkinWindow * pSkinWindow = NULL;

        skinxmlwin   xmlWin(xmlElement);
        KSGUI::CString strClassName;

        if (!xmlWin.GetSkinClassName(strClassName))
            return pSkinWindow;

        std::map<KSGUI::CString, pfnSkinCreateFunc*>::const_iterator iter = 
            m_mapCreator.find(strClassName);

        if (iter == m_mapCreator.end())
            return pSkinWindow;
        
        pfnSkinCreateFunc * pCreatorFunc = iter->second;

        if (pCreatorFunc != NULL)
            pSkinWindow = pCreatorFunc(xmlElement, hWndParent, MenuOrID);

        return pSkinWindow;
    }

    static SkinWindowCreator& Instance()
    {
        static SkinWindowCreator g_skinWinCreator;

        return g_skinWinCreator;
    }

    void AddSkinCreator( const KSGUI::CString& strWndClass, 
        pfnSkinCreateFunc* pfnCreator)
    {
        m_mapCreator[strWndClass] = pfnCreator;
    }


protected:


    SkinWindowCreator()
    {
        AddSkinCreator(KSGUI::skinxmlstatic::GetSkinWndClassName(), 
            SkinCreate_Static<CSkinStatic>);
        AddSkinCreator(KSGUI::skinxmlpicture::GetSkinWndClassName(), 
            SkinCreate_Static<CSkinStaticPicture>);
        
        AddSkinCreator(KSGUI::skinxmlgroupbox::GetSkinWndClassName(), 
            SkinCreate_Static<CSkinButton>);
        AddSkinCreator(KSGUI::skinxmlbutton::GetSkinWndClassName(), 
            SkinCreate_Static<CSkinButton>);
        AddSkinCreator(KSGUI::skinxmlcheckbox::GetSkinWndClassName(), 
            SkinCreate_Static<CSkinButton>);
        AddSkinCreator(KSGUI::skinxmlcheckboxex::GetSkinWndClassName(), 
            SkinCreate_Static<CSkinCheckboxButton>);
        AddSkinCreator(KSGUI::skinxmlradiobox::GetSkinWndClassName(), 
            SkinCreate_Static<CSkinButton>);


        AddSkinCreator(KSGUI::skinxmlcombobox::GetSkinWndClassName(), 
            SkinCreate_Static<CSkinComboBox>);

        AddSkinCreator(KSGUI::skinxmllistbox::GetSkinWndClassName(), 
            SkinCreate_Static<CSkinListBox>);

        AddSkinCreator(KSGUI::skinxmllistctrl::GetSkinWndClassName(), 
            SkinCreate_Static<CSkinListViewCtrl>);

        AddSkinCreator(KSGUI::skinxmltreectrl::GetSkinWndClassName(), 
            SkinCreate_Static<CSkinTreeViewCtrl>);

        AddSkinCreator(KSGUI::skinxmltablectrl::GetSkinWndClassName(), 
            SkinCreate_Static<CSkinTabCtrl>);

        AddSkinCreator(KSGUI::skinxmledit::GetSkinWndClassName(), 
            SkinCreate_Static<CSkinEdit>);

        AddSkinCreator(KSGUI::skinxmlhotkey::GetSkinWndClassName(), 
            SkinCreate_Static<CSkinHotKeyCtrl>);

        AddSkinCreator(KSGUI::skinxmlrichedit::GetSkinWndClassName(), 
            SkinCreate_Static<CSkinRichEditCtrl>);

        AddSkinCreator(KSGUI::skinxmldatetime::GetSkinWndClassName(), 
            SkinCreate_Static<CSkinDateTimePickerCtrl>);

        AddSkinCreator(KSGUI::skinxmlmonthcalendar::GetSkinWndClassName(), 
            SkinCreate_Static<CSkinMonthCalendarCtrl>);

        AddSkinCreator(KSGUI::skinxmlhscrollbar::GetSkinWndClassName(), 
            SkinCreate_Static<CSkinScrollBar>);

        AddSkinCreator(KSGUI::skinxmlvscrollbar::GetSkinWndClassName(), 
            SkinCreate_Static<CSkinScrollBar>);

        AddSkinCreator(KSGUI::skinxmlslider::GetSkinWndClassName(), 
            SkinCreate_Static<CSkinTrackBarCtrl>);

        AddSkinCreator(KSGUI::skinxmlspin::GetSkinWndClassName(), 
            SkinCreate_Static<CSkinUpDownCtrl>);

        AddSkinCreator(KSGUI::skinxmlprogress::GetSkinWndClassName(), 
            SkinCreate_Static<CSkinProgressBarCtrl>);
        AddSkinCreator(KSGUI::skinxmlanimation::GetSkinWndClassName(), 
            SkinCreate_Static<CSkinAnimateCtrl>);
        AddSkinCreator(KSGUI::skinxmlipaddress::GetSkinWndClassName(), 
            SkinCreate_Static<CSkinIPAddressCtrl>);

        //////////////////////////////////////////////////////////////////////////
        // ctrlx
        AddSkinCreator(KSGUI::skinxmlstaticex::GetSkinWndClassName(), 
            CSkinStaticEx::SkinCreate_Static);
        AddSkinCreator(KSGUI::skinxmlbmpbtn::GetSkinWndClassName(), 
            CSkinBitmapButton::SkinCreate_Static);
        AddSkinCreator(KSGUI::skinxmlclrbtn::GetSkinWndClassName(), 
            CSkinColorButton::SkinCreate_Static);

        AddSkinCreator(KSGUI::skinxmlhyperlink::GetSkinWndClassName(), 
            SkinHyperLink::SkinCreate_Static);
        AddSkinCreator(KSGUI::skinxmltablectrlex::GetSkinWndClassName(), 
            SkinTableCtrlEx::SkinCreate_Static);       

    }

    std::map<KSGUI::CString, pfnSkinCreateFunc*> m_mapCreator;
};



};