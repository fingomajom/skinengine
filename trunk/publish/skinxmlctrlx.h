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

namespace KSG{

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

    BOOL GetBmpResName( KSG::CString& strBmpResName)
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

};