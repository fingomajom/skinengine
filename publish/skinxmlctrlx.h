/********************************************************************
* CreatedOn: 2008-4-3   11:45
* FileName: skinxmlctrlx.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

namespace KSGUI{

class skinxmlstaticex : public skinxmlwin
{
public:

    skinxmlstaticex(const SkinXmlElement& xmlElement = SkinXmlElement()) 
        : skinxmlwin(xmlElement)
    {
    }

    const skinxmlstaticex& operator = (const SkinXmlElement& xmlElement)
    {
        m_xmlResElement = xmlElement;

        return *this;
    }

    static LPCTSTR GetSkinWndClassName() 
    {
        return _T("skinxmlstaticex");
    }

    BOOL GetPaintStyle(int& nStype  )
    {
        return GetObject(_T("PaintStyle"), nStype);
    }

    BOOL GetDrawTextFlags( DWORD& dwFlag)
    {
        return GetObject(_T("DrawTextFlags"), dwFlag);
    }

    BOOL GetLineColor( COLORREF& clrLine )
    {
        return GetColor( _T("LineColor"), clrLine );
    }
};

class skinxmlbmpbtn : public skinxmlwin
{
public:

    skinxmlbmpbtn(const SkinXmlElement& xmlElement = SkinXmlElement()) 
        : skinxmlwin(xmlElement)
    {
    }

    const skinxmlbmpbtn& operator = (const SkinXmlElement& xmlElement)
    {
        m_xmlResElement = xmlElement;

        return *this;
    }

    static LPCTSTR GetSkinWndClassName() 
    {
        return _T("skinxmlbmpbtn");
    }

    BOOL GetBmpResName( KSGUI::CString& strBmpResName)
    {
        return GetObject(_T("BmpResName"), strBmpResName);
    }

    BOOL GetBmpCount( int& nBmpCount )
    {
        nBmpCount = 3;
        return GetObject(_T("BmpCount"), nBmpCount);
    }

};

class skinxmlclrbtn : public skinxmlwin
{
public:

    skinxmlclrbtn(const SkinXmlElement& xmlElement = SkinXmlElement()) 
        : skinxmlwin(xmlElement)
    {
    }

    const skinxmlclrbtn& operator = (const SkinXmlElement& xmlElement)
    {
        m_xmlResElement = xmlElement;

        return *this;
    }

    static LPCTSTR GetSkinWndClassName() 
    {
        return _T("skinxmlclrbtn");
    }

};

class skinxmlhyperlink : public skinxmlwin
{
public:

    skinxmlhyperlink(const SkinXmlElement& xmlElement = SkinXmlElement()) 
        : skinxmlwin(xmlElement)
    {
    }

    const skinxmlhyperlink& operator = (const SkinXmlElement& xmlElement)
    {
        m_xmlResElement = xmlElement;

        return *this;
    }

    static LPCTSTR GetSkinWndClassName() 
    {
        return _T("skinxmlhyperlink");
    }


    BOOL GetToolTipText( KSGUI::CString& strText)
    {
        return GetObject(_T("ToolTipText"), strText);
    }

    BOOL GetLinkFont( HFONT& hLinkFont )
    {
        return GetFont( _T("LinkFont"), hLinkFont );
    }

};

class skinxmltablectrlex: public skinxmlwin
{
public:

    skinxmltablectrlex(const SkinXmlElement& xmlElement = SkinXmlElement()) 
        : skinxmlwin(xmlElement)
    {
    }

    const skinxmltablectrlex& operator = (const SkinXmlElement& xmlElement)
    {
        m_xmlResElement = xmlElement;

        return *this;
    }

    static LPCTSTR GetSkinWndClassName() 
    {
        return _T("skinxmltablectrlex");
    }

    BOOL GetHotTextFont( HFONT& hFont )
    {
        return GetFont( _T("HotTextFont"), hFont );
    }
    BOOL GetHotTextColor( COLORREF& clr )
    {
        return GetColor( _T("HotTextColor"), clr );
    }

    BOOL GetMainColor( COLORREF& clr )
    {
        return GetColor( _T("MainColor"), clr );
    }

};

class skinxmlhtmlctrl: public skinxmlwin
{
public:

    skinxmlhtmlctrl(const SkinXmlElement& xmlElement = SkinXmlElement()) 
        : skinxmlwin(xmlElement)
    {
    }

    const skinxmlhtmlctrl& operator = (const SkinXmlElement& xmlElement)
    {
        m_xmlResElement = xmlElement;

        return *this;
    }

    static LPCTSTR GetSkinWndClassName() 
    {
        return _T("skinxmlhtmlctrl");
    }
};


};