/********************************************************************
	created:	2009/05/03
	created:	3:5:2009   15:15
	filename: 	d:\lidengwang\se\trunk\src\dw_explorer\DWComDef.h
	file path:	d:\lidengwang\se\trunk\src\dw_explorer
	file base:	DWComDef
	file ext:	h
	author:		lidengwang@s-zone.com
	
	purpose:	
*********************************************************************/

#pragma once


#define WM_TABLE_BAR_MSG        (WM_USER + 1000)

    #define TGM_SYS_BTN_CLICK       1
    #define TGM_ITEM_CLOSE_CLICK    2
    #define TGM_SELECT_CHANGE       3



#define WM_CREATE_WEB_WND       (WM_USER + 1100)
#define WM_FAV_ICON_REFLASH     (WM_USER + 1110)

//#define WM_WEBWND_INFO_CHANGED  (WM_USER + 1200)
#define WM_WEBVIEW_CREATE       (WM_USER + 1201)
#define WM_WEBVIEW_OPENURL      (WM_USER + 1202)

#define WM_WEBVIEW_SETTITLE     (WM_USER + 1210)
#define WM_WEBVIEW_SETURL       (WM_USER + 1211)
#define WM_WEBVIEW_SETSTATUS    (WM_USER + 1212)



#define WM_USER_GET_WEBBROWSER2_CROSS_THREAD    (WM_USER + 1500)
#define WM_USER_GET_WEBBROWSER2_CROSS_PROCESS   (WM_USER + 1501)
#define WM_USER_MULTI_PROCESS_GET_STREAM		(WM_USER + 1502)



#define CLASS_NAME_IE_SERVER            _T("Internet Explorer_Server")
#define BLANK_URL                       L"about:blank"



#define ID_TOOL_ADDR_DROPDOWN       10
#define ID_TOOL_SERACH_DROPDOWN     11

inline LRESULT SendCopyData( HWND hWnd, UINT uMsg, const void* lpBuf, DWORD dwLen )
{
    COPYDATASTRUCT cds = { uMsg, dwLen, const_cast<void*>(lpBuf) };

    return SendMessage( hWnd, WM_COPYDATA, 0, (WPARAM)&cds );
}