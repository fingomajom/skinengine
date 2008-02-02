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


//class skinxmlstatic;
//class skinxmlpicture;
//
//class skinxmlgroupbox;
//class skinxmlbutton;
//class skinxmlcheckbox;
//class skinxmlradiobox;

//class skinxmlcombobox;
//class skinxmllistbox;
//
//class skinxmllistctrl;
//class skinxmltreectrl;
//class skinxmltablectrl;
//
//class skinxmledit;
//class skinxmlhotkey;
//class skinxmlrichedit;
//
//class skinxmldatetime;
//class skinxmlmonthcalendar;
//
//
//class skinxmlhscrollbar;
//class skinxmlvscrollbar;
//
//class skinxmlslider;
//class skinxmlspin;
//class skinxmlprogress;
//
//class skinxmlanimation;
//
//class skinxmlipaddress;
//


#define begin_skinxmlctrls(className, wcn) \
class className : public skinxmlwin \
{   \
public: \
    className(const SkinXmlElement& xmlElement = SkinXmlElement()) \
        : skinxmlwin(xmlElement)    \
    {   \
    }   \
    const className& operator = (const SkinXmlElement& xmlElement)  \
    {   \
        m_xmlResElement = xmlElement;   \
        return *this;   \
    }   \
    static LPCTSTR GetSkinWndClassName() \
    {   \
        return wcn;    \
    } 

#define end_skinxmlctrls()  \
};


template<const TCHAR*& SkinWndClassName>
class skinxmlctrls_t : public skinxmlwin
{
public:

    skinxmlctrls_t(const SkinXmlElement& xmlElement = SkinXmlElement()) 
        : skinxmlwin(xmlElement)
    {
    }

    const skinxmlctrls_t<SkinWndClassName>& operator = (const SkinXmlElement& xmlElement)
    {
        m_xmlResElement = xmlElement;

        return *this;
    }

    static LPCTSTR GetSkinWndClassName() 
    {
        return SkinWndClassName;
    }

};


#define define_xml_ctrls(class_name)   \
    _declspec(selectany) const TCHAR* g_psz##class_name##_xmlctrlstr = _T(#class_name);   \
    typedef skinxmlctrls_t<g_psz##class_name##_xmlctrlstr>  class_name;


define_xml_ctrls(skinxmlstatic);
define_xml_ctrls(skinxmlpicture);

define_xml_ctrls(skinxmlgroupbox);
define_xml_ctrls(skinxmlbutton);
define_xml_ctrls(skinxmlcheckbox);
define_xml_ctrls(skinxmlradiobox);

define_xml_ctrls(skinxmlcombobox);
define_xml_ctrls(skinxmllistbox);

define_xml_ctrls(skinxmllistctrl);
define_xml_ctrls(skinxmltreectrl);
define_xml_ctrls(skinxmltablectrl);

define_xml_ctrls(skinxmledit);
define_xml_ctrls(skinxmlhotkey);
define_xml_ctrls(skinxmlrichedit);

define_xml_ctrls(skinxmldatetime);
define_xml_ctrls(skinxmlmonthcalendar);

define_xml_ctrls(skinxmlhscrollbar);
define_xml_ctrls(skinxmlvscrollbar);

define_xml_ctrls(skinxmlslider);
define_xml_ctrls(skinxmlspin);
define_xml_ctrls(skinxmlprogress);

define_xml_ctrls(skinxmlanimation);

define_xml_ctrls(skinxmlipaddress);


};