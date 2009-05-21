

#pragma once

#include "DWComDef.h"
#include "DWFavIconMgt.h"
#include "DWConfigDB.h"

struct SearchItem
{
    ATL::CString strName;
    ATL::CString strURL;
    ATL::CString strURLFormat;
};

class CDWSearchMgt : public CDWSingleton<CDWSearchMgt>
{
    friend CDWSingleton<CDWSearchMgt>;
protected:
    CDWSearchMgt( const CDWSearchMgt& );

    CDWSearchMgt()
    {
        AddSearchItem(
            L"°Ù¶ÈËÑË÷",
            L"http://www.baidu.com",
            L"http://www.baidu.com/s?wd=%s");

        AddSearchItem(
            L"¹È¸èËÑË÷",
            L"http://www.google.cn",
            L"http://www.google.cn/search?hl=zh-CN&q=%s");

        AddSearchItem(
            L"ËÑ¹·ËÑË÷",
            L"http://www.sogou.com",
            L"http://www.sogou.com/sogou?query=%s");

        AddSearchItem(
            L"ÌÔ±¦ËÑË÷",
            L"http://www.taobao.com",
            L"http://search1.taobao.com/browse/search_auction.htm?q=%s");

        AddSearchItem(
            L"Ñ¸À××ÊÔ´ËÑË÷",
            L"http://www.gougou.com",
            L"http://www.gougou.com/search?search=%s");

        AddSearchItem( L"-", L"", L"" );

        AddSearchItem(
            L"ÊÓÆµËÑË÷",
            L"http://www.youku.com/",
            L"http://so.youku.com/search_video/q_%s");

        AddSearchItem( L"-", L"", L"" );

        AddSearchItem(
            L"°Ù¶Èmp3ËÑË÷",
            L"http://mp3.baidu.com",
            L"http://mp3.baidu.com/m?f=ms&rn=&tn=baidump3&ct=134217728&word=%s&lm=-1");

        AddSearchItem( L"-", L"", L"" );

        AddSearchItem(
            L"½ðÉ½°®´Ê°Ô",
            L"http://www.iciba.com",
            L"http://www.iciba.com/%s");

        AddSearchItem( L"-", L"", L"" );

        AddSearchItem(
            L"GoogleÍ¼Æ¬ËÑË÷",
            L"http://images.google.com",
            L"http://images.google.com/images?hl=zh-CN&q=%s&gbv=2&aq=f&oq=");

                
        m_bChanged  = FALSE;
        m_nSelIndex = 0;
        CDWBaseConfig cfg;
        m_nSelIndex = cfg.get_Default_Search(m_nSelIndex);
        if ( m_nSelIndex < 0 || 
             m_nSelIndex >= m_searchList.GetSize() || 
             m_searchList[m_nSelIndex].strName == L"-" )
        {
            m_nSelIndex = 0;
            m_bChanged  = TRUE;
        }

        CDWFavIconMgt& fimgt = CDWFavIconMgt::Instance();

        for ( int i = 0; i < m_searchList.GetSize(); i++ )
        {
            fimgt.GetFavIcon( m_searchList[i].strURL, NULL, NULL );
        }
    }

    ~CDWSearchMgt()
    {
        if ( m_bChanged )
        {
            CDWBaseConfig cfg;
            cfg.set_Default_Search(m_nSelIndex);
        }
    }

public:

    void AddSearchItem( LPCTSTR pszName, LPCTSTR pszURL, LPCTSTR pszFormat )
    {
        SearchItem item;
        item.strName    = pszName;
        item.strURL       = pszURL;
        item.strURLFormat = pszFormat;
        
        m_searchList.Add( item );
    }

    ATL::CString GetSearchName() const
    {
        return m_searchList[m_nSelIndex].strName;
    }

    ATL::CString GetSearchName(int nSelIndex) const
    {
        return m_searchList[nSelIndex].strName;
    }


    ATL::CString GetSearchURL() const
    {
        return m_searchList[m_nSelIndex].strURL;
    }

    ATL::CString GetSearchURL(int nSelIndex) const
    {
        return m_searchList[nSelIndex].strURL;
    }

    ATL::CString GetSearchURL( LPCTSTR pszKeyWord ) const
    {
        ATL::CString strURL;
        ATL::CString strKeyword = EscapeUrl(pszKeyWord);

        strURL.Format( m_searchList[m_nSelIndex].strURLFormat, strKeyword);

        return strURL;
    }

    ATL::CString GetSearchURL( int nSelIndex, LPCTSTR pszKeyWord ) const
    {
        ATL::CString strURL;
        ATL::CString strKeyword = EscapeUrl(pszKeyWord);

        strURL.Format( m_searchList[nSelIndex].strURLFormat, strKeyword);

        return strURL;
    }

    HMENU CreatePopueMenu()
    {
        CMenuHandle menu;
        menu.CreatePopupMenu();
        
        for ( int i = 0; i < m_searchList.GetSize(); i++ )
        {
            if ( m_searchList[i].strName == L"-" )
                menu.AppendMenu( MF_SEPARATOR, (UINT_PTR)ID_SEPARATOR, m_searchList[i].strName );
            else
                menu.AppendMenu( MF_STRING, 10+i, m_searchList[i].strName );
        }

        m_bChanged = TRUE;
        return menu;
    }


    static CString EscapeUrl( LPCTSTR pszURL )
    {
        ATL::CString strResult;
        ATL::CString strTemp;

        CW2AEX<> URL(pszURL, CP_ACP);

        for ( LPSTR pch = URL.m_psz; *pch ; pch++ )
        {
            if ( (*pch >= '0' && *pch <= '9') ||
                (*pch >= 'A' && *pch <= 'Z') ||
                (*pch >= 'a' && *pch <= 'z') )
            {
                strResult += *pch;
            }
            else
            {
                strTemp.Format(L"%%%02x", BYTE(*pch) );
                strResult += strTemp;
            }
        }

        return strResult;
    }



    BOOL m_bChanged;
    int  m_nSelIndex;
    ATL::CSimpleArray<SearchItem> m_searchList;
};