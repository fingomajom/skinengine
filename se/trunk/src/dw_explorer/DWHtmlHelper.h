
#pragma once

#include "DWHtmlView.h"


enum{
    DDTYPE_NONE,
    DDTYPE_FILE,
    DDTYPE_TEXT,
    DDTYPE_IMAGE,
    DDTYPE_LINK
};

#define	UNKNOWN_URL                 long(-1)
#define VALID_URL					long(0)
#define FILE_SCHEME_KEY				long(1)
#define SCRIPT_SCHEME_KEY			long(2)
#define SEARCH_DATA_KEY				long(3)
#define FTP_SCHEME_KEY				long(4)
#define DIRECTORY_EXIST             long(5)
#define NOT_FIND                    long(6)
#define NETWORK_PATH                long(7)
#define ABOUT_BLANK                 long(8)

class CDWHtmlHelper
{
public:



    static HRESULT GetElementOffset(IHTMLElement* pElementHit, long& x, long& y)
    {
        if(!pElementHit)
        {
            return E_FAIL ;
        }

        CComPtr<IHTMLElement2> spElementHit2 ;
        CComPtr<IHTMLRect> spHTMLRect ;
        if(FAILED(pElementHit->QueryInterface(IID_IHTMLElement2, (void**)&spElementHit2)) || !spElementHit2)
        {
            return E_FAIL ;
        }
        if(FAILED(spElementHit2->getBoundingClientRect(&spHTMLRect)) || !spHTMLRect)
        {
            return E_FAIL ;
        }
        if(FAILED(spHTMLRect->get_left(&x)) || FAILED(spHTMLRect->get_top(&y)))
        {
            return E_FAIL ;
        }

        return S_OK ;
    }

    static bool IsElementEqualAnchor(IHTMLElement* pUltimateElementHit, BSTR &bstrUrl)
    {
        HRESULT hr = S_OK ;
        CComPtr<IHTMLAreaElement> spAreaElement ;
        do
        {
            if(FAILED(pUltimateElementHit->QueryInterface(IID_IHTMLAreaElement, (void**)&spAreaElement)) || !spAreaElement)
                break;

            VARIANT_BOOL bnoHref = VARIANT_TRUE ;
            if(FAILED(spAreaElement->get_noHref(&bnoHref)))
                break;

            spAreaElement = NULL ;
            if(VARIANT_TRUE == bnoHref)
                break;
            else//这次点击能够引发一系列的动作，后面也就不用判断了
                return false;
        }while(0);

        CComPtr<IHTMLAnchorElement> spAnchorElement;
        CComPtr<IHTMLElement> spParentElement;
        CComPtr<IHTMLImgElement> spImgElement;
        do
        {
            if (spAnchorElement)
                spAnchorElement = NULL;

            if(FAILED(pUltimateElementHit->QueryInterface(IID_IHTMLAnchorElement, (void**)&spAnchorElement)) || !spAnchorElement)
            {
                if (spParentElement)
                    spParentElement = NULL;
                hr = pUltimateElementHit->get_parentElement(&spParentElement);
            }
            else//找到了anchor意味着用户在一个超链接上面发生了点击动作
            {
                spAnchorElement->get_href(&bstrUrl);
                ATL::CStringA strAnsi = CW2AEX<>(bstrUrl);
                wchar_t *lpBuf = new wchar_t[strAnsi.GetLength() + 1];

                ::MultiByteToWideChar(CP_ACP, 0, strAnsi, -1, lpBuf, strAnsi.GetLength() + 1);

                ::SysFreeString(bstrUrl);
                bstrUrl = ::SysAllocString(lpBuf);
                delete lpBuf;
                return true;
            }
            pUltimateElementHit = spParentElement;
        }while(SUCCEEDED(hr) && pUltimateElementHit);

        return false;
    }

    static bool IsElementEqualImage(IHTMLElement* pUltimateElementHit, BSTR &bstrUrl)
    {
        HRESULT hr = S_OK ;

        CComPtr<IHTMLElement> spParentElement;
        CComPtr<IHTMLImgElement> spImgElement;
        do
        {
            if (spImgElement)
                spImgElement = NULL;

            if(FAILED(pUltimateElementHit->QueryInterface(IID_IHTMLImgElement, (void**)&spImgElement)) || !spImgElement)
            {
                if (spParentElement)
                    spParentElement = NULL;
                hr = pUltimateElementHit->get_parentElement(&spParentElement);
            }
            else//找到了img, 意味着用户在一个图片上面发生了点击动作
            {
                spImgElement->get_href(&bstrUrl);
                // 参看IsAnchorClicked里的注释
                ATL::CStringA strAnsi = CW2AEX<>(bstrUrl);
                wchar_t *lpBuf = new wchar_t[strAnsi.GetLength() + 1];

                ::MultiByteToWideChar(CP_ACP, 0, strAnsi, -1, lpBuf, strAnsi.GetLength() + 1);

                ::SysFreeString(bstrUrl);
                bstrUrl = ::SysAllocString(lpBuf);
                delete lpBuf;
                return true;
            }
            pUltimateElementHit = spParentElement;
        }while(SUCCEEDED(hr) && pUltimateElementHit);

        return false;
    }


    static ULONG GetMouseClicked(CDWHtmlView* pHtmlView, const POINT &pt, BSTR &bstrUrl)
    {
        POINT ptClient;
        ptClient.x = pt.x;
        ptClient.y = pt.y;
        ::ScreenToClient(pHtmlView->m_hWnd, &ptClient);

        IWebBrowser2* pWebBrowser2 = pHtmlView->GetWebBrowser();
        if (!pWebBrowser2)
            return false;

        CComPtr<IDispatch> spDispDoc;
        pWebBrowser2->get_Document(&spDispDoc) ;
        if (!spDispDoc)
            return false;

        CComPtr<IHTMLDocument2> spHTMLDocument2;
        if (FAILED(spDispDoc->QueryInterface(IID_IHTMLDocument2, (void**)&spHTMLDocument2)))
            return false;

        CComPtr<IHTMLElement> spElement;
        do
        {
            if (spElement)
                spElement = NULL;

            spHTMLDocument2->elementFromPoint(ptClient.x, ptClient.y, &spElement);
            if (!spElement)
                return false;

            if (IsElementEqualAnchor(spElement, bstrUrl))
                return DDTYPE_LINK;
            else if (IsElementEqualImage(spElement, bstrUrl))
                return DDTYPE_IMAGE;
            else
            {
                do 
                {
                    spHTMLDocument2 = NULL;
                    CComPtr<IHTMLFrameBase2> spFrame = NULL;
                    CComPtr<IHTMLWindow2> spWin = NULL;
                    if (FAILED(spElement->QueryInterface(IID_IHTMLFrameBase2, (void**)&spFrame)))
                        break;

                    if (FAILED(spFrame->get_contentWindow(&spWin)))
                        break;	

                    if (spHTMLDocument2)
                        spHTMLDocument2 = NULL;
                    spWin->get_document(&spHTMLDocument2);
                } while (0);

                do 
                {
                    if (spHTMLDocument2)
                        break;

                    CComPtr<IWebBrowser2> spWebBrowser2 = NULL ;
                    if(FAILED(spElement->QueryInterface(IID_IWebBrowser2, (void**)&spWebBrowser2)) || !spWebBrowser2)
                        break ;

                    CComPtr<IDispatch> spDispatch = NULL ;
                    if(FAILED(spWebBrowser2->get_Document(&spDispatch)) || !spDispatch)
                        break ;

                    if(FAILED(spDispatch->QueryInterface(IID_IHTMLDocument2, (void**)&spHTMLDocument2)))
                        spHTMLDocument2 = NULL ;
                } while (0);

                if (!spHTMLDocument2)
                    break;

                long lx = 0 ;
                long ly = 0 ;
                //重新计算坐标
                if(FAILED(GetElementOffset(spElement, lx, ly)))
                    continue ;

                ptClient.x -= lx ;
                ptClient.y -= ly ;

            }
        }while (1);

        return DDTYPE_NONE;
    }


    static HRESULT ExtractUrlFromUrlLinkFile(LPCTSTR pszPath, LPTSTR pszUrl, UINT nUrlLen)
    {
        HRESULT hr = E_FAIL;
        pszUrl[0] = 0;

        LPTSTR pszExt = PathFindExtension(pszPath);
        // HACK: we hard code .URL. this should be a property of the file type
        if (0 == StrCmpI(pszExt, _T(".url")))
        {
            CComPtr<IShellLink> spShellLink;
            CComPtr<IPersistFile> spPersistFile;

            hr = ::CoCreateInstance(CLSID_InternetShortcut, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*) &spShellLink);
            if (FAILED(hr) || spShellLink == NULL) 
            {
                return hr;
            }

            hr = spShellLink->QueryInterface(IID_IPersistFile,(LPVOID*) &spPersistFile);
            if (FAILED(hr) || spPersistFile == NULL) 
            {
                return hr;
            }

            USES_CONVERSION;
            hr = spPersistFile->Load(T2CW(pszPath), STGM_READ);
            if (SUCCEEDED(hr))
            {
                WIN32_FIND_DATA find = {0};
                hr = spShellLink->GetPath(pszUrl, INTERNET_MAX_URL_LENGTH, &find, SLGP_UNCPRIORITY);
            }
        }
        return hr ;
    }

};