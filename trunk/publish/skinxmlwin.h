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


#define skinwinA

class skinxmlwin : public skinxmlobject
{

public:
    //////////////////////////////////////////////////////////////////////////
    // 标准属性

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

    //////////////////////////////////////////////////////////////////////////
    // 扩展属性
    
    BOOL GetFont ( LPCTSTR Name, HFONT    & hFont );
    BOOL GetColor( LPCTSTR Name, COLORREF & clr   );
    BOOL GetBrush( LPCTSTR Name, HBRUSH   & brush );
    BOOL GetPen  ( LPCTSTR Name, HPEN     & pen   );

  
    //////////////////////////////////////////////////////////////////////////
public:

};

} // amespace KSE{