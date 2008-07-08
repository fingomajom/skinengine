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


class SkinImageResDoc : public KSGUI::skinimageres
{
public:

    SkinImageResDoc() 
        : skinimageres(NULL)
    {
    }

    typedef struct _ImageItemInfo 
    {
        KSGUI::CString strIDName;
        KSGUI::CString strFileName;

    }IMAGE_ITEMINFO;

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
            root.FirstChildElement(KSGUI::skinimageres::GetResKeyName());

        if (!strnode.IsValid())
            return bResult;

        KSGUI::SkinXmlElement node = strnode.FirstChildElement();

        while ( node.IsValid() )
        {
            SkinImageResDoc::IMAGE_ITEMINFO itemInfo;

            node.Name(itemInfo.strIDName);
            node.GetValue(KSGUI::skinimageres::GetValueAttName(), itemInfo.strFileName);
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
            root.FirstChildElement(KSGUI::skinimageres::GetResKeyName());

        if (strnode.IsValid())
        {
            root.RemoveElement(strnode);
        }

        strnode = root.AppendElement(KSGUI::skinimageres::GetResKeyName());

        for (size_t i = 0; i < m_vtItemList.size(); i++)
        {
            KSGUI::SkinXmlElement node = strnode.AppendElement(m_vtItemList[i].strIDName);

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

};

