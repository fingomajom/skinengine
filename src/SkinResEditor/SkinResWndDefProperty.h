/********************************************************************
* CreatedOn: 2008-2-18   18:36
* FileName: SkinResWndDefProperty.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include "SkinDialogListResDoc.h"

class SkinResWndDefProperty
{
public:
    
    static void GetResClassWndDefProperty(KSG::CString strClassName,
        SkinWndPropertyList& WndProperty)
    {
        GetResWndDefProperty(WndProperty);
    }
    

    static void GetResWndDefProperty( SkinWndPropertyList& WndProperty )
    {
        static SkinWndPropertyList::WndPropertyItem DefPropertyList[] = 
        {
            { _T("IdName")       , _T("") },
            { _T("Caption")      , _T("") },
            { _T("SkinClassName"), _T("") },
            { _T("ItemId")       , _T("0") },
            { _T("Style")        , _T("0") },
            { _T("ExStyle")      , _T("0") },
            { _T("Left")         , _T("0") },
            { _T("Top")          , _T("0") },
            { _T("Width")        , _T("100") },
            { _T("Height")       , _T("20") },
            { _T("TextColor")    , _T("0x00000000") },
            { _T("BkColor")      , _T("0x00ffffff") },
            { _T("Font")         , _T("ËÎÌו,GB2312_CHARSET,9,1,400,0") }
            
        };
        
        for ( int i = 0; i < (sizeof(DefPropertyList) / sizeof(SkinWndPropertyList::WndPropertyItem)); i++ )
        {
            WndProperty.m_vtPropertyList.push_back( DefPropertyList[i] );
        }
    }

    static int GetResWndPropertyEditType(KSG::CString strClassName)
    {
        int nresult = SkinPropertyView::it_text;

        if (!strClassName.Compare(_T("IdName")))
        {
            nresult = SkinPropertyView::it_text;
        }
        else if (!strClassName.Compare(_T("Caption")))
        {
            nresult = SkinPropertyView::it_text;
        }
        else if (!strClassName.Compare(_T("SkinClassName")))
        {
            nresult = SkinPropertyView::it_text;
        }
        else if (!strClassName.Compare(_T("ItemId")))
        {
            nresult = SkinPropertyView::it_text;
        }
        else if (!strClassName.Compare(_T("Style")))
        {
            nresult = SkinPropertyView::it_number;
        }
        else if (!strClassName.Compare(_T("ExStyle")))
        {
            nresult = SkinPropertyView::it_number;
        }
        else if (!strClassName.Compare(_T("Left")))
        {
            nresult = SkinPropertyView::it_number;
        }
        else if (!strClassName.Compare(_T("Top")))
        {
            nresult = SkinPropertyView::it_number;
        }
        else if (!strClassName.Compare(_T("Width")))
        {
            nresult = SkinPropertyView::it_number;
        }
        else if (!strClassName.Compare(_T("Height")))
        {
            nresult = SkinPropertyView::it_number;
        }
        else if (!strClassName.Compare(_T("TextColor")))
        {
            nresult = SkinPropertyView::it_color;
        }
        else if (!strClassName.Compare(_T("BkColor")))
        {
            nresult = SkinPropertyView::it_color;
        }
        else if (!strClassName.Compare(_T("Font")))
        {
            nresult = SkinPropertyView::it_font;
        }

        return nresult;
    }


    static void GetDefClassNameList(std::vector<KSG::CString>& vtClassName)
    {
        vtClassName.clear();

        vtClassName.push_back(skinxmlstatic::GetSkinWndClassName());
        vtClassName.push_back(skinxmlpicture::GetSkinWndClassName());

        vtClassName.push_back(skinxmlgroupbox::GetSkinWndClassName());
        vtClassName.push_back(skinxmlbutton::GetSkinWndClassName());
        vtClassName.push_back(skinxmlcheckbox::GetSkinWndClassName());
        vtClassName.push_back(skinxmlradiobox::GetSkinWndClassName());

        vtClassName.push_back(skinxmlcombobox::GetSkinWndClassName());
        vtClassName.push_back(skinxmllistbox::GetSkinWndClassName());

        vtClassName.push_back(skinxmllistctrl::GetSkinWndClassName());
        vtClassName.push_back(skinxmltreectrl::GetSkinWndClassName());
        vtClassName.push_back(skinxmltablectrl::GetSkinWndClassName());

        vtClassName.push_back(skinxmledit::GetSkinWndClassName());
        vtClassName.push_back(skinxmlhotkey::GetSkinWndClassName());
        vtClassName.push_back(skinxmlrichedit::GetSkinWndClassName());

        vtClassName.push_back(skinxmldatetime::GetSkinWndClassName());
        vtClassName.push_back(skinxmlmonthcalendar::GetSkinWndClassName());

        vtClassName.push_back(skinxmlhscrollbar::GetSkinWndClassName());
        vtClassName.push_back(skinxmlvscrollbar::GetSkinWndClassName());

        vtClassName.push_back(skinxmlslider::GetSkinWndClassName());
        vtClassName.push_back(skinxmlspin::GetSkinWndClassName());
        vtClassName.push_back(skinxmlprogress::GetSkinWndClassName());

        vtClassName.push_back(skinxmlanimation::GetSkinWndClassName());
        vtClassName.push_back(skinxmlipaddress::GetSkinWndClassName());

    }

};