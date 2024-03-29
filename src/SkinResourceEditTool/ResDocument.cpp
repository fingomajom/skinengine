#include "StdAfx.h"
#include "ResDocument.h"


CResDocument::CResDocument(void)
{
    m_pxmlDocument = 0;
    m_pStringTable = 0;

    m_bChanged = FALSE;

    m_strFileName = _T("C:\\KSG.xml");

}

CResDocument::~CResDocument(void)
{
}

CResDocument& CResDocument::Instance()
{
    static CResDocument g_CResDocument_Instance;

    return g_CResDocument_Instance;
}

SkinXmlDocument& CResDocument::GetDocument()    
{
    return *m_pxmlDocument;
}



BOOL CResDocument::Init( LPCTSTR pszFile )
{
    BOOL bResult = FALSE;
    
    m_pxmlDocument = new SkinXmlDocument();
    m_pStringTable = new CStringTableResource();
    m_pskinimageedit = new skinimageedit;
    

    if (m_strFileName.GetLength() > 0)
    {
        bResult = m_pxmlDocument->LoadFile(m_strFileName);
    }
    else
    {
        LPCTSTR pszXML = _T("<?xml version=\"1.0\" encoding=\"UTF-8\"?><KSG></KSG>");

        bResult = m_pxmlDocument->LoadXML(pszXML);

    }
    
    if (!bResult)
        return bResult;

    SkinXmlElement root = m_pxmlDocument->RootElement();
    if (!root.IsValid())
        return FALSE;

    KSG::CString strValue;

    root.Name(strValue);

    SkinXmlElement strnode = root.FirstChildElement (KSG::skinstrresbase::GetResKeyName());
    if (!strnode.IsValid())
    {
        strnode = root.AppendElement(KSG::skinstrresbase::GetResKeyName());
    }
    
    m_pStringTable->AttachXmlElement(strnode);
    m_pStringTable->LoadStringTableList();

    SkinXmlElement imagenode = root.FirstChildElement(KSG::skinimageresbase::GetResKeyName());
    if (!imagenode.IsValid())
    {
        imagenode = root.AppendElement(KSG::skinimageresbase::GetResKeyName());
    }

    m_pskinimageedit->AttachXmlElement(imagenode);
    m_pskinimageedit->LoadImageList();


    return bResult;
}

void CResDocument::Uninit( )
{
    delete m_pxmlDocument;
    delete m_pStringTable;
    delete m_pskinimageedit;
}

CStringTableResource* CResDocument::GetStringTableResource()
{
    return m_pStringTable;
}


void CResDocument::Save()
{
    SkinXmlElement root = m_pxmlDocument->RootElement();
    if (!root.IsValid())
    {
        LPCTSTR pszXML = _T("<?xml version=\"1.0\" encoding=\"UTF-8\"?><KSG></KSG>");
        
        m_pxmlDocument->LoadXML(pszXML);
        
        root = m_pxmlDocument->RootElement();

        if (!root.IsValid())
            return;
    }

    m_pStringTable->SaveToDocument(root);
    m_pskinimageedit->SaveToDocument(root);

    m_pxmlDocument->SaveFile(m_strFileName);

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
