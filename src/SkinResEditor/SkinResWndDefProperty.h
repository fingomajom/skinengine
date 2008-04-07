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
    
    static void GetResClassWndDefProperty(KSGUI::CString strClassName,
        SkinWndPropertyList& WndProperty)
    {
        GetResWndDefProperty(WndProperty);
    }
    


    static int GetResWndPropertyEditType(KSGUI::CString strProperty)
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


    static void GetDefClassNameList(std::vector<KSGUI::CString>& vtClassName)
    {
        vtClassName.clear();

        WndClassDefInfo* pItemList = NULL;

        int nItemCount = GetWndClassDefInfoEntry(pItemList);

        for ( int i = 0; i < nItemCount; i++ )
        {
            vtClassName.push_back(pItemList[i].strWndClassName);
        }

    }



    static void GetWndStyle(KSGUI::CString strClassName, KSGUI::CString& strStyle)
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

        strStyle.Format(_T("0x%08x"), dwStyle);
    }

    static void GetWndExStyle(KSGUI::CString strClassName, KSGUI::CString& strStyle)
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

        strStyle.Format(_T("0x%08x"), dwStyle);
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
            { _T("Font")         , _T("ËÎÌו,GB2312_CHARSET,-12,0,400,0"), SkinPropertyView::it_font }
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
            { skinxmlstatic::GetSkinWndClassName()       , WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOPREFIX, 0 },
            { skinxmlpicture::GetSkinWndClassName()      , WS_CHILD | WS_VISIBLE | SS_BITMAP, 0 },

            { skinxmlgroupbox::GetSkinWndClassName()     , WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 0 },
            { skinxmlbutton::GetSkinWndClassName()       , WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 0 },
            { skinxmlcheckbox::GetSkinWndClassName()     , WS_CHILD | WS_VISIBLE | BS_CHECKBOX | WS_TABSTOP, 0 },
            { skinxmlradiobox::GetSkinWndClassName()     , WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | WS_TABSTOP, 0 },

            { skinxmlcombobox::GetSkinWndClassName()     , WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWN | WS_VSCROLL , 0 },
            { skinxmllistbox::GetSkinWndClassName()      , WS_CHILD | WS_VISIBLE | WS_TABSTOP, WS_EX_CLIENTEDGE },

            { skinxmllistctrl::GetSkinWndClassName()     , WS_CHILD | WS_VISIBLE | WS_TABSTOP, WS_EX_CLIENTEDGE },
            { skinxmltreectrl::GetSkinWndClassName()     , WS_CHILD | WS_VISIBLE | WS_TABSTOP, WS_EX_CLIENTEDGE },
            { skinxmltablectrl::GetSkinWndClassName()    , WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0 },

            { skinxmledit::GetSkinWndClassName()         , WS_CHILD | WS_VISIBLE | WS_TABSTOP, WS_EX_CLIENTEDGE },
            { skinxmlhotkey::GetSkinWndClassName()       , WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0 },
            { skinxmlrichedit::GetSkinWndClassName()     , WS_CHILD | WS_VISIBLE | WS_TABSTOP, WS_EX_CLIENTEDGE },

            { skinxmldatetime::GetSkinWndClassName()     , WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0 },
            { skinxmlmonthcalendar::GetSkinWndClassName(), WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0 },

            { skinxmlhscrollbar::GetSkinWndClassName()   , WS_CHILD | WS_VISIBLE, 0 },
            { skinxmlvscrollbar::GetSkinWndClassName()   , WS_CHILD | WS_VISIBLE, 0 },

            { skinxmlslider::GetSkinWndClassName()       , WS_CHILD | WS_VISIBLE, 0 },
            { skinxmlspin::GetSkinWndClassName()         , WS_CHILD | WS_VISIBLE, 0 },
            { skinxmlprogress::GetSkinWndClassName()     , WS_CHILD | WS_VISIBLE, 0 },

            { skinxmlanimation::GetSkinWndClassName()    , WS_CHILD | WS_VISIBLE, 0 },
            { skinxmlipaddress::GetSkinWndClassName()    , WS_CHILD | WS_VISIBLE | WS_TABSTOP, WS_EX_CLIENTEDGE },

            { skinxmlstaticex::GetSkinWndClassName()    , WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0 },
            { skinxmlbmpbtn::GetSkinWndClassName()    , WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0 },
            { skinxmlclrbtn::GetSkinWndClassName()    , WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0 }

        };

        pItemList = WndClassDefInfoyList;

        return sizeof(WndClassDefInfoyList) / sizeof(WndClassDefInfo);
    }

};