

#pragma once

typedef struct tag_Config_Item 
{
    UINT nCfgType;
    UINT nCfgID;
    ATL::CString strCfgData;

}CONFIGITEM, LPCONFIGITEM;

class CDWConfigDB : public CDWSingleton<CDWConfigDB>
{
    friend class CDWSingleton<CDWConfigDB>;
    CDWConfigDB() {}
    ~CDWConfigDB() {}

public:

    typedef ATL::CAtlMap< UINT, ATL::CString >   ConfigDataMap;
    typedef ATL::CAtlMap< UINT, ConfigDataMap* > ConfigMap;
    
    ConfigMap m_mapCfg;
};