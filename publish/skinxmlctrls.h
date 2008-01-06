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

#include <skinxmlwin.h>

namespace KSG{


class skinxmlstatic;
class skinxmlpicture;
class skinxmlgroupbox;

class skinxmlbutton;
class skinxmldefbutton;
class skinxmlcheckbox;
class skinxmlcombobox;
class skinxmllistbox;
class skinxmlradiobox;

class skinxmllistctrl;
class skinxmltreectrl;
class skinxmltablectrl;

class skinxmledit;
class skinxmlhotkey;
class skinxmlrichedit;

class skinxmldatetime;
class skinxmlmonthcalendar;


class skinxmlhscrollbar;
class skinxmlvscrollbar;

class skinxmlslider;
class skinxmlspin;
class skinxmlprogress;

class skinxmlanimation;

class skinxmlipaddress;
class skinxmldatetime;



class skinxmlstatic : public skinxmlwin
{
public:

    skinxmlstatic(const SkinXmlElement& xmlElement = SkinXmlElement()) 
        : skinxmlwin(xmlElement)
    {
    }

    const skinxmlstatic& operator = (const SkinXmlElement& xmlElement)
    {
        m_xmlResElement = xmlElement;

        return *this;
    }
    
    static LPCTSTR GetWndClassName() 
    {
        return _T("skinstatic");
    }

};

class skinxmlpicture : public skinxmlwin
{
public:

    skinxmlpicture(const SkinXmlElement& xmlElement = SkinXmlElement()) 
        : skinxmlwin(xmlElement)
    {
    }

    const skinxmlpicture& operator = (const SkinXmlElement& xmlElement)
    {
        m_xmlResElement = xmlElement;

        return *this;
    }

    static LPCTSTR GetWndClassName() 
    {
        return _T("skinpicture");
    }

};

class skinxmlgroupbox : public skinxmlwin
{
public:

    skinxmlgroupbox(const SkinXmlElement& xmlElement = SkinXmlElement()) 
        : skinxmlwin(xmlElement)
    {
    }

    const skinxmlgroupbox& operator = (const SkinXmlElement& xmlElement)
    {
        m_xmlResElement = xmlElement;

        return *this;
    }

    static LPCTSTR GetWndClassName() 
    {
        return _T("skingroupbox");
    }

};

};