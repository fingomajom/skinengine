/********************************************************************
* CreatedOn: 2007-12-17   17:08
* FileName: skinxmlwin.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include <skinxmlobject.h>


namespace KSE{

class skinxmlwin : public skinxmlobject
{

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

    virtual bool  AttachXmlNode(const SkinXmlElement& xmlElement)
    {
        m_xmlResElement = xmlElement;

        return true;
    }

    SkinXmlElement m_xmlResElement;

};

} // amespace KSE{