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
    template<class T>
    class skin_ptr
    {
    public:

        skin_ptr( T* ptr = NULL ) :
            m_ptr(ptr),
            m_bowner(false)
        {  }

        const skin_ptr& operator=(const skin_ptr& ptr)
        {
            if (this == &ptr)
                return *this;
            
            m_ptr = ptr.m_ptr;
            m_bowner = false;

            return *this;
        }

        T* ownerptr( T* ptr )
        {
            m_ptr = ptr;
            m_bowner = true;

            return m_ptr;
        }

        operator T*()
        {
            return m_ptr;
        }

        T* operator->()
        {
            return m_ptr;
        }
        
        T* getptr()
        {
            return m_ptr;
        }

        void release()
        {
            if (m_bowner && m_ptr != NULL)
                delete m_ptr;

            m_ptr = NULL;
            m_bowner = false;
        }

    private:        
        T*   m_ptr;
        bool m_bowner;
    };

public:

    skinres(  ) 
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


        m_skinconfig_ptr   = pskinconfig;
        m_skinstrres_ptr   = pskinstrres;
        m_skindlgres_ptr   = pskindlgres;
        m_skinimageres_ptr = pskinimageres;
        m_skinmenures_ptr  = pskinmenures;

        if (pszSkinXmlFile == NULL || _tcslen(pszSkinXmlFile) == NULL)
        {
            //

            return true;
        }

        while (pszSkinXmlFile != NULL)
        {
            if (m_skinconfig_ptr.getptr() == NULL)
            {
                m_skinconfig_ptr.ownerptr( new skinconfig() );
            }

            if (m_skinconfig_ptr.getptr() == NULL)
                break;

            CPath pathXmlFile;
            pathXmlFile.m_strPath = m_skinconfig_ptr->GetSkinResPath();
            pathXmlFile.Append(pszSkinXmlFile);

            m_xmldocument_ptr.ownerptr( new SkinXmlDocument() );
            if (m_xmldocument_ptr.getptr() == NULL)
                break;

            m_xmldocument_ptr->LoadFile(pathXmlFile);

            SkinXmlElement root = m_xmldocument_ptr->RootElement();
            if (!root.IsValid())
                break;

            while (m_skinstrres_ptr.getptr() == NULL)
            {
                m_skinstrres_ptr.ownerptr( new skinstrres() );
                if (m_skinstrres_ptr.getptr() == NULL)
                    break;

                SkinXmlElement strresnode = root.FirstChildElement(
                    skinstrresbase::GetResKeyName());

                if (strresnode.IsValid())
                {
                    ((skinstrres*)m_skinstrres_ptr.getptr())->AttachXmlElement( 
                        strresnode );
                }

            }

            while (m_skindlgres_ptr.getptr() == NULL)
            {
                m_skindlgres_ptr.ownerptr( new skindlgres() );
                if (m_skindlgres_ptr.getptr() == NULL)
                    break;

                SkinXmlElement strdlgnode = root.FirstChildElement(
                    skindlgresbase::GetResKeyName());

                if (strdlgnode.IsValid())
                {
                    ((skindlgres*)m_skindlgres_ptr.getptr())->AttachXmlElement( 
                        strdlgnode );
                }

            }

            while (m_skinimageres_ptr.getptr() == NULL)
            {
                m_skinimageres_ptr.ownerptr( new skinimageres(m_skinconfig_ptr.getptr()) );;
                if (m_skinimageres_ptr.getptr() == NULL)
                    break;

                SkinXmlElement strimagenode = root.FirstChildElement(
                    skinimageresbase::GetResKeyName());

                if (strimagenode.IsValid())
                {
                    ((skinimageres*)m_skinimageres_ptr.getptr())->AttachXmlElement( 
                        strimagenode );
                }

            }

            while (m_skinmenures_ptr.getptr() == NULL)
            {
                m_skinmenures_ptr.ownerptr( new skinmenures() );
                if (m_skinmenures_ptr.getptr() == NULL)
                    break;

                SkinXmlElement strmenunode = root.FirstChildElement(
                    skinmenuresbase::GetResKeyName());

                if (strmenunode.IsValid())
                {
                    ((skinmenures*)m_skinmenures_ptr.getptr())->AttachXmlElement( 
                        strmenunode );
                }

            }


            return true;
        }

        uinit_skin();

        return false;
   }


    void uinit_skin()
    {
        m_xmldocument_ptr.release();

        m_skinconfig_ptr.release();
        m_skinstrres_ptr.release();
        m_skindlgres_ptr.release();
        m_skinimageres_ptr.release();
        m_skinmenures_ptr.release();
    }

public:

    virtual skinconfigbase* get_skinconfig() 
    {
        ATLASSERT( m_skinconfig_ptr.getptr() != NULL );

        return m_skinconfig_ptr;
    }

    virtual skinstrresbase* get_skinstrres()
    {
        ATLASSERT( m_skinstrres_ptr.getptr() != NULL );

        return m_skinstrres_ptr;
    }

    virtual skindlgresbase* get_skindlgres()
    {
        ATLASSERT( m_skindlgres_ptr.getptr() != NULL );

        return m_skindlgres_ptr;
    }

    virtual skinimageresbase* get_skinimageres()
    {
        ATLASSERT( m_skinimageres_ptr.getptr() != NULL );

        return m_skinimageres_ptr;
    }

    virtual skinmenuresbase* get_skinmenures() 
    {
        ATLASSERT( m_skinmenures_ptr.getptr() != NULL );

        return m_skinmenures_ptr;
    }

public:

    skin_ptr<SkinXmlDocument>  m_xmldocument_ptr;
    
    skin_ptr<skinconfigbase>   m_skinconfig_ptr;
    skin_ptr<skinstrresbase>   m_skinstrres_ptr;
    skin_ptr<skindlgresbase>   m_skindlgres_ptr;
    skin_ptr<skinimageresbase> m_skinimageres_ptr;
    skin_ptr<skinmenuresbase>  m_skinmenures_ptr;
};

} // namespace KSGUI
