#include "StdAfx.h"
#include "SkinResDocument.h"

SkinResDocument::SkinResDocument(void)
{
    m_strFileName = IDS_EMPTY_FILENAME;   
    m_bModify     = FALSE;
}

SkinResDocument::~SkinResDocument(void)
{
}

BOOL SkinResDocument::NewDocument()
{
    BOOL bResult = FALSE;

    bResult = m_resStrDoc.NewResDoc();
    if (!bResult)
        return bResult;

    bResult = m_resImageDoc.NewResDoc();
    if (!bResult)
        return bResult;

    bResult = m_resDialogDoc.NewResDoc();
    if (!bResult)
        return bResult;

    
    m_strFileName = IDS_EMPTY_FILENAME;   
    m_bModify     = FALSE;

    return bResult;
}

BOOL SkinResDocument::OpenDocument(const KSG::CString& strFileName)
{
    BOOL bResult = FALSE;

    KSG::SkinXmlDocument doc;

    bResult = doc.LoadFile(strFileName);
    if (!bResult)
        return bResult;

    m_resStrDoc.OpenResDoc(doc);
    m_resImageDoc.OpenResDoc(doc);
    m_resDialogDoc.OpenResDoc(doc);

    m_strFileName = strFileName;
    m_bModify     = FALSE;


    return bResult;
}

BOOL SkinResDocument::SaveDocument(const KSG::CString& strFileName)
{
    BOOL bResult = FALSE;

    KSG::SkinXmlDocument doc;
    
    bResult = SaveDocument(doc);
    if (!bResult)
        return bResult;

    m_strFileName = strFileName;
    m_bModify     = FALSE;

    doc.SaveFile(strFileName);

    return bResult;
}

BOOL SkinResDocument::SaveDocument(KSG::SkinXmlDocument& doc)
{
    BOOL bResult = FALSE;

    LPCTSTR pszDefResXML = _T("<?xml version=\"1.0\" encoding=\"UTF-8\"?><KSG></KSG>");

    bResult = doc.LoadXML(pszDefResXML);
    if (!bResult)
        return bResult;

    m_resStrDoc.SaveResDoc(doc);
    m_resImageDoc.SaveResDoc(doc);
    m_resDialogDoc.SaveResDoc(doc);

    return bResult;
}


const KSG::CString& SkinResDocument::GetFileName() const
{
    return m_strFileName;
}

BOOL SkinResDocument::Modify() const
{
    return m_bModify;
}

void SkinResDocument::Modify(BOOL bModify)
{
    m_bModify = bModify;
}
