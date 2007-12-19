/********************************************************************
* CreatedOn: 2007-12-17   17:11
* FileName: skinimageedit.h
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

namespace KSE{


class skinimageedit : public skinimageres
{
public:

    skinimageedit() :
        skinimageres(new skinconfig())
    {

    }

    typedef struct _ImageItemInfo 
    {
        KSE::CString strIDName;
        KSE::CString strFileName;

    }IMAGE_ITEMINFO;

    virtual BOOL SaveImageFile(LPCTSTR pszIDName, LPCTSTR pszFileName)
    {
        SkinXmlElement node = m_xmlResElement.FirstChildElement(pszIDName);

        if (node.IsValid())
        {
            node.SetValue(skinimageresbase::GetValueAttName(), pszFileName);

            return TRUE;
        }

        node = m_xmlResElement.AppendElement(pszIDName);

        if (node.IsValid())
        {
            node.SetValue(skinimageresbase::GetValueAttName(), pszFileName);

            return TRUE;
        }

        return FALSE;
    }

    BOOL LoadImageList()
    {
        SkinXmlElement node = m_xmlResElement.FirstChildElement();

        while ( node.IsValid() )
        {

            skinimageedit::IMAGE_ITEMINFO itemInfo;

            node.Name(itemInfo.strIDName);
            node.GetValue(skinimageresbase::GetValueAttName(), itemInfo.strFileName);
            m_vtItemList.push_back(itemInfo);

            node = node.NextSiblingElement();

        }

        return TRUE;

    }


    BOOL SaveToDocument(SkinXmlElement& root)
    {
        SkinXmlElement strnode = root.FirstChildElement(KSE::skinimageresbase::GetResKeyName());
        if (strnode.IsValid())
        {
            root.RemoveElement(strnode);
        }

        m_xmlResElement = root.AppendElement(KSE::skinimageresbase::GetResKeyName());

        for (size_t i = 0; i < m_vtItemList.size(); i++)
        {
            SaveImageFile(m_vtItemList[i].strIDName, m_vtItemList[i].strFileName);
        }
        
        return TRUE;
    }
    //////////////////////////////////////////////////////////////////////////

    std::vector<skinimageedit::IMAGE_ITEMINFO>& GetImageList()
    {
        return m_vtItemList;
    }

public:

    std::vector<skinimageedit::IMAGE_ITEMINFO> m_vtItemList;

};

};