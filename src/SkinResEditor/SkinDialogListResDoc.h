
/********************************************************************
* CreatedOn: 2008-2-17   12:25
* FileName: SkinDialogListResDoc.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once


class SkinWndPropertyList
{
public:

    struct WndPropertyItem
    {
        KSGUI::CString strProperty;
        KSGUI::CString strValue;
    };
    
    BOOL NewResDoc()
    {
        BOOL bResult = TRUE;

        m_vtPropertyList.clear();

        return bResult;
    }

    BOOL AppendProperty(LPCTSTR pszProperty, LPCTSTR pszValue)
    {
        WndPropertyItem itemInfo;

        itemInfo.strProperty = pszProperty;
        itemInfo.strValue    = pszValue;

        m_vtPropertyList.push_back(itemInfo);

        return TRUE;
    }

    BOOL SetProperty(LPCTSTR pszProperty, LPCTSTR pszValue)
    {
        for (size_t i = 0; i < m_vtPropertyList.size(); i++)
        {
            if (m_vtPropertyList[i].strProperty.Compare(pszProperty) == 0)
            {
                m_vtPropertyList[i].strValue = pszValue;

                return TRUE;
            }
        }

        return TRUE;
    }
    
    BOOL SetProperty(LPCTSTR pszProperty, int nNum)
    {
        KSGUI::CString strName;
        
        strName.Format(_T("%d"), nNum);

        return SetProperty(pszProperty, strName);
    }


    BOOL SetProperty(int nIndex, LPCTSTR pszValue)
    {
        if (nIndex >= 0 && nIndex < (int)m_vtPropertyList.size())
            m_vtPropertyList[nIndex].strValue = pszValue;

        return TRUE;
    }

    BOOL GetProperty(LPCTSTR pszProperty, KSGUI::CString& strValue)
    {
        for (size_t i = 0; i < m_vtPropertyList.size(); i++)
        {
            if (m_vtPropertyList[i].strProperty.Compare(pszProperty) == 0)
            {
                strValue = m_vtPropertyList[i].strValue ;

                return TRUE;
            }
        }

        return FALSE;
    }

    BOOL DelProperty(LPCTSTR pszProperty, LPCTSTR pszValue)
    {
        for (size_t i = 0; i < m_vtPropertyList.size(); i++)
        {
            if (m_vtPropertyList[i].strProperty.Compare(pszProperty) == 0)
            {
                m_vtPropertyList.erase(m_vtPropertyList.begin() + i);

                return TRUE;
            }
        }

        return TRUE;
    }

    BOOL DelProperty(int nIndex, LPCTSTR pszValue)
    {
        if (nIndex >= 0 && nIndex < (int)m_vtPropertyList.size())
            m_vtPropertyList.erase(m_vtPropertyList.begin() + nIndex);

        return TRUE;
    }



    BOOL OpenResDoc( KSGUI::SkinXmlElement& xmlElement )
    {
        BOOL bResult = FALSE;

        NewResDoc();

        if (!xmlElement.IsValid())
            return bResult;

        xmlElement.Name(m_strIdName);

        TiXmlAttribute* xmlAttribute = xmlElement.FirstAttribute();

        while ( xmlAttribute != NULL )
        {
            WndPropertyItem itemInfo;

            itemInfo.strProperty = CA2TEX<>(xmlAttribute->Name(), CP_DEFAULT).m_psz;
            itemInfo.strValue = CA2TEX<>(xmlAttribute->Value(), CP_DEFAULT).m_psz;

            m_vtPropertyList.push_back(itemInfo);

            xmlAttribute = xmlAttribute->Next();
        }

        bResult = TRUE;

        return bResult;
    }

    KSGUI::SkinXmlElement SaveResDoc( KSGUI::SkinXmlElement& xmlParentElement )
    {
        BOOL bResult = FALSE;
        
        if (!xmlParentElement.IsValid())
            return KSGUI::SkinXmlElement();

        KSGUI::SkinXmlElement xmlElement = 
            xmlParentElement.AppendElement(m_strIdName);

        if (!xmlElement.IsValid())
            return KSGUI::SkinXmlElement();

        for (size_t i = 0; i < m_vtPropertyList.size(); i++)
        {
            xmlElement.SetValue( 
                m_vtPropertyList[i].strProperty,
                m_vtPropertyList[i].strValue);
        }

        return xmlElement;
    }

    KSGUI::CString& GetIdName()
    {
        return m_strIdName;
    }



public:
    
    std::vector<WndPropertyItem> m_vtPropertyList;
    KSGUI::CString m_strIdName;
};

class SkinDialogRes
{
public:

    BOOL NewResDoc()
    {
        BOOL bResult = TRUE;

        m_vtChildWndList.clear();
        m_dlgWndProperty.NewResDoc();

        return bResult;
    }

    BOOL OpenResDoc( KSGUI::SkinXmlElement& xmlElement )
    {
        BOOL bResult = FALSE;

        NewResDoc();

        if (!xmlElement.IsValid())
            return bResult;

        
        if (!m_dlgWndProperty.OpenResDoc(xmlElement))
            return FALSE;

        KSGUI::SkinXmlElement strnode = 
            xmlElement.FirstChildElement(KSGUI::skinxmldialog::GetChildWinKeyName());

        if (!strnode.IsValid())
            return bResult;

        KSGUI::SkinXmlElement node = strnode.FirstChildElement();

        while ( node.IsValid() )
        {
            SkinWndPropertyList wndPropertyList;

            if (wndPropertyList.OpenResDoc(node))
            {
                m_vtChildWndList.push_back(wndPropertyList);
            }

            node = node.NextSiblingElement();
        }

        bResult = TRUE;

        return bResult;
    }

    BOOL SaveResDoc(KSGUI::SkinXmlElement& xmlParentElement)
    {
        BOOL bResult = FALSE;

        KSGUI::SkinXmlElement xmlElement = 
            m_dlgWndProperty.SaveResDoc(xmlParentElement);
        if (!xmlElement.IsValid())
            return bResult;

        KSGUI::SkinXmlElement strnode = 
            xmlElement.AppendElement(KSGUI::skinxmldialog::GetChildWinKeyName());

        for (size_t i = 0; i < m_vtChildWndList.size(); i++)
        {
            m_vtChildWndList[i].SaveResDoc(strnode);
        }

        bResult = TRUE;

        return bResult;
    }

    BOOL IsChildIdNameExists( LPCTSTR pszIdName )
    {
        for (size_t i = 0; i < m_vtChildWndList.size(); i++)
        {
            if (!m_vtChildWndList[i].GetIdName().CompareNoCase(pszIdName))
                return TRUE;
        }

        return FALSE;
    }

public:

    std::vector<SkinWndPropertyList> m_vtChildWndList;
    SkinWndPropertyList m_dlgWndProperty;
};



class SkinDialogListResDoc
{
public:

    SkinDialogListResDoc()
    {
    }

    ~SkinDialogListResDoc(void)
    {
    }

    BOOL NewResDoc()
    {
        BOOL bResult = TRUE;
        
        m_vtDialogList.clear();

        return bResult;
    }

    BOOL OpenResDoc(KSGUI::SkinXmlDocument& doc)
    {
        BOOL bResult = FALSE;

        NewResDoc();

        KSGUI::SkinXmlElement root = doc.RootElement();

        if (!root.IsValid())
            return bResult;

        KSGUI::SkinXmlElement strnode = 
            root.FirstChildElement(KSGUI::skindlgres::GetResKeyName());

        if (!strnode.IsValid())
            return bResult;

        KSGUI::SkinXmlElement node = strnode.FirstChildElement();

        while ( node.IsValid() )
        {
            SkinDialogRes wndDlgRes;

            if (wndDlgRes.OpenResDoc(node))
            {
                m_vtDialogList.push_back(wndDlgRes);
            }


            node = node.NextSiblingElement();
        }

        bResult = TRUE;

        return bResult;
    }

    BOOL SaveResDoc(KSGUI::SkinXmlDocument& doc)
    {
        BOOL bResult = FALSE;

        KSGUI::SkinXmlElement root = doc.RootElement();
        if (!root.IsValid())
            return bResult;

        KSGUI::SkinXmlElement strnode = 
            root.FirstChildElement(KSGUI::skindlgres::GetResKeyName());

        if (strnode.IsValid())
        {
            root.RemoveElement(strnode);
        }

        strnode = root.AppendElement(KSGUI::skindlgres::GetResKeyName());

        for (size_t i = 0; i < m_vtDialogList.size(); i++)
        {
            m_vtDialogList[i].SaveResDoc(strnode);
        }

        bResult = TRUE;

        return bResult;
    }

    BOOL IsChildIdNameExists( LPCTSTR pszIdName )
    {
        for (size_t i = 0; i < m_vtDialogList.size(); i++)
        {
            if (m_vtDialogList[i].IsChildIdNameExists(pszIdName))
                return TRUE;
        }

        return FALSE;
    }

    BOOL CheckValid( BOOL bShowErrorMsg = TRUE )
    {
        typedef struct {
            KSGUI::CString strItemId;
            KSGUI::CString strDialogName;
        } Valid_Info;
        
        std::map<KSGUI::CString, Valid_Info> mapIdName;
        std::map<KSGUI::CString, BOOL> mapDlgName;
        
        for (size_t idx = 0; idx < m_vtDialogList.size(); idx++)
        {
            std::vector<SkinWndPropertyList>& vtChildWndList = m_vtDialogList[idx].m_vtChildWndList;

            KSGUI::CString strDialogName = m_vtDialogList[idx].m_dlgWndProperty.GetIdName();

            std::map<KSGUI::CString, BOOL>::const_iterator iter_dlgName =
                mapDlgName.find(strDialogName);
            if ( iter_dlgName == mapDlgName.end() )
            {
                mapDlgName[strDialogName] = TRUE;
            }
            else if (bShowErrorMsg)
            {
                // error 出现重复的IDD 对话框名

                KSGUI::CString strMsg;

                strMsg.Format(_T("对话框[%s]重名。请检查"), strDialogName);

                MessageBox(GetActiveWindow(), strMsg, _T("错误"), MB_ICONQUESTION | MB_OK);

                return FALSE;
            }


            std::map<KSGUI::CString, BOOL> mapIdNameExists;
            std::map<KSGUI::CString, KSGUI::CString> mapItemIdExists;

            for (size_t idv = 0; idv < vtChildWndList.size(); idv++)
            {
                KSGUI::CString strIdName;
                KSGUI::CString strItemId;

                strIdName = vtChildWndList[idv].GetIdName();

                if ( _tcslen(strIdName) <= _tcslen(_T("IDN_")) ||
                    _tcsncmp(strIdName, _T("IDN_"), _tcslen(_T("IDN_")) ) ) // 不合法的项名
                {
                    KSGUI::CString strMsg;

                    strMsg.Format(
                        _T("对话框[%s]中控件名[%s]不合法，\n控件名必顺以 IDN_ 开头的字符串。"),
                        strDialogName, strIdName);

                    MessageBox(GetActiveWindow(), strMsg, _T("错误"), MB_ICONQUESTION | MB_OK);

                    return FALSE;
                }

                
                std::map<KSGUI::CString, BOOL>::const_iterator iter_IdName =
                    mapIdNameExists.find(strIdName);
                if ( iter_IdName == mapIdNameExists.end() )
                {
                    mapIdNameExists[strIdName] = TRUE;
                }
                else if (bShowErrorMsg)
                {
                    // error  同一个对话框中出现相同的 控件名

                    KSGUI::CString strMsg;

                    strMsg.Format(_T("对话框[%s]中。出现相同的控件名[%s]"), strDialogName, strIdName);

                    MessageBox(GetActiveWindow(), strMsg, _T("错误"), MB_ICONQUESTION | MB_OK);

                    return FALSE;
                }


                if (!vtChildWndList[idv].GetProperty(_T("ItemId"), strItemId))
                    continue;

                std::map<KSGUI::CString, KSGUI::CString>::const_iterator iter_ItemId =
                    mapItemIdExists.find(strItemId);
                if ( iter_ItemId == mapItemIdExists.end() )
                {
                    mapItemIdExists[strItemId] = strIdName;
                }
                else if (bShowErrorMsg)
                {                    
                    // error 同一个对话框中出现相同的 ID 名

                    KSGUI::CString strMsg;

                    strMsg.Format(_T("对话框[%s]中。控件[%s]与控件[%s]有相同的ItemId=[%s]"), 
                        strDialogName, iter_ItemId->second , strIdName , strItemId);

                    MessageBox(GetActiveWindow(), strMsg, _T("错误"), MB_ICONQUESTION | MB_OK);

                    return FALSE;
                }

                std::map<KSGUI::CString, Valid_Info>::const_iterator iter_vaildinfo =
                    mapIdName.find(strIdName);

                if ( iter_vaildinfo == mapIdName.end() )
                {
                    Valid_Info info;

                    info.strDialogName = strDialogName;
                    info.strItemId     = strItemId;

                    mapIdName[strIdName] = info;
                }
                else if (bShowErrorMsg && strItemId != iter_vaildinfo->second.strItemId)
                {
                    // error 不同对话框间出现相同IDName但是ItemId不同

                    KSGUI::CString strMsg;

                    strMsg.Format(_T("对话框[%s]中的控件[%s]与\n对话框[%s]中的控件[%s]\n控件名相同但是ItemId不相同[%s!=%s]。"), 
                        strDialogName, strIdName , iter_vaildinfo->second.strDialogName, strIdName, strItemId, iter_vaildinfo->second.strItemId);

                    MessageBox(GetActiveWindow(), strMsg, _T("错误"), MB_ICONQUESTION | MB_OK);

                    return FALSE;
                }
            }
        }

        return TRUE;
    }

public:

    std::vector<SkinDialogRes> m_vtDialogList;

};