/********************************************************************
* CreatedOn: 2007-12-19   17:40
* FileName: ImageResEditDlg.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include "resource.h"

#include <atlframe.h>
#include <atlpath.h>
#include <skingdi.h>
#include "EditResBase.h"


enum {
    em_imagelist_height = 60
};

class CImageListViewCtrl : 
    public CWindowImpl<CImageListViewCtrl, CListBox>,
    public COwnerDraw<CImageListViewCtrl>
{
public:

    enum{
        em_image_unknown = 0,
        em_image_bitmap,
        em_image_icon,
        em_image_cursor,
        em_image_new
    };

    typedef struct image_item_info
    {
        int ntype;
        KSE::CString strIdName;
        KSE::CString strFileName;
    };

    std::vector<image_item_info> m_vtItems;

public:
    
    BEGIN_MSG_MAP(CImageResEditDlg)
        MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
        CHAIN_MSG_MAP_ALT(COwnerDraw<CImageListViewCtrl>, 1)
    END_MSG_MAP()


    LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if (wParam == VK_DELETE)
        {
            int nIndex = GetCurSel();

            if (nIndex >= 0 && nIndex < GetCount() - 1)
            {
                DeleteString(nIndex);
                m_vtItems.erase( m_vtItems.begin() + nIndex );

                CResDocument& document = CResDocument::Instance();
                document.SetChanged();
            }    
        }

        return DefWindowProc();
    }

    void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
    {
        CDCHandle dc = lpDrawItemStruct->hDC;

        RECT rcItem = lpDrawItemStruct->rcItem;
        int  nItem  = lpDrawItemStruct->itemID;

        bool bSelected = (lpDrawItemStruct->itemState & ODS_SELECTED) != 0;
        bool bFocus    = GetFocus() == m_hWnd;

        COLORREF clrBkGnd = 
            bSelected ? 
            (bFocus ? GetSysColor(COLOR_HIGHLIGHT) : GetSysColor( COLOR_BTNFACE) ) : 
            GetSysColor(COLOR_WINDOW);

        CBrush   brushBkgnd;
        TCHAR    szBuffer[MAX_PATH] = { 0 };

        brushBkgnd.CreateSolidBrush( clrBkGnd );
        dc.FillRect(&rcItem, brushBkgnd);

        COLORREF clrTextColor = 
            bSelected ? 
            (bFocus ? GetSysColor(COLOR_HIGHLIGHTTEXT) : GetSysColor( COLOR_BTNTEXT) ) : 
            GetSysColor(COLOR_BTNTEXT);

        
        const image_item_info& itemInfo = m_vtItems[nItem];

        KSE::CString strDisplayName;
        KSE::CString strFileName;

        if (itemInfo.ntype == em_image_new)
        {
            strDisplayName = _T("New...");
        }
        else
        {
            strDisplayName = itemInfo.strIdName;
            strFileName    = itemInfo.strFileName;
        }

        RECT rcIdName   = rcItem;
        RECT rcFileName = rcItem;
        RECT rcImage    = rcItem;

        rcIdName.left += 10;

        rcIdName.right = rcFileName.left = 220;
        rcFileName.right = rcImage.left = 440;

        dc.SetTextColor(clrTextColor);
        dc.SetBkMode(0);

        CFont font;
        CFont sfont;

        font.CreatePointFont(128,  _T("Courier New"), dc, true);
        sfont.CreatePointFont(96 , _T("Courier New"), dc);

        HFONT hOldFont = dc.SelectFont(font);

        dc.DrawText(strDisplayName, -1 , &rcIdName  , DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        dc.DrawText(strFileName   , -1 , &rcFileName, DT_LEFT | DT_VCENTER | DT_SINGLELINE );
        
        dc.SelectFont(sfont);

        strDisplayName.Format(_T("Index(%d)"), nItem);

        dc.DrawText(strDisplayName, -1 , &rcIdName  , DT_LEFT );
        //dc.DrawText(strFileName   , -1 , &rcFileName, DT_LEFT );

        dc.SelectFont(hOldFont);
        
        DrawImage(dc, &rcImage, itemInfo);
    }

    void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
    {
        if(lpMeasureItemStruct->CtlType != ODT_MENU)
        {
            lpMeasureItemStruct->itemHeight = em_imagelist_height;
        }
        else
            lpMeasureItemStruct->itemHeight = ::GetSystemMetrics(SM_CYMENU);
    }

    void CalcDrawRect( LPRECT lprect,  LPRECT lpDrawrect, int nWidth, int nHeight, int & bBig)
    {
        *lpDrawrect = *lprect;

        bBig = FALSE;

        //if (nWidth < lprect->right - lprect->left)
        //{
        //    lpDrawrect->left  = lprect->left + (lprect->right - lprect->left - nWidth) / 2;
        //    lpDrawrect->right = lpDrawrect->left + nWidth;
        //}
        //else
        //    bBig = TRUE;

        if (nHeight < lprect->bottom - lprect->top)
        {
            lpDrawrect->top  = lprect->top + (lprect->bottom - lprect->top - nHeight) / 2;
            lpDrawrect->bottom = lpDrawrect->top + nHeight;
        }
        else
            bBig = TRUE;

    }

    void DrawImage(CDCHandle dc, LPRECT lprect, const image_item_info& itemInfo )
    {
        CResDocument& document = CResDocument::Instance();

        CSkinDCHandle skindc = dc.m_hDC;

        RECT rcdraw = { 0 };
        int  bbig   = false;
        
        switch( itemInfo.ntype )
        {
        case em_image_bitmap:
            {
                CBitmap bmp;

                bmp.m_hBitmap = document.getskinimageedit()->LoadBitmap(itemInfo.strIdName);
                if (bmp.m_hBitmap == NULL)
                    break;
                
                BITMAP bm = { 0 };

                if (!bmp.GetBitmap(&bm))
                    return;

                CalcDrawRect(lprect, &rcdraw, bm.bmWidth, bm.bmHeight, bbig);
                
                POINT pt;
                pt.x = rcdraw.left;
                pt.y = rcdraw.top;

                if (bbig)
                {
                    skindc.SkinDrawBitmap(pt, lprect->right - lprect->left, 
                        lprect->bottom - lprect->top, bmp);
                }
                else
                {

                    skindc.SkinDrawBitmap(pt, bmp);
                }
                
            }
            break;
        case em_image_icon:
            {
                CIcon icon = (HICON)document.getskinimageedit()->LoadImage(itemInfo.strIdName,
                    IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTCOLOR | LR_DEFAULTSIZE);

                if (icon.m_hIcon != NULL)
                    icon.DrawIcon( dc, lprect->left, lprect->top );

            }
            break;
        case em_image_cursor:
            {
                CIcon icon = (HICON)document.getskinimageedit()->LoadImage(itemInfo.strIdName,
                    IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTCOLOR | LR_DEFAULTSIZE);

                if (icon.m_hIcon != NULL)
                    icon.DrawIcon( dc, lprect->left, lprect->top );

                //CCursor cursor = (HCURSOR)document.getskinimageedit().LoadImage(itemInfo.strIdName,
                //    IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTCOLOR | LR_DEFAULTSIZE);
                //

                //if (cursor.m_hCursor != NULL)
                //{
                //    CURSORINFO cursorinfo = { 0 };

                //    cursor.GetCursorInfo( &cursorinfo );

                //    icon.DrawIcon( dc, lprect->left, lprect->top );
                //}

            }

            break;
        case em_image_unknown:
        case em_image_new:
        default:
            break;
        }

    }


    BOOL AppendImage(const KSE::CString& strIdName,
        const KSE::CString& strFileName)
    {
        image_item_info item;
        
        item.ntype = GetType(strFileName);

        item.strIdName   = strIdName;
        item.strFileName = strFileName;

        m_vtItems.push_back(item);
        AddString(strIdName);        

        return TRUE;
    }

    BOOL AppendImageBefNew(const KSE::CString& strIdName,
        const KSE::CString& strFileName)
    {
        image_item_info item;

        item.ntype = GetType(strFileName);

        item.strIdName   = strIdName;
        item.strFileName = strFileName;

        int nIndex = GetCount() - 1;

        m_vtItems.insert(m_vtItems.begin() + nIndex, item);
        InsertString(nIndex, strIdName);        

        return TRUE;
    }


    BOOL SetImage(int nIndex, 
        const KSE::CString& strIdName,
        const KSE::CString& strFileName)
    {
        for (int i = 0; i < GetCount(); i++)
        {
            if (i == nIndex)
                continue;

            if (strIdName.CompareNoCase(m_vtItems[i].strIdName) == 0)
            {
                SetCurSel(i);

                return TRUE;
            }
        }

        m_vtItems[nIndex].ntype       = GetType(strFileName);
        m_vtItems[nIndex].strIdName   = strIdName;
        m_vtItems[nIndex].strFileName = strFileName;
   
        if (nIndex == GetCount() - 1)
        {
            AppendImage(_T(""), _T(""));

        }
        
        CResDocument& document = CResDocument::Instance();

        document.getskinimageedit()->SaveImageFile(strIdName, strFileName);


        return TRUE;
    }

    
    int GetType(const KSE::CString& strFileName)
    {
        int nresult = em_image_unknown;

        if (strFileName.GetLength() <= 0)
            return em_image_new;

        CPath path;

        path.m_strPath = strFileName;
        ATL::CString strExtension = path.GetExtension();

        if (strExtension.CompareNoCase(_T(".bmp")) == 0)
        {
            nresult = em_image_bitmap;
        }
        else if (strExtension.CompareNoCase(_T(".ico")) == 0)
        {
            nresult = em_image_icon;
        }
        else if (strExtension.CompareNoCase(_T(".cur")) == 0)
        {
            nresult = em_image_cursor;
        }

        return nresult;
    }


    BOOL ClearImage()
    {
        while (GetCount()  > 0)
        {
            DeleteString(0);
        }

        m_vtItems.clear();
        
        return TRUE;
    }
};

class CMyFileDialog : public CFileDialogImpl<CMyFileDialog>
{
public:
    CMyFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
        LPCTSTR lpszDefExt = NULL,
        LPCTSTR lpszFileName = NULL,
        DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        LPCTSTR lpszFilter = NULL,
        HWND hWndParent = NULL)
        : CFileDialogImpl<CMyFileDialog>(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, hWndParent)
    { }

    LPCTSTR GetStartPosition() const
    { return (LPCTSTR)m_ofn.lpstrFile; }


    WTL::CString GetNextPathName(LPCTSTR& pos) const
    {
        BOOL bExplorer = m_ofn.Flags & OFN_EXPLORER;
        TCHAR chDelimiter;
        if (bExplorer)
            chDelimiter = '\0';
        else
            chDelimiter = ' ';

        LPTSTR lpsz = (LPTSTR)pos;
        if (lpsz == m_ofn.lpstrFile) // first time
        {
            if ((m_ofn.Flags & OFN_ALLOWMULTISELECT) == 0)
            {
                pos = NULL;
                return m_ofn.lpstrFile;
            }

            // find char pos after first Delimiter
            while(*lpsz != chDelimiter && *lpsz != '\0')
                lpsz = _tcsinc(lpsz);
            lpsz = _tcsinc(lpsz);

            // if single selection then return only selection
            if (*lpsz == 0)
            {
                pos = NULL;
                return m_ofn.lpstrFile;
            }
        }

        WTL::CString strBasePath = m_ofn.lpstrFile;
        if (!bExplorer)
        {
            LPTSTR lpszPath = m_ofn.lpstrFile;
            while(*lpszPath != chDelimiter)
                lpszPath = _tcsinc(lpszPath);
            strBasePath = strBasePath.Left(int(lpszPath - m_ofn.lpstrFile));
        }

        LPTSTR lpszFileName = lpsz;
        WTL::CString strFileName = lpsz;

        // find char pos at next Delimiter
        while(*lpsz != chDelimiter && *lpsz != '\0')
            lpsz = _tcsinc(lpsz);

        if (!bExplorer && *lpsz == '\0')
            pos = NULL;
        else
        {
            if (!bExplorer)
                strFileName = strFileName.Left(int(lpsz - lpszFileName));

            lpsz = _tcsinc(lpsz);
            if (*lpsz == '\0') // if double terminated then done
                pos = NULL;
            else
                pos = (LPCTSTR)lpsz;
        }

        TCHAR strDrive[_MAX_DRIVE], strDir[_MAX_DIR], strName[_MAX_FNAME], strExt[_MAX_EXT];
        Checked::tsplitpath_s(strFileName, strDrive, _MAX_DRIVE, strDir, _MAX_DIR, strName, _MAX_FNAME, strExt, _MAX_EXT);
        TCHAR strPath[_MAX_PATH];
        if (*strDrive || *strDir)
        {
            Checked::tcscpy_s(strPath, _countof(strPath), strFileName);
        }
        else
        {
            Checked::tsplitpath_s(strBasePath+_T("\\"), strDrive, _MAX_DRIVE, strDir, _MAX_DIR, NULL, 0, NULL, 0);
            Checked::tmakepath_s(strPath, _MAX_PATH, strDrive, strDir, strName, strExt);
        }

        return strPath;
    }

    // override base class map and references to handlers
    DECLARE_EMPTY_MSG_MAP()
};



class CImageResEditDlg : 
    public CDialogImpl<CImageResEditDlg>,
    public CEditResBase
{
public:
    enum { IDD = IDD_IMAGE_EDIT_DIALOG };

    BEGIN_MSG_MAP(CImageResEditDlg)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_SIZE      , OnSize)
        COMMAND_HANDLER(IDC_BROWSE_BUTTON, BN_CLICKED, OnBnClickedBrowseButton)
        COMMAND_HANDLER(IDC_ADD_BUTTON, BN_CLICKED, OnBnClickedAddButton)
        COMMAND_ID_HANDLER(IDOK, OnOkCmd)


        COMMAND_HANDLER(IDC_ID_EDIT, EN_CHANGE, OnEnChangeIdEdit)
        COMMAND_HANDLER(IDC_FILE_EDIT, EN_CHANGE, OnEnChangeIdEdit)
        COMMAND_HANDLER(IDC_IMAGE_LIST, LBN_SELCHANGE, OnLbnSelchangeImageList)
        COMMAND_HANDLER(IDC_IMAGE_LIST, LBN_DBLCLK, OnLbnDblclkImageList)

        REFLECT_NOTIFICATIONS()

    END_MSG_MAP()

    // Handler prototypes (uncomment arguments if needed):
    //	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    //	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    //	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

    CImageListViewCtrl m_wndImageList;


    LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        RECT rcClient = { 0 };

        GetClientRect(&rcClient);

        RECT rcListbox = rcClient;

        rcClient.bottom -= 60;

        m_wndImageList.MoveWindow(&rcClient);

        RECT rcIdName   = { 10, rcClient.bottom + 4, rcListbox.right - 150 , rcClient.bottom + 24 } ;
        RECT rcFileName = { 10, rcClient.bottom + 28, rcListbox.right - 150,  rcClient.bottom + 48 } ;;

        RECT rcAddBtn     = rcIdName;
        RECT rcBrowseBtn  = rcFileName;
        RECT rcOkBtn      = rcFileName;

        rcAddBtn.left = rcAddBtn.right + 10;
        rcAddBtn.right = rcAddBtn.left + 60;
            
        rcBrowseBtn.left = rcAddBtn.left;
        rcBrowseBtn.right = rcAddBtn.right;

        rcOkBtn.left = rcBrowseBtn.right + 10;
        rcOkBtn.right = rcOkBtn.left + 60;

        
        GetDlgItem(IDC_ID_EDIT).MoveWindow(&rcIdName);
        GetDlgItem(IDC_FILE_EDIT).MoveWindow(&rcFileName);
        GetDlgItem(IDC_ADD_BUTTON).MoveWindow(&rcAddBtn);
        GetDlgItem(IDC_BROWSE_BUTTON).MoveWindow(&rcBrowseBtn);
        GetDlgItem(IDOK).MoveWindow(&rcOkBtn);


        return TRUE;
    }

    virtual BOOL LoadResFromDocumnet()
    {
        if (m_hWnd == NULL)
            return FALSE;

        CResDocument& document = CResDocument::Instance();

        std::vector<skinimageedit::IMAGE_ITEMINFO>& vtList =
            document.getskinimageedit()->GetImageList();

        for (size_t i = 0; i < vtList.size(); i++)
        {
            m_wndImageList.AppendImage(vtList[i].strIDName, vtList[i].strFileName);

        }

        m_wndImageList.AppendImage(_T(""), _T(""));

        return TRUE;
    }


    virtual BOOL SaveResToDocument()
    {
        if (m_hWnd == NULL)
            return FALSE;

        CResDocument& document = CResDocument::Instance();

        std::vector<skinimageedit::IMAGE_ITEMINFO>& vtList =
            document.getskinimageedit()->GetImageList();

        vtList.resize( m_wndImageList.m_vtItems.size() - 1 );

        for (size_t i = 0; i <  m_wndImageList.m_vtItems.size() - 1; i++)
        {
            vtList[i].strIDName   = m_wndImageList.m_vtItems[i].strIdName;
            vtList[i].strFileName = m_wndImageList.m_vtItems[i].strFileName;
        }

        return TRUE;
    }


    virtual BOOL ClearRes()
    {
        if (m_hWnd == NULL)
            return FALSE;

        m_wndImageList.ClearImage();


        m_wndImageList.AppendImage(_T(""), _T(""));
    
        CResDocument& document = CResDocument::Instance();
        document.SetChanged();

        return TRUE;

    }

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        m_wndImageList.SubclassWindow(GetDlgItem(IDC_IMAGE_LIST));

        m_wndImageList.SetItemHeight(0, em_imagelist_height);

        CenterWindow(GetParent());
        return TRUE;
    }

    LRESULT OnOkCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        int nindex = m_wndImageList.GetCurSel();
        if (nindex < 0)
            return 0;

        TCHAR szIdBuffer[4096] = { 0 };
        TCHAR szFileBuffer[4096] = { 0 };

        GetDlgItemText( IDC_ID_EDIT, szIdBuffer, 4096 );   
        GetDlgItemText( IDC_FILE_EDIT, szFileBuffer, 4096 ); 

        m_wndImageList.SetImage(nindex, szIdBuffer, szFileBuffer);
        
        CResDocument& document = CResDocument::Instance();
        document.SetChanged();

        return 0;
    }

    LRESULT OnBnClickedBrowseButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CFileDialog openDlg(TRUE, _T(""), _T(""),
            OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
            _T("Bitmap file(*.bmp)\0*.bmp\0Icon file(*.ico)\0*.ico\0Cursor file(*.cur)\0*.cur\0所有文件(*.*)\0*.*\0\0"));

        openDlg.m_ofn.lpstrTitle = (_T("打开文件"));

        if (IDOK == openDlg.DoModal())
        {
            CPath path = openDlg.m_ofn.lpstrFile;

            KSE::CString strFileName = path.m_strPath.Mid(path.FindFileName());

            CResDocument& document = CResDocument::Instance();

            skinconfigbase* pskinconfig = document.getskinimageedit()->m_pskinconfig;

            if (pskinconfig != 0)
            {
                KSE::CString strImagePath = pskinconfig->GetSkinImagePath();
                
                path.RemoveFileSpec();

                if ( strImagePath.CompareNoCase(path.m_strPath) != 0)
                {
                    CPath pathCopyTo = strImagePath;
                    pathCopyTo.Append(strFileName);

                    CopyFile(openDlg.m_ofn.lpstrFile, pathCopyTo.m_strPath, FALSE);
                }

            }

            return 0;
        }

        return 0;

    }

    LRESULT OnBnClickedAddButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {


        CMyFileDialog openDlg(TRUE, _T(""), _T(""),
            OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT, 
            _T("Bitmap file(*.bmp)\0*.bmp\0Icon file(*.ico)\0*.ico\0Cursor file(*.cur)\0*.cur\0所有文件(*.*)\0*.*\0\0"));

        openDlg.m_ofn.lpstrTitle = (_T("打开文件"));

        TCHAR szBuffer[4096] = { 0 };

        openDlg.m_ofn.lpstrFile = szBuffer;

        if (IDOK == openDlg.DoModal())
        {
            LPCTSTR pos = openDlg.GetStartPosition();

            CResDocument& document = CResDocument::Instance();

            while (pos)
            {
                WTL::CString strPathFile = openDlg.GetNextPathName(pos);

                CPath path = strPathFile;

                KSE::CString strFileName = path.m_strPath.Mid(path.FindFileName());
                KSE::CString strIdName   = path.m_strPath.Mid(path.FindFileName());

                strIdName = "IDB_";

                strIdName += (LPCTSTR)strFileName.Left( strFileName.GetLength() - path.GetExtension().GetLength() );


                skinconfigbase* pskinconfig = document.getskinimageedit()->m_pskinconfig;

                if (pskinconfig != 0)
                {
                    KSE::CString strImagePath = pskinconfig->GetSkinImagePath();


                    path.RemoveFileSpec();

                    if ( strImagePath.CompareNoCase(path.m_strPath) != 0)
                    {
                        CPath pathCopyTo = strImagePath;
                        pathCopyTo.Append(strFileName);

                        CopyFile(strPathFile, pathCopyTo.m_strPath, FALSE);
                    }

                }

                m_wndImageList.AppendImageBefNew(strIdName, strFileName );

                document.SetChanged();

            }

            return 0;
        }

        return 0;

    }    

    LRESULT CImageResEditDlg::OnEnChangeIdEdit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        TCHAR szBuffer[4096] = { 0 };

        GetDlgItemText( IDC_ID_EDIT, szBuffer, 4096 );

        if (_tcslen(szBuffer) == 0)
        {
            ::EnableWindow(GetDlgItem(IDOK), FALSE);
            return TRUE;
        }
        if (szBuffer[0] >= '0' && szBuffer[0] <= '9')
        {
            ::EnableWindow(GetDlgItem(IDOK), FALSE);
            return TRUE;
        }

        GetDlgItemText( IDC_FILE_EDIT, szBuffer, 4096 );
        if (_tcslen(szBuffer) == 0)
        {
            ::EnableWindow(GetDlgItem(IDOK), FALSE);
            return TRUE;
        }

        ::EnableWindow(GetDlgItem(IDOK), TRUE);

        return 0;
    }

    LRESULT CImageResEditDlg::OnLbnSelchangeImageList(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        int nindex = m_wndImageList.GetCurSel();
        if (nindex < 0)
            return 0;


        SetDlgItemText( IDC_ID_EDIT  , m_wndImageList.m_vtItems[nindex].strIdName ); 
        SetDlgItemText( IDC_FILE_EDIT, m_wndImageList.m_vtItems[nindex].strFileName ); 

        return 0;
    }
    LRESULT CImageResEditDlg::OnLbnDblclkImageList(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        int nindex = m_wndImageList.GetCurSel();
        if (nindex < 0)
            return 0;

        CResDocument& document = CResDocument::Instance();

        skinconfigbase* pskinconfig = document.getskinimageedit()->m_pskinconfig;

        if (pskinconfig != 0)
        {
            CPath path;
            path.m_strPath = pskinconfig->GetSkinImagePath();

            path.Append(m_wndImageList.m_vtItems[nindex].strFileName);

            ShellExecute(m_hWnd, _T("open"), path.m_strPath, NULL, NULL, SW_SHOWDEFAULT);

        }

        return 0;
    }
};

