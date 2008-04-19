/********************************************************************
* CreatedOn: 2007-12-19   17:40
* FileName: skinres.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include <skinresbase.h>
#include <skinstrres.h>
#include <skindlgres.h>
#include <skinconfig.h>
#include <skinimageres.h>
#include <skinmenures.h>
#include <atlpath.h>

namespace KSGUI{


class skinresbase
{
public:
    
    virtual skinconfigbase* get_skinconfig() = 0;

    virtual skinstrresbase* get_skinstrres() = 0;

    virtual skindlgresbase* get_skindlgres() = 0;
    
    virtual skinimageresbase* get_skinimageres() = 0;

    virtual skinmenuresbase* get_skinmenures() = 0;
};



class skinres : public skinresbase
{
public:

    skinres(  ) :
        m_pXmlDocument(NULL),
        m_pskinconfig(NULL),
        m_pskinstrres(NULL),
        m_pskindlgres(NULL),
        m_pskinimageres(NULL),
        m_pskinmenures(NULL)
    {
    }

    ~skinres()
    {
        uinit_skin();
    }

    bool init_skin(LPCTSTR pszSkinXmlFile,
        skinconfigbase* pskinconfig = NULL,
        skinstrresbase* pskinstrres = NULL,
        skindlgresbase* pskindlgres = NULL,
        skinimageresbase* pskinimageres = NULL,
        skinmenuresbase*  pskinmenures = NULL)
    {
        uinit_skin();

        m_pskinconfig   = pskinconfig;
        m_pskinstrres   = pskinstrres;
        m_pskindlgres   = pskindlgres;
        m_pskinimageres = pskinimageres;
        m_pskinmenures  = pskinmenures;

        if (pszSkinXmlFile == NULL || _tcslen(pszSkinXmlFile) == NULL)
        {
            //

            return true;
        }

        while (pszSkinXmlFile != NULL)
        {
            if (m_pskinconfig == NULL)
            {
                m_pskinconfig = new skinconfig();
            }

            if (m_pskinconfig == NULL)
                break;

            CPath pathXmlFile;
            pathXmlFile.m_strPath = m_pskinconfig->GetSkinResPath();
            pathXmlFile.Append(pszSkinXmlFile);

            m_pXmlDocument = new SkinXmlDocument();
            if (m_pXmlDocument == NULL)
                break;

            m_pXmlDocument->LoadFile(pathXmlFile);

            SkinXmlElement root = m_pXmlDocument->RootElement();
            if (!root.IsValid())
                break;

            while (m_pskinstrres == NULL)
            {
                m_pskinstrres = new skinstrres();
                if (m_pskinstrres == NULL)
                    break;
                SkinXmlElement strresnode = root.FirstChildElement(skinstrresbase::GetResKeyName());
                if (strresnode.IsValid())
                    ((skinstrres*)m_pskinstrres)->AttachXmlElement( strresnode );

            }

            while (m_pskindlgres == NULL)
            {
                m_pskindlgres = new skindlgres();
                if (m_pskindlgres == NULL)
                    break;
                SkinXmlElement strdlgnode = root.FirstChildElement(skindlgresbase::GetResKeyName());
                if (strdlgnode.IsValid())
                    ((skindlgres*)m_pskindlgres)->AttachXmlElement( strdlgnode );

            }

            while (m_pskinimageres == NULL)
            {
                m_pskinimageres = new skinimageres(m_pskinconfig);
                if (m_pskinimageres == NULL)
                    break;
                SkinXmlElement strimagenode = root.FirstChildElement(skinimageresbase::GetResKeyName());
                if (strimagenode.IsValid())
                    ((skinimageres*)m_pskinimageres)->AttachXmlElement( strimagenode );

            }

            while (m_pskinmenures == NULL)
            {
                m_pskinmenures = new skinmenures();
                if (m_pskinmenures == NULL)
                    break;
                SkinXmlElement strmenunode = root.FirstChildElement(skinmenuresbase::GetResKeyName());
                if (strmenunode.IsValid())
                    ((skinmenures*)m_pskinmenures)->AttachXmlElement( strmenunode );

            }


            return true;
        }

        uinit_skin();

        return false;
   }


    void uinit_skin()
    {
        skin_delete_p(m_pXmlDocument);

        skin_delete_p(m_pskinconfig);
        skin_delete_p(m_pskinstrres);
        skin_delete_p(m_pskindlgres);
        skin_delete_p(m_pskinimageres);
        skin_delete_p(m_pskinmenures);
    }

public:

    virtual skinconfigbase* get_skinconfig() 
    {
        ATLASSERT( m_pskinconfig != NULL );

        return m_pskinconfig;
    }

    virtual skinstrresbase* get_skinstrres()
    {
        ATLASSERT( m_pskinstrres != NULL );

        return m_pskinstrres;
    }

    virtual skindlgresbase* get_skindlgres()
    {
        ATLASSERT( m_pskindlgres != NULL );

        return m_pskindlgres;
    }

    virtual skinimageresbase* get_skinimageres()
    {
        ATLASSERT( m_pskinimageres != NULL );

        return m_pskinimageres;
    }

    virtual skinmenuresbase* get_skinmenures() 
    {
        ATLASSERT( m_pskinmenures != NULL );

        return m_pskinmenures;
    }

public:

    SkinXmlDocument* m_pXmlDocument;
    
    skinconfigbase* m_pskinconfig;
    skinstrresbase* m_pskinstrres;
    skindlgresbase* m_pskindlgres;
    skinimageresbase* m_pskinimageres;
    skinmenuresbase*  m_pskinmenures;
};

} // namespace KSGUI
