#include "StdAfx.h"
#include "ResDocument.h"


CResDocument::CResDocument(void)
{
    m_pxmlDocument = 0;
    m_pStringTable = 0;

    m_bChanged = FALSE;

    m_strFileName = _T("C:\\KSE.xml");

}

CResDocument::~CResDocument(void)
{
}

CResDocument& CResDocument::Instance()
{
    static CResDocument g_CResDocument_Instance;

    return g_CResDocument_Instance;
}

CXmlDocumentWrapper& CResDocument::GetDocument()    
{
    return *m_pxmlDocument;
}



BOOL CResDocument::Init( LPCTSTR pszFile )
{
    BOOL bResult = FALSE;
    
    m_pxmlDocument = new CXmlDocumentWrapper();
    m_pStringTable = new CStringTableResource();

    
    CXmlDocumentWrapper xx;

    if (m_strFileName.GetLength() > 0)
    {
        bResult = m_pxmlDocument->Load(m_strFileName);
    }
    else
    {
        LPCTSTR pszXML = _T("<?xml version=\"1.0\" encoding=\"UTF-8\"?><KSE></KSE>");

        bResult = m_pxmlDocument->LoadXML(pszXML);

    }
    
    if (!bResult)
        return bResult;

    CXmlNodeWrapper root = m_pxmlDocument->AsNode();
    if (!root.IsValid())
        return FALSE;

    KSE::CString strValue;

    root.Name(strValue);

    CXmlNodeWrapper strnode = root.FindNode(KSE::skinstrresbase::GetResKeyName());
    if (!strnode.IsValid())
    {
        strnode = root.AppendNode(KSE::skinstrresbase::GetResKeyName());
    }
    
    m_pStringTable->AttachXmlNode(strnode);
    m_pStringTable->LoadStringTableList();

    return bResult;
}

void CResDocument::Uninit( )
{
    delete m_pxmlDocument;
    delete m_pStringTable;
}

CStringTableResource* CResDocument::GetStringTableResource()
{
    return m_pStringTable;
}


void CResDocument::Save()
{
    CXmlNodeWrapper root = m_pxmlDocument->AsNode();
    if (!root.IsValid())
    {
        LPCTSTR pszXML = _T("<?xml version=\"1.0\" encoding=\"UTF-8\"?><KSE></KSE>");
        
        m_pxmlDocument->LoadXML(pszXML);
        
        root = m_pxmlDocument->AsNode();

        if (!root.IsValid())
            return;
    }

    m_pStringTable->SaveToDocument(root);

    m_pxmlDocument->Save(m_strFileName);

    m_bChanged = FALSE;

}

BOOL CResDocument::GetChanged() const
{
    return m_bChanged;
}

void CResDocument::SetChanged(BOOL bChanged)
{
    m_bChanged = bChanged;
}
