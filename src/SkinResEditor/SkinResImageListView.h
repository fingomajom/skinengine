/********************************************************************
* CreatedOn: 2007-12-19   17:40
* FileName: SkinResImageListView.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include <atlframe.h>
#include <atlpath.h>
#include <skingdi.h>
#include <skinxmlctrls.h>
#include "SkinControlsMgt.h"



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


enum {
    em_imagelist_height = 60
};

class SkinResImageListView : 
    public CWindowImpl<SkinResImageListView, CListBox>,
    public COwnerDraw<SkinResImageListView>,
    public SkinTreeItemControl,
    public SkinPropertyView::PropertyEditNotify
{
public:
    
    SkinResImageListView()
    {   
        m_neditindex = -1;
    }

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
        KSG::CString strIDName;
        KSG::CString strFileName;
    };

    std::vector<image_item_info> m_vtItems;

    int m_neditindex;

public:

    virtual void InitResult(HTREEITEM hTreeItem)
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        if (m_hWnd == NULL)
        {
        }

        ClearImage();

        std::vector<SkinImageResDoc::_ImageItemInfo>& vtList =
            ControlsMgt.m_resDocument.m_resImageDoc.m_vtItemList;

        for (size_t i = 0; i < vtList.size(); i++)
        {
            AppendImage(vtList[i].strIDName, vtList[i].strFileName);
        }

        AppendImage(_T(""), _T(""));
    }

    virtual void ShowResult(HTREEITEM hTreeItem, LPARAM lParam)
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();
    
        ::ShowWindow(GetParent(), SW_SHOW);

        ControlsMgt.m_piSkinFrame->SetActiveResultWindow(GetParent());

    }
    virtual void HideResult(HTREEITEM hTreeItem, LPARAM lParam)
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        ControlsMgt.m_skinResPropertyView.Clear();

        ::ShowWindow(GetParent(), SW_HIDE);

    }

public:

    BEGIN_MSG_MAP(SkinResImageListView)

        MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)

        REFLECTED_COMMAND_CODE_HANDLER(LBN_SELCHANGE, OnSelChanged)

        
        CHAIN_MSG_MAP_ALT(COwnerDraw<SkinResImageListView>, 1)

    END_MSG_MAP()

    LRESULT OnSelChanged(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        int index = CListBox::GetCurSel();

        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        ControlsMgt.m_skinResPropertyView.Clear();
        ControlsMgt.m_skinResPropertyView.SetPropertyEditNotify(this);
        
        m_neditindex = index;

        if (index >= 0 )
        {
            std::vector<SkinImageResDoc::_ImageItemInfo>& vtList =
                ControlsMgt.m_resDocument.m_resImageDoc.m_vtItemList;

            ControlsMgt.m_skinResPropertyView.AppendProperty(
                _T("ID"), (index < GetCount() - 1) ? vtList[index].strIDName : _T(""), SkinPropertyView::it_text);
            ControlsMgt.m_skinResPropertyView.AppendProperty(
                _T("FileName"), (index < GetCount() - 1) ? vtList[index].strFileName : _T(""), SkinPropertyView::it_button);
        }

        return DefWindowProc();
    }

    virtual void OnValueChange (
        LPCTSTR pszPropertyName,
        LPCTSTR pszOldValue,
        LPCTSTR pszNewValue)
    {

        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        std::vector<SkinImageResDoc::_ImageItemInfo>& vtList =
            ControlsMgt.m_resDocument.m_resImageDoc.m_vtItemList;

        ControlsMgt.m_resDocument.Modify(TRUE);

        if (!_tcscmp(pszPropertyName, _T("ID")))
        {
            if (m_neditindex >= 0)
            {
                if (pszNewValue[0] >= '0' && pszNewValue[0] <= '9') // 不合法的项名
                {
                    ControlsMgt.m_skinResPropertyView.SetProperty(_T("ID"),
                        pszOldValue);

                    return;
                }

                m_vtItems[m_neditindex].strIDName = pszNewValue;

                if ( m_neditindex < (GetCount() - 1) )
                {
                    vtList[m_neditindex].strIDName = pszNewValue;
                }
                else
                {
                    AppendImage(_T(""), _T(""));

                    SkinImageResDoc::_ImageItemInfo item;

                    item.strIDName = pszNewValue;
                    item.strFileName = m_vtItems[m_neditindex].strFileName;
                    
                    if (item.strFileName.GetLength() <= 0)
                    {
                        item.strFileName = _T("unknown");
                        m_vtItems[m_neditindex].strFileName = _T("unknown");
                        m_vtItems[m_neditindex].ntype = GetType(_T("unknown"));

                        ControlsMgt.m_skinResPropertyView.SetProperty(
                            _T("FileName"), _T("unknown"));

                    }

                    vtList.push_back(item);

                }


                CWindow::Invalidate(FALSE);
            }
        }
        else if (!_tcscmp(pszPropertyName, _T("FileName")))
        {
            if (m_neditindex >= 0)
            {
                m_vtItems[m_neditindex].strFileName = pszNewValue;
                m_vtItems[m_neditindex].ntype = GetType(pszNewValue);

                if ( m_neditindex < (GetCount() - 1) )
                {
                    vtList[m_neditindex].strFileName = pszNewValue;
                }
            }
        }
        //else 
        //    ATLASSERT(FALSE);
        

        CWindow::Invalidate(FALSE);
    }

    virtual void OnButtonClieck(LPCTSTR pszPropertyName)
    {
        CMyFileDialog openDlg(TRUE, _T(""), _T(""),
            OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,// | OFN_ALLOWMULTISELECT, 
            _T("Bitmap file(*.bmp)\0*.bmp\0Icon file(*.ico)\0*.ico\0Cursor file(*.cur)\0*.cur\0所有文件(*.*)\0*.*\0\0"));

        openDlg.m_ofn.lpstrTitle = (_T("打开文件"));

        TCHAR szBuffer[4096] = { 0 };

        openDlg.m_ofn.lpstrFile = szBuffer;

        if (IDOK == openDlg.DoModal())
        {
            SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();
            std::vector<SkinImageResDoc::_ImageItemInfo>& vtList =
                ControlsMgt.m_resDocument.m_resImageDoc.m_vtItemList;

            WTL::CString strPathFile = openDlg.m_ofn.lpstrFile;

            CPath path = strPathFile;

            KSG::CString strFileName = path.m_strPath.Mid(path.FindFileName());
            KSG::CString strIDName   = path.m_strPath.Mid(path.FindFileName());

            skinconfigbase* pskinconfig = ControlsMgt.m_resDocument.m_resImageDoc.m_pskinconfig;

            if (pskinconfig != 0)
            {
                KSG::CString strImagePath = pskinconfig->GetSkinImagePath();

                path.RemoveFileSpec();

                if ( strImagePath.CompareNoCase(path.m_strPath) != 0)
                {
                    CPath pathCopyTo = strImagePath;
                    pathCopyTo.Append(strFileName);

                    CopyFile(strPathFile, pathCopyTo.m_strPath, FALSE);
                }
            }

            ControlsMgt.m_skinResPropertyView.SetProperty(_T("FileName"),
                strFileName);
            
            if (m_neditindex >= 0)
            {
                m_vtItems[m_neditindex].strFileName = strFileName;
                m_vtItems[m_neditindex].ntype = GetType(strFileName);
                
                if ( m_neditindex < (GetCount() - 1) )
                    vtList[m_neditindex].strFileName = strFileName;

                CWindow::Invalidate(FALSE);
            }


            ControlsMgt.m_resDocument.Modify(TRUE);

            //AppendImageBefNew(strIDName, strFileName );

        }


    }

    LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if (wParam == VK_DELETE)
        {
            int nIndex = GetCurSel();

            if (nIndex >= 0 && nIndex < GetCount() - 1)
            {
                DeleteString(nIndex);
                m_vtItems.erase( m_vtItems.begin() + nIndex );

                SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

                ControlsMgt.m_resDocument.m_resImageDoc.m_vtItemList.erase(
                    ControlsMgt.m_resDocument.m_resImageDoc.m_vtItemList.begin() + nIndex);

                ControlsMgt.m_resDocument.Modify(TRUE);

                CListBox::SetCurSel(nIndex);

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

        KSG::CString strDisplayName;
        KSG::CString strFileName;

        if (itemInfo.ntype == em_image_new)
        {
            strDisplayName = _T("New...");
        }
        else
        {
            strDisplayName = itemInfo.strIDName;
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
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        CSkinDCHandle skindc = dc.m_hDC;

        RECT rcdraw = { 0 };
        int  bbig   = false;


        skinconfigbase* pskinconfig = ControlsMgt.m_resDocument.m_resImageDoc.m_pskinconfig;

        CPath pathImage;

        pathImage.m_strPath = pskinconfig->GetSkinImagePath();
        pathImage.Append(itemInfo.strFileName);


        switch( itemInfo.ntype )
        {
        case em_image_bitmap:
            {
                CBitmap bmp;

                bmp.m_hBitmap = (HBITMAP)::LoadImage(NULL, pathImage.m_strPath, 
                    IMAGE_BITMAP, 0, 0, 
                    LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);

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
                CIcon icon = (HICON)::LoadImage(NULL, pathImage.m_strPath ,
                    IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTCOLOR | LR_DEFAULTSIZE);

                if (icon.m_hIcon != NULL)
                    icon.DrawIcon( dc, lprect->left, lprect->top );

            }
            break;
        case em_image_cursor:
            {
                CIcon icon = (HICON)LoadImage(NULL, pathImage.m_strPath,
                    IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTCOLOR | LR_DEFAULTSIZE);

                if (icon.m_hIcon != NULL)
                    icon.DrawIcon( dc, lprect->left, lprect->top );

                //CCursor cursor = (HCURSOR)document.getskinimageedit().LoadImage(itemInfo.strIDName,
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


    BOOL AppendImage(const KSG::CString& strIDName,
        const KSG::CString& strFileName)
    {
        image_item_info item;

        item.ntype = GetType(strFileName);

        item.strIDName   = strIDName;
        item.strFileName = strFileName;

        m_vtItems.push_back(item);
        AddString(strIDName);        

        return TRUE;
    }

    BOOL AppendImageBefNew(const KSG::CString& strIDName,
        const KSG::CString& strFileName)
    {
        image_item_info item;

        item.ntype = GetType(strFileName);

        item.strIDName   = strIDName;
        item.strFileName = strFileName;

        int nIndex = GetCount() - 1;

        m_vtItems.insert(m_vtItems.begin() + nIndex, item);
        InsertString(nIndex, strIDName);        

        return TRUE;
    }


    BOOL SetImage(int nIndex, 
        const KSG::CString& strIDName,
        const KSG::CString& strFileName)
    {
        for (int i = 0; i < GetCount(); i++)
        {
            if (i == nIndex)
                continue;

            if (strIDName.CompareNoCase(m_vtItems[i].strIDName) == 0)
            {
                SetCurSel(i);

                return TRUE;
            }
        }

        m_vtItems[nIndex].ntype       = GetType(strFileName);
        m_vtItems[nIndex].strIDName   = strIDName;
        m_vtItems[nIndex].strFileName = strFileName;

        if (nIndex == GetCount() - 1)
        {
            AppendImage(_T(""), _T(""));

        }

        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        std::vector<SkinImageResDoc::_ImageItemInfo>& vtList =
            ControlsMgt.m_resDocument.m_resImageDoc.m_vtItemList;

        vtList[nIndex].strIDName = strIDName;
        vtList[nIndex].strFileName = strFileName;


        return TRUE;
    }


    int GetType(const KSG::CString& strFileName)
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

class CImageResEditDlg : 
    public CDialogImpl<CImageResEditDlg>
{
public:

    SkinResImageListView m_wndResImageList;

    enum { IDD = IDD_RESEDITIMAGE_DIALOG };

    BEGIN_MSG_MAP(CImageResEditDlg)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_SIZE      , OnSize)

        REFLECT_NOTIFICATIONS()

    END_MSG_MAP()

    LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {

        if (m_wndResImageList.m_hWnd != NULL)
        {
            RECT rcClient = { 0 };

            GetClientRect(&rcClient);

            m_wndResImageList.MoveWindow(&rcClient);

        }

        return DefWindowProc();
    }

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        m_wndResImageList.SubclassWindow(GetDlgItem(IDC_IMAGE_LIST));
        m_wndResImageList.SetItemHeight(0, em_imagelist_height);

        return TRUE;
    }

};