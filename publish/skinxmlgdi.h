/********************************************************************
* CreatedOn: 2007-12-19   17:40
* FileName: skinxmlgdi.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/


#pragma once

#include <skinstr.h>

namespace KSGUI{
    
class skinxmlfont : public KSGUI::CString
{
public:

    void operator << (const LOGFONT& font)
    {
        KSGUI::CString strCharSet;

        GetCharSet(font.lfCharSet, strCharSet);

        Format(_T("%s,%s,%d,%d,%d,%d,%d"), font.lfFaceName, strCharSet, font.lfHeight, font.lfWidth, font.lfWeight, font.lfItalic, font.lfUnderline);
    }
    
    void operator >> ( LOGFONT& font )
    {
        memset(&font, 0, sizeof(LOGFONT));

        KSGUI::CString strRef = (*this);
        KSGUI::CString strFaceName;
        KSGUI::CString strCharSet;

        int nDotIndex = 0;

        nDotIndex = strRef.Find(',');
        if (nDotIndex < 0)
            return;

        strFaceName = (LPCTSTR)strRef.Left(nDotIndex);
        strRef = (LPCTSTR)strRef.Mid(nDotIndex + 1);

        nDotIndex = strRef.Find(',');
        if (nDotIndex < 0)
            return;

        strCharSet = (LPCTSTR)strRef.Left(nDotIndex);
        strRef = (LPCTSTR)strRef.Mid(nDotIndex + 1);

        
        LONG lfItalic    = 0;
        LONG lfUnderline = 0;

        _stscanf_s((LPCTSTR)strRef, _T(" %d , %d , %d , %d , %d "), 
            &font.lfHeight, &font.lfWidth, &font.lfWeight, &lfItalic, &lfUnderline);

        font.lfItalic    = (BYTE)lfItalic;
        font.lfUnderline = (BYTE)lfUnderline;

        _tcsncpy_s(font.lfFaceName, strFaceName, sizeof(font.lfFaceName));
        
        GetCharSet(strCharSet, font.lfCharSet);

        if (font.lfWeight == 0)
            font.lfWeight = FW_NORMAL;
    }


    void operator << (HFONT font)
    {
        if (font == 0)
            return;

        LOGFONT LogFont = { 0 };

        if (::GetObject(font, sizeof(LOGFONT), &LogFont) != sizeof(LOGFONT))
            return;

        (*this) << LogFont;
    }

    void operator >> (HFONT& font)
    {
        if (GetLength() <= 0)
        {
            font = NULL;
            return;
        }

        LOGFONT LogFont = { 0 };
        
        (*this) >> LogFont;

        font = ::CreateFontIndirect(&LogFont);
    }

    static HFONT ToHandle( LPCTSTR pszFont )
    {
        HFONT hResult = NULL;

        skinxmlfont xmlfont;
        
        (CString&)xmlfont = (pszFont);

        xmlfont >> hResult;

        return hResult;
    }


protected:

    typedef struct _CharSet_Item
    {
        BYTE    lfCharSet;
        LPCTSTR pszCharSet;
    }CHARSETITEM, *LPCHARSETITEM;

    int GetCharSet(LPCHARSETITEM& pCharSet)
    {
        static CHARSETITEM CharSetList[] = 
        {
            {DEFAULT_CHARSET    , _T("DEFAULT_CHARSET")  } ,
            {ANSI_CHARSET       , _T("ANSI_CHARSET")  } ,
            {BALTIC_CHARSET     , _T("BALTIC_CHARSET")  } ,
            {CHINESEBIG5_CHARSET, _T("CHINESEBIG5_CHARSET")  } ,
            {EASTEUROPE_CHARSET , _T("EASTEUROPE_CHARSET")  } ,
            {GB2312_CHARSET     , _T("GB2312_CHARSET")  } ,
            {GREEK_CHARSET      , _T("GREEK_CHARSET")  } ,
            {HANGUL_CHARSET     , _T("HANGUL_CHARSET")  } ,
            {MAC_CHARSET        , _T("MAC_CHARSET")  } ,
            {OEM_CHARSET        , _T("OEM_CHARSET")  } ,
            {RUSSIAN_CHARSET    , _T("RUSSIAN_CHARSET")  } ,
            {SHIFTJIS_CHARSET   , _T("SHIFTJIS_CHARSET")  } ,
            {SYMBOL_CHARSET     , _T("SYMBOL_CHARSET")  } ,
            {TURKISH_CHARSET    , _T("TURKISH_CHARSET")  } ,
            {VIETNAMESE_CHARSET , _T("VIETNAMESE_CHARSET")  } ,
            {JOHAB_CHARSET      , _T("JOHAB_CHARSET")  } ,
            {ARABIC_CHARSET     , _T("ARABIC_CHARSET")  } ,
            {HEBREW_CHARSET     , _T("HEBREW_CHARSET")  } ,
            {THAI_CHARSET       , _T("THAI_CHARSET")  } 
        };

        pCharSet = CharSetList;

        return (sizeof(CharSetList) / sizeof(CharSetList[0]));
    }

    void GetCharSet( BYTE  lfCharSet, KSGUI::CString& strCharSet )
    {
        int ListCount = 0;
        LPCHARSETITEM CharSetList = NULL;

        ListCount = GetCharSet(CharSetList);
        
        strCharSet = CharSetList[0].pszCharSet;

        if (lfCharSet == CharSetList[0].lfCharSet)
            return;

        for (int index = 1; index < ListCount; index++)
        {
            if (lfCharSet == CharSetList[index].lfCharSet)
            {
                strCharSet = CharSetList[index].pszCharSet;
                break;
            }
        }
    }

    void GetCharSet( const KSGUI::CString& strCharSet, BYTE &lfCharSet)
    {
        int ListCount = 0;
        LPCHARSETITEM CharSetList = NULL;

        ListCount = GetCharSet(CharSetList);

        lfCharSet = CharSetList[0].lfCharSet;

        if (!strCharSet.CompareNoCase(CharSetList[0].pszCharSet))
            return;

        for (int index = 1; index < ListCount; index++)
        {
            if (!strCharSet.CompareNoCase(CharSetList[index].pszCharSet))
            {
                lfCharSet = CharSetList[index].lfCharSet;
                break;
            }
        }
    }

    

};

class skinxmlbrush : public KSGUI::CString
{
public:

    void operator << (const LOGBRUSH& brush )
    {
        Format(_T("%d,%d,0x%08x"), brush.lbStyle, brush.lbHatch, brush.lbColor);
    }

    void operator >> ( LOGBRUSH& brush )
    {
        memset(&brush, 0, sizeof(brush));

        _stscanf_s((LPCTSTR)(*this), _T(" %u , %d , 0x%08x "),
            &brush.lbStyle, &brush.lbHatch, &brush.lbColor);
    }

    void operator << ( HBRUSH hbrush )
    {
        if (hbrush == 0)
            return;

        LOGBRUSH brush = { 0 };

        if(::GetObject(hbrush, sizeof(LOGBRUSH), &brush) != sizeof(LOGBRUSH))
            return;

        (*this) << brush;
    }

    void operator >> ( HBRUSH& hbrush )
    {
        if (GetLength() <= 0)
        {
            hbrush = NULL;
            return;
        }

        LOGBRUSH brush = { 0 };

        (*this) >> brush;

        hbrush = ::CreateBrushIndirect(&brush);
    }

    static HBRUSH ToHandle( LPCTSTR pszFont )
    {
        HBRUSH hResult = NULL;

        skinxmlbrush xmlfont;

        (CString&)xmlfont = (pszFont);

        xmlfont >> hResult;

        return hResult;
    }


};

class skinxmlpen : public KSGUI::CString
{
public:
    
    void operator << (const LOGPEN& pen )
    {
        Format(_T("%u,%d,0x%08x"), pen.lopnStyle, pen.lopnWidth.x, pen.lopnColor);
    }

    void operator >> ( LOGPEN& pen )
    {
        memset(&pen, 0, sizeof(pen));

        _stscanf_s((LPCTSTR)(*this), _T(" %u , %d , 0x%08x "),
            &pen.lopnStyle, &pen.lopnWidth.x, &pen.lopnColor);
    }
    
    void operator << ( HPEN hpen )
    {
        if (hpen == 0)
            return;

        LOGPEN pen = { 0 };

        if(::GetObject(hpen, sizeof(LOGPEN), &pen) != sizeof(LOGPEN))
            return;

        (*this) << pen;
    }

    void operator >> ( HPEN& hpen )
    {
        if (GetLength() <= 0)
        {
            hpen = NULL;
            return;
        }

        LOGPEN pen = { 0 };
        
        (*this) >> pen;

        hpen = ::CreatePenIndirect(&pen);
    }

    static HPEN ToHandle( LPCTSTR pszFont )
    {
        HPEN hResult = NULL;

        skinxmlpen xmlfont;

        (CString&)xmlfont = (pszFont);

        xmlfont >> hResult;

        return hResult;
    }

};

class skinxmlcolor : public KSGUI::CString
{
public:

    void operator << ( COLORREF clr )
    {
        Format(_T("0x%08x"), clr);
    }

    void operator >> ( COLORREF& clr )
    {
        if (GetLength() <= 0)
        {
            clr = 0;
            return;
        }

        _stscanf_s((LPCTSTR)(*this), _T(" 0x %x "), &clr);
    }

    void Color ( BYTE r, BYTE g, BYTE b )
    {
        COLORREF clr = RGB(r,g,b);

        (*this) << clr;
    }

    static COLORREF ToHandle( LPCTSTR pszFont )
    {
        COLORREF hResult = NULL;

        skinxmlcolor xmlfont;

        (CString&)xmlfont = (pszFont);

        xmlfont >> hResult;

        return hResult;
    }

};

};