/********************************************************************
* CreatedOn: 2007-12-17   17:08
* FileName: skinstrres.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include <skinstr.h>
#include <skinxmlobject.h>


namespace KSG{

class skinstrresbase
{
public:
    virtual BOOL LoadString(LPCTSTR pszIDName, KSG::CString& strValue) = 0;
    virtual BOOL LoadString(UINT    uResID   , KSG::CString& strValue) = 0;

    static LPCTSTR GetResKeyName()   { return _T("StringTable"); }
    static LPCTSTR GetValueAttName() { return _T("Text")       ; }
    static LPCTSTR GetResIdFormat()  { return _T("ResId(%d)")  ; }
};


class skinstrres : 
    public skinstrresbase,
    public skinxmlobject
{
public:

    skinstrres(const SkinXmlElement& xmlElement = SkinXmlElement()) :
        skinxmlobject(xmlElement)
    {
    }

    const skinstrres& operator = (const SkinXmlElement& xmlElement)
    {
        m_xmlResElement = xmlElement;

        return *this;
    }

    ~skinstrres(void)
    {

    }


    BOOL LoadString(LPCTSTR pszIDName, KSG::CString& strValue)
    {
        SkinXmlElement node = m_xmlResElement.FirstChildElement(pszIDName);

        if (!node.IsValid())
            return FALSE;

        return node.GetValue(skinstrresbase::GetValueAttName(), strValue);
    }

    BOOL LoadString(UINT uResID, KSG::CString& strValue)
    {
        KSG::CString strIdName;

        strIdName.Format(skinstrresbase::GetResIdFormat(), uResID);

        return LoadString(strIdName, strValue);
    }


public:


};

} // namespace KSG
