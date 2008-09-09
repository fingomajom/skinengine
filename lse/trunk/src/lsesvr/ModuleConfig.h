/********************************************************************
* CreatedOn: 2008-9-9   13:29
* FileName: ModuleConfig.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once


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


class IModuleConfigMgt
{
public:

    virtual BOOL LoadConfig() = 0;
    virtual BOOL Unload() = 0;

    virtual LONG GetModuleCount() = 0;
    virtual BOOL GetModuleConfig( int idx, Module_Config_Info& config) = 0;

    virtual BOOL ClearAll() = 0;
    virtual BOOL DeleteModuleConfig(const Module_Config_Info& config) = 0;
    virtual BOOL SetModuleConfig(const Module_Config_Info& config) = 0;

};


class CModuleConfigReg : public IModuleConfigMgt
{
public:

    virtual BOOL LoadConfig();
    virtual BOOL Unload();

    virtual LONG GetModuleCount();
    virtual BOOL GetModuleConfig( int idx, Module_Config_Info& config);

    virtual BOOL ClearAll();
    virtual BOOL DeleteModuleConfig(const Module_Config_Info& config);
    virtual BOOL SetModuleConfig(const Module_Config_Info& config);

private:
    
    ATL::CSimpleArray<Module_Config_Info> m_config;
};
