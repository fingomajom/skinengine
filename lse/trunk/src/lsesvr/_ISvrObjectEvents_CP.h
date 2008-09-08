/********************************************************************
* CreatedOn: 2008-9-8   11:53
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

template<class T>
class CProxy_ISvrObjectEvents :
	public IConnectionPointImplMT<T, &__uuidof(IDispatch), CComDynamicUnkArray>
{
public:
	HRESULT Fire_CallbackFunction( ULONG uTargetId,  ULONG uCallerId,  ULONG uFunctionId,  VARIANT varParameter,  VARIANT * varResault)
	{
		HRESULT hr = S_OK;
		T * pThis = static_cast<T *>(this);
		int cConnections = m_vec.GetSize();

		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			pThis->Lock();
			CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
			pThis->Unlock();

			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection.p);

			if (pConnection)
			{
				CComVariant avarParams[5];
				avarParams[4] = uTargetId;
				avarParams[4].vt = VT_UI4;
				avarParams[3] = uCallerId;
				avarParams[3].vt = VT_UI4;
				avarParams[2] = uFunctionId;
				avarParams[2].vt = VT_UI4;
				avarParams[1] = varParameter;
				avarParams[0].pvarVal = varResault;
				avarParams[0].vt = VT_VARIANT|VT_BYREF;
				CComVariant varResult;

				DISPPARAMS params = { avarParams, NULL, 5, 0 };
				hr = pConnection->Invoke(1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
		return hr;
	}
};

