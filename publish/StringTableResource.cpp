
#include "StringTableResource.h"

CStringTableResource::CStringTableResource()
{
}

CStringTableResource::~CStringTableResource(void)
{
}


BOOL CStringTableResource::SaveString(LPCTSTR pszIDName, LPCTSTR strValue)
{
    CXmlNodeWrapper node = m_xmlResNode.FindNode(pszIDName);

    if (node.IsValid())
    {
        node.SetValue(skinstrresbase::GetValueAttName(), strValue);

        return TRUE;
    }

    node = m_xmlResNode.AppendNode(pszIDName);
    
    if (node.IsValid())
    {
        node.SetValue(skinstrresbase::GetValueAttName(), strValue);

        return TRUE;
    }
    
    return FALSE;
}

BOOL CStringTableResource::GetStringTableList(std::vector<CStringTableResource::STRINGTABLE_ITEMINFO>& vtItemList)
{
    CXmlNodeListWrapper nodelist = m_xmlResNode.GetChildNodes();

    if (!nodelist.IsValid())
        return FALSE;

    int nCount = nodelist.Count();
    
    nodelist.Start();

    CXmlNodeWrapper node = nodelist.Next();

    while ( node.IsValid() )
    {

        CStringTableResource::STRINGTABLE_ITEMINFO itemInfo;

        node.Name(itemInfo.strIDName);
        node.GetValue(skinstrresbase::GetValueAttName(), itemInfo.strValue);
        vtItemList.push_back(itemInfo);

        node = nodelist.Next();

    }

    return TRUE;
}


BOOL CStringTableResource::SetStringTableList(const std::vector<STRINGTABLE_ITEMINFO>& vtItemList)
{
    for (size_t i = 0; i < vtItemList.size(); i++)
    {
        SaveString(vtItemList[i].strIDName, vtItemList[i].strValue);
    }

    return TRUE;
}
