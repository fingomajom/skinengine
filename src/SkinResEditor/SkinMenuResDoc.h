
#pragma once


#include "SkinItemIdMgt.h"

class SkinMenuItem 
{

public:

    BOOL NewResDoc()
    {
        BOOL bResult = TRUE;

        m_vtChildPopupMenu.clear();

        return bResult;
    }

    BOOL OpenResDoc(KSGUI::SkinXmlElement& menunode)
    {

        KSGUI::SkinXmlElement node = menunode.FirstChildElement();

        while ( node.IsValid() )
        {
            SkinMenuItem MenuItemInfo;

            node.GetValue(_T("IdName") , MenuItemInfo.strIdName);
            node.GetValue(_T("Caption"), MenuItemInfo.strItemText);
            node.GetValue(_T("ItemId") , MenuItemInfo.strItemId);
            node.GetValue(_T("IsPopup"), MenuItemInfo.strIsPopupMenu);

            SkinItemIdMgt::instance().UsedItemId( MenuItemInfo.strIdName, MenuItemInfo.strItemId );

            if (!MenuItemInfo.strIsPopupMenu.CompareNoCase(_T("true")))
            {
                MenuItemInfo.OpenResDoc(node);
            }
            
            m_vtChildPopupMenu.push_back(MenuItemInfo);

            node = node.NextSiblingElement();
        }

        return TRUE;
    }

    BOOL SaveResDoc(KSGUI::SkinXmlElement& menunode)
    {
        if (m_vtChildPopupMenu.size() == 0)
            return TRUE;

        for (size_t i = 0; i < m_vtChildPopupMenu.size(); i++)
        {
            KSGUI::SkinXmlElement node = menunode.AppendElement(_T("Item"));
            if (!node.IsValid())
                continue;

            SkinMenuItem& MenuItemInfo = m_vtChildPopupMenu[i];


            node.SetValue(_T("IdName") , MenuItemInfo.strIdName);
            node.SetValue(_T("Caption"), MenuItemInfo.strItemText);
            node.SetValue(_T("ItemId") , MenuItemInfo.strItemId);

            if (MenuItemInfo.m_vtChildPopupMenu.size() > 0)
            {
                node.SetValue(_T("IsPopup"), _T("True") );

                MenuItemInfo.SaveResDoc(node);
            }
            else
            {
                node.SetValue(_T("IsPopup"),_T("False"));
            }

        }


        return TRUE;
    }

public:
    KSGUI::CString strItemText;
    KSGUI::CString strItemId;
    KSGUI::CString strIdName;
    KSGUI::CString strIsPopupMenu;

    std::vector<SkinMenuItem> m_vtChildPopupMenu;
};

class SkinMenuResDoc
{
public:

    SkinMenuResDoc()
    {
    }

    ~SkinMenuResDoc(void)
    {
    }

    BOOL NewResDoc()
    {
        BOOL bResult = TRUE;

        m_vtMenuList.clear();

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
            root.FirstChildElement(KSGUI::skinmenures::GetResKeyName());

        if (!strnode.IsValid())
            return bResult;

        KSGUI::SkinXmlElement node = strnode.FirstChildElement();

        while ( node.IsValid() )
        {
                
            SkinMenuItem MenuItemInfo;

            node.Name(MenuItemInfo.strIdName);
            node.GetValue(_T("IsPopup"), MenuItemInfo.strIsPopupMenu);


            if (MenuItemInfo.OpenResDoc(node))
            {
                m_vtMenuList.push_back(MenuItemInfo);
            }

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

        KSGUI::SkinXmlElement menunode = 
            root.FirstChildElement(KSGUI::skinmenures::GetResKeyName());

        if (menunode.IsValid())
        {
            root.RemoveElement(menunode);
        }

        menunode = root.AppendElement(KSGUI::skinmenures::GetResKeyName());

        for (size_t i = 0; i < m_vtMenuList.size(); i++)
        {
            KSGUI::SkinXmlElement newnode = 
                menunode.AppendElement( m_vtMenuList[i].strIdName );
            if (!newnode.IsValid())
                continue;

            newnode.SetValue( _T("IsPopup"), m_vtMenuList[i].strIsPopupMenu);
            
            m_vtMenuList[i].SaveResDoc(newnode);
        }

        bResult = TRUE;

        return bResult;
    }


public:

    std::vector<SkinMenuItem> m_vtMenuList;

};