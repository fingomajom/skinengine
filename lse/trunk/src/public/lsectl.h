/********************************************************************
* CreatedOn: 2008-9-8   12:56
* FileName: lsectl.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include "IDataBuffer.h"

struct __declspec(uuid("B0B1411F-5D5E-4de1-B4B3-89A276084E2E")) ILSECaller;
interface ILSECaller : public IUnknown
{
public:

    virtual HRESULT STDMETHODCALLTYPE Initialize( 
        /* [in] */ ULONG     uCallerId,
        /* [in] */ IUnknown *piCallback) = 0;

    virtual HRESULT STDMETHODCALLTYPE Uninitialize( void ) = 0;

    virtual HRESULT STDMETHODCALLTYPE CallSvrFunc( 
        /* [in]  */ ULONG uTargetId,
        /* [in]  */ ULONG uFunctionId,
        /* [in]  */ IDataBuffer*  pParameter,
        /* [out] */ IDataBuffer** ppResult) = 0;

};

MIDL_INTERFACE("44E918C4-0540-4d63-BFEA-1BBA0D5CCBFA")
ILSECallback : public IUnknown
{
public:

    virtual HRESULT STDMETHODCALLTYPE CallbackFunction( 
        /* [in]  */ ULONG uTargetId,
        /* [in]  */ ULONG uCallerId,
        /* [in]  */ ULONG uFunctionId,
        /* [in]  */ IDataBuffer*  pParameter,
        /* [out] */ IDataBuffer** ppResult) = 0;

    virtual HRESULT STDMETHODCALLTYPE OnQuit() = 0;

};


class DECLSPEC_UUID("36663AC3-0D27-4B77-A230-7FD7426BB4D3")
SvrCaller;
