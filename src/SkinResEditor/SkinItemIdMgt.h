
#pragma once

class SkinItemIdMgt
{
public:

    static SkinItemIdMgt& instance()
    {
        static SkinItemIdMgt g_instace;

        return g_instace;
    }

    void Clear()
    {
        GetMapItemIdInfo().clear();
    }

    void UsedItemId(KSGUI::CString strIdName, KSGUI::CString& strItemId)
    {
        strIdName = GetRealIdName(strIdName);

        std::map<KSGUI::CString, UsedItemIdInfo>::iterator iter =
            GetMapItemIdInfo().find(strIdName);

        if (iter != GetMapItemIdInfo().end())
        {
            iter->second.nRefCount++;

            strItemId = iter->second.strItemId;
        }
        else
        {
            GetMapItemIdInfo()[strIdName] = UsedItemIdInfo(strItemId);
        }
    }


    void DelItemId( KSGUI::CString strIdName )
    {
        strIdName = GetRealIdName(strIdName);

        std::map<KSGUI::CString, UsedItemIdInfo>::iterator iter =
            GetMapItemIdInfo().find(strIdName);

        if (iter != GetMapItemIdInfo().end())
        {
            iter->second.nRefCount--;
            if (iter->second.nRefCount <= 0)
                GetMapItemIdInfo().erase(iter);
        }
    }

    void ChangeItemId( KSGUI::CString strIdName, KSGUI::CString strItemId )
    {
        strIdName = GetRealIdName(strIdName);

        std::map<KSGUI::CString, UsedItemIdInfo>::iterator iter =
            GetMapItemIdInfo().find(strIdName);

        if (iter != GetMapItemIdInfo().end())
        {
            iter->second.strItemId = strItemId;
        }
    }


protected:

    KSGUI::CString GetRealIdName( KSGUI::CString strIdName )
    {
        return strIdName.Mid(4);
    }


    struct UsedItemIdInfo 
    {
        UsedItemIdInfo() :
            nRefCount(0)
        {}

        UsedItemIdInfo( KSGUI::CString id, int Ref = 1) :
            strItemId(id), nRefCount(Ref)
        {}

        KSGUI::CString strItemId;
        int nRefCount;
    };


    static std::map<KSGUI::CString, UsedItemIdInfo>& GetMapItemIdInfo()
    {
        static std::map<KSGUI::CString, UsedItemIdInfo> m_mapUsedIdName;
        return m_mapUsedIdName;
    }

};