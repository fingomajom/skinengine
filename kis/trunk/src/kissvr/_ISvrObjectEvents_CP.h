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

template<class T>
class CProxy_ISvrObjectEvents :
	public IConnectionPointImplMT<T, &__uuidof(IDispatch), CComDynamicUnkArray>
{
public:
	HRESULT Fire_NotifyMessage( ULONG uDesCallerId,  ULONG uSrcModuleId,  BSTR bstrFunctionName,  BSTR bstrParameter,  BSTR * bstrResult)
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
				CComVariant avarParams[4];
                CComVariant varResult;

                avarParams[3] = uDesCallerId;
				avarParams[3].vt = VT_UI4;
				avarParams[2] = uSrcModuleId;
				avarParams[2].vt = VT_UI4;
				avarParams[1] = bstrFunctionName;
				avarParams[1].vt = VT_BSTR;
				avarParams[0] = bstrParameter;
				avarParams[0].vt = VT_BSTR;

				DISPPARAMS params = { avarParams, NULL, 5, 0 };
				hr = pConnection->Invoke(1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
                
                if (SUCCEEDED(hr) && bstrResult != NULL && varResult.vt == VT_BSTR)
                {
                    *bstrResult = varResult.bstrVal;

                    varResult.vt = VT_EMPTY;
                }

			}
		}
		return hr;
	}
};

