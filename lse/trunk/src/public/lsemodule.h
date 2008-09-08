/********************************************************************
* CreatedOn: 2008-9-8   16:03
* FileName: lsesvrmodule.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include "lsectl.h"

struct __declspec(uuid("E6285703-1255-4cff-9142-20D049B37917")) ISvrCallbackObject;
interface ISvrCallbackObject : public IUnknown
{
public:

    virtual HRESULT STDMETHODCALLTYPE CallSvrFunc( 
        /* [in]  */ ULONG uTargetId,
        /* [in]  */ ULONG uCallerId,
        /* [in]  */ ULONG uFunctionId,
        /* [in]  */ IDataBuffer*  pParameter,
        /* [out] */ IDataBuffer** ppResult) = 0;

};


struct __declspec(uuid("4F364374-A3BA-4eef-B9F2-02E0FE336FA8")) IModuleObject;
interface IModuleObject : public IUnknown
{
public:

    virtual HRESULT STDMETHODCALLTYPE Initialize( 
        /* [in] */ LPVOID pvParameter ) = 0;

    virtual HRESULT STDMETHODCALLTYPE Uninitialize( void ) = 0;

    virtual HRESULT STDMETHODCALLTYPE CallModuleFunc( 
        /* [in]  */ ULONG uTargetId,
        /* [in]  */ ULONG uCallerId,
        /* [in]  */ ULONG uFunctionId,
        /* [in]  */ IDataBuffer*  pParameter,
        /* [out] */ IDataBuffer** ppResult) = 0;

};
