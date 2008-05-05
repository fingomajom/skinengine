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

    HRESULT AddCallback(IUnknown* pUnkSink, DWORD dwCookie);
    HRESULT RemoveCallback( DWORD dwCookie );

    HRESULT Fire_NotifyMessage( ULONG uDesCallerId,  ULONG uSrcModuleId,  BSTR bstrFunctionName,  BSTR bstrParameter,  BSTR * bstrResult);

protected:

    CSimpleArray<Callback_Info> m_callbacks;

    ATL::CComAutoCriticalSection m_cs;
};
