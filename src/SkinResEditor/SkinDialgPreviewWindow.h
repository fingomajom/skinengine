/********************************************************************
* CreatedOn: 2008-4-4   12:25
* FileName: SkinDialgPreviewWindow.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#include <skindialog.h>

class SkinDialgPreviewWindow : 
    public KSG::SkinDialogImpl<SkinDialgPreviewWindow>
{
public:
    
    BEGIN_MSG_MAP(SkinDialgPreviewWindow)
    END_MSG_MAP()


    DWORD GetDefaultStyle()
    {
        return WS_CHILD | WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_DISABLED;
    }

    DWORD GetDefaultExStyle()
    {
        return WS_EX_CONTROLPARENT;
    }


    HWND ReCreatePreviewWindow(HWND hWndParent,
        SkinDialogRes& xmlDlgRes)
    {
        if (m_hWnd != NULL)
            DestroyWindow();
        
        SkinXmlDocument doc;

        if (!doc.LoadXML(_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?> <KSG />")))
            return NULL;

        xmlDlgRes.SaveResDoc(doc.RootElement());

        m_xmlDlgElement.CopyFrom(doc.RootElement().FirstChildElement());

        Create(hWndParent);

        ShowWindow(SW_SHOWNOACTIVATE);

        return m_hWnd;
    }


public:

    HWND m_hWndParent;
};