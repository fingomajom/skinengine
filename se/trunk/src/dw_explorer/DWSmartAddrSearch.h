
#pragma once

#include "DWSmartAddrProvider.h"
#include "DWThread.h"
#include "DWHtmlHelper.h"
#include "DWSearchMgt.h"

class CDWSmartAddrSearch : 
    public CDWSmartAddrProvider,
    public CDWThread
{
public:
    CDWSmartAddrSearch()
    {
        if ( m_hThread == NULL )
            CreateThread();
    }

    ~CDWSmartAddrSearch()
    {
    }

    void Stop()
    {
        PostThreadMessage(m_dwThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject( m_hThread, 1000 );
    }

    int QueryAddrDropList( HWND hWndN, LPCTSTR pszIText, 
        AddrDropItemList& aList, int nMaxCount )
    {
        if ( pszIText == NULL || lstrlenW(pszIText) <= 0 )
            return QS_NONE;

        if ( StrStrI( pszIText, L"." )   != NULL || 
             StrStrI( pszIText, L"://" ) != NULL ||
             StrStrI( pszIText, L":\\" ) != NULL )
            return QS_NONE;

        m_strIText = pszIText;

        ::PostThreadMessage( m_dwThreadId, WM_TIMER, 
            (WPARAM)hWndN, 0 );

        return QS_ASYN;
    }

    void _QuerySearchInfo()
    {
        CComPtr<IHTMLDocument2> spHTMLDocument;

        HRESULT hr = CoCreateInstance(
            CLSID_HTMLDocument,
            NULL, 
            CLSCTX_INPROC_SERVER, 
            __uuidof(IHTMLDocument2), 
            (void**)&spHTMLDocument);
        
        if ( FAILED(hr) )
        {
            Sleep(500);
            return;
        }

        MSG msg;

        while ( ::GetMessage(&msg, NULL, 0, 0) )
        {
            if ( msg.message == WM_TIMER )
            {
                ATL::CString  strQueryURL;
                ATL::CStringA strBuffer;
                ATL::CString  strIText = m_strIText;

                AddrDropItemList aList;

                TCHAR szHost[512]     = { 0 };
                TCHAR szScheme[32]    = { 0 };
                TCHAR szUrlPath[2049] = { 0 };

                CDWSearchMgt& serMgt = CDWSearchMgt::Instance();

                if ( serMgt.m_nSelIndex > 2 )
                    strQueryURL = CDWSearchMgt::InstancePtr()->GetSearchURL( 0, strIText );
                else 
                    strQueryURL = CDWSearchMgt::InstancePtr()->GetSearchURL(strIText );

                URL_COMPONENTS urlComponents = { 0 };
                urlComponents.dwStructSize = sizeof(URL_COMPONENTS);
                urlComponents.lpszHostName = szHost;
                urlComponents.dwHostNameLength = 512;
                urlComponents.lpszScheme = szScheme;
                urlComponents.dwSchemeLength = 32;
                urlComponents.lpszUrlPath = szUrlPath;
                urlComponents.dwUrlPathLength = 2048;

                if (!::InternetCrackUrl(strQueryURL, 0, 0, &urlComponents))
                {
                    continue;
                }

                if ( CDWHtmlHelper::HttpGetRes( szHost, szUrlPath, strBuffer, 500 ) )
                {
                    ATL::CString strHtml(CA2WEX<>(strBuffer).m_psz);

                    RepairHtml(strHtml);

                    SAFEARRAY* psa   = SafeArrayCreateVector(VT_VARIANT, 0, 1);
                    VARIANT *  param = NULL;

                    hr =  SafeArrayAccessData(psa, (LPVOID*)&param);
                    param->vt      = VT_BSTR;
                    param->bstrVal = strHtml.AllocSysString();

                    spHTMLDocument->put_designMode(L"On"); 
                    hr = spHTMLDocument->write( psa );
                    spHTMLDocument->close();

                    SafeArrayUnaccessData(psa);
                    SafeArrayDestroy(psa);

                    if ( AnalyzeSearchResult(spHTMLDocument, aList) > 0 )
                    {
                        ::SendMessage( (HWND)msg.wParam, WM_QUERY_SEARCH_RESULT , 
                            (WPARAM)&aList, (LPARAM)(LPCTSTR)strIText );
                    }
                }
            }
        }

    }

    int AnalyzeSearchResult( IHTMLDocument2* piHTMLDocument, AddrDropItemList& aList )
    {
        int nResult = 0;

        HRESULT hr = E_FAIL;

        long lLinks = 0;
        CComPtr<IHTMLElementCollection> spiLinkColleciton;

        hr = piHTMLDocument->get_links(&spiLinkColleciton);
        if ( FAILED(hr) )
            return 0;

        hr = spiLinkColleciton->get_length(&lLinks);
        if ( FAILED(hr) )
            return 0;

        if ( lLinks == 0 )
            return 0;

        ATL::CString strMsg;

        for (long idx = 0; idx < lLinks; idx++)
        {
            CComPtr<IDispatch> spiLinkDisp;
            CComBSTR		   bstrUrl;
            CComBSTR		   bstrName;
            CComBSTR		   bstrTarget;
            CComVariant		   index = idx;

            hr = spiLinkColleciton->item(index, index, &spiLinkDisp);
            if (FAILED(hr))
                continue;

            CComPtr<IHTMLAnchorElement> spiLinkElement;
            CComPtr<IHTMLElement> spiElement;

            hr = spiLinkDisp->QueryInterface(&spiLinkElement);
            if (FAILED(hr))
                continue;

            hr = spiLinkDisp->QueryInterface(&spiElement);
            if (FAILED(hr))
                continue;

            hr = spiLinkElement->get_href(&bstrUrl);
            if (FAILED(hr))
                continue;

            hr = spiElement->get_innerText( &bstrName );
            if (FAILED(hr))
                continue;

            if ( bstrUrl == NULL ||
                 bstrName == NULL )
                 continue;

            spiLinkElement->get_target(&bstrTarget);

            if ( !StrCmpI( bstrName, L"下一页" ) )
            {
                break;
            }

            if ( bstrTarget.m_str == NULL || 
                 StrCmpI(bstrTarget, L"_blank"))
                 continue;

            if ( !StrCmpI(bstrName, L"百度快照") ||
                 !StrCmpI(bstrName, L"网页快照") ||
                 !StrCmpI(bstrName, L"搜狗快照") ||
                 !StrCmpNI(bstrName, L"发布/查看关于", 7) )
            {
                 continue;
            }


            struct URL_FILTER_ITEM{
                LPCTSTR pszFilter;
                DWORD   dwLen;
            };
            
            static URL_FILTER_ITEM filterItems[] = {
                L"http://ma.baidu.com/ma/"    , 23,
                L"http://post.baidu.com/"     , 22,
                L"http://e.baidu.com/"        , 19,
                L"http://www.baidu.com"       , 20,

                L"http://ditu.google.cn/maps" , 26,
                L"http://news.google.cn"      , 21,
                L"http://www.google.cn"       , 20,

                L"http://www.sogou.com"       , 20,
                L"http://lk.brand.sogou.com"  , 25
            };
            
            int i = 0;
            for ( i = 0; i < sizeof(filterItems)/sizeof(filterItems[0]); i++ )
            {
                if ( !StrCmpNI(bstrUrl, filterItems[i].pszFilter, filterItems[i].dwLen) )
                    break;
            }

            if ( i < sizeof(filterItems)/sizeof(filterItems[0]) )
                continue;

            if ( !StrCmpNI(bstrUrl, L"http://", 7) )
            {
                ADDRDROPLISTITEM dropItem;

                dropItem.strTitle = bstrName;
                dropItem.strURL   = bstrUrl;

                aList.AddTail( dropItem );
                
                nResult++;
            }
        }

        return nResult;
    }

    int RepairHtml( ATL::CString& strHtmlText )
    {
        return 0;
        strHtmlText = L"111<body>ssss</body>111";

        int nPos = strHtmlText.Find(_T("<body"));
        if ( nPos >= 0 )
            strHtmlText.Delete( 0, nPos );

        nPos = strHtmlText.Find(_T("</body>"));
        if ( nPos >= 0 )
            strHtmlText.Delete( nPos+7, strHtmlText.GetLength() );

        return 1;
    }

    virtual DWORD _RunThreadFunc()
    {
        HRESULT hRes = ::OleInitialize(NULL);
        ATLASSERT(SUCCEEDED(hRes));

        while (true)
        {
            __try
            {
                _QuerySearchInfo();
                break;
            }
            __except(1)
            {
            }
        }

        OleUninitialize();

        return 1L;
    }

    ATL::CString m_strIText;
};


__declspec(selectany) CDWSmartAddrSearch g_CDWSmartAddrSearch;
