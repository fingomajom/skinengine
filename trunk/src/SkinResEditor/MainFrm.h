// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SkinControlsMgt.h"
#include "SkinResDocument.h"

#include "SkinResStringListView.h"
#include "SkinResImageListView.h"
#include "SkinResEditView.h"
#include "SkinResDialogListView.h"

#define IDC_TREE_VIEW       1000
#define IDC_LIST_VIEW       1001
#define IDC_PROPERTY_VIEW   1002

class CMainFrame : 
    public CFrameWindowImpl<CMainFrame>, 
    public CUpdateUI<CMainFrame>,
	public CMessageFilter, 
    public CIdleHandler,
    public SkinFrame
{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	CCommandBarCtrl m_CmdBar;

    CSplitterWindow    m_wndLRSplitter;
    CHorSplitterWindow m_wndLTBlitter;
    

    SkinResStringListView m_wndResStringListView;
    SkinResDialogListView m_wndResDialogListView;
    CImageResEditDlg      m_wndResImageDlg;
    SkinResEditView       m_wndResEditView;
    

    CMainFrame() : m_wndResEditView(this)
    {    
    }

    virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return FALSE;//CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg);
	}

    virtual HWND SetActiveResultWindow(HWND hWndResult)
    {
        HWND hResult = m_wndLTBlitter.GetSplitterPane(SPLIT_PANE_RIGHT);

        m_wndLRSplitter.SetSplitterPanes(m_wndLTBlitter, hWndResult);

        UpdateLayout();


        return hResult;
    }

    virtual HWND GetResultParentWnd()
    {
        return m_wndLRSplitter;
    }

    virtual void OnReUpdateResView()
    {

        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        ControlsMgt.m_skinTreeControlView.CTreeViewCtrl::DeleteAllItems();


        ControlsMgt.m_skinTreeControlView.InsertControlItem(
            TVI_ROOT, ControlsMgt.m_resDocument.GetFileName(), 0, &m_wndResEditView, 0);

        HTREEITEM hTreeItem = ControlsMgt.m_skinTreeControlView.InsertControlItem(
            ControlsMgt.m_skinTreeControlView.GetRootItem(),
            _T("StringTable"), 0, &m_wndResStringListView, 0);

        hTreeItem = ControlsMgt.m_skinTreeControlView.InsertControlItem(
            ControlsMgt.m_skinTreeControlView.GetRootItem(),
            _T("Images"), 0, &m_wndResImageDlg.m_wndResImageList, 0);

        hTreeItem = ControlsMgt.m_skinTreeControlView.InsertControlItem(
            ControlsMgt.m_skinTreeControlView.GetRootItem(),
            _T("Dialog"), 0, &m_wndResDialogListView, 0);


        ControlsMgt.m_skinTreeControlView.Expand(
            ControlsMgt.m_skinTreeControlView.GetRootItem());

        ControlsMgt.m_skinTreeControlView.SelectItem(NULL);
        ControlsMgt.m_skinTreeControlView.SelectItem(
            ControlsMgt.m_skinTreeControlView.GetRootItem());

    }

	virtual BOOL OnIdle()
	{
		UIUpdateToolBar();
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)

		MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_CLOSE , OnClose)

		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
        COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
        COMMAND_ID_HANDLER(ID_FILE_OPEN, OnFileOpen)
        COMMAND_ID_HANDLER(ID_FILE_SAVE, OnFileSave)
        COMMAND_ID_HANDLER(ID_FILE_SAVE_AS, OnFileSaveAs)

		COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
		COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)

        /************************************************************************/
        /* Notify  消息过滤                                                     */
        /************************************************************************/
        bHandled = TRUE; 
        lResult  = ReflectNotifications(uMsg, wParam, lParam, bHandled);
        if(bHandled) 
            return TRUE;
        /************************************************************************/
        /*                                                                      */
        /************************************************************************/

        
        CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)

	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// create command bar window
		HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
		// attach menu
		m_CmdBar.AttachMenu(GetMenu());
		// load command bar images
		m_CmdBar.LoadImages(IDR_MAINFRAME);
		// remove old menu
		SetMenu(NULL);

		HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

		CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
		AddSimpleReBarBand(hWndCmdBar);
		AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

		CreateSimpleStatusBar();

		UIAddToolBar(hWndToolBar);
		UISetCheck(ID_VIEW_TOOLBAR, 1);
		UISetCheck(ID_VIEW_STATUS_BAR, 1);

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

        OnCreateChildWindow();

		return 0;
	}

    LRESULT OnCreateChildWindow()
    {

        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        ControlsMgt.m_piSkinFrame = this;

        m_hWndClient = m_wndLRSplitter.Create(m_hWnd, rcDefault, NULL, 
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
            WS_EX_CONTROLPARENT);

        m_wndLTBlitter.Create(m_wndLRSplitter, rcDefault, NULL, 
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
            WS_EX_CONTROLPARENT);

        ControlsMgt.m_skinTreeControlView.Create(m_wndLTBlitter, rcDefault, NULL, 
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_HASLINES ,
            WS_EX_CLIENTEDGE | WS_EX_CONTROLPARENT, IDC_TREE_VIEW);
        
        ControlsMgt.m_skinResPropertyView.Create(m_wndLTBlitter, rcDefault, NULL, 
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_SINGLESEL | LVS_NOSORTHEADER,
            WS_EX_CLIENTEDGE | WS_EX_CONTROLPARENT, IDC_PROPERTY_VIEW);

        m_wndLRSplitter.SetSplitterPanes(m_wndLTBlitter, NULL);
        m_wndLTBlitter.SetSplitterPanes(ControlsMgt.m_skinTreeControlView, ControlsMgt.m_skinResPropertyView);
        
        m_wndResImageDlg.Create(m_wndLRSplitter);

        UpdateLayout();

        RECT rect;
        GetClientRect(&rect);
        m_wndLRSplitter.SetSplitterPos((rect.right - rect.left) / 4);
        m_wndLTBlitter.SetSplitterPos((rect.bottom - rect.top) / 3);
        

        BOOL bHandled = FALSE;
        OnFileNew(0, 0, 0, bHandled);


        return TRUE;
    }

	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		PostMessage(WM_CLOSE);
		return 0;
	}

	LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
	{
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();


        if (ControlsMgt.m_resDocument.Modify())
        {
            WTL::CString strMsg;

            strMsg.Format(_T("文件 [%s] 已更改，需要保存文件吗？"), ControlsMgt.m_resDocument.GetFileName());
                
            int nRet = MessageBox(strMsg, _T("提示"), MB_YESNOCANCEL | MB_ICONQUESTION);
            
            if (nRet == IDYES)
            {
                OnFileSave(0, 0, 0, bHandled);
            }
            else if (nRet == IDCANCEL)
            {
                return 0;
            }
        }

        ControlsMgt.m_resDocument.NewDocument();

        OnReUpdateResView();

		return 0;
	}

    LRESULT OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        if (ControlsMgt.m_resDocument.Modify())
        {
            WTL::CString strMsg;

            strMsg.Format(_T("文件 [%s] 已更改，需要保存文件吗？"), ControlsMgt.m_resDocument.GetFileName());

            int nRet = MessageBox(strMsg, _T("提示"), MB_YESNOCANCEL | MB_ICONQUESTION);

            if (nRet == IDYES)
            {
                OnFileSave(0, 0, 0, bHandled);
            }
            else if (nRet == IDCANCEL)
            {
                return 0;
            }
        }

        CFileDialog saveDlg(TRUE, _T("xml"), _T(""),
            OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,// | OFN_ALLOWMULTISELECT, 
            _T("xml file(*.xml)\0*.xml\0所有文件(*.*)\0*.*\0\0"));

        saveDlg.m_ofn.lpstrTitle = (_T("打开"));

        if (saveDlg.DoModal() != IDOK)
            return 0;

        if (!ControlsMgt.m_resDocument.OpenDocument(saveDlg.m_ofn.lpstrFile))
        {
            MessageBox(_T("打开文件失败"), _T("失败"));
            return FALSE;
        }

        OnReUpdateResView();

        ControlsMgt.m_skinTreeControlView.SetItemText(
            ControlsMgt.m_skinTreeControlView.GetRootItem(),
            ControlsMgt.m_resDocument.GetFileName());


        return 0;
    }

    LRESULT OnFileSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        if (!ControlsMgt.m_resDocument.GetFileName().CompareNoCase(IDS_EMPTY_FILENAME))
        {
            OnFileSaveAs(0, 0, 0, bHandled);

            return 0;

        }

        ControlsMgt.m_resDocument.SaveDocument(ControlsMgt.m_resDocument.GetFileName());

        return 0;
    } 

    LRESULT OnFileSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();

        CFileDialog saveDlg(FALSE, _T("xml"), _T(""),
            OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,// | OFN_ALLOWMULTISELECT, 
            _T("xml file(*.xml)\0*.xml\0所有文件(*.*)\0*.*\0\0"));

        saveDlg.m_ofn.lpstrTitle = (_T("另存为"));

        if (saveDlg.DoModal() != IDOK)
            return 0;

        ControlsMgt.m_resDocument.SaveDocument(saveDlg.m_ofn.lpstrFile);

        ControlsMgt.m_skinTreeControlView.SetItemText(
            ControlsMgt.m_skinTreeControlView.GetRootItem(),
            ControlsMgt.m_resDocument.GetFileName());

        return 0;
    }

	LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		static BOOL bVisible = TRUE;	// initially visible
		bVisible = !bVisible;
		CReBarCtrl rebar = m_hWndToolBar;
		int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
		rebar.ShowBand(nBandIndex, bVisible);
		UISetCheck(ID_VIEW_TOOLBAR, bVisible);
		UpdateLayout();
		return 0;
	}

	LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
		::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
		UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
		UpdateLayout();
		return 0;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}

    LRESULT CMainFrame::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        SkinControlsMgt& ControlsMgt = SkinControlsMgt::Instance();


        if (ControlsMgt.m_resDocument.Modify())
        {
            WTL::CString strMsg;

            strMsg.Format(_T("文件 [%s] 已更改，需要保存文件吗？"), ControlsMgt.m_resDocument.GetFileName());

            int nRet = MessageBox(strMsg, _T("提示"), MB_YESNOCANCEL | MB_ICONQUESTION);

            if (nRet == IDYES)
            {
                OnFileSave(0, 0, 0, bHandled);
            }
            else if (nRet == IDCANCEL)
            {
                return 0;
            }

        }

        return DefWindowProc();
    }

    /************************************************************************
    /*  @brief  ...
    /*  @param  ...
    /*  @return ...
    /*  @remark ...
    /************************************************************************/
    LRESULT CMainFrame::ReflectNotifications(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        HWND hWndChild = NULL;

        switch(uMsg)
        {
        case WM_NOTIFY:
            hWndChild = ((LPNMHDR)lParam)->hwndFrom;
            break;
        case WM_DRAWITEM:
            if(wParam)	// not from a menu
                hWndChild = ((LPDRAWITEMSTRUCT)lParam)->hwndItem;
            break;
        }

        if(hWndChild == NULL)
        {
            bHandled = FALSE;
            return 1;
        }

        ATLASSERT(::IsWindow(hWndChild));

        bHandled = FALSE;
        return ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
    }

};
