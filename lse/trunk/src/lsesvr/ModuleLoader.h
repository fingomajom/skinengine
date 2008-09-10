/********************************************************************
* CreatedOn: 2008-9-9   14:32
* FileName: ModuleLoader.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include "..\public\lsesvr_m.h"


class ModuleLoader
{
public:
    ModuleLoader(void);
    ~ModuleLoader(void);

    HRESULT LoadModule( PModule_Config_Info pModuleInfo );

    HRESULT LoadAllModule();
    

    static HANDLE RunProcessModule( LPCTSTR pszDllFile, LPCTSTR pszType );

};
