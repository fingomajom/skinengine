

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