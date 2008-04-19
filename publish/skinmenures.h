

#pragma once

#include <skinxmlobject.h>
#include <atluser.h>

namespace KSGUI{

class skinmenuresbase
{
public:

    virtual HMENU LoadMenu(LPCTSTR pszIDName) = 0;

    static LPCTSTR GetResKeyName()   { return _T("Menu");    }
};


class skinxmlmenuitem : public skinxmlobject
{
public:
    skinxmlmenuitem(const SkinXmlElement& xmlElement = SkinXmlElement()) 
        : skinxmlobject(xmlElement)
    {
    }

    const skinxmlmenuitem& operator = (const SkinXmlElement& xmlElement)
    {
        m_xmlResElement = xmlElement;

        return *this;
    }

    BOOL GetMenuItemText( KSGUI::CString& strMenuItemText )
    {
        return GetObject(_T("Caption"), strMenuItemText);
    }

    BOOL GetIsSeparator( BOOL& bIsSeparator )
    {
        bIsSeparator = TRUE;

        if ( ! GetMenuItemText( m_strMenuItemText ) )
            return TRUE;

        if (m_strMenuItemText.GetLength() <= 0)
            return TRUE;

        if ( m_strMenuItemText.GetLength() > 1 || m_strMenuItemText[0] != '-')
            bIsSeparator = FALSE;

        return TRUE;
    }

    BOOL GetIdName( KSGUI::CString& strIdName )
    {
        return GetObject(_T("IdName"), strIdName);
    }

    BOOL GetItemId(DWORD& dwId)
    {
        return GetObject(_T("ItemId"), dwId);
    }

    BOOL GetIsPopupMenu( BOOL& bIsPopupMenu )
    {
        bIsPopupMenu = FALSE;

        KSGUI::CString strIsPopupMenu;

        if (!GetObject(_T("IsPopup"), strIsPopupMenu))
            return FALSE;
        
        if (!strIsPopupMenu.CompareNoCase(_T("true")))
            bIsPopupMenu = TRUE;

        return TRUE;
    }

protected:

    KSGUI::CString m_strMenuItemText;
};

class skinmenures : 
    public skinmenuresbase,
    public skinxmlobject
{
public:

    skinmenures(const SkinXmlElement& xmlElement = SkinXmlElement()) :
        skinxmlobject(xmlElement)
    {
    }

    const skinmenures& operator = (const SkinXmlElement& xmlElement)
    {
        m_xmlResElement = xmlElement;

        return *this;
    }

    ~skinmenures(void)
    {

    }

    //////////////////////////////////////////////////////////////////////////

    virtual HMENU LoadMenu(LPCTSTR pszIDName)
    {
        return NULL;
    }

protected:

    HMENU LoadMenu(HMENU hMenu, KSGUI::SkinXmlElement& xmlmenu )
    {
        
        KSGUI::SkinXmlElement menunode = xmlmenu.FirstChildElement();

        while ( menunode.IsValid() )
        {
            skinxmlmenuitem xmlmenuitem = menunode;
            
            KSGUI::CString strMenuItemText;
            BOOL   bIsSeparator = FALSE;
            DWORD  dwId         = 0;
            BOOL   bIsPopupMenu = FALSE;

            if ( xmlmenuitem.GetMenuItemText(strMenuItemText) )
            {
                xmlmenuitem.GetIsSeparator(bIsSeparator);
                xmlmenuitem.GetItemId(dwId);
                xmlmenuitem.GetIsPopupMenu(bIsPopupMenu);

                if ( bIsSeparator )
                {
                    ::AppendMenu( hMenu, MF_SEPARATOR, 0, NULL );
                }
                else if ( bIsPopupMenu )
                {
                    WTL::CMenuHandle menuSub;

                    menuSub.CreatePopupMenu();

                    if (menuSub.m_hMenu)
                    {
                        menuSub.m_hMenu = LoadMenu( menuSub, menunode );
                        
                        ::AppendMenu( hMenu, 
                            MF_POPUP, 
                            (UINT_PTR)menuSub.m_hMenu, 
                            strMenuItemText );
                    }

                }
                else
                {
                    ::AppendMenu( hMenu, MF_STRING, dwId, strMenuItemText );
                }

            }

            menunode = menunode.NextSiblingElement();
        }

        return hMenu;
    }
};

}; // namespace KSGUI