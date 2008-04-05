
#pragma once

#include <atlfile.h>

class CSRETextFile : public ATL::CAtlFile
{
public:

    BOOL CreateFile( LPCTSTR pszFile )
    {
        HRESULT hr = ATL::CAtlFile::Create( pszFile, 
            GENERIC_WRITE, FILE_SHARE_READ,
            CREATE_ALWAYS);

        return (hr == S_OK);
    }
    
    BOOL WriteLine(const KSG::CString& strLine)
    {
        CT2AEX<> line(strLine);

        if (Write(line.m_psz, strlen(line.m_psz)) != S_OK)
            return FALSE;
        
        return Write("\r\n", 2);
    }

};

class OutputHeaderFileMgt
{
public:

    OutputHeaderFileMgt()
    {

    }
    

    BOOL OutputHeadFile()
    {
        SREConfigMgt config;
        KSG::CString strHF;

        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();
        if (!config.GetHeaderFileName( ControlsMgt.m_resDocument.GetFileName(),
            strHF))
            return FALSE;

        if (!m_outfile.CreateFile(strHF))
            return FALSE;

        if (!OutCommentHeaer())
            return FALSE;

        return TRUE;
    }
    
    BOOL OutCommentHeaer()
    {
        m_outfile.WriteLine(_T("// Kingsoft SkinResEditor generated include file."));
        m_outfile.WriteLine(_T("// Used by SkinGUITest.rc"));
        
        
        return TRUE;
    }

private:

    CSRETextFile m_outfile;
};