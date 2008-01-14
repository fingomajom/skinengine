/********************************************************************
* CreatedOn: 2008-1-11   03:08
* FileName: skinwincreator.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include <map>
#include "skinctrls.h"
#include "skinxmlctrls.h"

namespace KSG{


template<class SkinWindowType>
SkinWindow* SkinCreate_Static(
    const SkinXmlElement& xmlElement,
    HWND hWndParent, _U_MENUorID MenuOrID = 0U )
{
    SkinWindowType * pSkinWindow = new SkinWindowType;

    if (pSkinWindow == NULL)
        return pSkinWindow;

    if (pSkinWindow->SkinCreate(xmlElement, hWndParent, MenuOrID) == NULL)
    {
        delete pSkinWindow;

        pSkinWindow = NULL;
    }

    return pSkinWindow;
}



class SkinWindowCreator
{

protected:

    typedef SkinWindow* pfnSkinCreateFunc(const SkinXmlElement& xmlElement,
        HWND hWndParent, _U_MENUorID MenuOrID );

public:

    SkinWindow* SkinCreate(
        const SkinXmlElement& xmlElement,
        HWND hWndParent, _U_MENUorID MenuOrID = 0U )
    {
        SkinWindow * pSkinWindow = NULL;

        skinxmlwin   xmlWin(xmlElement);
        KSG::CString strClassName;

        if (!xmlWin.GetSkinClassName(strClassName))
            return pSkinWindow;

        std::map<KSG::CString, pfnSkinCreateFunc*>::const_iterator iter = 
            m_mapCreator.find(strClassName);

        if (iter == m_mapCreator.end())
            return pSkinWindow;
        
        pfnSkinCreateFunc * pCreatorFunc = iter->second;

        if (pCreatorFunc != NULL)
            pSkinWindow = pCreatorFunc(xmlElement, hWndParent, MenuOrID);

        return pSkinWindow;
    }

    static SkinWindowCreator& Instance()
    {
        static SkinWindowCreator g_skinWinCreator;

        return g_skinWinCreator;
    }

    void AddSkinCreator( const KSG::CString& strWndClass, 
        pfnSkinCreateFunc* pfnCreator)
    {
        m_mapCreator[strWndClass] = pfnCreator;
    }


protected:

    SkinWindowCreator()
    {
        AddSkinCreator(KSG::skinxmlstatic::GetSkinWndClassName(), 
            SkinCreate_Static<CSkinStatic>);

        AddSkinCreator(KSG::skinxmlpicture::GetSkinWndClassName(), 
            SkinCreate_Static<CSkinStatic>);

        AddSkinCreator(KSG::skinxmlbutton::GetSkinWndClassName(), 
            SkinCreate_Static<CSkinButton>);
    }


    std::map<KSG::CString, pfnSkinCreateFunc*> m_mapCreator;
};



};