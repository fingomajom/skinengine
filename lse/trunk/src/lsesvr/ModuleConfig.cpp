#include "StdAfx.h"
#include "ModuleConfig.h"


#define RegSoftKey          L"SOFTWARE"
#define RegLseKey           L"SOFTWARE\\LSE"
#define RegModuleListKey    L"SOFTWARE\\LSE\\ModuleList"


BOOL CModuleConfigReg::LoadConfig()
{
    CRegKey reg;

    if ( reg.Open( HKEY_LOCAL_MACHINE , RegModuleListKey, KEY_READ) != ERROR_SUCCESS )
        return FALSE;

    for (int idx = 0; true ; idx++)
    {
        Module_Config_Info configInfo = { 0 };
        DWORD dwLength = MAX_MODULE_NAME;

        if (reg.EnumKey( idx, configInfo.szModuleName, &dwLength, NULL ) != ERROR_SUCCESS )
        {
            break;
        }

        CRegKey regModule;

        if ( regModule.Open( reg , configInfo.szModuleName, KEY_READ) != ERROR_SUCCESS )
            continue;
        
        dwLength = MAX_PATH;

        if ( regModule.QueryStringValue( _T("ImagePathFile"), configInfo.szModulePathFile, &dwLength) != ERROR_SUCCESS )
            continue;

        if ( regModule.QueryDWORDValue( _T("ModuleId"), configInfo.uModuleId) != ERROR_SUCCESS )
            continue;

        if ( regModule.QueryDWORDValue( _T("ModuleType"), configInfo.uModuleType) != ERROR_SUCCESS )
            continue;

        m_config.Add( configInfo );
    }

    return TRUE;
}
BOOL CModuleConfigReg::Unload()
{
    m_config.RemoveAll();

    return TRUE;
}

LONG CModuleConfigReg::GetModuleCount()
{
    return m_config.GetSize();

    return TRUE;
}

BOOL CModuleConfigReg::GetModuleConfig( int idx, Module_Config_Info& config)
{
    if (idx < 0 || idx >= m_config.GetSize())
        return FALSE;

    config = m_config[idx];

    return TRUE;
}

BOOL CModuleConfigReg::ClearAll()
{
    CRegKey reg;

    if ( reg.Open( HKEY_LOCAL_MACHINE , RegModuleListKey, KEY_READ | KEY_WRITE) != ERROR_SUCCESS )
        return TRUE;

    for (int idx = 0; true ; idx++)
    {
        WCHAR szModuleName[MAX_MODULE_NAME];
        DWORD dwLength = MAX_MODULE_NAME;

        if (reg.EnumKey( idx, szModuleName, &dwLength, NULL ) != ERROR_SUCCESS )
        {
            break;
        }
        
        reg.DeleteSubKey( szModuleName );
    }

    return TRUE;
}

BOOL CModuleConfigReg::DeleteModuleConfig(const Module_Config_Info& config)
{
    CRegKey reg;

    if ( reg.Open( HKEY_LOCAL_MACHINE , RegModuleListKey, KEY_READ | KEY_WRITE) != ERROR_SUCCESS )
        return TRUE;

    for (int idx = 0; true ; idx++)
    {
        WCHAR szModuleName[MAX_MODULE_NAME];
        DWORD dwLength = MAX_MODULE_NAME;

        if (reg.EnumKey( idx, szModuleName, &dwLength, NULL ) != ERROR_SUCCESS )
        {
            break;
        }

        if (  CAtlModule::WordCmpI( szModuleName, config.szModuleName ) == 0 )
            reg.DeleteSubKey( szModuleName );
    }


    return TRUE;
}

BOOL CModuleConfigReg::SetModuleConfig(const Module_Config_Info& config)
{
    
    CRegKey reg;
    CRegKey regNew;


    if ( reg.Open( HKEY_LOCAL_MACHINE , RegModuleListKey, KEY_READ | KEY_WRITE) != ERROR_SUCCESS )
    {
        reg.Create( HKEY_LOCAL_MACHINE, RegLseKey ); reg.Close();
        reg.Create( HKEY_LOCAL_MACHINE, RegModuleListKey ); ; reg.Close();

        if ( reg.Open( HKEY_LOCAL_MACHINE , RegModuleListKey, KEY_READ | KEY_WRITE) != ERROR_SUCCESS )
        {
            return FALSE;
        }
    }

    if ( regNew.Open( reg , config.szModuleName, KEY_READ | KEY_WRITE) != ERROR_SUCCESS )
    {
        if (regNew.Create( reg, config.szModuleName ) != ERROR_SUCCESS )
            return FALSE;
    }

    if ( regNew.SetStringValue( _T("ImagePathFile"), config.szModulePathFile) != ERROR_SUCCESS )
        return FALSE;

    if ( regNew.SetDWORDValue( _T("ModuleId"), config.uModuleId) != ERROR_SUCCESS )
        return FALSE;

    if ( regNew.SetDWORDValue( _T("ModuleType"), config.uModuleType) != ERROR_SUCCESS )
        return FALSE;


    return TRUE;
}
