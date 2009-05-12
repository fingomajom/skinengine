
#pragma once


#include "DWHtmlHelper.h"


class CDWDragDropMgt : public IDropTarget
{
public:

    CDWDragDropMgt()
    {
        m_dwRef = 0;
        m_spDropTarget = NULL ;
        m_bDragTextToInputBox = FALSE;
        m_uDraggingType = DDTYPE_NONE;
        m_bstrHref = NULL;
    }


    ULONG STDMETHODCALLTYPE AddRef()
    {
        return ++m_dwRef ;
    }

    ULONG STDMETHODCALLTYPE Release()
    {
        return --m_dwRef ;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(const IID &riid, void **ppvObject)
    {
        if (riid == IID_IUnknown) 
        { 
            *ppvObject = (IUnknown*)(this); 
            return S_OK; 
        } 
        if (riid == IID_IDropTarget)
        {
            *ppvObject = (IDropTarget*)(this); 
            return S_OK; 
        }

        return E_NOINTERFACE ;
    }


    ULONG GetDraggingType(IDataObject *pDataObj, POINTL pt)
    {
        if (!pDataObj)
            return FALSE ;

        FORMATETC FormatEtc = { 0 };
        STGMEDIUM StgMedium = { 0 };
        FormatEtc.cfFormat  = CF_HDROP;
        FormatEtc.tymed     = TYMED_HGLOBAL;
        pDataObj->GetData(&FormatEtc, &StgMedium);
        if (StgMedium.hGlobal)
        {
            HDROP hDrop = (HDROP)GlobalLock(StgMedium.hGlobal);
            UINT nCount = DragQueryFile(hDrop, -1, NULL, NULL);
            if (nCount > 0)
            {
                GlobalUnlock(StgMedium.hGlobal);
                GlobalFree(StgMedium.hGlobal) ;
                return DDTYPE_FILE;
            }	
        }

        ATL::CString cstrText;
        FormatEtc.cfFormat = CF_UNICODETEXT;
        FormatEtc.tymed    = TYMED_HGLOBAL;
        pDataObj->GetData(&FormatEtc, &StgMedium);
        if (StgMedium.hGlobal)
        {
            LPCTSTR lpcszData = (LPCTSTR)GlobalLock(StgMedium.hGlobal);
            cstrText = lpcszData;
            GlobalUnlock(StgMedium.hGlobal);
            GlobalFree(StgMedium.hGlobal);
        }

        POINT ptClient;
        ptClient.x = pt.x;
        ptClient.y = pt.y;

        CDWHtmlHelper::GetMouseClicked(m_pHtmlView, ptClient, m_bstrHref);

        //std::string szT = Util::URLMisc::UrlDecode(std::string(W2A(cstrText.GetBuffer())));
        //wchar_t wszUrl[1024] = { 0 };
        //::MultiByteToWideChar(CP_ACP, 0, szT.c_str(), -1, wszUrl, 1024);

        if (cstrText.GetLength() < 1 && m_bstrHref != NULL)
            return DDTYPE_IMAGE;
        if (cstrText.GetLength() > 1 && m_bstrHref != NULL && StrCmpNI(m_bstrHref, cstrText, 512) == 0)
        {
            ::SysFreeString(m_bstrHref);
            m_bstrHref = ::SysAllocString(cstrText.GetBuffer());
            return DDTYPE_LINK;
        }
        if (cstrText.GetLength() > 1)
        {
            ::SysFreeString(m_bstrHref);
            m_bstrHref = ::SysAllocString(cstrText.GetBuffer());
            return DDTYPE_TEXT;
        }

        return DDTYPE_NONE;
    }



    HRESULT STDMETHODCALLTYPE DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
    {
        if (m_spDropTarget)
        {
            *pdwEffect |= DROPEFFECT_COPY | DROPEFFECT_LINK ;
            m_spDropTarget->DragEnter(pDataObj, grfKeyState, pt, pdwEffect) ;
        }


        if (m_uDraggingType == DDTYPE_NONE)
            m_uDraggingType = GetDraggingType(pDataObj, pt);

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
    {
        if (m_spDropTarget)
        {
            DWORD dwEffect = *pdwEffect;
            m_spDropTarget->DragOver(grfKeyState, pt, &dwEffect) ;

            m_bDragTextToInputBox = IsDraggingTextToInputBox(dwEffect, *pdwEffect);
            if ((dwEffect & ~DROPEFFECT_SCROLL) != DROPEFFECT_NONE)
            {
                *pdwEffect = dwEffect;
                return S_OK ;
            }
        }

        *pdwEffect |= DROPEFFECT_COPY | DROPEFFECT_LINK ;

        return S_OK;
    }

    BOOL IsDraggingTextToInputBox(DWORD dwOldEffect, DWORD dwNewEffect)
    {
        BOOL bTextSelectionToInputBox = (dwOldEffect == DROPEFFECT_COPY) && (dwNewEffect == dwOldEffect);
        BOOL bInputBoxToInputBox = (dwOldEffect == (DROPEFFECT_MOVE)) && (dwNewEffect == (DROPEFFECT_MOVE | DROPEFFECT_COPY));
        return bTextSelectionToInputBox || bInputBoxToInputBox;
    }

    HRESULT STDMETHODCALLTYPE DragLeave()
    {
        if (m_spDropTarget)
        {
            m_spDropTarget->DragLeave() ;
        }
        return S_OK ;
    }

    HRESULT STDMETHODCALLTYPE Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
    {
        if (m_bDragTextToInputBox)
        {
            if (m_spDropTarget)
            {
                m_spDropTarget->Drop(pDataObj, grfKeyState, pt, pdwEffect);
                *pdwEffect = DROPEFFECT_NONE;
                return S_OK;
            }
        }

        *pdwEffect = DROPEFFECT_NONE;

        CString cstrText = L"";
        if (m_bstrHref)
        {
            cstrText = (LPCWSTR)m_bstrHref;
            ::SysFreeString(m_bstrHref);
            m_bstrHref = NULL;
        }

        if (m_uDraggingType == DDTYPE_FILE)
            if (ProcessDropFile(pDataObj))
                return S_OK;

        if (m_uDraggingType != DDTYPE_NONE)
        {
            m_uDraggingType = DDTYPE_NONE;
            if (ProcessDropData(cstrText))
                return S_OK;
        }

        HRESULT hr = 0 ;
        if (m_spDropTarget)
        {
            hr = m_spDropTarget->Drop(pDataObj, grfKeyState, pt, pdwEffect) ;
        }

        cstrText = L"" ;
        CString cstrUrl = L"" ;
        FORMATETC FormatEtc = { 0 } ;
        FormatEtc.cfFormat = CF_UNICODETEXT ;
        FormatEtc.tymed = TYMED_HGLOBAL ;
        STGMEDIUM StgMedium = { 0 } ;
        pDataObj->GetData(&FormatEtc, &StgMedium) ;
        if (StgMedium.hGlobal)
        {
            LPCTSTR lpcszData = (LPCTSTR)GlobalLock(StgMedium.hGlobal);
            cstrText = lpcszData;
            GlobalUnlock(StgMedium.hGlobal);
            GlobalFree(StgMedium.hGlobal) ;
        }

        if (cstrText.IsEmpty())
            return S_OK ;

        ProcessDropData(cstrText);

        return S_OK ;
    }

    long GetKindOfNavigateUrl(LPCTSTR lpszKey)
    {
        if (!lpszKey || !lpszKey[0])
            return UNKNOWN_URL ;
        if (::PathIsDirectory(lpszKey))
            return DIRECTORY_EXIST ;
        if (::PathIsNetworkPath(lpszKey))
            return NETWORK_PATH ;

        TCHAR szUrlPath[512] = { 0 };
        TCHAR szScheme[32] = { 0 };
        URL_COMPONENTS urlComponents = { 0 };
        urlComponents.dwStructSize = sizeof(URL_COMPONENTS);
        urlComponents.lpszUrlPath = szUrlPath;
        urlComponents.dwUrlPathLength = 512;
        urlComponents.lpszScheme = szScheme;
        urlComponents.dwSchemeLength = 32;
        WIN32_FIND_DATA wfd = { 0 } ; 
        if (::InternetCrackUrl(lpszKey, 0, 0, &urlComponents) && urlComponents.nScheme == INTERNET_SCHEME_FILE)
            if (INVALID_HANDLE_VALUE != FindFirstFile(lpszKey, &wfd))
                return FILE_SCHEME_KEY ;
            else if (::PathIsDirectory(szUrlPath))
                return DIRECTORY_EXIST ;
            else
                return NOT_FIND ;

            if (!StrCmpNI(L"about:", lpszKey, 6))
                return ABOUT_BLANK ;

            if (!StrCmpNI(L"javascript:", lpszKey, 11)  ||
                !StrCmpNI(L"vbscript:", lpszKey, 9)           ||
                !StrCmpNI(L"mailto:", lpszKey, 7)              )
                return SCRIPT_SCHEME_KEY ;

            if (!StrCmpNI(L"localhost", lpszKey, 9))
                return VALID_URL ;

            if (!StrCmpNI(L"ftp", lpszKey, 3))
                return FTP_SCHEME_KEY ;

            // 如果碰上类似url，要判断成合法url
            // http://58.251.57.206/down?cid=FC4C689C7851773472C2DB828D267621840B98E9&t=4&fmt=
            if (!StrCmpNI(L"http", lpszKey, 4))
                return VALID_URL ;

            BOOL bUsualFix = FALSE ;
            if (StrStr(lpszKey, L"www.")   ||
                StrStr(lpszKey, L".com")   ||
                StrStr(lpszKey, L".cn")    ||
                StrStr(lpszKey, L".net")   ||
                StrStr(lpszKey, L".gov")   ||
                StrStr(lpszKey, L".org")   ||
                StrStr(lpszKey, L".edu")   ||
                StrStr(lpszKey, L".ac")     )
                bUsualFix = TRUE ;

            if (bUsualFix)
            {
                return VALID_URL ;
            }
            else
            {
                CString cstrReplace = lpszKey ;
                cstrReplace.Replace(L"\\", L"/") ;
                if ( PathIsDirectoryW(cstrReplace) )
                {
                    if (::InternetCrackUrl(cstrReplace, 0, 0, &urlComponents) && urlComponents.nScheme == INTERNET_SCHEME_FILE)
                        if (INVALID_HANDLE_VALUE != FindFirstFile(cstrReplace, &wfd))
                            return FILE_SCHEME_KEY ;
                        else if (::PathIsDirectory(szUrlPath))
                            return DIRECTORY_EXIST ;
                        else
                            return NOT_FIND ;
                }
            }
            return SEARCH_DATA_KEY ;
    }

    BOOL ProcessDropData(ATL::CString &cstrText)
    {
        MessageBox(GetActiveWindow(), cstrText, L"", MB_OK);
        switch ( GetKindOfNavigateUrl(cstrText.Trim()) )
        {
        case VALID_URL:
            break;
        case SCRIPT_SCHEME_KEY:
            //m_pAxControl->SendMessage(WM_AX_NAVIGATE, NULL, (LPARAM)(LPCTSTR)cstrText);
            break;
        case SEARCH_DATA_KEY:
            break;
        case UNKNOWN_URL:
            return FALSE;
        }
        return TRUE;
    }

    BOOL ProcessDropFile(IDataObject *pDataObj)
    {
        CString cstrText;
        CString cstrUrl;
        HRESULT hr;
        FORMATETC FormatEtc = { 0 } ;
        STGMEDIUM StgMedium = { 0 } ;
        FormatEtc.cfFormat = CF_HDROP ;
        FormatEtc.tymed = TYMED_HGLOBAL ;
        pDataObj->GetData(&FormatEtc, &StgMedium) ;
        if (StgMedium.hGlobal)
        {
            HDROP hDrop = (HDROP)GlobalLock(StgMedium.hGlobal) ;
            UINT nCount = DragQueryFile(hDrop, -1, NULL, NULL) ;
            for (int i = 0; i < (int)nCount; ++i)
            {
                DragQueryFile(hDrop, i, cstrText.GetBuffer(MAX_PATH), MAX_PATH) ;
                cstrText.ReleaseBuffer() ;
                hr = CDWHtmlHelper::ExtractUrlFromUrlLinkFile(cstrText, cstrUrl.GetBuffer(INTERNET_MAX_URL_LENGTH), INTERNET_MAX_URL_LENGTH);
                cstrUrl.ReleaseBuffer();
                if (SUCCEEDED(hr))
                {
                    cstrText = cstrUrl ;
                }
                WIN32_FIND_DATA w32fd ;
                HANDLE hFind = INVALID_HANDLE_VALUE ;
                hFind = FindFirstFile(cstrText, &w32fd) ;
                if (INVALID_HANDLE_VALUE != hFind)//1、拖拽的是文件
                {
                    MessageBox(GetActiveWindow(), cstrText, L"", MB_OK);
                }
            }
            GlobalUnlock(StgMedium.hGlobal) ;
            GlobalFree(StgMedium.hGlobal) ;
            return TRUE;
        }
        return FALSE;
    }

public:

    CDWHtmlView*         m_pHtmlView;

    DWORD                m_dwRef;
    CComPtr<IDropTarget> m_spDropTarget ;
    BOOL                 m_bDragTextToInputBox;
    ULONG                m_uDraggingType;
    BSTR                 m_bstrHref;

};