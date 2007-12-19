
#include "StringTableResource.h"

CStringTableResource::CStringTableResource()
{
}

CStringTableResource::~CStringTableResource(void)
{
}


BOOL CStringTableResource::SaveString(LPCTSTR pszIDName, LPCTSTR strValue)
{
    SkinXmlElement node = m_xmlResElement.FirstChildElement(pszIDName);

    if (node.IsValid())
    {
        node.SetValue(skinstrresbase::GetValueAttName(), strValue);

        return TRUE;
    }

    node = m_xmlResElement.AppendElement(pszIDName);
    
    if (node.IsValid())
    {
        node.SetValue(skinstrresbase::GetValueAttName(), strValue);

        return TRUE;
    }
    
    return FALSE;
} 


BOOL CStringTableResource::LoadStringTableList()
{
    SkinXmlElement node = m_xmlResElement.FirstChildElement();

    while ( node.IsValid() )
    {

        CStringTableResource::STRINGTABLE_ITEMINFO itemInfo;

        node.Name(itemInfo.strIDName);
        node.GetValue(skinstrresbase::GetValueAttName(), itemInfo.strValue);
        m_vtItemList.push_back(itemInfo);

        node = node.NextSiblingElement();

    }

    return TRUE;
}


//BOOL CStringTableResource::SetStringTableList(const std::vector<STRINGTABLE_ITEMINFO>& vtItemList)
//{
//    for (size_t i = 0; i < vtItemList.size(); i++)
//    {
//        SaveString(vtItemList[i].strIDName, vtItemList[i].strValue);
//    }
//
//    return TRUE;
//}

BOOL CStringTableResource::SaveToDocument(SkinXmlElement& root)
{
    SkinXmlElement strnode = root.FirstChildElement(KSE::skinstrresbase::GetResKeyName());
    if (strnode.IsValid())
    {
        root.RemoveElement(strnode);
    }

    m_xmlResElement = root.AppendElement(KSE::skinstrresbase::GetResKeyName());
    
    for (size_t i = 0; i < m_vtItemList.size(); i++)
    {
        SaveString(m_vtItemList[i].strIDName, m_vtItemList[i].strValue);
    }

    return TRUE;
}


std::vector<CStringTableResource::STRINGTABLE_ITEMINFO>& CStringTableResource::GetStringTableList()
{
    return m_vtItemList;
}

