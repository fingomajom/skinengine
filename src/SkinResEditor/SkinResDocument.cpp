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

    m_pskinconfig = new skinconfig;
    
    _Module.init_skin(NULL, m_pskinconfig);

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
    
    OpenDocument(doc);

    CPath path = strFileName;
    path.RemoveFileSpec();

    m_pskinconfig = new skinconfig;

    m_pskinconfig->m_pathSkinImagePath = path;
    m_pskinconfig->m_pathSkinResPath = path;
    m_pskinconfig->m_pathWorkDirectory = path;

    _Module.init_skin(strFileName.Mid(path.m_strPath.GetLength() ), m_pskinconfig);

    KSG::CString strOnlyFileName = path.FindFileName();

    m_strFileName = strFileName;
    m_bModify     = FALSE;

    return bResult;
}

BOOL SkinResDocument::OpenDocument(KSG::SkinXmlDocument& doc)
{
    m_resStrDoc.OpenResDoc(doc);
    m_resImageDoc.OpenResDoc(doc);
    m_resDialogDoc.OpenResDoc(doc);

    return TRUE;
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
