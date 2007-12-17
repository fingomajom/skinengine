
#pragma once

#include <skinstr.h>
#include <skinxmldialog.h>


namespace KSE{

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

    skindlgres()
    {
    }

    ~skindlgres(void)
    {

    }

    bool AttachXmlNode(const CXmlNodeWrapper& xmlNode)
    {
        m_xmlResNode = xmlNode;

        return true;
    }

    virtual BOOL LoadDialog( LPCTSTR pszIDName, skinxmldialog& dlg )
    {
        CXmlNodeWrapper node = m_xmlResNode.FindNode(pszIDName);

        if (!node.IsValid())
            return FALSE;

        return dlg.AttachXmlNode(node);
    }

public:

    CXmlNodeWrapper m_xmlResNode;

};

} // namespace KSE
