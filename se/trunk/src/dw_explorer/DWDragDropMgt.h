
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


    BOOL ProcessDropData(ATL::CString &cstrText)
    {
        switch ( CDWHtmlHelper::GetKindOfNavigateUrl(cstrText.Trim()) )
        {
        case VALID_URL:
            cstrText = cstrText.TrimLeft();
            ::SendCopyData(m_hNotifyWnd, 
                WM_WEBVIEW_CREATE, (LPCTSTR)cstrText, (cstrText.GetLength()+1)*sizeof(TCHAR));
            break;
        case SCRIPT_SCHEME_KEY:
            m_pHtmlView->Navigate2(cstrText);
            break;
        case SEARCH_DATA_KEY:
            ::SendCopyData(m_hNotifyWnd, 
                WM_WEBVIEW_OPENSEARCH, (LPCTSTR)cstrText, (cstrText.GetLength()+1)*sizeof(TCHAR));
            break;
        case UNKNOWN_URL:
            return FALSE;
        }
        return TRUE;
    }

    BOOL ProcessDropFile(IDataObject *pDataObj)
    {
        ATL::CString cstrText;
        ATL::CString cstrUrl;
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
                    HWND hWndChildFrm = (HWND)::SendCopyData(m_hNotifyWnd, 
                        WM_WEBVIEW_CREATE, (LPCTSTR)cstrText, (cstrText.GetLength()+1)*sizeof(TCHAR)); 
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
    HWND                 m_hNotifyWnd;

};