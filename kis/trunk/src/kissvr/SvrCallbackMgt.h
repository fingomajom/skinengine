/********************************************************************
* CreatedOn: 2008-5-5   17:02
* FileName: SvrCallbackMgt.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include <atlsimpcoll.h>

class SvrCallbackMgt
{
    struct Callback_Info{
        ULONG      uCallerId ;
        DWORD      dwCookie;
        IDispatch* piDispatch;
    };

public:

    static SvrCallbackMgt& Instance();

    SvrCallbackMgt(void);
    ~SvrCallbackMgt(void);

    BOOL AddCallback(IUnknown* pUnkSink, DWORD dwCookie);
    BOOL RemoveCallback( DWORD dwCookie );

protected:
    CSimpleArray<Callback_Info> m_callbacks;
};
