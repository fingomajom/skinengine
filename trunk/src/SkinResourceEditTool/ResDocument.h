
#pragma once

#include "StringTableResource.h"
#include "skinimageedit.h"


class CResDocument
{
public:
    CResDocument(void);
    ~CResDocument(void);

    static CResDocument& Instance();

    CXmlDocumentWrapper& GetDocument() ;       

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

    skinimageedit& getskinimageedit()
    {
        return m_skinimageedit;
    }

private:
    
    CXmlDocumentWrapper*  m_pxmlDocument;
    CStringTableResource* m_pStringTable;

    skinimageedit m_skinimageedit;

    BOOL m_bChanged ;
    
    KSE::CString m_strFileName;
};
