
#pragma once

#include "StringTableResource.h"
#include "skinimageedit.h"


class CResDocument
{
public:
    CResDocument(void);
    ~CResDocument(void);

    static CResDocument& Instance();

    SkinXmlDocument& GetDocument() ;       

    //////////////////////////////////////////////////////////////////////////

    BOOL Init( LPCTSTR pszFile = NULL );
    void Uninit( );

    void Save();

    BOOL GetChanged() const;
    void SetChanged(BOOL bChanged = TRUE);

    
    LPCTSTR GetFileName() const
    {
        return m_strFileName;
    }
    
    CStringTableResource* GetStringTableResource();

    skinimageedit* getskinimageedit()
    {
        return m_pskinimageedit;
    }

private:
    
    SkinXmlDocument*  m_pxmlDocument;
    CStringTableResource* m_pStringTable;

    skinimageedit* m_pskinimageedit;

    BOOL m_bChanged ;
    
    KSG::CString m_strFileName;
};
