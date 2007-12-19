/********************************************************************
* CreatedOn: 2007-12-19   17:40
* FileName: EditResBase.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

class CEditResBase
{
public:

    virtual BOOL LoadResFromDocumnet()
    {
        return FALSE;
    }

    virtual BOOL SaveResToDocument()
    {
        return FALSE;
    }

    virtual BOOL ClearRes()
    {
        return FALSE;
    }
};