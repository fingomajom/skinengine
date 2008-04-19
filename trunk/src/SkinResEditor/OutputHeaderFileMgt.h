
#pragma once

#include <atlfile.h>
#include <atlcomtime.h>

#define IDS_TEXTFORMAT  _T("_T(\"%s\")")

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
    
    BOOL WriteLine(const KSGUI::CString& strLine)
    {
        CT2AEX<> line(strLine);

        if (Write(line.m_psz, strlen(line.m_psz)) != S_OK)
            return FALSE;
        
        return Write("\r\n", 2) == S_OK;
    }

    BOOL WriteText(const KSGUI::CString& strLine)
    {
        CT2AEX<> line(strLine);

        return (Write(line.m_psz, strlen(line.m_psz)) != S_OK);

    }


    BOOL WriteLineEnd()
    {
        return Write("\r\n", 2) == S_OK;
    }

};

class OutputHeaderFileMgt
{
public:

    OutputHeaderFileMgt()
    {

    }
    

    std::map<KSGUI::CString, BOOL> mapIdName;
    std::map<KSGUI::CString, BOOL> mapItemId;


    BOOL OutputHeadFile()
    {
        SREConfigMgt config;
        KSGUI::CString strHF;

        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();
        if (!config.GetHeaderFileName( ControlsMgt.m_resDocument.GetFileName(),
            strHF) || strHF.GetLength() == 0)
        {
            if (MessageBox(GetActiveWindow(), _T("你不没有设置输入的文件名。\n您是否现在进行设置。"), _T("提示"), MB_ICONQUESTION | MB_YESNO) == IDYES)
            {
                COutputSetDlg dlg;
                dlg.DoModal();
            }

            return FALSE;
        }

        if (!ControlsMgt.m_resDocument.m_resDialogDoc.CheckValid(TRUE))
            return FALSE;

        if (!m_outfile.CreateFile(strHF))
        {
            KSGUI::CString strMsg;

            strMsg.Format(_T("不能创建文件[%s]。\n您是否现在进行重新设置。"), strHF);

            if (MessageBox(GetActiveWindow(), strMsg, _T("提示"), MB_ICONQUESTION | MB_YESNO) == IDYES)
            {
                COutputSetDlg dlg;
                dlg.DoModal();
            }

            return FALSE;
        }

        //////////////////////////////////////////////////////////////////////////

        mapIdName.clear();
        mapItemId.clear();

        if (!OutCommentHeaer())
            return FALSE;

        if (!OutStrResDef())
            return FALSE;

        if (!OutImageResDef())
            return FALSE;

        if (!OutMenuResDef())
            return FALSE;        

        if (!OutDialogListResDef())
            return FALSE;

        m_outfile.Close();

        //////////////////////////////////////////////////////////////////////////

        return TRUE;
    }
    
    BOOL OutCommentHeaer()
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        m_outfile.WriteLine(_T("// Kingsoft SkinResEditor generated include file."));
        
        CPath pathFile = ControlsMgt.m_resDocument.GetFileName();
        pathFile.RemoveFileSpec();

        KSGUI::CString strOutLine;

        strOutLine = _T("// Build From [");
        strOutLine += ControlsMgt.m_resDocument.GetFileName().Mid(
                            pathFile.m_strPath.GetLength() > 0 ?
                            pathFile.m_strPath.GetLength() + 1 : 0);
        
        strOutLine += _T("]");

        m_outfile.WriteLine( strOutLine );

        COleDateTime timeCurrent = COleDateTime::GetCurrentTime();

        strOutLine.Format(_T("// Build Time [%s]"), 
            timeCurrent.Format(_T("%Y-%m-%d %H:%M:%S")));

        m_outfile.WriteLine( strOutLine );

        TCHAR szBuffer[MAX_PATH] = { 0 };
        DWORD dwLen = MAX_PATH;

        GetUserName(szBuffer, &dwLen);

        strOutLine.Format(_T("// Build User [%s]"), szBuffer);

        m_outfile.WriteLine( strOutLine );

        return TRUE;
    }

    BOOL OutStrResDef()
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        std::vector<SkinStrResDoc::STRRESITEMINFO>& vtResList = 
            ControlsMgt.m_resDocument.m_resStrDoc.m_vtItemList;

        m_outfile.WriteLineEnd();
        m_outfile.WriteLineEnd();
        m_outfile.WriteLine(_T("//////////////////////////////////////////////////////////////////////////"));
        m_outfile.WriteLine(_T("// "));
        m_outfile.WriteLine(_T("// String define "));
        m_outfile.WriteLine(_T("// "));
        m_outfile.WriteLineEnd();

        for (size_t index = 0; index < vtResList.size(); index++)
        {
            WriteDefineLine( vtResList[index].strIDName,
                GetFormatText(vtResList[index].strIDName));
        }

        //m_outfile.WriteLineEnd();
        //m_outfile.WriteLine(_T("// String define end"));
        //m_outfile.WriteLine(_T("//////////////////////////////////////////////////////////////////////////"));

        return TRUE;
    }

    BOOL OutImageResDef()
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        std::vector<SkinImageResDoc::IMAGE_ITEMINFO>& vtResList = 
            ControlsMgt.m_resDocument.m_resImageDoc.m_vtItemList;

        m_outfile.WriteLineEnd();
        m_outfile.WriteLineEnd();
        m_outfile.WriteLine(_T("//////////////////////////////////////////////////////////////////////////"));
        m_outfile.WriteLine(_T("// "));
        m_outfile.WriteLine(_T("// Image define "));
        m_outfile.WriteLine(_T("// "));
        m_outfile.WriteLineEnd();

        for (size_t index = 0; index < vtResList.size(); index++)
        {
            WriteDefineLine( vtResList[index].strIDName,
                GetFormatText(vtResList[index].strIDName));
        }

        return TRUE;
    }

    BOOL OutMenuResDef()
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        std::vector<SkinMenuItem>& vtMenuList = 
            ControlsMgt.m_resDocument.m_resMenuDoc.m_vtMenuList;

        m_outfile.WriteLineEnd();
        m_outfile.WriteLineEnd();
        m_outfile.WriteLine(_T("//////////////////////////////////////////////////////////////////////////"));
        m_outfile.WriteLine(_T("// "));
        m_outfile.WriteLine(_T("// Menu define "));
        m_outfile.WriteLine(_T("// "));
        m_outfile.WriteLineEnd();

        for (size_t index = 0; index < vtMenuList.size(); index++)
        {
            KSGUI::CString strOutLine;

            strOutLine = _T("// Menu "); 
            strOutLine += vtMenuList[index].strIdName;

            m_outfile.WriteLineEnd();
            m_outfile.WriteLine(_T("//////////////////////////////////////////////////////////////////////////"));
            m_outfile.WriteLine(strOutLine);
            m_outfile.WriteLineEnd();

            WriteDefineLine( vtMenuList[index].strIdName,
                GetFormatText(vtMenuList[index].strIdName ));
            m_outfile.WriteLineEnd();

            OutMenuItemResDef( vtMenuList[index].m_vtChildPopupMenu );

        }

        return TRUE;
    }

    void OutMenuItemResDef( std::vector<SkinMenuItem>& vtMenuItem )
    {
        for (size_t index = 0; index < vtMenuItem.size(); index++)
        {
            BOOL bComment = FALSE;

            std::map<KSGUI::CString, BOOL>::const_iterator iter_ItemId =
                mapItemId.find(vtMenuItem[index].strIdName);
            if ( iter_ItemId == mapItemId.end() )
            {
                mapItemId[vtMenuItem[index].strIdName] = TRUE;

                bComment = FALSE;
            }
            else
            {                    
                bComment = TRUE;
            }

            WriteDefineLine( vtMenuItem[index].strIdName, vtMenuItem[index].strItemId, bComment);

            OutMenuItemResDef( vtMenuItem[index].m_vtChildPopupMenu );
        }

    }

    BOOL OutDialogListResDef()
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();
        
        std::vector<SkinDialogRes>& vtDialogList = ControlsMgt.m_resDocument.m_resDialogDoc.m_vtDialogList;

        m_outfile.WriteLineEnd();
        m_outfile.WriteLineEnd();
        m_outfile.WriteLine(_T("//////////////////////////////////////////////////////////////////////////"));
        m_outfile.WriteLine(_T("// "));
        m_outfile.WriteLine(_T("// DialogList define "));
        m_outfile.WriteLine(_T("// "));


        for (size_t i = 0; i < vtDialogList.size(); i++)
        {
            SkinDialogRes& DialogRes = vtDialogList[i];

            KSGUI::CString strOutLine;

            strOutLine = _T("// Dialog "); 
            strOutLine += DialogRes.m_dlgWndProperty.GetIdName();

            m_outfile.WriteLineEnd();
            m_outfile.WriteLine(_T("//////////////////////////////////////////////////////////////////////////"));
            m_outfile.WriteLine(strOutLine);
            m_outfile.WriteLineEnd();


            WriteDefineLine( DialogRes.m_dlgWndProperty.GetIdName(),
                GetFormatText(DialogRes.m_dlgWndProperty.GetIdName()));

            m_outfile.WriteLineEnd();

            for (size_t index = 0; index < DialogRes.m_vtChildWndList.size(); index++)
            {
                BOOL bComment = FALSE;

                std::map<KSGUI::CString, BOOL>::const_iterator iter_IdName =
                    mapIdName.find(DialogRes.m_vtChildWndList[index].GetIdName());
                if ( iter_IdName == mapIdName.end() )
                {
                    mapIdName[DialogRes.m_vtChildWndList[index].GetIdName()] = TRUE;

                    bComment = FALSE;
                }
                else
                {                    
                    bComment = TRUE;
                }

                WriteDefineLine( DialogRes.m_vtChildWndList[index].GetIdName(),
                    GetFormatText(DialogRes.m_vtChildWndList[index].GetIdName()), bComment);
            }

            m_outfile.WriteLineEnd();

            for (size_t index = 0; index < DialogRes.m_vtChildWndList.size(); index++)
            {
                KSGUI::CString strItemId;
                KSGUI::CString strIdName = DialogRes.m_vtChildWndList[index].GetIdName();
                strIdName.SetAt(2, 'C');

                DialogRes.m_vtChildWndList[index].GetProperty(_T("ItemId"), strItemId);

                BOOL bComment = FALSE;

                std::map<KSGUI::CString, BOOL>::const_iterator iter_ItemId =
                    mapItemId.find(strIdName);
                if ( iter_ItemId == mapItemId.end() )
                {
                    mapItemId[strIdName] = TRUE;

                    bComment = FALSE;
                }
                else
                {                    
                    bComment = TRUE;
                }

                WriteDefineLine( strIdName, strItemId, bComment);
            }

        }

        return TRUE;
    }

    BOOL OutDialogResDef(SkinDialogRes& DialogRes)
    {
        KSGUI::CString strOutLine;

        strOutLine = _T("// Dialog "); 
        strOutLine += DialogRes.m_dlgWndProperty.GetIdName();

        m_outfile.WriteLineEnd();
        m_outfile.WriteLine(_T("//////////////////////////////////////////////////////////////////////////"));
        m_outfile.WriteLine(strOutLine);
        m_outfile.WriteLineEnd();

        WriteDefineLine( DialogRes.m_dlgWndProperty.GetIdName(),
            GetFormatText(DialogRes.m_dlgWndProperty.GetIdName()));

        m_outfile.WriteLineEnd();

        for (size_t index = 0; index < DialogRes.m_vtChildWndList.size(); index++)
        {
            WriteDefineLine( DialogRes.m_vtChildWndList[index].GetIdName(),
                GetFormatText(DialogRes.m_vtChildWndList[index].GetIdName()));
        }

        m_outfile.WriteLineEnd();

        for (size_t index = 0; index < DialogRes.m_vtChildWndList.size(); index++)
        {
            KSGUI::CString strItemId;
            KSGUI::CString strIdName = DialogRes.m_vtChildWndList[index].GetIdName();
            strIdName.SetAt(2, 'C');
            
            DialogRes.m_vtChildWndList[index].GetProperty(_T("ItemId"), strItemId);

            WriteDefineLine( strIdName, strItemId );
        }

        return TRUE;
    }

private:

    enum
    {
        em_value_linepos = 45
    };

    KSGUI::CString GetFormatText(LPCTSTR pszText)
    {
        KSGUI::CString strResult;

        strResult.Format(IDS_TEXTFORMAT, pszText);

        return strResult;
    }

    BOOL WriteDefineLine(LPCTSTR pszDefName, LPCTSTR pszDefValue, BOOL bComment = FALSE)
    {
        KSGUI::CString strOutLine;

        strOutLine = _T("#define ");
        strOutLine += pszDefName;

        while (strOutLine.GetLength() < em_value_linepos)
        {
            strOutLine += _T(" ");
        }

        strOutLine += _T(" ");

        strOutLine += pszDefValue;

        if (bComment)
            m_outfile.WriteText(_T("//"));

        return m_outfile.WriteLine( strOutLine );
    }

    CSRETextFile m_outfile;

};