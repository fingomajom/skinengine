/********************************************************************
* CreatedOn: 2008-9-14   0:20
* FileName: lsecaller.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include "lsectl.h"
#include "lsesvr.h"

class CLSECaller : 
    public IDispatch
{
public:

    CLSECaller() :
        m_dwEventCookie( 0xFEFEFEFE )
    {
    }

    HRESULT Initialize( 
        /* [in] */ ULONG     uCallerId,
        /* [in] */ IUnknown *piCallback)
    {
        CComPtr<IClassFactory> spFactory;

        m_uCallerId = uCallerId;
        if (m_uCallerId == CALLERID_UNKNOWN)
            return E_FAIL;

        HRESULT hr = CoGetClassObject( 
            __uuidof(SvrObject), CLSCTX_LOCAL_SERVER, 0, 
            IID_IClassFactory, (void**)&spFactory );

        if (FAILED(hr))
            goto Exit0;

        hr = spFactory->CreateInstance(NULL, 
            __uuidof(IDispatch), (void**)&m_spSvrObject );

        if (FAILED(hr))
            goto Exit0;

        if (piCallback != NULL)
        {
            hr = piCallback->QueryInterface(__uuidof(ILSECallback),
                (void**)&m_spCallback);
            if (FAILED(hr) || m_spCallback.p == NULL)
                goto Exit0;

            hr = DispEventAdvise(m_spSvrObject);
            if (FAILED(hr))
                goto Exit0;
        }

        return S_OK;

Exit0:
        Uninitialize();

        return E_FAIL;
    }


    HRESULT Uninitialize( void )
    {
        m_uCallerId = CALLERID_UNKNOWN;

        if (m_spSvrObject.p != NULL)
        {
            DispEventUnadvise(m_spSvrObject);

            m_spSvrObject.Detach()->Release();
        }

        m_spCallback.Release();

        return S_OK;
    }


    HRESULT CallSvrFunc( 
        /* [in]  */ ULONG uTargetId,
        /* [in]  */ ULONG uFunctionId,
        /* [in]  */ IDataBuffer*  pParameter,
        /* [out] */ IDataBuffer** ppResult)
    {
        HRESULT hr = E_FAIL;

        if ( uTargetId == m_uCallerId )
            return E_NOTIMPL;


        ATLASSERT( m_uCallerId != CALLERID_UNKNOWN );
        if ( m_uCallerId == CALLERID_UNKNOWN )
            return E_FAIL;

        CComVariant avarParams[5];
        CComVariant varResult;

        avarParams[4] = uTargetId;
        avarParams[4].vt = VT_UI4;
        avarParams[3] = m_uCallerId;
        avarParams[3].vt = VT_UI4;
        avarParams[2] = uFunctionId;
        avarParams[2].vt = VT_UI4;

        avarParams[1].vt = VT_EMPTY;

        if (pParameter != NULL)
        {
            avarParams[1].parray = DataBuffer2SafeArray( pParameter );
            avarParams[1].vt = avarParams[0].parray ? VT_UI1 | VT_ARRAY : VT_EMPTY;
        }

        DISPPARAMS params = { avarParams, NULL, 5, 0 };

        bool bfirst = true;

        do 
        {
            if (m_spSvrObject.p != NULL)
            {
                hr = m_spSvrObject->Invoke(dispAPI_CallFunction, 
                    IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, 
                    &params, &varResult, NULL, NULL);
            }

            if ( !bfirst || (m_spSvrObject.p != NULL && hr != ERROR_RPC_SERVER_UNAVAILABLE ) )
                break; 

            Relive();

            bfirst = false;

        } while( true );


        if (SUCCEEDED(hr) && ppResult != NULL && (varResult.vt & VT_ARRAY) )
        {
            *ppResult = SafeArray2DataBuffer( varResult.parray );
        }

        return hr;
    }



protected:

    HRESULT Relive()
    {
        HRESULT hResult = E_FAIL;

        ULONG             uCallerId  = m_uCallerId;
        CComPtr<IUnknown> spCallback = m_spCallback;

        Uninitialize();

        hResult = Initialize(uCallerId, spCallback);

        if ( FAILED(hResult) )
        {
            m_uCallerId  = uCallerId;
            m_spCallback = spCallback;
        }

        return hResult;
    }

    STDMETHODIMP QueryInterface(const struct _GUID &iid,void ** ppv)
    {
        if ((iid) == __uuidof(IDispatch)) 
        { 
            *ppv = static_cast<IDispatch*>(this); 
            AddRef();
            return S_OK;
        }
        else if ((iid) == IID_IUnknown)
        {
            *ppv = static_cast<IUnknown*>(this); 
            AddRef();
            return S_OK;
        }

        return E_FAIL; 
    }

    ULONG __stdcall AddRef(void)
    {	return 1;	}

    ULONG __stdcall Release(void)
    {	return 0;	}	

    STDMETHODIMP GetTypeInfoCount(UINT* pctinfo)
    {	
        if (pctinfo != NULL)
            *pctinfo = m_uCallerId;

        return S_OK;
    }	

    STDMETHODIMP GetTypeInfo(unsigned int,unsigned long,struct ITypeInfo ** )
    {	
        return E_NOTIMPL;	
    }	

    STDMETHODIMP GetIDsOfNames( 
        /* [in] */ REFIID riid,
        /* [size_is][in] */ LPOLESTR *rgszNames,
        /* [in] */ UINT cNames,
        /* [in] */ LCID lcid,
        /* [size_is][out] */ DISPID *rgDispId)
    {	
        if (rgDispId != NULL)
            *rgDispId = m_uCallerId;

        return E_NOTIMPL;	
    }	


    STDMETHODIMP Invoke(
        /* [in] */ DISPID dispIdMember,
        /* [in] */ REFIID riid,
        /* [in] */ LCID lcid,
        /* [in] */ WORD wFlags,
        /* [out][in] */ DISPPARAMS *pDispParams,
        /* [out] */ VARIANT *pVarResult,
        /* [out] */ EXCEPINFO *pExcepInfo,
        /* [out] */ UINT *puArgErr)
    {

        if ( m_spCallback.p == NULL )
            return S_OK;

        if ( dispIdMember == dispAPI_CallFunction)
        {
            CComPtr<IDataBuffer> spParameter;
            CComPtr<IDataBuffer> spResult;

            if (pDispParams == NULL || pDispParams->cArgs != 5)
                return E_FAIL;

            if (  pDispParams->rgvarg[4].vt != VT_UI4 ||
                pDispParams->rgvarg[3].vt != VT_UI4 ||
                pDispParams->rgvarg[2].vt != VT_UI4 ||
                !( pDispParams->rgvarg[1].vt & VT_ARRAY ) )
                return E_FAIL;

            spParameter = SafeArray2DataBuffer( pDispParams->rgvarg[1].parray );

            HRESULT hr = m_spCallback->CallbackFunction (
                pDispParams->rgvarg[4].ulVal,
                pDispParams->rgvarg[3].ulVal,
                pDispParams->rgvarg[2].ulVal,
                spParameter,
                &spResult);

            if (spResult.p != NULL)
                spResult.p->AddRef();

            if (SUCCEEDED(hr) && spResult != NULL && pVarResult != NULL)
            {
                pVarResult[0].parray = DataBuffer2SafeArray( spResult );
                pVarResult[0].vt = pVarResult[0].parray ? VT_UI1 | VT_ARRAY : VT_EMPTY;
            }
        }
        else if ( dispIdMember == dispAPI_Quit )
        {
            m_spCallback->OnQuit();
        }


        return S_OK;
    }

    HRESULT DispEventAdvise(IUnknown* pUnk)
    {
        if (m_dwEventCookie != 0xFEFEFEFE)
            return S_OK;

        return AtlAdvise(pUnk, (IUnknown*)this, __uuidof(IDispatch), &m_dwEventCookie);
    }

    HRESULT DispEventUnadvise(IUnknown* pUnk)
    {

        HRESULT hr = S_OK;
        
        if (m_dwEventCookie != 0xFEFEFEFE )
            hr = AtlUnadvise(pUnk, __uuidof(IDispatch), m_dwEventCookie);

        m_dwEventCookie = 0xFEFEFEFE;

        return hr;
    }


private:

    DWORD m_dwEventCookie;
    ULONG m_uCallerId;

    CComPtr<IDispatch>    m_spSvrObject;
    CComPtr<ILSECallback> m_spCallback;
};
