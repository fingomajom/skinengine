/********************************************************************
	created:	2009/04/28
	created:	28:4:2009   18:33
	filename: 	d:\Simple\DW_Explorer\DW_Explorer\DWFrame.h
	file path:	d:\Simple\DW_Explorer\DW_Explorer
	file base:	CDWMainFrame.h
	author:		lidengwang@sohu-rd.com
	
	purpose:	
*********************************************************************/

#pragma once

#include "resource.h"
#include "DWFrameUI.h"
#include "DWSuperToolbar.h"
#include "DWFavoriteBar.h"
#include "DWChildFrm.h"
#include "DWStatusBar.h"

//#define __TEST_WEB_WND__
#ifdef __TEST_WEB_WND__
#include "DWWebView.h"
#endif

class CDWMainFrame : 
    public CWindowImpl<CDWMainFrame, CWindow, CFrameWinTraits>,
    public CDWFrameUIImpl<CDWMainFrame>,
    public CMessageFilter,
    public CDWEventCallback
{
    friend class CDWFrameUIImpl<CDWMainFrame>;
public:
    CDWMainFrame(void);
    ~CDWMainFrame(void);

    HWND CreateEx();

    BEGIN_MSG_MAP(CDWMainFrame)

        CHAIN_MSG_MAP(CDWFrameUIImpl<CDWMainFrame>)

        MESSAGE_HANDLER(WM_CREATE  , OnCreate   )
        MESSAGE_HANDLER(WM_DESTROY , OnDestroy  )
        MESSAGE_HANDLER(WM_SIZE    , OnSize     )
        MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus )

        MESSAGE_HANDLER(WM_TABLE_BAR_MSG      , OnTableBarMsg)
        MESSAGE_HANDLER(WM_WEBVIEW_CREATE     , OnWebViewCreate)
        MESSAGE_HANDLER(WM_WEBVIEW_OPENSEARCH , OnWebViewOpenSearch)
        MESSAGE_HANDLER(WM_WINDOWPOSCHANGED   , OnWndPosChanged )

        COMMAND_ID_HANDLER( ID_APP_EXIT          , OnFileExit      )
        COMMAND_ID_HANDLER( ID_FILE_NEW          , OnFileNew       )
        COMMAND_ID_HANDLER( ID_APP_ABOUT         , OnAppAbout      )

    END_MSG_MAP()

protected:

    virtual BOOL PreTranslateMessage(MSG* pMsg);

    void ReRgnWindow();
    void RePositionCtrls();


    CDWChildFrm* OnNewURL ( LPCTSTR pszURL, BOOL bActive = TRUE );
    void OnOpenURL( LPCTSTR pszURL );

    void OnCloseURL ( int nIndex );
    void OnSelectURL( int nIndex );

    virtual LRESULT OnEventMessage( UINT uMsg, WPARAM wParam, LPARAM lParam );

    void GetChildFrmRect(RECT& rcChildFrm);
protected:

    LRESULT OnCreate    (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnDestroy   (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnSize      (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnSetFocus  (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    LRESULT OnWndPosChanged     (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnTableBarMsg       (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnWebViewCreate     (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnWebViewOpenSearch (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnFileNew (WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

public:

    DECLARE_WND_CLASS(_T("DWExplorer_MainFrame"));

protected:

    CDWSuperToolbar m_wndSuperbar;
    CDWFavoriteBar  m_wndFavoriteBar;
    CDWTableBar     m_wndTableBar;
    CDWStatusBar    m_wndStatusBar;
#ifdef __TEST_WEB_WND__
    CDWWebView      m_wndClient;
#endif
    ATL::CAtlList<CDWChildFrm*> m_listChildFrm;
    CDWChildFrm* m_pNowChildFrm;

};
