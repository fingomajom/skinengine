/********************************************************************
	created:	2009/05/03
	created:	3:5:2009   20:10
	filename: 	e:\lidengwang\gcode\se\trunk\src\dw_explorer\DWAxHost.h
	file path:	e:\lidengwang\gcode\se\trunk\src\dw_explorer
	file base:	DWAxHost.h
	author:		lidengwang@sohu-rd.com
	
	purpose:	
*********************************************************************/

#include <atlhost.h>

#pragma once


class CDWAxHost : public CAxHostWindow
{
public:

    ~CDWAxHost()
    {
    }

    STDMETHOD(CreateControl)(HWND hWnd)
    {
        CComPtr<IUnknown> p;

        ReleaseAll();

        ATLASSERT(m_hWnd == NULL);
        ATLASSERT(::IsWindow(hWnd));
        SubclassWindow(hWnd);

        HRESULT hr = CoCreateInstance(CLSID_WebBrowser, NULL, CLSCTX_INPROC_SERVER, __uuidof(IUnknown), (void**)&m_spUnknown);
        for (int i = 1; i < 10; ++i)
        {
            if (FAILED(hr) || !m_spUnknown)
                hr = CoCreateInstance(CLSID_WebBrowser, NULL, CLSCTX_INPROC_SERVER, __uuidof(IUnknown), (void**)&m_spUnknown);
            else
                break ;
            ATLASSERT(SUCCEEDED(hr));
            ATLASSERT(m_spUnknown);
            Sleep(i * 500) ;
        }

        m_spUnknown->QueryInterface(__uuidof(IOleObject), (void**)&m_spOleObject);
        if (m_spOleObject)
        {
            m_spOleObject->GetMiscStatus(DVASPECT_CONTENT, &m_dwMiscStatus);
            if (m_dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST)
            {
                CComQIPtr<IOleClientSite> spClientSite(GetControllingUnknown());
                m_spOleObject->SetClientSite(spClientSite);
            }

            CComQIPtr<IPersistStreamInit> spPSI(m_spOleObject);
            if (spPSI)
                hr = spPSI->InitNew();

            if (FAILED(hr)) // If the initialization of the control failed...
            {
                // Clean up and return
                if (m_dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST)
                    m_spOleObject->SetClientSite(NULL);

                m_dwMiscStatus = 0;
                m_spOleObject.Release();
                m_spUnknown.Release();

                return hr;
            }

            if (0 == (m_dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST))
            {
                CComQIPtr<IOleClientSite> spClientSite(GetControllingUnknown());
                m_spOleObject->SetClientSite(spClientSite);
            }

            m_dwViewObjectType = 0;
            hr = m_spOleObject->QueryInterface(__uuidof(IViewObjectEx), (void**) &m_spViewObject);
            if (FAILED(hr))
            {
                hr = m_spOleObject->QueryInterface(__uuidof(IViewObject2), (void**) &m_spViewObject);
                if (SUCCEEDED(hr))
                    m_dwViewObjectType = 3;
            } else
                m_dwViewObjectType = 7;

            if (FAILED(hr))
            {
                hr = m_spOleObject->QueryInterface(__uuidof(IViewObject), (void**) &m_spViewObject);
                if (SUCCEEDED(hr))
                    m_dwViewObjectType = 1;
            }
            CComQIPtr<IAdviseSink> spAdviseSink(GetControllingUnknown());
            m_spOleObject->Advise(spAdviseSink, &m_dwOleObject);
            if (m_spViewObject)
                m_spViewObject->SetAdvise(DVASPECT_CONTENT, 0, spAdviseSink);
            m_spOleObject->SetHostNames(OLESTR("AXWIN"), NULL);

            if ((m_dwMiscStatus & OLEMISC_INVISIBLEATRUNTIME) == 0)
            {
                GetClientRect(&m_rcPos);
                m_pxSize.cx = m_rcPos.right - m_rcPos.left;
                m_pxSize.cy = m_rcPos.bottom - m_rcPos.top;
                AtlPixelToHiMetric(&m_pxSize, &m_hmSize);
                m_spOleObject->SetExtent(DVASPECT_CONTENT, &m_hmSize);
                m_spOleObject->GetExtent(DVASPECT_CONTENT, &m_hmSize);
                AtlHiMetricToPixel(&m_hmSize, &m_pxSize);
                m_rcPos.right = m_rcPos.left + m_pxSize.cx;
                m_rcPos.bottom = m_rcPos.top + m_pxSize.cy;

                CComQIPtr<IOleClientSite> spClientSite(GetControllingUnknown());
                hr = m_spOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, spClientSite, 0, m_hWnd, &m_rcPos);
                RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_FRAME);
            }
        }
        CComPtr<IObjectWithSite> spSite;
        m_spUnknown->QueryInterface(__uuidof(IObjectWithSite), (void**)&spSite);
        if (spSite != NULL)
            spSite->SetSite(GetControllingUnknown());

        return hr;
    }


};