/********************************************************************
* CreatedOn: 2007-12-17   17:08
* FileName: skinxmlobject.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/


#pragma once

#include <skintinyxml.h>

namespace KSG{

class skinxmlobject
{
public:

    skinxmlobject(const SkinXmlElement& xmlElement = SkinXmlElement())
    {
        m_xmlResElement = xmlElement;
    }

    const skinxmlobject& operator = (const SkinXmlElement& xmlElement)
    {
        m_xmlResElement = xmlElement;

        return *this;
    }


    operator SkinXmlElement () 
    {
        return m_xmlResElement; 
    }

    virtual BOOL  AttachXmlElement(const SkinXmlElement& xmlElement)
    {
        m_xmlResElement = xmlElement;

        return TRUE;
    }

    virtual BOOL GetObject( LPCTSTR Name, KSG::CString& strOjbect )
    {
        if (!m_xmlResElement.IsValid())
            return FALSE;

        return m_xmlResElement.GetValue(Name, strOjbect);
    }

    virtual BOOL GetObject( LPCTSTR Name, DWORD& uOjbect )
    {
        KSG::CString strObject;

        if (!GetObject(Name, strObject))
            return FALSE;

        if (strObject.GetLength() > 2 && 
            strObject[0] == '0' && 
            (strObject[1] == 'x' || strObject[1] == 'X'))
        {
            _stscanf_s(strObject, _T("0x%x"), &uOjbect);
        }
        else
        {
            uOjbect = (DWORD)_ttoi64(strObject);
        }

        return TRUE;
    }

    virtual BOOL GetObject( LPCTSTR Name, int & nOjbect )
    {
        KSG::CString strObject;

        if (!GetObject(Name, strObject))
            return FALSE;

        nOjbect = _ttoi(strObject);

        return TRUE;
    }


public:

    SkinXmlElement m_xmlResElement;

};



} // amespace KSG{

