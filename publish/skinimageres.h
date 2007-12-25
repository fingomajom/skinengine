/********************************************************************
* CreatedOn: 2007-12-17   17:08
* FileName: skinimageres.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include <skinxmlobject.h>
#include <skinconfig.h>
#include <atlpath.h>

namespace KSE{

class skinimageresbase
{
public:
    virtual BOOL    LoadImageFile(LPCTSTR pszIDName, KSE::CString& strFileName) = 0;

    virtual HBITMAP LoadBitmap(LPCTSTR pszIDName) = 0;
    virtual HCURSOR LoadCursor(LPCTSTR pszIDName) = 0;
    virtual HICON   LoadIcon  (LPCTSTR pszIDName, int cxDesired, int cyDesired) = 0;

    static LPCTSTR GetResKeyName()   { return _T("Image");    }
    static LPCTSTR GetValueAttName() { return _T("FileName"); }

};


class skinimageres : 
    public skinimageresbase,
    public skinxmlobject
{
public:

    skinimageres(skinconfigbase* pskinconfig) :
        m_pskinconfig(pskinconfig)
    {

    }

    skinimageres(const SkinXmlElement& xmlElement = SkinXmlElement()) :
        skinxmlobject(xmlElement)
    {
    }

    const skinimageres& operator = (const SkinXmlElement& xmlElement)
    {
        m_xmlResElement = xmlElement;

        return *this;
    }


    virtual HBITMAP LoadBitmap(LPCTSTR pszIDName)
    {
        return (HBITMAP)LoadImage(pszIDName, IMAGE_BITMAP, 0, 0, 
            LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
    }

    virtual HCURSOR LoadCursor(LPCTSTR pszIDName)
    {
        return (HCURSOR)LoadImage(pszIDName, IMAGE_CURSOR, 0, 0, 
            LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
    }

    virtual HICON LoadIcon(LPCTSTR pszIDName, int cxDesired, int cyDesired)
    {
        return (HICON)LoadImage(pszIDName, IMAGE_ICON, cxDesired, cyDesired, 
            LR_LOADFROMFILE | LR_DEFAULTCOLOR);
    }

    virtual BOOL LoadImageFile(LPCTSTR pszIDName, KSE::CString& strFileName)
    {
        SkinXmlElement node = m_xmlResElement.FirstChildElement(pszIDName);

        if (!node.IsValid())
            return FALSE;

        return node.GetValue(skinimageresbase::GetValueAttName(), strFileName);
    }


    HANDLE LoadImage(LPCTSTR pszIDName,
        UINT uType,
        int cxDesired,
        int cyDesired,
        UINT fuLoad)
    {
        if (m_pskinconfig == NULL)
            return NULL;

        KSE::CString strFileName;
        if (!LoadImageFile(pszIDName, strFileName))
            return NULL;
        
        CPath pathImage;

        pathImage.m_strPath = m_pskinconfig->GetSkinImagePath();
        pathImage.Append(strFileName);


        return ::LoadImage(NULL, pathImage.m_strPath, uType, cxDesired, cyDesired, fuLoad);
    }


public:   
    skinconfigbase* m_pskinconfig;
};

};