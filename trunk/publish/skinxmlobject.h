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

namespace KSE{

class skinxmlobject
{
public:

    operator SkinXmlElement ()
    {
        return m_xmlResElement;
    }

    
    void operator = (const SkinXmlElement& xmlElement)
    {
        m_xmlResElement = xmlElement;
    }


    virtual BOOL  AttachXmlElement(const SkinXmlElement& xmlElement)
    {
        m_xmlResElement = xmlElement;

        return TRUE;
    }

    virtual BOOL GetObject( LPCTSTR Name, KSE::CString& strOjbect )
    {
        if (!m_xmlResElement.IsValid())
            return FALSE;

        return m_xmlResElement.GetValue(Name, strOjbect);
    }

    virtual BOOL GetObject( LPCTSTR Name, DWORD& uOjbect )
    {
        KSE::CString strObject;

        if (!GetObject(Name, strObject))
            return FALSE;

        uOjbect = (DWORD)_ttol(strObject);

        return TRUE;
    }

    virtual BOOL GetObject( LPCTSTR Name, int & nOjbect )
    {
        KSE::CString strObject;

        if (!GetObject(Name, strObject))
            return FALSE;

        nOjbect = (DWORD)_ttoi(strObject);

        return TRUE;
    }



public:

    SkinXmlElement m_xmlResElement;


};

} // amespace KSE{