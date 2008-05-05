#include "StdAfx.h"
#include "SvrCallbackMgt.h"

SvrCallbackMgt::SvrCallbackMgt(void)
{
}

SvrCallbackMgt::~SvrCallbackMgt(void)
{
}

SvrCallbackMgt& SvrCallbackMgt::Instance()
{
    static SvrCallbackMgt gs_SvrCallbackMgt_Instance;

    return gs_SvrCallbackMgt_Instance;
}
