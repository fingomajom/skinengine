#pragma once

#include "..\public\lsemodule.h"


class ATL_NO_VTABLE CTestObject :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CTestObject, &__uuidof(IModuleObject)>,
    public IModuleObject
{
public:

    DECLARE_REGISTRY_RESOURCEID(2)


    BEGIN_COM_MAP(CTestObject)
        COM_INTERFACE_ENTRY(IModuleObject)
        COM_INTERFACE_ENTRY(IUnknown)
    END_COM_MAP()

    virtual HRESULT STDMETHODCALLTYPE Initialize( 
    /* [in] */ IUnknown* piSvrCallbackObject )
    {
        if (piSvrCallbackObject == NULL)
            Sleep(15000);

        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE Uninitialize( void )
    {
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE GetModuleId( /* out */ ULONG* puID )
    {
        *puID = 10;

        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE CallModuleFunc( 
        /* [in]  */ ULONG uCallerId,
        /* [in]  */ ULONG uFunctionId,
        /* [in]  */ IDataBuffer*  pParameter,
        /* [out] */ IDataBuffer** ppResult)
    {
        IDataBuffer* piDataBuffer = LSECreateDataBuffer(5);

        ((BYTE*)piDataBuffer->GetDataBuffer())[0] = 0 ;
        ((BYTE*)piDataBuffer->GetDataBuffer())[1] = 1 ;
        ((BYTE*)piDataBuffer->GetDataBuffer())[2] = 2 ;
        ((BYTE*)piDataBuffer->GetDataBuffer())[3] = 3 ;
        ((BYTE*)piDataBuffer->GetDataBuffer())[4] = 4 ;

        *ppResult = piDataBuffer;


        return S_OK;
    }

};


OBJECT_ENTRY_AUTO(__uuidof(IModuleObject), CTestObject)
