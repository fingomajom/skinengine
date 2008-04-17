/********************************************************************
* CreatedOn: 2008-2-17   12:25
* FileName: SkinResDialogView.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#include "SkinControlsMgt.h"
#include "SkinResWndDefProperty.h"
#include "SkinDialgPreviewWindow.h"
#include "SkinUpDownDlg.h"

class SkinResDialogView : 
    public CDialogImpl<SkinResDialogView>,
    public SkinTreeItemControl,
    public SkinPropertyView::PropertyEditNotify
{
public:

    enum
    {
        em_itemid_begin = 1000
    };

    int m_ndialogindex;
    int m_nNewItemId;
    

    HTREEITEM      m_hTreeItem;

    CTreeViewCtrl  m_wndTree;
    CComboBox      m_wndComboBox;
    CButton        m_wndAddBtn;
    CButton        m_wndDelBtn;

    SkinUpDownDlg  m_wndUpdown;

    SkinDialgPreviewWindow m_wndPreView;

    HTREEITEM      m_hLastSelItem;

    CImageList m_imagelist;

    static std::map<KSGUI::CString, KSGUI::CString> m_mapUsedIdName;

    SkinDialogRes& GetResDialog()
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();
        
        static SkinDialogRes null;
        
        if (m_ndialogindex < 0 || m_ndialogindex >= (int)ControlsMgt.m_resDocument.m_resDialogDoc.m_vtDialogList.size())
            return null;

        return ControlsMgt.m_resDocument.m_resDialogDoc.m_vtDialogList[m_ndialogindex];
    }


    SkinResDialogView(int ndialogindex)
        : m_ndialogindex(ndialogindex), m_hTreeItem(NULL)
    {
        m_nNewItemId = em_itemid_begin;
    }

    ~SkinResDialogView()
    {
        if (m_wndPreView.IsWindow())
            m_wndPreView.DestroyWindow();
        if (m_wndUpdown.IsWindow())
            m_wndUpdown.DestroyWindow();
        
    }

public:

    virtual void InitResult(HTREEITEM hTreeItem)
    {
        m_hTreeItem = hTreeItem;

        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        if (m_hWnd == NULL)
        {
            Create(ControlsMgt.m_piSkinFrame->GetResultParentWnd());
        }

        m_wndTree.DeleteAllItems();

        SkinDialogRes& dialogRes = GetResDialog();

        if (dialogRes.m_dlgWndProperty.m_vtPropertyList.size() == 0)
        {
            SkinResWndDefProperty::GetResClassWndDefProperty(
                KSGUI::CString(), dialogRes.m_dlgWndProperty);

            

            dialogRes.m_dlgWndProperty.SetProperty(_T("IdName"), dialogRes.m_dlgWndProperty.GetIdName());
            dialogRes.m_dlgWndProperty.SetProperty(_T("Width") , _T("200"));
            dialogRes.m_dlgWndProperty.SetProperty(_T("Height"), _T("150"));
            dialogRes.m_dlgWndProperty.SetProperty(_T("Style") , _T("0x5c0a0000"));
            dialogRes.m_dlgWndProperty.SetProperty(_T("Font"),   _T("宋体,GB2312_CHARSET,9,1,400,0"));

        }

        m_wndTree.InsertItem( dialogRes.m_dlgWndProperty.GetIdName(),
            2, 2, TVI_ROOT, TVI_LAST);

        for (size_t i = 0; i < dialogRes.m_vtChildWndList.size(); i++)
        {
            HTREEITEM hInsertItem = m_wndTree.InsertItem( 
                dialogRes.m_vtChildWndList[i].GetIdName(),
                3, 3, m_wndTree.GetRootItem(), TVI_LAST);

            m_wndTree.SetItemData(hInsertItem, i);

            //////////////////////////////////////////////////////////////////////////
            
            KSGUI::CString strItemId;

            if (!dialogRes.m_vtChildWndList[i].GetProperty(_T("ItemId"), strItemId))
                continue;

            std::map<KSGUI::CString, KSGUI::CString>::const_iterator iter =
                m_mapUsedIdName.find(dialogRes.m_vtChildWndList[i].GetIdName());

            if (iter == m_mapUsedIdName.end())
            {
                m_mapUsedIdName[dialogRes.m_vtChildWndList[i].GetIdName()] = strItemId;
            }
            else if (strItemId != iter->second)
            {
                // error
            }

            //////////////////////////////////////////////////////////////////////////
        }

        std::vector<KSGUI::CString> vtClassName;

        SkinResWndDefProperty::GetDefClassNameList(vtClassName);


        for (size_t i = 0; i < vtClassName.size(); i++)
        {
            m_wndComboBox.AddString(vtClassName[i]);
        }

        m_wndTree.Expand(m_wndTree.GetRootItem());

    }

    virtual void ShowResult(HTREEITEM hTreeItem, LPARAM lParam)
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();


        m_wndTree.SetItemText(m_wndTree.GetRootItem(), GetResDialog().m_dlgWndProperty.GetIdName());

        CreatePreviewWindow();

        SkinHookMouse::instance().m_hHookWindow = m_wndPreView;

        ControlsMgt.m_piSkinFrame->SetActiveResultWindow(m_hWnd);

        //m_wndUpdown.ShowWindow(SW_SHOW);

        m_wndTree.SelectItem(m_wndTree.GetRootItem());

        ShowWindow(SW_SHOW);

    }

    virtual void HideResult(HTREEITEM hTreeItem, LPARAM lParam)
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        ControlsMgt.m_skinResPropertyView.Clear();

        SkinHookMouse::instance().m_hHookWindow = NULL;
        SkinHookMouse::instance().m_bHookKey    = FALSE;

        m_wndTree.SelectItem(NULL);
        m_wndPreView.m_wndSelectFlag.ShowWindow(SW_HIDE);

        m_wndUpdown.ShowWindow(SW_HIDE);

        ShowWindow(SW_HIDE);
    }

    enum { IDD = IDD_EDITDLG_DIALOG };

    
    void CreatePreviewWindow()
    {
        SkinXmlElement xmlElement;
        m_wndPreView.ReCreatePreviewWindow(GetDlgItem(IDC_REVIEW_STATIC), GetResDialog());
        
        SkinHookMouse::instance().m_hHookWindow = m_wndPreView;
    }

    BEGIN_MSG_MAP(SkinResDialogListView)

        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_SIZE      , OnSize)
        
        MESSAGE_HANDLER(WM_USER_KEYDOWN   , OnUserKeyDown)

        MESSAGE_HANDLER(WM_SELECTCHILDWINDOW, OnSelectWindow)

        COMMAND_HANDLER(IDC_ADD   , BN_CLICKED, OnAdd)
        COMMAND_HANDLER(IDC_DELETE, BN_CLICKED, OnDel)

        COMMAND_HANDLER(IDC_UP_BUTTON  , BN_CLICKED, OnUp)
        COMMAND_HANDLER(IDC_DOWN_BUTTON, BN_CLICKED, OnDown)

        NOTIFY_CODE_HANDLER( TVN_SELCHANGED, OnSelChanged)

    END_MSG_MAP()

    int GetNextItemId()
    {
        int nResult = 0;

        SkinDialogRes& dialogRes = GetResDialog();
        
        std::vector<SkinWndPropertyList>& vtChildWndList = dialogRes.m_vtChildWndList;

        KSGUI::CString strItemIdNew;
        KSGUI::CString strItemId;
        KSGUI::CString strIDName;

        for ( ; true; m_nNewItemId++ )
        {
            strItemIdNew.Format(_T("%d") , m_nNewItemId);
            strIDName.Format(_T("IDN_%d"), m_nNewItemId);
                        
            BOOL bUsed = FALSE;

            for ( size_t i = 0; i < vtChildWndList.size(); i++ )
            {
                if (!strIDName.CompareNoCase(vtChildWndList[i].GetIdName()))
                {
                    bUsed = TRUE;

                    break;
                }

                if (!vtChildWndList[i].GetProperty(_T("ItemId"), strItemId) )
                    continue;
                
                if (!strItemIdNew.CompareNoCase(strItemId))
                {
                    bUsed = TRUE;

                    break;
                }

            }

            if (!bUsed)
            {
                nResult = m_nNewItemId;

                break;
            }
        }

        return nResult;
    }

    int GetItemIndex(HTREEITEM hTreeItem)
    {
        int nResult = -1;

        while (hTreeItem != NULL)
        {
            hTreeItem = m_wndTree.GetPrevSiblingItem(hTreeItem);

            nResult++;
        }

        return nResult;
    }

    LRESULT OnAdd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        TCHAR szBuffer[MAX_PATH] = { 0 };

        m_wndComboBox.GetWindowText(szBuffer, MAX_PATH);

        if ( ( szBuffer[0] >= '0' && szBuffer[0] <= '9' ) || _tcslen(szBuffer) <= 0) // 不合法的项名
        {
            return TRUE;
        }

        SkinDialogRes& dialogRes = GetResDialog();

        SkinWndPropertyList WndProperty;

        SkinResWndDefProperty::GetResClassWndDefProperty(
            szBuffer, WndProperty);

        int nNewItemId = GetNextItemId();

        KSGUI::CString strItemId;

        strItemId.Format(_T("%d"), nNewItemId);

        WndProperty.GetIdName().Format(_T("IDN_%d"), nNewItemId);

        //////////////////////////////////////////////////////////////////////////
        std::map<KSGUI::CString, KSGUI::CString>::const_iterator iter =
            m_mapUsedIdName.find(WndProperty.GetIdName());

        if (iter != m_mapUsedIdName.end())
        {
            strItemId = iter->second;
        }
        else
        {
            m_mapUsedIdName[WndProperty.GetIdName()] = strItemId;
        }
        //////////////////////////////////////////////////////////////////////////

        WndProperty.SetProperty(_T("IdName"), WndProperty.GetIdName());
        WndProperty.SetProperty(_T("SkinClassName"), szBuffer);
        WndProperty.SetProperty(_T("ItemId"), strItemId);


        KSGUI::CString strStyle;
        KSGUI::CString strExStyle;

        SkinResWndDefProperty::GetWndStyle(szBuffer, strStyle);
        SkinResWndDefProperty::GetWndExStyle(szBuffer, strExStyle);

        WndProperty.SetProperty(_T("Style"), strStyle);
        WndProperty.SetProperty(_T("ExStyle"), strExStyle);


        dialogRes.m_vtChildWndList.push_back(WndProperty);


        HTREEITEM hInsertItem = m_wndTree.InsertItem( 
            WndProperty.GetIdName(),
            3, 3, m_wndTree.GetRootItem(), TVI_LAST);

        m_wndTree.SetItemData(hInsertItem, 
            dialogRes.m_vtChildWndList.size() - 1);

        TVITEM tvItem;

        tvItem.mask  = TVIF_CHILDREN ;
        tvItem.hItem = m_wndTree.GetRootItem();

        m_wndTree.GetItem(&tvItem);
        tvItem.cChildren = TRUE;
        m_wndTree.SetItem(&tvItem);

        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();
        ControlsMgt.m_resDocument.Modify(TRUE);

        m_wndPreView.AddSkinWindow(WndProperty);

        m_wndTree.SelectItem(hInsertItem);

        return TRUE;
    }

    LRESULT OnDel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        HTREEITEM hTreeItem = m_wndTree.GetSelectedItem();

        if (hTreeItem == m_wndTree.GetRootItem())
            return TRUE;

        int nindex = GetItemIndex(hTreeItem);

        if (nindex >= 0)
        {
            SkinDialogRes& dialogRes = GetResDialog();

            ATLASSERT((size_t)nindex < dialogRes.m_vtChildWndList.size());

            m_wndPreView.DelSkinWindow(dialogRes.m_vtChildWndList[nindex]) ;
            
            KSGUI::CString strIdName = dialogRes.m_vtChildWndList[nindex].GetIdName();

            dialogRes.m_vtChildWndList.erase(
                dialogRes.m_vtChildWndList.begin() + nindex);

            if (!ControlsMgt.m_resDocument.m_resDialogDoc.IsChildIdNameExists(strIdName))
                m_mapUsedIdName.erase(strIdName);


            m_wndTree.DeleteItem(hTreeItem);

            SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();
            ControlsMgt.m_resDocument.Modify(TRUE);

            //CreatePreviewWindow();

            return TRUE;
        }

        return TRUE;
    }


    LRESULT OnSelChanged(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
    {
        LPNMTREEVIEW pNMTV = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

        LRESULT lResult = DefWindowProc();
        SkinDialogRes& dialogRes = GetResDialog();


        m_hLastSelItem = pNMTV->itemNew.hItem;
        
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();
        
        ControlsMgt.m_skinResPropertyView.Clear();
        ControlsMgt.m_skinResPropertyView.SetPropertyEditNotify(this);

        if (m_hLastSelItem != NULL)
        {
            std::vector<SkinWndPropertyList::WndPropertyItem>* pPropertyList = NULL;
            
            if (m_hLastSelItem == m_wndTree.GetRootItem())
            {             
                pPropertyList = &dialogRes.m_dlgWndProperty.m_vtPropertyList;

                m_wndPreView.ClearSelectWindow();
                MoveUpdownWindow(-1);

            }
            else
            {
                int nindex = (int)GetItemIndex(m_hLastSelItem);

                ATLASSERT(nindex >= 0 && nindex < (int)dialogRes.m_vtChildWndList.size());
                
                pPropertyList = &dialogRes.m_vtChildWndList[nindex].m_vtPropertyList;

                KSGUI::CString strClassName;

                dialogRes.m_vtChildWndList[nindex].GetProperty(_T("SkinClassName") , strClassName);

                m_wndComboBox.SetWindowText(strClassName);

                m_wndPreView.SelectWindow(dialogRes.m_vtChildWndList[nindex]);

                MoveUpdownWindow(nindex);
            }
            
            ATLASSERT(pPropertyList != NULL);

            for (size_t i = 0; i < pPropertyList->size(); i++)
            {
                int ntype = SkinResWndDefProperty::GetResWndPropertyEditType((*pPropertyList)[i].strProperty);

                if (m_hLastSelItem == m_wndTree.GetRootItem())
                {
                    if (!_tcscmp((*pPropertyList)[i].strProperty, _T("IdName")))
                    {
                        ntype = SkinPropertyView::it_readonly;
                    }
                    else if (!_tcscmp((*pPropertyList)[i].strProperty, _T("Font")))
                    {
                        ntype = SkinPropertyView::it_button;
                    }
               }

                ControlsMgt.m_skinResPropertyView.AppendProperty( 
                    (*pPropertyList)[i].strProperty, 
                    (*pPropertyList)[i].strValue, 
                    ntype);
            }

        }

        return lResult;
    }

    void MoveUpdownWindow(int index)
    {
        if (index < 0)
        {
            m_wndUpdown.ShowWindow(SW_HIDE);
            return;
        }

        int nheight = m_wndTree.GetItemHeight();

        RECT rcClient = { 0 };

        m_wndTree.GetClientRect(&rcClient);
        ClientToScreen(&rcClient);
        
        rcClient.top += ( (index + 1) * nheight);
        rcClient.bottom = rcClient.top + 20;

        rcClient.left = rcClient.right - 20;
    
        m_wndUpdown.MoveWindow(&rcClient);
        m_wndUpdown.ShowWindow(SW_SHOW);
    }


    LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        if (m_wndTree.m_hWnd != NULL)
        {
            RECT rcClient   = { 0 };
            RECT rcTree     = { 0 };
            RECT rcComboBox = { 0 };
            RECT rcAddBtn   = { 0 };
            RECT rcDelBtn   = { 0 };
            RECT rcStatic   = { 0 };

            GetClientRect(&rcClient);

            m_wndTree.GetClientRect(&rcTree);

            rcTree.bottom = rcClient.bottom - 50;;
            
            rcComboBox = rcTree;
            rcComboBox.top = rcComboBox.bottom + 2;
            rcComboBox.bottom = rcComboBox.top + 22;

            rcAddBtn = rcComboBox;
            rcAddBtn.top = rcAddBtn.bottom + 2;
            rcAddBtn.bottom = rcAddBtn.top + 22;

            rcAddBtn.left += 2;
            rcAddBtn.right = (rcAddBtn.left + rcAddBtn.right) / 2 + 1;
            
            rcDelBtn = rcAddBtn;
            rcDelBtn.left = rcAddBtn.right + 2;
            rcDelBtn.right = rcTree.right - 2;

            rcStatic = rcClient;
            rcStatic.left = rcTree.right + 2;

            m_wndTree.MoveWindow(&rcTree);
            m_wndComboBox.MoveWindow(&rcComboBox);
            m_wndAddBtn.MoveWindow(&rcAddBtn);
            m_wndDelBtn.MoveWindow(&rcDelBtn);

            GetDlgItem(IDC_REVIEW_STATIC).MoveWindow(&rcStatic);
        }

        m_wndPreView.UpdateSelectFlag();

        return DefWindowProc();
    }

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        m_wndTree     = GetDlgItem(IDC_WND_TREE);
        m_wndComboBox = GetDlgItem(IDC_COMBO);

        m_wndAddBtn  = GetDlgItem(IDC_ADD);
        m_wndDelBtn  = GetDlgItem(IDC_DELETE);

        CBitmap bmp;

        bmp.LoadBitmap(IDB_RESTYPE_BITMAP);

        m_imagelist.Create(16, 16, ILC_COLOR24 | ILC_MASK, 3, 1);
        m_imagelist.Add(bmp, RGB(255, 0, 255));

        m_wndTree.SetImageList(m_imagelist);

        if (!m_wndUpdown.IsWindow())
        {
            m_wndUpdown.Create(m_hWnd);
            m_wndUpdown.m_hWndParent = m_hWnd;
        }

        return TRUE;
    }


    LRESULT OnSelectWindow(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        LPCTSTR lpIdName = (LPCTSTR)wParam;

        if (lpIdName == NULL)
            m_wndTree.SelectItem(m_wndTree.GetRootItem());
        else
        {
            HTREEITEM hTreeItem = m_wndTree.GetChildItem(m_wndTree.GetRootItem());
            size_t index = 0;

            SkinDialogRes& dialogRes = GetResDialog();

            while (hTreeItem != NULL)
            {
                if (index >= dialogRes.m_vtChildWndList.size())
                    break;

                if (!dialogRes.m_vtChildWndList[index].GetIdName().CollateNoCase(lpIdName))
                {
                    m_wndTree.SelectItem(hTreeItem);

                    break;
                }

                index++;
                hTreeItem = m_wndTree.GetNextSiblingItem(hTreeItem);

            }
        }

        return TRUE;
    }

    void OnValueChange (
        LPCTSTR pszPropertyName,
        LPCTSTR pszOldValue,
        LPCTSTR pszNewValue)
    {
        SkinDialogRes& dialogRes = GetResDialog();

        HTREEITEM hTreeItem = m_wndTree.GetSelectedItem();


        if (hTreeItem == m_wndTree.GetRootItem())
        {
            UpdateWndProperty(dialogRes.m_dlgWndProperty,
                pszPropertyName,
                pszOldValue, pszNewValue);

            CreatePreviewWindow();
        }
        else
        {
            int nindex = GetItemIndex(hTreeItem);
            
            UpdateWndProperty(dialogRes.m_vtChildWndList[nindex],
                pszPropertyName,
                pszOldValue, pszNewValue);
        }
        
    }

    void UpdateWndProperty( SkinWndPropertyList& WndPropertyList, 
        LPCTSTR pszPropertyName,
        LPCTSTR pszOldValue,
        LPCTSTR pszNewValue)
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();
        SkinDialogRes&   dialogRes   = GetResDialog();


        if ( !_tcscmp(pszPropertyName, _T("IdName") ) )
        {
            if ( _tcslen(pszNewValue) <= _tcslen(_T("IDN_")) ||
                 _tcsncmp(pszNewValue, _T("IDN_"), _tcslen(_T("IDN_")) ) ) // 不合法的项名
            {
                ControlsMgt.m_skinResPropertyView.SetProperty(_T("IdName"),
                    pszOldValue);

                KSGUI::CString strMsg;

                strMsg.Format(
                    _T("[%s]不是合法的项名\n必顺以 IDN_ 开头的字符串。"),
                    pszNewValue);

                MessageBox(strMsg, _T("错误"));

                return;
            }

            if (&WndPropertyList != &dialogRes.m_dlgWndProperty)
            {

                for ( size_t i = 0; i < dialogRes.m_vtChildWndList.size(); i++ )
                {
                    if ( !dialogRes.m_vtChildWndList[i].GetIdName().CollateNoCase(pszNewValue) ) // 不合法的项名
                    {
                        ControlsMgt.m_skinResPropertyView.SetProperty(_T("IdName"),
                            pszOldValue);

                        KSGUI::CString strMsg;

                        strMsg.Format(
                            _T("[%s]项名已存在。请输入其它的名称。"),
                            pszNewValue);

                        MessageBox(strMsg, _T("错误"));

                        return;
                    }
                }

                m_wndPreView.DelSkinWindow( WndPropertyList );

            }

            m_wndTree.SetItemText( m_wndTree.GetSelectedItem(), pszNewValue);

            WndPropertyList.GetIdName() = pszNewValue;
            WndPropertyList.SetProperty( pszPropertyName, pszNewValue );

            if (&WndPropertyList != &dialogRes.m_dlgWndProperty)
            {
                m_wndPreView.AddSkinWindow(WndPropertyList);

                //////////////////////////////////////////////////////////////////////////

                std::map<KSGUI::CString, KSGUI::CString>::const_iterator iter =
                    m_mapUsedIdName.find(WndPropertyList.GetIdName());

                if (iter != m_mapUsedIdName.end())
                {
                    WndPropertyList.SetProperty( _T("ItemId"), iter->second );

                    ControlsMgt.m_skinResPropertyView.SetProperty(_T("ItemId"),
                        iter->second);
                }
                else
                {
                    KSGUI::CString strItemId;

                    WndPropertyList.GetProperty(_T("ItemId"), strItemId);
                    
                    m_mapUsedIdName[WndPropertyList.GetIdName()] = strItemId;
                }

                if (!ControlsMgt.m_resDocument.m_resDialogDoc.IsChildIdNameExists(pszOldValue))
                    m_mapUsedIdName.erase(pszOldValue);
                //////////////////////////////////////////////////////////////////////////
           }

            return;
        } 
        else if ( !_tcscmp(pszPropertyName, _T("ItemId") ) )
        {
            m_mapUsedIdName[WndPropertyList.GetIdName()] = pszNewValue;
        }


        WndPropertyList.SetProperty( pszPropertyName, pszNewValue );
        
        m_wndPreView.UpdateSkinWindow(WndPropertyList);

    }


    void OnButtonClieck(LPCTSTR pszPropertyName)
    {

        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        HTREEITEM hTreeItem = m_wndTree.GetSelectedItem();

        if (hTreeItem != m_wndTree.GetRootItem())
            return;


        if ( !_tcscmp(pszPropertyName, _T("Font")) )
        {
            KSGUI::CString strOldFont;
            TCHAR szNewBuffer[1024] = { 0 };
            
            GetResDialog().m_dlgWndProperty.GetProperty(_T("Font"), strOldFont);

            LOGFONT logFont = { 0 };

            KSGUI::skinxmlfont xmlfont;

            (KSGUI::CString&)xmlfont = (strOldFont);

            xmlfont >> logFont;

            CFontDialog fontdlg(&logFont);

            if (fontdlg.DoModal() != IDOK)
                return ;

            logFont.lfHeight = fontdlg.GetSize() / 10;

            xmlfont << logFont;

            if (!_tcscmp(strOldFont, xmlfont))
                return ;

            ControlsMgt.m_skinResPropertyView.SetProperty(_T("Font"),
                xmlfont);

            OnValueChange(pszPropertyName,
                strOldFont, xmlfont);

            return ;
        }

    }

    LRESULT OnUp(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        
        return 0L;
    }

    LRESULT OnDown(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {

        return 0L;
    }


    LRESULT OnUserKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {
        if ( wParam == VK_LEFT ||
             wParam == VK_RIGHT ||
             wParam == VK_UP ||
             wParam == VK_DOWN )
        {

            HTREEITEM hTreeItem = m_wndTree.GetSelectedItem();

            if (hTreeItem == NULL || hTreeItem == m_wndTree.GetRootItem())
                return 1L;

            SkinDialogRes& dialogRes = GetResDialog();
            SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

            int nindex = GetItemIndex(hTreeItem);

            SkinWndPropertyList& WndProperty = dialogRes.m_vtChildWndList[nindex];

            KSGUI::CString strLeft;
            KSGUI::CString strTop;
            KSGUI::CString strWidth;
            KSGUI::CString strHeight;

            WndProperty.GetProperty(_T("Left"), strLeft);
            WndProperty.GetProperty(_T("Top") , strTop);
            WndProperty.GetProperty(_T("Width"), strWidth);
            WndProperty.GetProperty(_T("Height"), strHeight);

            int nleft   = _ttoi(strLeft);
            int ntop    = _ttoi(strTop);
            int nwidth  = _ttoi(strWidth);
            int nheight = _ttoi(strHeight);

            char bShiftState   = ( GetKeyState(VK_SHIFT) >> 8 ) & 0x01;
            char bControlState = ( GetKeyState(VK_CONTROL) >> 8) & 0x01;

            int nstep = bControlState ? 1 : 5;

            switch(wParam)
            {
            case VK_LEFT:
                if (!bShiftState)
                {
                    nleft -= nstep; nleft = nleft > 0 ? nleft : 0;
                    nleft = nleft / nstep * nstep;
                }
                else
                {
                    nwidth -= nstep; nwidth = nwidth > 0 ? nwidth : 0;
                    nwidth = nwidth / nstep * nstep;
                }
                break;

            case VK_UP:
                if (!bShiftState)
                {
                    ntop -= nstep; ntop = ntop > 0 ? ntop : 0;
                    ntop = ntop / nstep * nstep;
                }
                else
                {
                    nheight -= nstep; nheight = nheight > 0 ? nheight : 0;
                    nheight = nheight / nstep * nstep;
                }
                break;

            case VK_RIGHT:
                if (!bShiftState)
                {
                    nleft += nstep; nleft = nleft > 0 ? nleft : 0;
                    nleft = nleft / nstep * nstep;
                }
                else
                {
                    nwidth += nstep; nwidth = nwidth > 0 ? nwidth : 0;
                    nwidth = nwidth / nstep * nstep;
                }
                break;

            case VK_DOWN:
                if (!bShiftState)
                {
                    ntop += nstep; ntop = ntop > 0 ? ntop : 0;
                    ntop = ntop / nstep * nstep;
                }
                else
                {
                    nheight += nstep; nheight = nheight > 0 ? nheight : 0;
                    nheight = nheight / nstep * nstep;
                }
                break;
            }


            strLeft.Format(_T("%d"), nleft);
            strTop.Format(_T("%d"), ntop);
            strWidth.Format(_T("%d"), nwidth);
            strHeight.Format(_T("%d"), nheight);

            WndProperty.SetProperty(_T("Left"), strLeft);
            WndProperty.SetProperty(_T("Top") , strTop);
            WndProperty.SetProperty(_T("Width"), strWidth);
            WndProperty.SetProperty(_T("Height"), strHeight);

            ControlsMgt.m_skinResPropertyView.SetProperty(_T("Left"),
                strLeft);
            ControlsMgt.m_skinResPropertyView.SetProperty(_T("Top"),
                strTop);
            ControlsMgt.m_skinResPropertyView.SetProperty(_T("Width"),
                strWidth);
            ControlsMgt.m_skinResPropertyView.SetProperty(_T("Height"),
                strHeight);

            m_wndPreView.UpdateSkinWindow(WndProperty);

        }

        return 1L;
    }

};