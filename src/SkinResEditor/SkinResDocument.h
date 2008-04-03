/********************************************************************
* CreatedOn: 2008-2-17   12:25
* FileName: SkinResDocument.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/
#pragma once


#include "SkinStrResDoc.h"
#include "SkinImageResDoc.h"
#include "SkinDialogListResDoc.h"

#define IDS_EMPTY_FILENAME _T("Œﬁ±ÍÃ‚")



class SkinResDocument
{
public:

    SkinResDocument(void);
    ~SkinResDocument(void);

    BOOL NewDocument();
    BOOL OpenDocument(const KSG::CString& strFileName);
    BOOL OpenDocument(KSG::SkinXmlDocument& doc);
    BOOL SaveDocument(const KSG::CString& strFileName);
    BOOL SaveDocument(KSG::SkinXmlDocument& doc);
    
    const KSG::CString& GetFileName() const;

    BOOL Modify() const;
    void Modify(BOOL bModify);

public:

    SkinStrResDoc   m_resStrDoc;
    SkinImageResDoc m_resImageDoc;
    SkinDialogListResDoc m_resDialogDoc;

    skinconfig* m_pskinconfig;
private:

    KSG::CString m_strFileName;   
    BOOL m_bModify;

};
