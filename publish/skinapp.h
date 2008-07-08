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


namespace KSGUI{

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
        skinimageresbase* pskinimageres = NULL,
        skinmenuresbase*  pskinmenures = NULL)
    {
        bool bskininit = m_skinresloader.load_skin(
            pszSkinXmlFile,
            pskinconfig,
            pskinstrres,
            pskindlgres,
            pskinimageres,
            pskinmenures);

        ATLASSERT(bskininit);

        CString::get_pfn_loadstring_str()  = LoadString_S;
        CString::get_pfn_loadstring_uint() = LoadString_U;
        
        return bskininit;
    }
    
    void destroy_skin()
    {
        m_skinresloader.destroy_skin();
    }

public:

    static BOOL LoadString_S(LPCTSTR pszIDName, KSGUI::CString& strValue)
    {
        skinresbase* pskinres = skinresloader::instance().get_skinres();

        if (!pskinres)
            return FALSE;

        skinstrresbase* pskinstrres = pskinres->get_skinstrres();

        if (!pskinstrres)
            return FALSE;

        return pskinstrres->LoadString(pszIDName, strValue);
    }

    static BOOL LoadString_U(UINT uResID, KSGUI::CString& strValue)
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


inline BOOL SkinLoadString(LPCTSTR pszIDName, KSGUI::CString& strValue)
{
    return KSGUI::CSkinAppModule::LoadString_S(pszIDName, strValue);
}

inline KSGUI::CString SkinLoadString(LPCTSTR pszIDName)
{
	KSGUI::CString str;
	KSGUI::CSkinAppModule::LoadString_S(pszIDName, str);
	return str;
}

inline BOOL SkinLoadString(UINT uResID, KSGUI::CString& strValue)
{
    return KSGUI::CSkinAppModule::LoadString_U(uResID, strValue);
}

inline KSGUI::CString SkinLoadString(UINT uResID)
{
	KSGUI::CString str;
	return KSGUI::CSkinAppModule::LoadString_U(uResID, str);
	return str;
}

inline HBITMAP SkinLoadBitmap(LPCTSTR pszIDName)
{
    KSGUI::skinresbase* pskinres = KSGUI::skinresloader::instance().get_skinres();

    if (!pskinres)
        return NULL;

    KSGUI::skinimageresbase* pskinimageres = pskinres->get_skinimageres();

    if (!pskinimageres)
        return NULL;

    return pskinimageres->LoadBitmap(pszIDName);
}

inline HCURSOR SkinLoadCursor(LPCTSTR pszIDName)
{
    KSGUI::skinresbase* pskinres = KSGUI::skinresloader::instance().get_skinres();

    if (!pskinres)
        return NULL;

    KSGUI::skinimageresbase* pskinimageres = pskinres->get_skinimageres();

    if (!pskinimageres)
        return NULL;

    return pskinimageres->LoadCursor(pszIDName);
}

inline HICON SkinLoadIcon(LPCTSTR pszIDName, int cxDesired, int cyDesired)
{
    KSGUI::skinresbase* pskinres = KSGUI::skinresloader::instance().get_skinres();

    if (!pskinres)
        return NULL;

    KSGUI::skinimageresbase* pskinimageres = pskinres->get_skinimageres();

    if (!pskinimageres)
        return NULL;

    return pskinimageres->LoadIcon(pszIDName, cxDesired, cyDesired);
}


inline HMENU SkinLoadMenu( LPCTSTR pszIDName )
{
    KSGUI::skinresbase* pskinres = KSGUI::skinresloader::instance().get_skinres();

    if (!pskinres)
        return NULL;

    KSGUI::skinmenuresbase* pskinmenures = pskinres->get_skinmenures();

    if (!pskinmenures)
        return NULL;

    return pskinmenures->LoadMenu(pszIDName);
}


}; // namespace KSGUI


extern KSGUI::CSkinAppModule _Module;
