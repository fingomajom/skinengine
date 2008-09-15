/********************************************************************
* CreatedOn: 2008-9-8   23:09
* FileName: ProcessModuleMgt.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include "..\public\lsesvr_m.h"
#include "..\public\lsecaller.h"
#include "ModuleInfo.h"


class CProcessModuleMgt : 
    public ISvrCallbackObject,
    public ILSECallback
{
public:
    CProcessModuleMgt(void);
    ~CProcessModuleMgt(void);


    HRESULT ProcessModule( LPCTSTR pszDllFile );


    HRESULT STDMETHODCALLTYPE CallSvrFunc( 
        /* [in]  */ ULONG uTargetId,
        /* [in]  */ ULONG uCallerId,
        /* [in]  */ ULONG uFunctionId,
        /* [in]  */ IDataBuffer*  pParameter,
        /* [out] */ IDataBuffer** ppResult);


    HRESULT STDMETHODCALLTYPE CallbackFunction( 
        /* [in]  */ ULONG uTargetId,
        /* [in]  */ ULONG uCallerId,
        /* [in]  */ ULONG uFunctionId,
        /* [in]  */ IDataBuffer*  pParameter,
        /* [out] */ IDataBuffer** ppResult);

    HRESULT STDMETHODCALLTYPE OnQuit();

    STDMETHODIMP QueryInterface(const struct _GUID &iid,void ** ppv)
    {
        if ((iid) == __uuidof(ISvrCallbackObject)) 
        { 
            *ppv = static_cast<ISvrCallbackObject*>(this); 
            AddRef();
            return S_OK;
        }
        else if ((iid) == __uuidof(ILSECallback))
        {
            *ppv = static_cast<ILSECallback*>( static_cast<ILSECallback*>(this)); 
            AddRef();
            return S_OK;
        }
        else if ((iid) == IID_IUnknown)
        {
            *ppv = static_cast<IUnknown*>( static_cast<ISvrCallbackObject*>(this)); 
            AddRef();
            return S_OK;
        }

        return E_FAIL; 
    }

    ULONG __stdcall AddRef(void)
    {	return 1;	}

    ULONG __stdcall Release(void)
    {	return 0;	}	


private:

    HANDLE m_hQuitEvent;
    
    CModuleInfo m_ModuleInfo;
    CLSECaller  m_lseCaller;
};
