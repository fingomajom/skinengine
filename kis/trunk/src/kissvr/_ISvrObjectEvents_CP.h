/********************************************************************
* CreatedOn: 2008-5-4   14:11
* FileName: _ISvrObjectEvents_CP.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include "ATLCPImplMT.h"
#include "SvrCallbackMgt.h"

template<class T>
class CProxy_ISvrObjectEvents :
	public IConnectionPointImplMT<T, &__uuidof(IDispatch), CComDynamicUnkArray>
{
public:
	HRESULT Fire_NotifyMessage( ULONG uDesCallerId,  ULONG uSrcModuleId,  BSTR bstrFunctionName,  BSTR bstrParameter,  BSTR * bstrResult)
	{
        return SvrCallbackMgt::Instance().Fire_NotifyMessage(
            uDesCallerId, 
            uSrcModuleId, 
            bstrFunctionName, 
            bstrParameter, 
            bstrResult);
	}
};

