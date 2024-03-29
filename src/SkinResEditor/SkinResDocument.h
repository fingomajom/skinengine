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
#include "SkinMenuResDoc.h"
#include "SkinDialogListResDoc.h"

#define IDS_EMPTY_FILENAME _T("�ޱ���")



class SkinResDocument
{
public:

    SkinResDocument(void);
    ~SkinResDocument(void);

    BOOL NewDocument();
    BOOL OpenDocument(const KSGUI::CString& strFileName);
    BOOL OpenDocument(KSGUI::SkinXmlDocument& doc);
    BOOL SaveDocument(const KSGUI::CString& strFileName);
    BOOL SaveDocument(KSGUI::SkinXmlDocument& doc);
    
    const KSGUI::CString& GetFileName() const;

    BOOL Modify() const;
    void Modify(BOOL bModify);

    BOOL CheckValid( BOOL bShowErrorMsg = TRUE );

public:

    SkinStrResDoc   m_resStrDoc;
    SkinImageResDoc m_resImageDoc;
    SkinMenuResDoc  m_resMenuDoc;
    SkinDialogListResDoc m_resDialogDoc;

    skinconfig* m_pskinconfig;
private:

    KSGUI::CString m_strFileName;   
    BOOL m_bModify;

};
