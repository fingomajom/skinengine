

#pragma once

#include <skinresbase.h>
#include <skinstrres.h>
#include <skindlgres.h>
#include <skinconfig.h>
#include <skinimageres.h>
#include <atlpath.h>

namespace KSE{


class skinresbase
{
public:
    
    virtual skinconfigbase* get_skinconfig() = 0;

    virtual skinstrresbase* get_skinstrres() = 0;

    virtual skindlgresbase* get_skindlgres() = 0;
    
    virtual skinimageresbase* get_skinimageres() = 0;
};



class skinres : public skinresbase
{
public:

    skinres(  ) :
        m_pXmlDocument(NULL),
        m_pskinconfig(NULL),
        m_pskinstrres(NULL),
        m_pskindlgres(NULL),
        m_pskinimageresbase(NULL)
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
        skinimageresbase* pskinimageresbase = NULL)
    {
        uinit_skin();

        m_pskinconfig = pskinconfig;
        m_pskinstrres = pskinstrres;
        m_pskindlgres = pskindlgres;
        m_pskinimageresbase = pskinimageresbase;

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

            if (m_pskinstrres == NULL)
            {
                m_pskinstrres = new skinstrres();
                if (m_pskinstrres == NULL)
                    break;
                SkinXmlElement strresnode = root.FirstChildElement(skinstrresbase::GetResKeyName());
                if (strresnode.IsValid())
                    ((skinstrres*)m_pskinstrres)->AttachXmlNode( strresnode );

            }

            if (m_pskindlgres == NULL)
            {
                m_pskindlgres = new skindlgres();
                if (m_pskindlgres == NULL)
                    break;
                SkinXmlElement strdlgnode = root.FirstChildElement(skindlgresbase::GetResKeyName());
                if (strdlgnode.IsValid())
                    ((skindlgres*)m_pskindlgres)->AttachXmlNode( strdlgnode );

            }

            if (m_pskinimageresbase == NULL)
            {
                m_pskinimageresbase = new skinimageres(m_pskinconfig);
                if (m_pskinimageresbase == NULL)
                    break;
                SkinXmlElement strdimgnode = root.FirstChildElement(skinimageresbase::GetResKeyName());
                if (strdimgnode.IsValid())
                    ((skinimageres*)m_pskinimageresbase)->AttachXmlNode( strdimgnode );

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
        skin_delete_p(m_pskinimageresbase);
    }

public:

    virtual skinconfigbase* get_skinconfig() 
    {
        return m_pskinconfig;
    }

    virtual skinstrresbase* get_skinstrres()
    {
        return m_pskinstrres;
    }

    virtual skindlgresbase* get_skindlgres()
    {
        return m_pskindlgres;
    }

    virtual skinimageresbase* get_skinimageres()
    {
        return m_pskinimageresbase;
    }


public:

    SkinXmlDocument* m_pXmlDocument;
    
    skinconfigbase* m_pskinconfig;
    skinstrresbase* m_pskinstrres;
    skindlgresbase* m_pskindlgres;
    skinimageresbase* m_pskinimageresbase;
};

} // namespace KSE
