
#pragma once

#define IDS_CONFIG_FILENAME _T("sreconfig.xml")

class SREConfigMgt
{
public:
    
    SREConfigMgt()
    {
        TCHAR szBuffer[MAX_PATH] = { 0 };

        GetModuleFileName(NULL, szBuffer, MAX_PATH);

        m_pathFileName = szBuffer;
        m_pathFileName.RemoveFileSpec();
        m_pathFileName.Append(IDS_CONFIG_FILENAME);
    }

    BOOL GetHeaderFileName(LPCTSTR pszFileName, KSGUI::CString& strHeaderFile );
    BOOL SetHeaderFileName(LPCTSTR pszFileName, KSGUI::CString& strHeaderFile );

private:
    ATL::CPath m_pathFileName;
};


inline BOOL SREConfigMgt::GetHeaderFileName(LPCTSTR pszFileName, KSGUI::CString& strHeaderFile )
{
    SkinXmlDocument doc;

    if (!doc.LoadFile(m_pathFileName))
    {
        return FALSE;
    }

    SkinXmlElement root = doc.RootElement();

    SkinXmlElement hfconfig = root.FirstChildElement(_T("hfconfig"));
    if (!hfconfig.IsValid())
        return FALSE;

    KSGUI::CString strResFile;

    SkinXmlElement hfitem = hfconfig.FirstChildElement();
    while (hfitem.IsValid())
    {
        if (hfitem.GetValue(_T("ResFile"), strResFile) &&
            !strResFile.CompareNoCase(pszFileName))
        {
            break;
        }

        hfitem = hfitem.NextSiblingElement();
    }

    if (!hfitem.IsValid())
    {
        return FALSE;
    }

    return hfitem.GetValue(_T("HeaderFile"), strHeaderFile);
}

inline BOOL SREConfigMgt::SetHeaderFileName(LPCTSTR pszFileName, KSGUI::CString& strHeaderFile )
{
    SkinXmlDocument doc;

    LPCTSTR pszDefResXML = _T("<?xml version=\"1.0\" encoding=\"UTF-8\"?><SREConfig></SREConfig>");

    if (!doc.LoadFile(m_pathFileName))
    {
        if (!doc.LoadXML(pszDefResXML))
            return FALSE;
    }

    SkinXmlElement root = doc.RootElement();

    SkinXmlElement hfconfig = root.FirstChildElement(_T("hfconfig"));
    if (!hfconfig.IsValid())
        hfconfig = root.AppendElement(_T("hfconfig"));

    if (!hfconfig.IsValid())
        return FALSE;

    KSGUI::CString strResFile;

    SkinXmlElement hfitem = hfconfig.FirstChildElement();
    while (hfitem.IsValid())
    {
        if (hfitem.GetValue(_T("ResFile"), strResFile) &&
            !strResFile.CompareNoCase(pszFileName))
        {
            break;
        }

        hfitem = hfitem.NextSiblingElement();
    }

    if (!hfitem.IsValid())
    {
        hfitem = hfconfig.AppendElement(_T("Item"));
        if (!hfitem.IsValid())
            return FALSE;

        hfitem.SetValue(_T("ResFile"), pszFileName);
    }

    hfitem.SetValue(_T("HeaderFile"), strHeaderFile);

    if (!doc.SaveFile(m_pathFileName))
        return FALSE;

    return TRUE;
}