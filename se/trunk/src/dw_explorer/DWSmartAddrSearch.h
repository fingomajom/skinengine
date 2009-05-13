
#pragma once

#include "DWSmartAddrProvider.h"

class CDWSmartAddrSearch : public CDWSmartAddrProvider
{
public:
    CDWSmartAddrSearch()
    {
        CDWSmartAddrProviderList::AddSmartAddrProvider(this);
    }

    int QueryAddrDropList( HWND hWndN, AddrDropItemList& aList, int nMaxCount )
    {
        CComPtr<IHTMLDocument2>       spHTMLDocument;
        CComPtr<IUnknown>             spUnknown;

        CComBSTR bstrTitle;

        HRESULT hr = CoCreateInstance(
            CLSID_HTMLDocument,
            NULL, 
            CLSCTX_INPROC_SERVER, 
            __uuidof(IUnknown), 
            (void**)&spUnknown);

        spUnknown->QueryInterface(&spHTMLDocument);


        CStdioFile file;
        file.Open(L"F:\\s[1].txt", CFile::modeRead );

        CStringA strBuffer;

        file.Read(strBuffer.GetBuffer(((int)file.GetLength())+2),(int)file.GetLength());
        strBuffer.GetBuffer(0)[(int)file.GetLength()] = 0;
        strBuffer.ReleaseBuffer();

        IStream* piStream = NULL;

        CStringW strHtml;
        strHtml = strBuffer;

        SAFEARRAY* psa = SafeArrayCreateVector(VT_VARIANT, 0, 1);
        VARIANT *param;
        CComBSTR bsData = (LPCTSTR)strHtml;
        hr =  SafeArrayAccessData(psa, (LPVOID*)&param);
        param->vt = VT_BSTR;
        param->bstrVal = (BSTR)bsData;


        spHTMLDocument->put_designMode(L"On"); 
        hr = spHTMLDocument->write( psa );
        spHTMLDocument->close();
        hr = spHTMLDocument->get_title(&bstrTitle);

        SafeArrayUnaccessData(psa);
        SafeArrayDestroy(psa);

        return 0;
    }
};


__declspec(selectany) CDWSmartAddrSearch g_CDWSmartAddrSearch;
