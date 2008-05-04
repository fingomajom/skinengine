// SvrObject.cpp : Implementation of CSvrObject

#include "stdafx.h"
#include "SvrObject.h"



CSvrObjectFactory::CSvrObjectFactory()
{
    CSvrObject::instance().QueryInterface(IID_IUnknown, (void**)&m_spObj);
    m_hrCreate = S_OK;
}


CSvrObject& CSvrObject::instance()
{
    static CComObject<CSvrObject> gs_ComSvrObject;

    return gs_ComSvrObject;
}

// CSvrObject

