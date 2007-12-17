
#pragma once

#include "StringTableResource.h"


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
    std::vector<CStringTableResource::STRINGTABLE_ITEMINFO>& GetStringTableList();
private:
    
    CXmlDocumentWrapper*  m_pxmlDocument;
    CStringTableResource* m_pStringTable;

    BOOL m_bChanged ;

    std::vector<CStringTableResource::STRINGTABLE_ITEMINFO> m_vtItemList;
    
    KSE::CString m_strFileName;
};
