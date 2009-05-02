/********************************************************************
	created:	2009/04/24
	created:	24:4:2009   14:56
	filename: 	d:\Simple\quickserach\quickserach\QSearchPopWnd.h
	file path:	d:\Simple\quickserach\quickserach
	file base:	QSearchPopWnd.h
	author:		lidengwang@sohu-rd.com
	
	purpose:	
*********************************************************************/

#pragma once

#include "resource.h"
#include <atlcoll.h>
#include "QuickSearchProvider.h"

class IWindowLayer
{
public:
    
    virtual ~IWindowLayer() {}

    virtual BOOL Create( HWND hWndParent ) = 0;
    virtual BOOL Destroy() = 0;

    virtual BOOL ShowWindow( BOOL bShow   , HWND hWndParent ) = 0;
    virtual BOOL MoveWindow( RECT rcClient, HWND hWndParent ) = 0;

    virtual void OnPaint   ( HDC hDC, RECT rcClient ) = 0;

    virtual BOOL IsWindowVisible() = 0;
    virtual BOOL GetClientSize( SIZE& sizeClient ) = 0;

    virtual BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
        LRESULT& lResult, DWORD dwMsgMapID) = 0;

};

enum{
    em_cid_qstoolbar    = 0,
    em_cid_sogoutitle   = 1,
    em_cid_qsresultview = 2,
    em_cid_settingview  = 3,
    em_cid_settoolbar   = 4,

    em_cid_count        = 5
};


#define IDC_KEYWORD_EDIT        2000
#define IDC_QSEARCH_BTN         2001
#define IDC_COPY_BTN            2002
#define IDC_SETTING_BTN         2003
#define IDC_DOCT_BTN            2004

#define IDC_RESULT_EDIT_BTN     2010


#define IDC_SETTING_CBOX        2020
#define IDC_RETURN_BTN          2030

//#define IDC_SETTING_BTN         2040


class CQSearchPopWnd : 
    public CDialogImpl< CQSearchPopWnd >,
    public CComObjectRootEx<CComSingleThreadModel>,
    public IQuickSearchEventCallback
{
public:
    enum { IDD = IDD_QSEARCH_DIALOG };

    CQSearchPopWnd();


    BOOL ShowQSPopWnd( POINT pt, LPCTSTR pszQSText = NULL );    
    BOOL ShowQSPopWnd_ForQSBar( POINT pt );
    
    
    virtual HRESULT STDMETHODCALLTYPE OnQSearchResult( 
        /* [in ] */ LPCWSTR   pszResult,
        /* [in ] */ LPQSPINFO pQSInfo );

    BEGIN_COM_MAP(CQSearchPopWnd)
        COM_INTERFACE_ENTRY(IQuickSearchEventCallback)
        COM_INTERFACE_ENTRY(IUnknown)
    END_COM_MAP()

public:

    void ClearResult();

    BOOL QSearchKeyword( LPCTSTR pszKeyword );
    void ShowChildWindow( int id, BOOL bShow );    
    void ShowAutoSizeWnd();
    void SetTransparency( BYTE byteTran );

public:


    BEGIN_MSG_MAP(CQSearchPopWnd)

        MESSAGE_HANDLER( WM_INITDIALOG , OnInitDialog  )
        MESSAGE_HANDLER( WM_PAINT      , OnPaint       )
        MESSAGE_HANDLER( WM_LBUTTONDOWN, OnLButtonDown )
        MESSAGE_HANDLER( WM_MOUSEMOVE  , OnMouseMove   )

        MESSAGE_HANDLER( WM_KILLFOCUS  , OnKillFocus   )

        MESSAGE_HANDLER( WM_SHOWWINDOW , OnShowWindow  )

        MESSAGE_HANDLER( WM_CTLCOLORBTN   , OnCtlColor )
        MESSAGE_HANDLER( WM_CTLCOLORSTATIC, OnCtlColor )

        COMMAND_HANDLER( IDC_QSEARCH_BTN, BN_CLICKED, OnQSearchBtnClicked )
        COMMAND_HANDLER( IDC_COPY_BTN   , BN_CLICKED, OnCopyBtnClicked )
        COMMAND_HANDLER( IDC_SETTING_BTN, BN_CLICKED, OnSettingBtnClicked )
        COMMAND_HANDLER( IDC_DOCT_BTN   , BN_CLICKED, OnDoctBtnClicked )

        COMMAND_RANGE_CODE_HANDLER( IDC_SETTING_CBOX, IDC_SETTING_CBOX + 9  , BN_CLICKED, OnSettubgCBBtnClicked )
        COMMAND_HANDLER( IDC_RETURN_BTN , BN_CLICKED, OnReturnBtnClicked )

        for ( size_t idx = 0; idx < m_vtChildWindow.GetCount(); idx++ )
        {
            IWindowLayer* piChildWnd = m_vtChildWindow[idx];
            if ( piChildWnd != NULL )
                piChildWnd->ProcessWindowMessage( hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID );
        }

    END_MSG_MAP();

    BOOL IsDialogMessage(LPMSG lpMsg);


    LRESULT OnInitDialog (UINT /* uMsg */, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT OnPaint      (UINT /* uMsg */, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT OnLButtonDown(UINT /* uMsg */, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT OnKillFocus  (UINT /* uMsg */, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT OnCtlColor   (UINT /* uMsg */, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

    LRESULT OnShowWindow (UINT /* uMsg */, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT OnMouseMove  (UINT /* uMsg */, WPARAM wParam, LPARAM lParam, BOOL &bHandled);


    LRESULT OnQSearchBtnClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnCopyBtnClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnSettingBtnClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnDoctBtnClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    LRESULT OnSettubgCBBtnClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnReturnBtnClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

protected:

    virtual void OnFinalMessage(HWND /*hWnd*/)
    {
        m_spQSService.Release();
    }

public:

    CComPtr<IQuickSearchService> m_spQSService;

    COLORREF m_clrBorder;
    COLORREF m_clrWindow;

    BYTE     m_byteTran;
    BOOL     m_bMouseIn;
    BOOL     m_bDoct;

    CToolTipCtrl m_tooltip;
    //////////////////////////////////////////////////////////////////////////

    ATL::CAtlArray<IWindowLayer*> m_vtChildWindow;

    BOOL m_bChildShow[em_cid_count];
    ATL::CString m_strLastTitle;
};