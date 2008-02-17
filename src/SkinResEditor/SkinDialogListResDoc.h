
/********************************************************************
* CreatedOn: 2008-2-17   12:25
* FileName: SkinDialogListResDoc.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once


class SkinWndPropertyList
{
public:

    struct WndPropertyItem
    {
        KSG::CString strProperty;
        KSG::CString strValue;
    };
    
    BOOL NewResDoc()
    {
        BOOL bResult = TRUE;

        m_vtPropertyList.clear();

        return bResult;
    }

    BOOL AppendProperty(LPCTSTR pszProperty, LPCTSTR pszValue)
    {
        WndPropertyItem itemInfo;

        itemInfo.strProperty = pszProperty;
        itemInfo.strValue    = pszValue;

        m_vtPropertyList.push_back(itemInfo);

        return TRUE;
    }

    BOOL SetProperty(LPCTSTR pszProperty, LPCTSTR pszValue)
    {
        for (size_t i = 0; i < m_vtPropertyList.size(); i++)
        {
            if (m_vtPropertyList[i].strProperty.Compare(pszProperty) == 0)
            {
                m_vtPropertyList[i].strValue = pszValue;

                return TRUE;
            }
        }

        return TRUE;
    }
    
    BOOL SetProperty(int nIndex, LPCTSTR pszValue)
    {
        if (nIndex >= 0 && nIndex < (int)m_vtPropertyList.size())
            m_vtPropertyList[nIndex].strValue = pszValue;

        return TRUE;
    }

    BOOL DelProperty(LPCTSTR pszProperty, LPCTSTR pszValue)
    {
        for (size_t i = 0; i < m_vtPropertyList.size(); i++)
        {
            if (m_vtPropertyList[i].strProperty.Compare(pszProperty) == 0)
            {
                m_vtPropertyList.erase(m_vtPropertyList.begin() + i);

                return TRUE;
            }
        }

        return TRUE;
    }

    BOOL DelProperty(int nIndex, LPCTSTR pszValue)
    {
        if (nIndex >= 0 && nIndex < (int)m_vtPropertyList.size())
            m_vtPropertyList.erase(m_vtPropertyList.begin() + nIndex);

        return TRUE;
    }



    BOOL OpenResDoc( KSG::SkinXmlElement& xmlElement )
    {
        BOOL bResult = FALSE;

        NewResDoc();

        if (!xmlElement.IsValid())
            return bResult;

        xmlElement.Name(m_strIdName);

        TiXmlAttribute* xmlAttribute = xmlElement.FirstAttribute();

        while ( xmlAttribute != NULL )
        {
            WndPropertyItem itemInfo;

            itemInfo.strProperty = CA2TEX<>(xmlAttribute->Name(), CP_DEFAULT).m_psz;
            itemInfo.strValue = CA2TEX<>(xmlAttribute->Value(), CP_DEFAULT).m_psz;

            m_vtPropertyList.push_back(itemInfo);

            xmlAttribute = xmlAttribute->Next();
        }

        bResult = TRUE;

        return bResult;
    }

    KSG::SkinXmlElement SaveResDoc( KSG::SkinXmlElement& xmlParentElement )
    {
        BOOL bResult = FALSE;
        
        if (!xmlParentElement.IsValid())
            return KSG::SkinXmlElement();

        KSG::SkinXmlElement xmlElement = 
            xmlParentElement.AppendElement(m_strIdName);

        if (!xmlElement.IsValid())
            return KSG::SkinXmlElement();

        for (size_t i = 0; i < m_vtPropertyList.size(); i++)
        {
            xmlElement.SetValue( 
                m_vtPropertyList[i].strProperty,
                m_vtPropertyList[i].strValue);
        }

        return xmlElement;
    }

    KSG::CString& GetIdName()
    {
        return m_strIdName;
    }



public:
    
    std::vector<WndPropertyItem> m_vtPropertyList;
    KSG::CString m_strIdName;
};

class SkinDialogRes
{
public:

    BOOL NewResDoc()
    {
        BOOL bResult = TRUE;

        m_vtChildWndList.clear();
        m_dlgWndProperty.NewResDoc();

        return bResult;
    }

    BOOL OpenResDoc( KSG::SkinXmlElement& xmlElement )
    {
        BOOL bResult = FALSE;

        NewResDoc();

        if (!xmlElement.IsValid())
            return bResult;

        
        if (!m_dlgWndProperty.OpenResDoc(xmlElement))
            return FALSE;

        KSG::SkinXmlElement strnode = 
            xmlElement.FirstChildElement(KSG::skinxmldialog::GetChildWinKeyName());

        if (!strnode.IsValid())
            return bResult;

        KSG::SkinXmlElement node = strnode.FirstChildElement();

        while ( node.IsValid() )
        {
            SkinWndPropertyList wndPropertyList;

            if (wndPropertyList.OpenResDoc(node))
            {
                m_vtChildWndList.push_back(wndPropertyList);
            }

            node = node.NextSiblingElement();
        }

        bResult = TRUE;

        return bResult;
    }

    BOOL SaveResDoc(KSG::SkinXmlElement& xmlParentElement)
    {
        BOOL bResult = FALSE;

        KSG::SkinXmlElement xmlElement = 
            m_dlgWndProperty.SaveResDoc(xmlParentElement);
        if (!xmlElement.IsValid())
            return bResult;

        KSG::SkinXmlElement strnode = 
            xmlElement.AppendElement(KSG::skinxmldialog::GetChildWinKeyName());

        for (size_t i = 0; i < m_vtChildWndList.size(); i++)
        {
            m_vtChildWndList[i].SaveResDoc(strnode);
        }

        bResult = TRUE;

        return bResult;
    }


public:

    std::vector<SkinWndPropertyList> m_vtChildWndList;
    SkinWndPropertyList m_dlgWndProperty;
};



class SkinDialogListResDoc
{
public:

    SkinDialogListResDoc()
    {
    }

    ~SkinDialogListResDoc(void)
    {
    }

    BOOL NewResDoc()
    {
        BOOL bResult = TRUE;
        
        m_vtDialogList.clear();

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
            root.FirstChildElement(KSG::skindlgres::GetResKeyName());

        if (!strnode.IsValid())
            return bResult;

        KSG::SkinXmlElement node = strnode.FirstChildElement();

        while ( node.IsValid() )
        {
            SkinDialogRes wndDlgRes;

            if (wndDlgRes.OpenResDoc(node))
            {
                m_vtDialogList.push_back(wndDlgRes);
            }


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
            root.FirstChildElement(KSG::skindlgres::GetResKeyName());

        if (strnode.IsValid())
        {
            root.RemoveElement(strnode);
        }

        strnode = root.AppendElement(KSG::skindlgres::GetResKeyName());

        for (size_t i = 0; i < m_vtDialogList.size(); i++)
        {
            m_vtDialogList[i].SaveResDoc(strnode);
        }

        bResult = TRUE;

        return bResult;
    }

public:

    std::vector<SkinDialogRes> m_vtDialogList;

};