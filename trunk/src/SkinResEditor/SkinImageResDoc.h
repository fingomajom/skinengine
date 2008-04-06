/********************************************************************
* CreatedOn: 2008-2-17   12:25
* FileName: SkinImageResDoc.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/
#pragma once


#include <skinconfig.h>
#include <skinimageres.h>


class SkinImageResDoc : public KSG::skinimageres
{
public:

    SkinImageResDoc() 
        : skinimageres(NULL)
    {
        skinimageres::m_pskinconfig = &m_config;
    }

    typedef struct _ImageItemInfo 
    {
        KSG::CString strIDName;
        KSG::CString strFileName;

    }IMAGE_ITEMINFO;

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
            root.FirstChildElement(KSG::skinimageres::GetResKeyName());

        if (!strnode.IsValid())
            return bResult;

        KSG::SkinXmlElement node = strnode.FirstChildElement();

        while ( node.IsValid() )
        {
            SkinImageResDoc::IMAGE_ITEMINFO itemInfo;

            node.Name(itemInfo.strIDName);
            node.GetValue(KSG::skinimageres::GetValueAttName(), itemInfo.strFileName);
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
            root.FirstChildElement(KSG::skinimageres::GetResKeyName());

        if (strnode.IsValid())
        {
            root.RemoveElement(strnode);
        }

        strnode = root.AppendElement(KSG::skinimageres::GetResKeyName());

        for (size_t i = 0; i < m_vtItemList.size(); i++)
        {
            KSG::SkinXmlElement node = strnode.AppendElement(m_vtItemList[i].strIDName);

            node.SetValue(skinimageres::GetValueAttName(), m_vtItemList[i].strFileName);
        }

        bResult = TRUE;

        return bResult;
    }


    //////////////////////////////////////////////////////////////////////////

    std::vector<SkinImageResDoc::IMAGE_ITEMINFO>& GetImageList()
    {
        return m_vtItemList;
    }

public:

    std::vector<SkinImageResDoc::IMAGE_ITEMINFO> m_vtItemList;

    KSG::skinconfig m_config;
};

