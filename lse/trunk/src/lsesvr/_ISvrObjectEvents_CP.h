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
};

