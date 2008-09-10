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

#include "IDataBuffer.h"


#define MAX_MODULE_NAME  50


enum{
    em_module_type_normal = 0,
    em_module_type_run_process = 1,
    em_module_type_processmodule = 2
};

typedef struct tag_Module_Config_Info
{
    ULONG uModuleId;
    ULONG uModuleType;
    WCHAR szModuleName[MAX_MODULE_NAME];
    WCHAR szModulePathFile[MAX_PATH];

}Module_Config_Info, *PModule_Config_Info;


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

struct __declspec(uuid("FFCBC744-D941-4e5e-9EB6-622334135E6D")) ISvrModuleMgt;
interface ISvrModuleMgt : public IUnknown
{
public:

    virtual HRESULT STDMETHODCALLTYPE StartModule(
        /* [in]  */ ULONG uModuleId ) = 0;

    virtual HRESULT STDMETHODCALLTYPE StartDllModule(
        /* [in]  */ PModule_Config_Info pModuleInfo ) = 0;

    virtual HRESULT STDMETHODCALLTYPE StopModule(
        /* [in]  */ ULONG uModuleId ) = 0;
};

