#include "StdAfx.h"
#include "ModuleLoader.h"
#include "ModuleConfig.h"
#include "ModuleMgt.h"

ModuleLoader::ModuleLoader(void)
{
}

ModuleLoader::~ModuleLoader(void)
{
}


HANDLE ModuleLoader::RunProcessModule( LPCTSTR pszDllFile, LPCTSTR pszType )
{
    TCHAR szCurDirectory[MAX_PATH] = { 0 };
    TCHAR szCmdLine[MAX_PATH] = L"lsesvr.exe ";

    wcsncat_s( szCmdLine, pszType,    MAX_PATH );
    wcsncat_s( szCmdLine, pszDllFile, MAX_PATH );
    
    ::GetCurrentDirectory(MAX_PATH, szCurDirectory);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    ZeroMemory( &pi, sizeof(pi) );

    si.cb = sizeof(si);

    if( !CreateProcess( NULL,   // No module name (use command line). 
        szCmdLine, // Command line. 
        NULL,                   // Process handle not inheritable. 
        NULL,                   // Thread handle not inheritable. 
        FALSE,                   // Set handle inheritance to FALSE. 
        0,                      // No creation flags. 
        NULL,                   // Use parent's environment block. 
        szCurDirectory,         // Use parent's starting directory. 
        &si,                    // Pointer to STARTUPINFO structure.
        &pi )                   // Pointer to PROCESS_INFORMATION structure.
        ) 
    {
        return NULL;
    }

    //CreateProcessAsUser(  );

    return pi.hProcess;
}

HRESULT ModuleLoader::LoadModule( PModule_Config_Info pModuleInfo )
{
    HRESULT hResult        = E_FAIL;
    HANDLE  hProcessHandle = NULL;

    if ( pModuleInfo == NULL )
        return hResult;

    if ( pModuleInfo->uModuleType == em_module_type_normal )
    {
        hResult = CModuleMgt::Instance().AddModuleObject( pModuleInfo->szModulePathFile );
    }
    else if ( pModuleInfo->uModuleType == em_module_type_run_process )
    {
        hProcessHandle = RunProcessModule( pModuleInfo->szModulePathFile, L"/RunProcess " );
        if (hProcessHandle != NULL)
            hResult = S_OK;
    }
    else if ( pModuleInfo->uModuleType == em_module_type_processmodule )
    {
        hProcessHandle = RunProcessModule( pModuleInfo->szModulePathFile, L"/ProcessModule " );
        if (hProcessHandle != NULL)
            hResult = S_OK;
    }

    return hResult;
}


HRESULT ModuleLoader::LoadAllModule()
{
    HRESULT hResult = E_FAIL;
    
    CModuleConfigReg config;

    config.LoadConfig();

    for (int idx = 0; idx < config.GetModuleCount(); idx++)
    {
        Module_Config_Info info = { 0 };
        if ( config.GetModuleConfig( idx, info ) )
        {
            hResult = LoadModule(&info);
        }
    }

    return hResult;
}
