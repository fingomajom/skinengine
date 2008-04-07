/********************************************************************
* CreatedOn: 2007-12-17   17:08
* FileName: skinresloader.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include "skinres.h"

namespace KSGUI{

class skinresloader
{

public:

    skinresloader() :
        m_pskinres(NULL)
    {

    }

    static skinresloader& instance()
    {
        static skinresloader g_skinresloader_instance;

        return g_skinresloader_instance;
    }

    bool load_skin(LPCTSTR pszSkinXmlFile,
        skinconfigbase* pskinconfig = NULL,
        skinstrresbase* pskinstrres = NULL,
        skindlgresbase* pskindlgres = NULL,
        skinimageresbase* pskinimageresbase = NULL)
    {
        destroy_skin();

        m_pskinres = new skinres();
        if (m_pskinres == NULL)
            return false;

        bool bresult = m_pskinres->init_skin(
           pszSkinXmlFile,
           pskinconfig,
           pskinstrres,
           pskindlgres,
           pskinimageresbase);
        
        return bresult;
    }
    
    void destroy_skin()
    {
        if (m_pskinres != NULL)
        {
            m_pskinres->uinit_skin();
            skin_delete_p(m_pskinres);
        }
    }

    skinres* get_skinres()
    {
        return m_pskinres;
    }


public:

    skinres* m_pskinres;
    
};



} // namespace KSGUI
