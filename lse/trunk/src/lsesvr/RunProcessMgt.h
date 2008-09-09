/********************************************************************
* CreatedOn: 2008-9-8   22:51
* FileName: RunProcessMgt.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

class RunProcessMgt
{
public:
    RunProcessMgt(void);
    ~RunProcessMgt(void);

    HRESULT RunProcess( LPCTSTR pszDllFile );
};
