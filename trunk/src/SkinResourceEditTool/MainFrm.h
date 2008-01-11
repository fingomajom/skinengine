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
#include "ImageResEditDlg.h"

#define IDC_LEFT_TREE   1000
#define IDC_RIGHT_LIST  1001

#include <skinwin.h>
#include "skinwincreator.h"

class CMainFrame : 
    public CFrameWindowImpl<CMainFrame, SkinWindow >, 
    public CUpdateUI<CMainFrame>,
	public CMessageFilter, 
    public CIdleHandler
{
    typedef CFrameWindowImpl<CMainFrame, SkinWindow > TBase;

public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

    CSplitterWindow m_wndSplitter;
    CLeftTreeView   m_wndLeftTreeView;
    CRightListView  m_wndRightListView;
    
    CImageResEditDlg m_wndImageEditDlg;

    CWindow         m_wndLastView;

    CStatic         m_wndNoneStatic;

	CCommandBarCtrl m_CmdBar;

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if(TBase::PreTranslateMessage(pMsg))
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

        COMMAND_ID_HANDLER(ID_FILE_SAVE, OnFileSave)
        COMMAND_ID_HANDLER(ID_FILE_SAVE_AS, OnFileSaveAs)
        MESSAGE_HANDLER(WM_CLOSE, OnClose)
        COMMAND_ID_HANDLER(ID_FILE_OPEN, OnFileOpen)
        CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP( TBase )
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
            

        SkinWindowCreator::Instance();
        //CWindow * pthis = const_cast<CMainFrame*>(this);

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
        case em_rvt_image:
            
            if (m_wndImageEditDlg.m_hWnd == NULL)
            {
                m_wndImageEditDlg.Create(m_wndSplitter);

                m_wndImageEditDlg.LoadResFromDocumnet();
            }

            m_wndLastView = m_wndImageEditDlg;
            

            break;

        case em_rvt_string_table:

            if (m_wndRightListView.m_hWnd == NULL)
            {
                m_wndRightListView.Create(m_wndSplitter, rcDefault, NULL, 
                    WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL, 
                    WS_EX_CLIENTEDGE, IDC_RIGHT_LIST );

                m_wndRightListView.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);

                m_wndRightListView.LoadResFromDocumnet();
            }

            m_wndLastView = m_wndRightListView;

            break;
        case em_rvt_none:
        default:
            m_wndLastView = m_wndNoneStatic;
            break;
        }

        m_wndSplitter.SetSplitterPanes(m_wndLeftTreeView, m_wndLastView);
        m_wndLastView.ShowWindow(SW_SHOWDEFAULT);

    }


	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		PostMessage(WM_CLOSE);
		return 0;
	}

	LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
        m_wndRightListView.ClearRes();
        m_wndImageEditDlg.ClearRes();


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
    LRESULT OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
