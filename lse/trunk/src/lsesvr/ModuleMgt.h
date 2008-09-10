/********************************************************************
* CreatedOn: 2008-9-8   15:58
* FileName: ModuleMgt.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include <atlcoll.h>

#include "ModuleInfo.h"

class CModuleMgt 
{
public:

    static CModuleMgt& Instance();

protected:

    CModuleMgt(void);

public:

    ~CModuleMgt(void);

public:

    HRESULT STDMETHODCALLTYPE CModuleMgt::Invoke(
        DISPPARAMS* pdispparams, 
        VARIANT*    pvarResult);

    HRESULT STDMETHODCALLTYPE CModuleMgt::CallSvrFunc( 
        /* [in]  */ ULONG uTargetId,
        /* [in]  */ ULONG uCallerId,
        /* [in]  */ ULONG uFunctionId,
        /* [in]  */ IDataBuffer*  pParameter,
        /* [out] */ IDataBuffer** ppResult);

public:

    HRESULT AddModuleObject      ( LPCTSTR pszModuleFile );

    HRESULT AddCallerCallback    ( IUnknown* pUnkSink, DWORD dwCookie );
    HRESULT RemoveCallerCallback ( DWORD dwCookie );



public:

    HRESULT AddModuleInfo( CModuleInfo* pModuleInfo, BOOL bCaller = FALSE );
    HRESULT RemoveModuleInfo ( ULONG uModuleId , DWORD dwCookie = -1);

private:

    ATL::CAtlMap<DWORD, ATL::CSimpleArray< CModuleInfo* > > m_mapModuleId2Info;

    ATL::CAtlMap<DWORD, DWORD> m_mapCookie2ModuleId;

    ATL::CComAutoCriticalSection m_cs;

};
