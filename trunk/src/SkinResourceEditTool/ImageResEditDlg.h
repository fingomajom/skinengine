#pragma once

#include "resource.h"

#include <atlframe.h>


enum {
    em_imagelist_height = 150
};

class CImageListViewCtrl : 
    public CWindowImpl<CImageListViewCtrl, CListBox>,
    public COwnerDraw<CImageListViewCtrl>
{
public:

public:
    
    BEGIN_MSG_MAP(CImageResEditDlg)
        CHAIN_MSG_MAP_ALT(COwnerDraw<CImageListViewCtrl>, 1)
    END_MSG_MAP()


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
        CRect    rcSubItem;
        TCHAR    szBuffer[MAX_PATH] = { 0 };

        brushBkgnd.CreateSolidBrush( clrBkGnd );
        dc.FillRect(&rcItem, brushBkgnd);

        COLORREF clrTextColor = 
            bSelected ? 
            (bFocus ? GetSysColor(COLOR_HIGHLIGHTTEXT) : GetSysColor( COLOR_BTNTEXT) ) : 
            GetSysColor(COLOR_BTNTEXT);


        //GetItemText( nItem, nColumn, szBuffer, MAX_PATH);


        //dc.DrawText(szBuffer, -1 ,rcSubItem, DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
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


};

class CImageResEditDlg : public CDialogImpl<CImageResEditDlg>
{
public:
    enum { IDD = IDD_IMAGE_EDIT_DIALOG };

    BEGIN_MSG_MAP(CImageResEditDlg)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
        COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)

        REFLECT_NOTIFICATIONS()

    END_MSG_MAP()

    // Handler prototypes (uncomment arguments if needed):
    //	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    //	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    //	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

    CImageListViewCtrl m_wndImageList;

    LRESULT ReLoadImages()
    {

    }

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        m_wndImageList.SubclassWindow(GetDlgItem(IDC_IMAGE_LIST));

        m_wndImageList.SetItemHeight(0, em_imagelist_height);

        CenterWindow(GetParent());
        return TRUE;
    }

    LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        EndDialog(wID);
        return 0;
    }
};

