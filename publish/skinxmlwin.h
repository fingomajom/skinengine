

#pragma once

#include <skinxmlobject.h>


namespace KSE{

class skinxmlwin : public skinxmlobject
{
public:

    virtual bool  AttachXmlNode(const CXmlNodeWrapper& xmlNode)
    {
        m_xmlResNode = xmlNode;

        return true;
    }
    
public:

    BOOL GetIdName   (KSE::CString& strIdName );
    BOOL GetClassName(KSE::CString& strIdName );
    BOOL GetCaption  (KSE::CString& strCaption);

    BOOL GetItemId ( DWORD& dwId     );
    BOOL GetStyle  ( DWORD& dwStype  );
    BOOL GetExStyle( DWORD& dwExStype);

    BOOL GetLeft  ( int& nValue );
    BOOL GetTop   ( int& nValue );
    BOOL GetWidth ( int& nValue );
    BOOL GetHeight( int& nValue );

    BOOL GetTextColor ( COLORREF& clrColor );
    BOOL GetBkColor   ( COLORREF& clrColor );

    BOOL GetFont( HFONT& hFont );

public:

    CXmlNodeWrapper m_xmlResNode;

};

} // amespace KSE{