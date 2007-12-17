
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
        if (!m_xmlResNode.IsValid() || pcallback == NULL )
            return FALSE;

        CXmlNodeWrapper childnode = m_xmlResNode.FindNode( GetChildWinKeyName() );
        if (!childnode.IsValid())
            return FALSE;
   
        CXmlNodeListWrapper childwinlist = childnode.GetChildNodes();

        childwinlist.Start();

        CXmlNodeWrapper childwinnode = childwinlist.Next();

        while ( childwinnode.IsValid() )
        {
            skinxmlwin win;

            win.AttachXmlNode(childwinnode);

            pcallback->onchildwin(win);

            childwinnode = childwinlist.Next();
        }

        return TRUE;
    }

};

} // amespac