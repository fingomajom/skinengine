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

#include "..\public\lsesvr_m.h"

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
