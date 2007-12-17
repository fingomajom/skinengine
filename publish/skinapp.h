/********************************************************************
* CreatedOn: 2007-12-17   17:07
* FileName: skinapp.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include <skinresloader.h>


namespace KSE{

class CSkinAppModule : public WTL::CAppModule
{
public:

    CSkinAppModule() :
        m_skinresloader(skinresloader::instance())
    {

    }

    bool init_skin(LPCTSTR pszSkinXmlFile,
        skinconfigbase* pskinconfig = NULL,
        skinstrresbase* pskinstrres = NULL,
        skindlgresbase* pskindlgres = NULL,
        skinimageresbase* pskinimageresbase = NULL)
    {
        bool bskininit = m_skinresloader.load_skin(
            pszSkinXmlFile,
            pskinconfig,
            pskinstrres,
            pskindlgres,
            pskinimageresbase);

        ATLASSERT(bskininit);

        CString::s_pfn_loadstring_str  = LoadString_S;
        CString::s_pfn_loadstring_uint = LoadString_U;
        
        return bskininit;
    }
    
    void destroy_skin()
    {
        m_skinresloader.destroy_skin();
    }

public:

    static BOOL LoadString_S(LPCTSTR pszIDName, KSE::CString& strValue)
    {
        skinresbase* pskinres = skinresloader::instance().get_skinres();

        if (!pskinres)
            return FALSE;

        skinstrresbase* pskinstrres = pskinres->get_skinstrres();

        if (!pskinstrres)
            return FALSE;

        return pskinstrres->LoadString(pszIDName, strValue);
    }

    static BOOL LoadString_U(UINT uResID, KSE::CString& strValue)
    {
        skinresbase* pskinres = skinresloader::instance().get_skinres();

        if (!pskinres)
            return FALSE;

        skinstrresbase* pskinstrres = pskinres->get_skinstrres();

        if (!pskinstrres)
            return FALSE;

        return pskinstrres->LoadString(uResID, strValue);
    }


    skinresloader& m_skinresloader;
};

} // namespace KSE


extern KSE::CSkinAppModule _Module;


inline BOOL SkinLoadString(LPCTSTR pszIDName, KSE::CString& strValue)
{
    return KSE::CSkinAppModule::LoadString_S(pszIDName, strValue);
}

inline BOOL SkinLoadString(UINT uResID, KSE::CString& strValue)
{
    return KSE::CSkinAppModule::LoadString_U(uResID, strValue);
}

inline HBITMAP SkinLoadBitmap(LPCTSTR pszIDName)
{
    KSE::skinresbase* pskinres = KSE::skinresloader::instance().get_skinres();

    if (!pskinres)
        return NULL;

    KSE::skinimageresbase* pskinimageres = pskinres->get_skinimageres();

    if (!pskinimageres)
        return NULL;

    return pskinimageres->LoadBitmap(pszIDName);
}

inline HCURSOR SkinLoadCursor(LPCTSTR pszIDName)
{
    KSE::skinresbase* pskinres = KSE::skinresloader::instance().get_skinres();

    if (!pskinres)
        return NULL;

    KSE::skinimageresbase* pskinimageres = pskinres->get_skinimageres();

    if (!pskinimageres)
        return NULL;

    return pskinimageres->LoadCursor(pszIDName);
}

inline HICON SkinLoadIcon(LPCTSTR pszIDName, int cxDesired, int cyDesired)
{
    KSE::skinresbase* pskinres = KSE::skinresloader::instance().get_skinres();

    if (!pskinres)
        return NULL;

    KSE::skinimageresbase* pskinimageres = pskinres->get_skinimageres();

    if (!pskinimageres)
        return NULL;

    return pskinimageres->LoadIcon(pszIDName, cxDesired, cyDesired);
}
