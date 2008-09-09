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

class ModuleLoader
{
public:
    ModuleLoader(void);
    ~ModuleLoader(void);

    HRESULT LoadAllModule();


    static HANDLE RunProcessModule( LPCTSTR pszDllFile, LPCTSTR pszType );

};
