/********************************************************************
* CreatedOn: 2007-12-17   17:08
* FileName: skinxmldialog.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include <skinxmlwin.h>


namespace KSE{

class skinxmldialog : public skinxmlwin
{
    static LPCTSTR GetChildWinKeyName() { return _T("ChildWindow"); }
public:

    class enumchildwincallback
    {
    public:
        virtual BOOL onchildwin( skinxmlwin& ) = 0;
    };
    

public:

    BOOL EnumChildWindows( enumchildwincallback * pcallback )
    {        
        if (!m_xmlResElement.IsValid() || pcallback == NULL )
            return FALSE;

        SkinXmlElement childnode = m_xmlResElement.FirstChildElement( GetChildWinKeyName() );
        if (!childnode.IsValid())
            return FALSE;

        SkinXmlElement childwinnode = childnode.FirstChildElement();

        while ( childwinnode.IsValid() )
        {
            skinxmlwin win;

            win.AttachXmlElement(childwinnode);

            pcallback->onchildwin(win);

            childwinnode = childwinnode.NextSiblingElement();
        }

        return TRUE;
    }

};

} // amespac