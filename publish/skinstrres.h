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


namespace KSE{

class skinstrresbase
{
public:
    virtual BOOL LoadString(LPCTSTR pszIDName, KSE::CString& strValue) = 0;
    virtual BOOL LoadString(UINT    uResID   , KSE::CString& strValue) = 0;

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


    BOOL LoadString(LPCTSTR pszIDName, KSE::CString& strValue)
    {
        SkinXmlElement node = m_xmlResElement.FirstChildElement(pszIDName);

        if (!node.IsValid())
            return FALSE;

        return node.GetValue(skinstrresbase::GetValueAttName(), strValue);
    }

    BOOL LoadString(UINT uResID, KSE::CString& strValue)
    {
        KSE::CString strIdName;

        strIdName.Format(skinstrresbase::GetResIdFormat(), uResID);

        return LoadString(strIdName, strValue);
    }


public:


};

} // namespace KSE
