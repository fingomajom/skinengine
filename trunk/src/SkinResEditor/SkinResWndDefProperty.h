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
    


    static int GetResWndPropertyEditType(KSG::CString strProperty)
    {
        int nresult = SkinPropertyView::it_text;

        WndPropertyEntryItem* pItemList = NULL;

        int nItemCount = GetWndPropertyEntry(pItemList);

        for ( int i = 0; i < nItemCount; i++ )
        {
            if ( !_tcscmp(strProperty, pItemList[i].strProperty) )
            {
                nresult = pItemList[i].nedittype;

                break;
            }
        }


        return nresult;
    }


    static void GetDefClassNameList(std::vector<KSG::CString>& vtClassName)
    {
        vtClassName.clear();

        WndClassDefInfo* pItemList = NULL;

        int nItemCount = GetWndClassDefInfoEntry(pItemList);

        for ( int i = 0; i < nItemCount; i++ )
        {
            vtClassName.push_back(pItemList[i].strWndClassName);
        }

    }



    static void GetWndStyle(KSG::CString strClassName, KSG::CString& strStyle)
    {        
        DWORD dwStyle = (DWORD)_ttoi64(strStyle);

        WndClassDefInfo* pItemList = NULL;

        int nItemCount = GetWndClassDefInfoEntry(pItemList);

        for ( int i = 0; i < nItemCount; i++ )
        {
            if ( !_tcscmp(strClassName, pItemList[i].strWndClassName) )
            {
                dwStyle |= pItemList[i].dwHasStyle;

                break;
            }
        }

        strStyle.Format(_T("%ld"), dwStyle);
    }

    static void GetWndExStyle(KSG::CString strClassName, KSG::CString& strStyle)
    {
        DWORD dwStyle = (DWORD)_ttoi64(strStyle);

        WndClassDefInfo* pItemList = NULL;

        int nItemCount = GetWndClassDefInfoEntry(pItemList);

        for ( int i = 0; i < nItemCount; i++ )
        {
            if ( !_tcscmp(strClassName, pItemList[i].strWndClassName) )
            {
                dwStyle |= pItemList[i].dwHasExStyle;

                break;
            }
        }

        strStyle.Format(_T("%ld"), dwStyle);
    }



private:

    static void GetResWndDefProperty( SkinWndPropertyList& WndProperty )
    {
        WndPropertyEntryItem* pItemList = NULL;

        int nItemCount = GetWndPropertyEntry(pItemList);

        for ( int i = 0; i < nItemCount; i++ )
        {
            SkinWndPropertyList::WndPropertyItem item;

            item.strProperty = pItemList[i].strProperty;
            item.strValue    = pItemList[i].strDefValue;

            WndProperty.m_vtPropertyList.push_back( item );
        }
    }

    struct WndPropertyEntryItem
    {
        LPCTSTR strProperty;
        LPCTSTR strDefValue;
        int     nedittype;
    };

    static int GetWndPropertyEntry( WndPropertyEntryItem*& pItemList )
    {
        static WndPropertyEntryItem DefPropertyList[] = 
        {
            { _T("IdName")       , _T("")           , SkinPropertyView::it_text},
            { _T("Caption")      , _T("")           , SkinPropertyView::it_text},
            { _T("SkinClassName"), _T("")           , SkinPropertyView::it_readonly},
            { _T("ItemId")       , _T("0")          , SkinPropertyView::it_number},
            { _T("Style")        , _T("0")          , SkinPropertyView::it_number},
            { _T("ExStyle")      , _T("0")          , SkinPropertyView::it_number},
            { _T("Left")         , _T("0")          , SkinPropertyView::it_number},
            { _T("Top")          , _T("0")          , SkinPropertyView::it_number},
            { _T("Width")        , _T("100")        , SkinPropertyView::it_number},
            { _T("Height")       , _T("20")         , SkinPropertyView::it_number},
            { _T("TextColor")    , _T("0x00000000") , SkinPropertyView::it_color},
            { _T("BkColor")      , _T("0x00ffffff") , SkinPropertyView::it_color},
            { _T("Font")         , _T("����,GB2312_CHARSET,9,1,400,0"), SkinPropertyView::it_font }
        };
        
        pItemList = DefPropertyList;

        return sizeof(DefPropertyList) / sizeof(WndPropertyEntryItem);
    }

    struct WndClassDefInfo
    {
        LPCTSTR strWndClassName;
        DWORD   dwHasStyle;
        DWORD   dwHasExStyle;

    };

    static int GetWndClassDefInfoEntry( WndClassDefInfo*& pItemList )
    {
        static WndClassDefInfo WndClassDefInfoyList[] = 
        {
            { skinxmlstatic::GetSkinWndClassName()       , 0, 0 },
            { skinxmlpicture::GetSkinWndClassName()      , 0, 0 },

            { skinxmlgroupbox::GetSkinWndClassName()     , 0, 0 },
            { skinxmlbutton::GetSkinWndClassName()       , 0, 0 },
            { skinxmlcheckbox::GetSkinWndClassName()     , 0, 0 },
            { skinxmlradiobox::GetSkinWndClassName()     , 0, 0 },

            { skinxmlcombobox::GetSkinWndClassName()     , 0, 0 },
            { skinxmllistbox::GetSkinWndClassName()      , 0, 0 },

            { skinxmllistctrl::GetSkinWndClassName()     , 0, 0 },
            { skinxmltreectrl::GetSkinWndClassName()     , 0, 0 },
            { skinxmltablectrl::GetSkinWndClassName()    , 0, 0 },

            { skinxmledit::GetSkinWndClassName()         , 0, 0 },
            { skinxmlhotkey::GetSkinWndClassName()       , 0, 0 },
            { skinxmlrichedit::GetSkinWndClassName()     , 0, 0 },

            { skinxmldatetime::GetSkinWndClassName()     , 0, 0 },
            { skinxmlmonthcalendar::GetSkinWndClassName(), 0, 0 },

            { skinxmlhscrollbar::GetSkinWndClassName()   , 0, 0 },
            { skinxmlvscrollbar::GetSkinWndClassName()   , 0, 0 },

            { skinxmlslider::GetSkinWndClassName()       , 0, 0 },
            { skinxmlspin::GetSkinWndClassName()         , 0, 0 },
            { skinxmlprogress::GetSkinWndClassName()     , 0, 0 },

            { skinxmlanimation::GetSkinWndClassName()    , 0, 0 },
            { skinxmlipaddress::GetSkinWndClassName()    , 0, 0 }

        };

        pItemList = WndClassDefInfoyList;

        return sizeof(WndClassDefInfoyList) / sizeof(WndClassDefInfo);
    }

};