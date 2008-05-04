#pragma once

template<class T>
class CProxy_ISvrObjectEvents :
	public IConnectionPointImpl<T, &__uuidof(IDispatch)>
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
				CComVariant avarParams[5];
				avarParams[4] = uDesCallerId;
				avarParams[4].vt = VT_UI4;
				avarParams[3] = uSrcModuleId;
				avarParams[3].vt = VT_UI4;
				avarParams[2] = bstrFunctionName;
				avarParams[2].vt = VT_BSTR;
				avarParams[1] = bstrParameter;
				avarParams[1].vt = VT_BSTR;
				avarParams[0].byref = bstrResult;
				avarParams[0].vt = VT_BSTR|VT_BYREF;
				CComVariant varResult;

				DISPPARAMS params = { avarParams, NULL, 5, 0 };
				hr = pConnection->Invoke(1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
		return hr;
	}
};

