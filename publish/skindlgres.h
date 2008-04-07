/********************************************************************
* CreatedOn: 2007-12-17   17:07
* FileName: skindlgres.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include <skinstr.h>
#include <skinxmldialog.h>


namespace KSGUI{

class skindlgresbase
{
public:

    virtual BOOL LoadDialog( LPCTSTR pszIDName, skinxmldialog& dlg ) = 0;
    
    static LPCTSTR GetResKeyName()   { return _T("DialogList"); }

};


class skindlgres : 
    public skindlgresbase,
    public skinxmlobject
{
public:

    skindlgres(const SkinXmlElement& xmlElement = SkinXmlElement()) :
        skinxmlobject(xmlElement)
    {
    }

    const skindlgres& operator = (const SkinXmlElement& xmlElement)
    {
        m_xmlResElement = xmlElement;

        return *this;
    }

    ~skindlgres(void)
    {

    }

    virtual BOOL LoadDialog( LPCTSTR pszIDName, skinxmldialog& dlg )
    {
        SkinXmlElement node = m_xmlResElement.FirstChildElement(pszIDName);

        if (!node.IsValid())
            return FALSE;

        return dlg.AttachXmlElement(node);
    }

public:

};

} // namespace KSGUI
