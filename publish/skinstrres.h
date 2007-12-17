
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

    skinstrres(MSXML2::IXMLDOMNode* pStringTableNode = NULL)
    {
       
        AttachXmlNode(CXmlNodeWrapper(pStringTableNode));
    }

    ~skinstrres(void)
    {

    }

    bool  AttachXmlNode(const CXmlNodeWrapper& xmlNode)
    {
        m_xmlResNode = xmlNode;

        return true;
    }

    BOOL LoadString(LPCTSTR pszIDName, KSE::CString& strValue)
    {
        CXmlNodeWrapper node = m_xmlResNode.FindNode(pszIDName);

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

    CXmlNodeWrapper m_xmlResNode;

};

} // namespace KSE
