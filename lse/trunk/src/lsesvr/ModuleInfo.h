/********************************************************************
* CreatedOn: 2008-9-8   15:59
* FileName: ModuleInfo.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include "..\public\lsemodule.h"
#include "..\public\kis_com_s\kis_esc\KComInterfacePtr.h"

class CModuleInfo
{
public:

    enum {
        em_info_type_module,
        em_info_type_caller
    };

public:

    CModuleInfo(void);
    ~CModuleInfo(void);


    HRESULT STDMETHODCALLTYPE Invoke(
        DISPPARAMS* pdispparams, 
        VARIANT*    pvarResult);

    HRESULT STDMETHODCALLTYPE CallSvrFunc( 
        /* [in]  */ ULONG uTargetId,
        /* [in]  */ ULONG uCallerId,
        /* [in]  */ ULONG uFunctionId,
        /* [in]  */ IDataBuffer*  pParameter,
        /* [out] */ IDataBuffer** ppResult);


    HRESULT InitModule( LPCTSTR   pszModuleFile );
    HRESULT InitCaller( IUnknown* piCallback );
    
    HRESULT Uninit();

public:

    ULONG   m_utype;
    ULONG   m_uModuleId;
    ULONG   m_uCookie;
    ULONG   m_uEntryCount;

    ATL::CComAutoCriticalSection m_cs;

    KComInterfacePtr<IModuleObject> m_spModuleObject;
    CComPtr<IDispatch> m_spCallback;

};
