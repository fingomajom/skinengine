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
#include <skinxmlgdi.h>

namespace KSE{


class skinxmlwin : public skinxmlobject
{
public:

    skinxmlwin(const SkinXmlElement& xmlElement = SkinXmlElement()) :
        skinxmlobject(xmlElement)
    {
    }

    const skinxmlwin& operator = (const SkinXmlElement& xmlElement)
    {
        m_xmlResElement = xmlElement;

        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    // 标准属性

    BOOL GetIdName   (KSE::CString& strIdName );
    BOOL GetClassName(KSE::CString& strClassName );
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

    //////////////////////////////////////////////////////////////////////////
    // 扩展属性
    
    BOOL GetFont ( LPCTSTR Name, HFONT    & hFont );
    BOOL GetColor( LPCTSTR Name, COLORREF & clr   );
    BOOL GetBrush( LPCTSTR Name, HBRUSH   & brush );
    BOOL GetPen  ( LPCTSTR Name, HPEN     & pen   );
  
    //////////////////////////////////////////////////////////////////////////

public:

};


inline BOOL skinxmlwin::GetIdName(KSE::CString& strIdName )
{
    return GetObject(_T("IdName"), strIdName);
}
inline BOOL skinxmlwin::GetClassName(KSE::CString& strClassName )
{
    return GetObject(_T("ClassName"), strClassName);
}
inline BOOL skinxmlwin::GetCaption(KSE::CString& strCaption)
{
    return GetObject(_T("Caption"), strCaption);
}

inline BOOL skinxmlwin::GetItemId(DWORD& dwId)
{
    return GetObject(_T("ItemId"), dwId);
}
inline BOOL skinxmlwin::GetStyle(DWORD& dwStype  )
{
    return GetObject(_T("Style"), dwStype);
}
inline BOOL skinxmlwin::GetExStyle( DWORD& dwExStype)
{
    return GetObject(_T("ExStyle"), dwExStype);
}

inline BOOL skinxmlwin::GetLeft( int& nValue )
{
    return GetObject(_T("Left"), nValue);
}
inline BOOL skinxmlwin::GetTop( int& nValue )
{
    return GetObject(_T("Top"), nValue);
}
inline BOOL skinxmlwin::GetWidth ( int& nValue )
{
    return GetObject(_T("Width"), nValue);
}
inline BOOL skinxmlwin::GetHeight( int& nValue )
{
    return GetObject(_T("Height"), nValue);
}

inline BOOL skinxmlwin::GetTextColor ( COLORREF& clrColor )
{
    return GetColor(_T("TextColor"), clrColor);
}
inline BOOL skinxmlwin::GetBkColor( COLORREF& clrColor )
{
    return GetColor(_T("BkColor"), clrColor);
}

inline BOOL skinxmlwin::GetFont( HFONT& hFont )
{
    return GetFont(_T("Font"), hFont);
}


inline BOOL skinxmlwin::GetFont ( LPCTSTR Name, HFONT & hFont )
{
    skinxmlfont xmlfont;

    if (!GetObject(Name, xmlfont))
        return FALSE;

    xmlfont >> hFont;

    return TRUE;
}

inline BOOL skinxmlwin::GetColor( LPCTSTR Name, COLORREF & clr   )
{
    skinxmlcolor xmlclr;

    if (!GetObject(Name, xmlclr))
        return FALSE;

    xmlclr >> clr;

    return TRUE;
}

inline BOOL skinxmlwin::GetBrush( LPCTSTR Name, HBRUSH & brush )
{
    skinxmlbrush xmlbrush;

    if (!GetObject(Name, xmlbrush))
        return FALSE;

    xmlbrush >> brush;

    return TRUE;
}

inline BOOL skinxmlwin::GetPen  ( LPCTSTR Name, HPEN & pen   )
{
    skinxmlpen xmlpen;

    if (!GetObject(Name, xmlpen))
        return FALSE;

    xmlpen >> pen;

    return TRUE;
}


} // amespace KSE{