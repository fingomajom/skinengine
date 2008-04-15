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
        LPCTSTR pszPropertyDef = GetWndPropertyDef(strClassName);

        if ( pszPropertyDef == NULL )
            pszPropertyDef = _T("ABCDEFGHIJKLMV");

        GetResWndProperty(WndProperty, pszPropertyDef);
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

    static LPCTSTR GetWndPropertyDef(KSGUI::CString strClassName)
    {        
        LPCTSTR pszResult = NULL;

        WndClassDefInfo* pItemList = NULL;

        int nItemCount = GetWndClassDefInfoEntry(pItemList);

        for ( int i = 0; i < nItemCount; i++ )
        {
            if ( !_tcscmp(strClassName, pItemList[i].strWndClassName) )
            {
                pszResult = pItemList[i].strWndPropertyList;

                break;
            }
        }

        return pszResult;
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

    static void GetResWndProperty( SkinWndPropertyList& WndProperty , LPCTSTR pszPropertyDef)
    {
        WndPropertyEntryItem* pItemList = NULL;

        int nItemCount = GetWndPropertyEntry(pItemList);
        int nPropertyCount = _tcslen(pszPropertyDef);

        ATLASSERT(pszPropertyDef != NULL);

        for ( int i = 0; i < nPropertyCount; i++ )
        {
            TCHAR ch  = pszPropertyDef[i];
            int   idx = -1;

            if ( (ch >= 'A' && ch <= 'Z') )
            {
                idx = ch - 'A';
            }
            else if ( (ch >= 'a' && ch <= 'z') )
            {
                idx = ch - 'a' + 26;
            }
            else
                continue;

            if (idx < 0 || idx >= nItemCount)
            {
                ATLASSERT(FALSE);
                continue;
            }

            SkinWndPropertyList::WndPropertyItem item;

            item.strProperty = pItemList[idx].strProperty;
            item.strValue    = pItemList[idx].strDefValue;

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
        /* A*/ { _T("IdName")       , _T("")           , SkinPropertyView::it_text},
        /* B*/ { _T("Caption")      , _T("Caption")    , SkinPropertyView::it_text},
        /* C*/ { _T("SkinClassName"), _T("")           , SkinPropertyView::it_readonly},
        /* D*/ { _T("ItemId")       , _T("0")          , SkinPropertyView::it_number},
        /* E*/ { _T("Style")        , _T("0")          , SkinPropertyView::it_number},
        /* F*/ { _T("ExStyle")      , _T("0")          , SkinPropertyView::it_number},
        /* G*/ { _T("Left")         , _T("0")          , SkinPropertyView::it_number},
        /* H*/ { _T("Top")          , _T("0")          , SkinPropertyView::it_number},
        /* I*/ { _T("Width")        , _T("100")        , SkinPropertyView::it_number},
        /* J*/ { _T("Height")       , _T("20")         , SkinPropertyView::it_number},
        /* K*/ { _T("TextColor")    , _T("0x00000000") , SkinPropertyView::it_color},
        /* L*/ { _T("BkColor")      , _T("0x00ffffff") , SkinPropertyView::it_color},
        /* M*/ { _T("Font")         , _T("ËÎÌו,GB2312_CHARSET,-12,0,400,0"), SkinPropertyView::it_font },

        /* N*/ { _T("PaintStyle")   , _T("0")          , SkinPropertyView::it_number },
        /* O*/ { _T("DrawTextFlags"), _T("0x0")        , SkinPropertyView::it_number },

        /* P*/ { _T("BmpResName")   , _T("")           , SkinPropertyView::it_text   },
        /* Q*/ { _T("BmpCount")     , _T("0")          , SkinPropertyView::it_number },

        /* R*/ { _T("Bitbmp")       , _T("")           , SkinPropertyView::it_text   },

        /* S*/ { _T("PenStyle")     , _T("0")          , SkinPropertyView::it_number },
        /* T*/ { _T("PenWidth")     , _T("1")          , SkinPropertyView::it_number },
        /* U*/ { _T("RoundWidth")   , _T("0")          , SkinPropertyView::it_number },

        /* V*/ { _T("MainColor")    , _T("0x00008000") , SkinPropertyView::it_color  }

        
        };
        
        pItemList = DefPropertyList;

        return sizeof(DefPropertyList) / sizeof(WndPropertyEntryItem);
    }

    struct WndClassDefInfo
    {
        LPCTSTR strWndClassName;
        LPCTSTR strWndPropertyList;
        DWORD   dwHasStyle;
        DWORD   dwHasExStyle;

    };

    static int GetWndClassDefInfoEntry( WndClassDefInfo*& pItemList )
    {
        

        static WndClassDefInfo WndClassDefInfoyList[] = 
        {
            { skinxmlstatic::GetSkinWndClassName()       , _T("ABCDEFGHIJ"), WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOPREFIX, 0 },
            { skinxmlpicture::GetSkinWndClassName()      , _T("ABCDEFGHIJR"), WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_REALSIZEIMAGE, 0 },

            { skinxmlgroupbox::GetSkinWndClassName()     , _T("ABCDEFGHIJ"), WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 0 },
            { skinxmlbutton::GetSkinWndClassName()       , _T("ABCDEFGHIJ"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 0 },
            { skinxmlcheckbox::GetSkinWndClassName()     , _T("ABCDEFGHIJ"), WS_CHILD | WS_VISIBLE | BS_CHECKBOX | BS_AUTOCHECKBOX | WS_TABSTOP, WS_EX_TRANSPARENT },
            { skinxmlcheckboxex::GetSkinWndClassName()   , _T("ABCDEFGHIJ"), WS_CHILD | WS_VISIBLE | BS_CHECKBOX | BS_AUTOCHECKBOX | WS_TABSTOP, WS_EX_TRANSPARENT },
            { skinxmlradiobox::GetSkinWndClassName()     , _T("ABCDEFGHIJ"), WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | WS_TABSTOP, 0 },

            { skinxmlcombobox::GetSkinWndClassName()     , _T("ABCDEFGHIJ"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWN | WS_VSCROLL , 0 },
            { skinxmllistbox::GetSkinWndClassName()      , _T("ABCDEFGHIJ"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, WS_EX_CLIENTEDGE },

            { skinxmllistctrl::GetSkinWndClassName()     , _T("ABCDEFGHIJ"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, WS_EX_CLIENTEDGE },
            { skinxmltreectrl::GetSkinWndClassName()     , _T("ABCDEFGHIJ"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, WS_EX_CLIENTEDGE },
            { skinxmltablectrl::GetSkinWndClassName()    , _T("ABCDEFGHIJ"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0 },

            { skinxmledit::GetSkinWndClassName()         , _T("ABCDEFGHIJ"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, WS_EX_CLIENTEDGE },
            { skinxmlhotkey::GetSkinWndClassName()       , _T("ABCDEFGHIJ"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0 },
            { skinxmlrichedit::GetSkinWndClassName()     , _T("ABCDEFGHIJ"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, WS_EX_CLIENTEDGE },

            { skinxmldatetime::GetSkinWndClassName()     , _T("ABCDEFGHIJ"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0 },
            { skinxmlmonthcalendar::GetSkinWndClassName(), _T("ABCDEFGHIJ"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0 },

            { skinxmlhscrollbar::GetSkinWndClassName()   , _T("ABCDEFGHIJ"), WS_CHILD | WS_VISIBLE, 0 },
            { skinxmlvscrollbar::GetSkinWndClassName()   , _T("ABCDEFGHIJ"), WS_CHILD | WS_VISIBLE, 0 },

            { skinxmlslider::GetSkinWndClassName()       , _T("ABCDEFGHIJ"), WS_CHILD | WS_VISIBLE, 0 },
            { skinxmlspin::GetSkinWndClassName()         , _T("ABCDEFGHIJ"), WS_CHILD | WS_VISIBLE, 0 },
            { skinxmlprogress::GetSkinWndClassName()     , _T("ABCDEFGHIJ"), WS_CHILD | WS_VISIBLE, 0 },

            { skinxmlanimation::GetSkinWndClassName()    , _T("ABCDEFGHIJ"), WS_CHILD | WS_VISIBLE, 0 },
            { skinxmlipaddress::GetSkinWndClassName()    , _T("ABCDEFGHIJ"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, WS_EX_CLIENTEDGE },


            { skinxmlstaticex::GetSkinWndClassName()     , _T("ABCDEFGHIJKLMNO"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0 },
            { skinxmlbmpbtn::GetSkinWndClassName()       , _T("ABCDEFGHIJPQ"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0 },
            { skinxmlclrbtn::GetSkinWndClassName()       , _T("ABCDEFGHIJLMN"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0 },
                

            // directui
            { _T("skinduidrawtext") , _T("ABCGHIJKMO")  , 0, 0 },
            { _T("skinduidrawbmp")  , _T("ABCGHIJR")    , 0, 0 },
            { _T("skinduidrawrect") , _T("ABCGHIJKLSTU"), 0, 0 }
        };

        pItemList = WndClassDefInfoyList;

        return sizeof(WndClassDefInfoyList) / sizeof(WndClassDefInfo);
    }

};