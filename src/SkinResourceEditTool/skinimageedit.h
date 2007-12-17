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

    virtual BOOL SaveImageFile(LPCTSTR pszIDName, KSE::CString& strFileName)
    {
        CXmlNodeWrapper node = m_xmlResNode.FindNode(pszIDName);

        if (node.IsValid())
        {
            node.SetValue(skinimageresbase::GetValueAttName(), strFileName);

            return TRUE;
        }

        node = m_xmlResNode.AppendNode(pszIDName);

        if (node.IsValid())
        {
            node.SetValue(skinimageresbase::GetValueAttName(), strFileName);

            return TRUE;
        }

        return FALSE;
    }

    BOOL LoadImageList()
    {
        CXmlNodeListWrapper nodelist = m_xmlResNode.GetChildNodes();

        if (!nodelist.IsValid())
            return FALSE;

        nodelist.Start();

        CXmlNodeWrapper node = nodelist.Next();

        while ( node.IsValid() )
        {

            skinimageedit::IMAGE_ITEMINFO itemInfo;

            node.Name(itemInfo.strIDName);
            node.GetValue(skinimageresbase::GetValueAttName(), itemInfo.strFileName);
            m_vtItemList.push_back(itemInfo);

            node = nodelist.Next();

        }

        return TRUE;

    }


    BOOL SaveToDocument(CXmlNodeWrapper& root)
    {
        CXmlNodeWrapper strnode = root.FindNode(KSE::skinimageresbase::GetResKeyName());
        if (strnode.IsValid())
        {
            root.RemoveNode(strnode.Interface());
        }

        m_xmlResNode = root.AppendNode(KSE::skinimageresbase::GetResKeyName());

        for (size_t i = 0; i < m_vtItemList.size(); i++)
        {
            SaveImageFile(m_vtItemList[i].strIDName, m_vtItemList[i].strFileName);
        }

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