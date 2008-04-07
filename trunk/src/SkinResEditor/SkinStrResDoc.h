/********************************************************************
* CreatedOn: 2008-2-17   12:25
* FileName: SkinStrResDoc.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/
#pragma once


class SkinStrResDoc : public KSGUI::skinstrres
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
        KSGUI::CString strIDName;
        KSGUI::CString strValue;

    }STRRESITEMINFO;

    BOOL NewResDoc()
    {
        BOOL bResult = TRUE;
        
        m_vtItemList.clear();

        return bResult;
    }

    BOOL OpenResDoc(KSGUI::SkinXmlDocument& doc)
    {
        BOOL bResult = FALSE;

        NewResDoc();

        KSGUI::SkinXmlElement root = doc.RootElement();

        if (!root.IsValid())
            return bResult;

        KSGUI::SkinXmlElement strnode = 
            root.FirstChildElement(KSGUI::skinstrresbase::GetResKeyName());

        if (!strnode.IsValid())
            return bResult;

        KSGUI::SkinXmlElement node = strnode.FirstChildElement();

        while ( node.IsValid() )
        {
            SkinStrResDoc::STRRESITEMINFO itemInfo;

            node.Name(itemInfo.strIDName);
            node.GetValue(KSGUI::skinstrresbase::GetValueAttName(), itemInfo.strValue);
            m_vtItemList.push_back(itemInfo);

            node = node.NextSiblingElement();
        }

        bResult = TRUE;

        return bResult;
    }

    BOOL SaveResDoc(KSGUI::SkinXmlDocument& doc)
    {
        BOOL bResult = FALSE;

        KSGUI::SkinXmlElement root = doc.RootElement();
        if (!root.IsValid())
            return bResult;

        KSGUI::SkinXmlElement strnode = 
            root.FirstChildElement(KSGUI::skinstrresbase::GetResKeyName());

        if (strnode.IsValid())
        {
            root.RemoveElement(strnode);
        }

        strnode = root.AppendElement(KSGUI::skinstrresbase::GetResKeyName());

        for (size_t i = 0; i < m_vtItemList.size(); i++)
        {
            KSGUI::SkinXmlElement node = strnode.AppendElement(m_vtItemList[i].strIDName);
            
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