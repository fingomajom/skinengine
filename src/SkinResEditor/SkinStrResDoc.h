
#pragma once



class SkinStrResDoc : public KSG::skinstrres
{
public:

    SkinStrResDoc()
    {
    }

    ~SkinStrResDoc(void)
    {
    }

    typedef struct _StrResItemInfo 
    {
        KSG::CString strIDName;
        KSG::CString strValue;

    }STRRESITEMINFO;

    BOOL NewResDoc()
    {
        BOOL bResult = TRUE;
        
        m_vtItemList.clear();

        return bResult;
    }

    BOOL OpenResDoc(KSG::SkinXmlDocument& doc)
    {
        BOOL bResult = FALSE;

        NewResDoc();

        KSG::SkinXmlElement root = doc.RootElement();

        if (!root.IsValid())
            return bResult;

        KSG::SkinXmlElement strnode = 
            root.FirstChildElement(KSG::skinstrresbase::GetResKeyName());

        if (!strnode.IsValid())
            return bResult;

        KSG::SkinXmlElement node = strnode.FirstChildElement();

        while ( node.IsValid() )
        {
            SkinStrResDoc::STRRESITEMINFO itemInfo;

            node.Name(itemInfo.strIDName);
            node.GetValue(KSG::skinstrresbase::GetValueAttName(), itemInfo.strValue);
            m_vtItemList.push_back(itemInfo);

            node = node.NextSiblingElement();
        }

        bResult = TRUE;

        return bResult;
    }

    BOOL SaveResDoc(KSG::SkinXmlDocument& doc)
    {
        BOOL bResult = FALSE;

        KSG::SkinXmlElement root = doc.RootElement();
        if (!root.IsValid())
            return bResult;

        KSG::SkinXmlElement strnode = 
            root.FirstChildElement(KSG::skinstrresbase::GetResKeyName());

        if (strnode.IsValid())
        {
            root.RemoveElement(strnode);
        }

        strnode = root.AppendElement(KSG::skinstrresbase::GetResKeyName());

        for (size_t i = 0; i < m_vtItemList.size(); i++)
        {
            KSG::SkinXmlElement node = strnode.AppendElement(m_vtItemList[i].strIDName);
            
            node.SetValue(skinstrresbase::GetValueAttName(), m_vtItemList[i].strValue);
        }

        bResult = TRUE;

        return bResult;
    }

    //////////////////////////////////////////////////////////////////////////

    std::vector<SkinStrResDoc::STRRESITEMINFO>& GetStrTableList()
    {
        m_vtItemList;
    }

public:

    std::vector<SkinStrResDoc::STRRESITEMINFO> m_vtItemList;

};