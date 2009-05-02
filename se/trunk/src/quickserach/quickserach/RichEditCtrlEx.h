/********************************************************************
* CreatedOn: 2008-5-20   10:18
* FileName: skinricheditctrlex.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/
#pragma once


#define RICHEDIT_TEXT_FONT_BOLD			(0x0001)
#define RICHEDIT_TEXT_FONT_ITALIC		(0x0002)
#define RICHEDIT_TEXT_FONT_OUTLINE		(0x0004)
#define RICHEDIT_TEXT_FONT_STRIKEOUT	(0x0008)
#define RICHEDIT_TEXT_FONT_UNDERLINE	(0x0010)

#define KM_RICHEDIT_LINK_CLICKED_NOTIFY (WM_USER + 1983)


class CDataObject : public IDataObject
{
protected:

    HBITMAP m_hBitmap;

public:
    CDataObject(HBITMAP hBitmap)
    {
        m_hBitmap = hBitmap;
    }

    virtual ~CDataObject()
    {
    }

public:

    HRESULT __stdcall QueryInterface(REFIID iid, void ** ppvObject)
    {
        *ppvObject = (IDataObject*) this;

        return S_OK;
    }

    ULONG __stdcall AddRef()
    {
        return 1;
    }

    ULONG __stdcall Release()
    {
        return 0;
    }

    HRESULT __stdcall GetData(FORMATETC *pFormatetc, STGMEDIUM * pmedium)
    {
        if (pFormatetc->cfFormat != CF_BITMAP  || !pmedium)
            return S_FALSE;

        if (!m_hBitmap)
            return S_FALSE; 

        pmedium->tymed = TYMED_GDI ;
        pmedium->pUnkForRelease = NULL;
        pmedium->hBitmap = m_hBitmap;

        return S_OK;
    }

    HRESULT __stdcall GetDataHere(FORMATETC * pFormatetc, STGMEDIUM * pmedium)
    {
        return S_OK;
    }

    HRESULT __stdcall QueryGetData(FORMATETC * pFormatetc)
    {
        return S_OK;
    }

    HRESULT __stdcall GetCanonicalFormatEtc(FORMATETC * pFormatetcIn, FORMATETC * pFormatetcOut)
    {
        return S_OK;
    }

    HRESULT __stdcall SetData(FORMATETC * pFormatetc, STGMEDIUM * pmedium, BOOL fRelease)
    {
        return S_OK;
    }

    HRESULT __stdcall EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC ** ppenumFormatetc)
    {
        if ( dwDirection == DATADIR_GET )
        {
            return S_OK;
        }
        else
            return S_FALSE;
    }

    HRESULT __stdcall DAdvise(FORMATETC * pFormatetc, DWORD advf, IAdviseSink * pAdvSink, DWORD * pdwConnection)
    {
        return S_OK;
    }

    HRESULT __stdcall DUnadvise(DWORD dwConnection)
    {
        return S_OK;
    }

    HRESULT __stdcall EnumDAdvise(IEnumSTATDATA ** ppenumAdvise)
    {
        return S_OK;
    }
};


class CRichEditCtrlEx : public CWindowImpl<CRichEditCtrlEx, CRichEditCtrl>
{

public:
    
    CRichEditCtrlEx( HWND hNotifyWnd = NULL ) : 
        m_hNotifyWnd(hNotifyWnd),
        m_lLastLinkMin(-1)
    {
		m_bNoFocus = FALSE;
    }

    ~CRichEditCtrlEx(void)
    {
    }

    BEGIN_MSG_MAP(CRichEditCtrlEx)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        //MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkGnd)
        //MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
        REFLECTED_NOTIFY_CODE_HANDLER(EN_LINK, OnLinkClicked)
    END_MSG_MAP()


    BOOL ClearAll(void)
    {
        if ( m_hWnd != NULL)
        {
            SetReadOnly(FALSE);

            SetSel(0, -1);
            Clear();

            SetReadOnly(TRUE);
        }

        m_mapLinkId.RemoveAll();

        Invalidate(TRUE);

        return TRUE;
    }

    BOOL AppendLink(LPCTSTR lpszTextLink, UINT uLinkID = 0)
    {
        CHARFORMAT2  Format2;
        CHARRANGE charRange = {0};
        CHARFORMAT2 cf;

        LONG lBegin = 0;
        LONG lEnd = 0;

        ::ZeroMemory(&cf, sizeof(CHARFORMAT2));

        cf.cbSize = sizeof(CHARFORMAT2);

        GetDefaultCharFormat(cf);

        cf.dwMask |= CFM_BOLD;
        cf.dwEffects &= ~CFE_BOLD;

        SetReadOnly(FALSE);
        _PushCurrentSelPos();

        GetSelectionCharFormat(Format2);

        SetSel(-1, -1);
        lBegin = GetCurrentSelPos();
        ReplaceSel(lpszTextLink);

        SetSel(-1, -1);
        lEnd = GetCurrentSelPos();

        charRange.cpMin = lBegin;
        charRange.cpMax = lEnd;
        SetSel(charRange);

        Format2.dwMask |= CFM_LINK;
        Format2.dwEffects |= CFE_LINK;

        Format2.dwMask |= CFM_BOLD;
        Format2.dwEffects &= ~CFE_BOLD;

        SendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&Format2);

        _PopCurrentSelPos();

        Format2.dwMask &= (~CFM_LINK);
        Format2.dwEffects &= (~CFE_LINK);

        SendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&Format2);
        SetSelectionCharFormat(cf);	

        m_mapLinkId[charRange.cpMin] = uLinkID;

        SetReadOnly(TRUE);

        return TRUE;
    }

    BOOL AppendText(LPCTSTR lpszText, UINT uFormat, COLORREF crText = RGB(0, 0, 0))
    {
        CHARRANGE charRange = {0};
        CHARFORMAT2 cf;

        LONG lBegin = 0;
        LONG lEnd = 0;

        HDC hDesktop = ::GetDC(HWND_DESKTOP);

        int TwipsPerPixelY = 1440 / ::GetDeviceCaps(hDesktop, LOGPIXELSY);

        ::ReleaseDC(HWND_DESKTOP, hDesktop);

        ::ZeroMemory(&cf, sizeof(CHARFORMAT2));

        cf.cbSize = sizeof(CHARFORMAT2);

        SetReadOnly(FALSE);
        GetDefaultCharFormat(cf);

        cf.dwMask |= CFM_SIZE;
        cf.yHeight = TwipsPerPixelY * 12;

        cf.dwMask |= CFM_COLOR;
        cf.crTextColor = crText;

        if (uFormat & RICHEDIT_TEXT_FONT_BOLD)
        {
            cf.dwMask |= CFM_BOLD;
            cf.dwEffects |=CFE_BOLD;
        }
        if (uFormat & RICHEDIT_TEXT_FONT_ITALIC)
        {
            cf.dwMask |= CFM_ITALIC;
            cf.dwEffects |=CFE_ITALIC;
        }
        if (uFormat & RICHEDIT_TEXT_FONT_OUTLINE)
        {
            cf.dwMask |= CFM_OUTLINE;
            cf.dwEffects |=CFE_OUTLINE;
        }
        if (uFormat & RICHEDIT_TEXT_FONT_STRIKEOUT)
        {
            cf.dwMask |= CFM_STRIKEOUT;
            cf.dwEffects |=CFE_STRIKEOUT;
        }
        if (uFormat & RICHEDIT_TEXT_FONT_UNDERLINE)
        {
            cf.dwMask |= CFM_UNDERLINE;
            cf.dwEffects |=CFE_UNDERLINE;
        }

        cf.crTextColor = crText;
        cf.dwMask |= CFM_COLOR;
        cf.dwEffects &= ~CFE_AUTOCOLOR; 

        _PushCurrentSelPos();

        SetSel(-1, -1);
        lBegin = GetCurrentSelPos();
        ReplaceSel(lpszText);

        SetSel(-1, -1);
        lEnd = GetCurrentSelPos();

        charRange.cpMin = lBegin;
        charRange.cpMax = lEnd;
        SetSel(charRange);
        SetSelectionCharFormat(cf);

        _PopCurrentSelPos();

        SetReadOnly(TRUE);

        return TRUE;
    }

    BOOL AppendBitmap(HBITMAP hImg)
    {
        int nRetCode = FALSE;

        SetReadOnly(FALSE);
        _PushCurrentSelPos();

        SetSel(-1, -1);
        nRetCode = _InsertBitmap(hImg);	

        _PopCurrentSelPos();
        SetReadOnly(TRUE);

        return nRetCode;
    }

    BOOL AppendBitmap(LPCTSTR lpszFileName)
    {
        int nRetCode = FALSE;
        HBITMAP hBmp = NULL;

        ATLASSERT(lpszFileName);

        SetReadOnly(FALSE);
        hBmp = (HBITMAP)::LoadImage(NULL, lpszFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        if (!hBmp)
        {
            return FALSE;
        }

        _PushCurrentSelPos();

        SetSel(-1, -1);
        nRetCode = _InsertBitmap(hBmp);	

        _PopCurrentSelPos();

        ::DeleteObject(hBmp);
        SetReadOnly(TRUE);

        return TRUE;	
    }


    BOOL AppendEndOfLine(void)
    {
        SetReadOnly(FALSE);
        _PushCurrentSelPos();

        SetSel(-1, -1);
        ReplaceSel(TEXT("\n"));	

        _PopCurrentSelPos();
        SetReadOnly(TRUE);

        return TRUE;
    }

    void SetLinkClickedNotifyWindow(HWND hWnd)
    {
        m_hNotifyWnd = hWnd;
    }

    BOOL SubclassWindow(HWND hWnd)
    {
        BOOL bResult = CWindowImpl<CRichEditCtrlEx, CRichEditCtrl>::SubclassWindow(hWnd);
        if (!bResult)
            return bResult;
            
        _InitRichEdit();

        return bResult;
    }


	void SetNoFocus(BOOL bNoFocus)
	{
		m_bNoFocus	= bNoFocus;
	}
protected:

    void _InitRichEdit()
    {
        SetEventMask(ENM_LINK | GetEventMask());

        if (!m_hNotifyWnd)
        {
            m_hNotifyWnd = GetParent();
        }

    }

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        _InitRichEdit();

        return DefWindowProc();
    }

    LRESULT OnEraseBkGnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return 1L;
    }


    LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {
        HCURSOR hHandCursor= ::LoadCursor(NULL, IDC_HAND);
        HCURSOR hNow = NULL;

        LRESULT lResult = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);

        return lResult;

        hNow = ::GetCursor();
        if (hNow != hHandCursor)
        {
            ::SetCursor(::LoadCursor(NULL, IDC_ARROW));   
        }

        return lResult;
    }

	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (m_bNoFocus)
			return TRUE;
		
		bHandled = FALSE;
		return 0;
	}
    LRESULT OnLinkClicked(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
    {
        ENLINK *pLink   = (ENLINK *)pnmh;
        UINT    uLinkID = 0;
        

        LRESULT lResult = DefWindowProc();
        if (pLink == NULL)
            return lResult;

        _PushCurrentSelPos();

        if (pLink->msg == WM_LBUTTONDOWN)
        {
            m_lLastLinkMin = pLink->chrg.cpMin;
        }
        if (pLink->msg == WM_LBUTTONUP && m_lLastLinkMin == pLink->chrg.cpMin)
        {
            uLinkID = m_mapLinkId[pLink->chrg.cpMin];

            TEXTRANGE text = { 0 };

            text.chrg = pLink->chrg;

            SetSel(text.chrg);

            if ( (text.chrg.cpMax - text.chrg.cpMin) > 0)
            {
                text.lpstrText = new TCHAR[text.chrg.cpMax - text.chrg.cpMin + 1];

                if (text.lpstrText != NULL)
                {
                    memset(text.lpstrText, 
                        0, 
                        sizeof(text.lpstrText[0]) * (text.chrg.cpMax - text.chrg.cpMin + 1) );

                    GetSelText(text.lpstrText);

                    ::SendMessage(
                        m_hNotifyWnd, 
                        KM_RICHEDIT_LINK_CLICKED_NOTIFY, 
                        WPARAM(uLinkID), 
                        LPARAM(text.lpstrText)
                        );

                    delete []text.lpstrText;
                }
            }

            m_lLastLinkMin = -1;
        }
        
        _PopCurrentSelPos();

        return lResult;
    }


    void _PushCurrentSelPos(void)
    {
        GetSel(m_charRange);
    }

    void _PopCurrentSelPos(void)
    {
        SetSel(m_charRange);
    }

    LONG GetCurrentSelPos(void)
    {
        CHARRANGE charRange = {0};

        GetSel(charRange);

        return charRange.cpMax;
    }

    BOOL _InsertBitmap(HBITMAP hBitmap)
    {
        int nResult = FALSE;

        STGMEDIUM stgm;
        IStorage *pStorage = NULL;
        FORMATETC fm;

        LPLOCKBYTES lpLockBytes = NULL;
        IOleObject *pOleObject = NULL;

        CDataObject DataObject(hBitmap);

        LPOLECLIENTSITE lpClientSite;

        HRESULT hr = E_FAIL;

        CLSID clsid;
        REOBJECT reobject = {0};

        if (hBitmap == NULL)
            return FALSE;

        stgm.tymed = TYMED_GDI;    // Storage medium = HBITMAP handle
        stgm.hBitmap = hBitmap;
        stgm.pUnkForRelease = NULL; // Use ReleaseStgMedium


        fm.cfFormat = CF_BITMAP;    // Clipboard format = CF_BITMAP
        fm.ptd = NULL;       // Target Device = Screen
        fm.dwAspect = DVASPECT_CONTENT;   // Level of detail = Full content
        fm.lindex = -1;       // Index = Not applicaple
        fm.tymed = TYMED_GDI;  


        hr = ::CreateILockBytesOnHGlobal(NULL, TRUE, &lpLockBytes);
        if (FAILED(hr))
            goto Exit0;

        ATLASSERT(lpLockBytes != NULL);

        hr = ::StgCreateDocfileOnILockBytes(
            lpLockBytes,
            STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 
            0, 
            &pStorage
            );

        ATLASSERT(pStorage);

        if (FAILED(hr) || pStorage == NULL)
            goto Exit0;

        GetOleInterface()->GetClientSite( &lpClientSite );

        //创建OLE对象

        hr = OleCreateStaticFromData(&DataObject, IID_IOleObject, OLERENDER_FORMAT,
            &fm, lpClientSite, pStorage, (void **)&pOleObject);
        if (FAILED(hr) || pOleObject == NULL)
            goto Exit0;

        //插入OLE对象

        reobject.cbStruct = sizeof(REOBJECT);

        hr = pOleObject->GetUserClassID(&clsid);
        if (FAILED(hr))
            goto Exit0;

        reobject.clsid = clsid;
        reobject.cp = REO_CP_SELECTION;
        reobject.dvaspect = DVASPECT_CONTENT;
        reobject.poleobj = pOleObject;
        reobject.polesite = lpClientSite;
        reobject.pstg = pStorage;

        hr = GetOleInterface()->InsertObject(&reobject);

        nResult = TRUE;

    Exit0:

        if (lpLockBytes)
        {
            lpLockBytes = NULL;
        }

        return nResult;
    }



private:

    ATL::CAtlMap<UINT, UINT> m_mapLinkId;

    LONG				m_lLastLinkMin;
    CHARRANGE			m_charRange;
    HWND				m_hNotifyWnd;

	BOOL				m_bNoFocus;						// 禁止点选

};

