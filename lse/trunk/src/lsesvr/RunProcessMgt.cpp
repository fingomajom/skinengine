#include "StdAfx.h"
#include "RunProcessMgt.h"
#include "ModuleInfo.h"

RunProcessMgt::RunProcessMgt(void)
{
}

RunProcessMgt::~RunProcessMgt(void)
{
}

HRESULT RunProcessMgt::RunProcess( LPCTSTR pszDllFile )
{
    HRESULT hResult = E_FAIL;
    
    
    CModuleInfo moduleInfo;

    hResult = moduleInfo.InitModule( pszDllFile , NULL);
    if (FAILED(hResult))
        return hResult;

    hResult = moduleInfo.Uninit();

    return hResult;
}
