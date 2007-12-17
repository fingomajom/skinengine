// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "AboutDlg.h"
#include "resource.h"
#include "LeftTreeView.h"
#include "RightListView.h"
#include "ResDocument.h"
#include "EditStringDlg.h"

#define IDC_LEFT_TREE   1000
#define IDC_RIGHT_LIST  1001


class CMainFrame : public CFrameWindowImpl<CMainFrame>, public CUpdateUI<CMainFrame>,
		public CMessageFilter, public CIdleHandler
{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

    CSplitterWindow m_wndSplitter;
    CLeftTreeView   m_wndLeftTreeView;
    CRightListView  m_wndRightListView;

    CWindow         m_wndLastView;

    CStatic         m_wndNoneStatic;

	CCommandBarCtrl m_CmdBar;

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
			return TRUE;

        return FALSE;

		//return m_view.PreTranslateMessage(pMsg);
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
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
		COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
		COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)


        NOTIFY_HANDLER(IDC_LEFT_TREE, TVN_SELCHANGED, OnSelChanged)
        NOTIFY_HANDLER(IDC_RIGHT_LIST, NM_DBLCLK    , OnRightListDbClk)


        COMMAND_ID_HANDLER(ID_FILE_SAVE, OnFileSave)
        COMMAND_ID_HANDLER(ID_FILE_SAVE_AS, OnFileSaveAs)
        MESSAGE_HANDLER(WM_CLOSE, OnClose)
        CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
        REFLECT_NOTIFICATIONS()
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

        m_hWndClient = m_wndSplitter.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
        

        m_wndLeftTreeView.Create(m_wndSplitter, rcDefault, NULL, 
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_HASLINES ,
            WS_EX_CLIENTEDGE, IDC_LEFT_TREE);

		m_wndRightListView.Create(m_wndSplitter, rcDefault, NULL, 
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL, 
            WS_EX_CLIENTEDGE, IDC_RIGHT_LIST );

        m_wndRightListView.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);
        
        m_wndNoneStatic.Create(m_wndSplitter, rcDefault, NULL,  WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);

        m_wndLastView = m_wndNoneStatic;

        m_wndSplitter.SetSplitterPanes(m_wndLeftTreeView, m_wndLastView);


        UpdateLayout();

        RECT rect;
        GetClientRect(&rect);
        m_wndSplitter.SetSplitterPos((rect.right - rect.left) / 4);


		UIAddToolBar(hWndToolBar);
		UISetCheck(ID_VIEW_TOOLBAR, 1);
		UISetCheck(ID_VIEW_STATUS_BAR, 1);

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);


		return 0;
	}

    LRESULT OnSelChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
    {
        LPNMTREEVIEW pNMTREEVIEW = (LPNMTREEVIEW)pnmh;

        if (pNMTREEVIEW != NULL && pNMTREEVIEW->itemNew.hItem != NULL)
        {
            int ntvtype = em_rvt_none;

            ntvtype = m_wndLeftTreeView.GetItemData(pNMTREEVIEW->itemNew.hItem);
            
            SwitchView(ntvtype);
        }

        return DefWindowProc();
    }
    
    void SwitchView(int ntvtype = em_rvt_none)
    {
        m_wndLastView.ShowWindow(SW_HIDE);

        switch(ntvtype)
        {
        case em_rvt_string_table:
            m_wndLastView = m_wndRightListView;

            if ( m_wndRightListView.GetHeader().GetItemCount() == 0 )
            {
                InitStringTableWnd();
            }

            break;
        case em_rvt_none:
        default:
            m_wndLastView = m_wndNoneStatic;
            break;
        }

        m_wndSplitter.SetSplitterPanes(m_wndLeftTreeView, m_wndLastView);
        m_wndLastView.ShowWindow(SW_SHOWDEFAULT);

    }

    void InitStringTableWnd()
    {
        CResDocument& document = CResDocument::Instance();

        std::vector<CStringTableResource::STRINGTABLE_ITEMINFO>& vtList =
            document.GetStringTableList();

        m_wndRightListView.InsertColumn(0, _T("Type"), LVCFMT_LEFT, 60);
        m_wndRightListView.InsertColumn(2, _T("ID"), LVCFMT_LEFT, 150);
        m_wndRightListView.InsertColumn(3, _T("Caption"), LVCFMT_LEFT, 550);

        for (size_t i = 0; i < vtList.size(); i++)
        {
            int nIndex = m_wndRightListView.InsertItem(0xFFFFF, _T("String"));
            m_wndRightListView.SetItemText(nIndex, 1, vtList[i].strIDName );
            m_wndRightListView.SetItemText(nIndex, 2, vtList[i].strValue );
        }
    
        int nIndex = m_wndRightListView.InsertItem(0xFFFFF, _T("New..."));

    }

    LRESULT OnRightListDbClk(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
    {
        int nIndex = m_wndRightListView.GetSelectedIndex();

        if (nIndex < 0 )
            return DefWindowProc();

        CEditStringDlg dlg;

        m_wndRightListView.GetItemText(nIndex, 1, dlg.m_strId);
        m_wndRightListView.GetItemText(nIndex, 2, dlg.m_strCaption);
        
        while (dlg.DoModal() == IDOK)
        {
            for (int i = 0; i < m_wndRightListView.GetItemCount() - 1; i++)
            {
                if (i == nIndex)
                    continue;

                WTL::CString strId;

                m_wndRightListView.GetItemText(i, 1, strId);

                if (strId == dlg.m_strId)
                {
                    m_wndRightListView.SelectItem(i);

                    return DefWindowProc();
                }
            }
            
            if ( nIndex == m_wndRightListView.GetItemCount() - 1 )
            {
                m_wndRightListView.SetItemText(nIndex, 0, _T("String"));
                m_wndRightListView.InsertItem(0xFFFFF, _T("New..."));
            }

            m_wndRightListView.SetItemText(nIndex, 1, dlg.m_strId );
            m_wndRightListView.SetItemText(nIndex, 2, dlg.m_strCaption );

            CResDocument& document = CResDocument::Instance();
            document.SetChanged();

            break;
        }

        return DefWindowProc();
    }



	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		PostMessage(WM_CLOSE);
		return 0;
	}

	LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// TODO: add code to initialize document

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
    LRESULT OnFileSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnFileSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};
