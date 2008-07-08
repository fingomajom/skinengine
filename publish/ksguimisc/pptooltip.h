//
//	Class:		CPPTooltip
//
//	Compiler:	Visual C++
//	Tested on:	Visual C++ 6.0
//				Visual C++ .NET 2003
//
//	Version:	See GetVersionC() or GetVersionI()
//
//	Created:	xx/xxxx/2004
//	Updated:	21/November/2004
//
//	Author:		Eugene Pustovoyt	pustovoyt@mail.ru
//
//	Disclaimer
//	----------
//	THIS SOFTWARE AND THE ACCOMPANYING FILES ARE DISTRIBUTED "AS IS" AND WITHOUT
//	ANY WARRANTIES WHETHER EXPRESSED OR IMPLIED. NO REPONSIBILITIES FOR POSSIBLE
//	DAMAGES OR EVEN FUNCTIONALITY CAN BE TAKEN. THE USER MUST ASSUME THE ENTIRE
//	RISK OF USING THIS SOFTWARE.
//
//	Terms of use
//	------------
//	THIS SOFTWARE IS FREE FOR PERSONAL USE OR FREEWARE APPLICATIONS.
//	IF YOU WISH TO THANK MY WORK, YOU MAY DONATE ANY SUM OF MONEY TO ME 
//  FOR SUPPORT OF DEVELOPMENT OF THIS CLASS.
//	IF YOU USE THIS SOFTWARE IN COMMERCIAL OR SHAREWARE APPLICATIONS YOU
//	ARE GENTLY ASKED TO DONATE ANY SUM OF MONEY TO THE AUTHOR:
//
//
//--- History ------------------------------ 
// 2004/03/01 *** Releases version 2.0 ***
//------------------------------------------
//		2004/04/04 [ADD] Added method SetCssStyles(DWORD dwIdCssStyle, LPCTSTR lpszPathDll /* = NULL */)
//		2004/04/14 [FIX] Fixed correct drawing for some tooltip's directions
//		2004/04/15 [FIX] Fixed changing a z-order of the some windows by show a tooltip on Win9x
//		2004/04/27 [FIX] Corrected a work with a tooltip's directions with a large tooltip
//		2004/04/28 [ADD] Disables a message translation if object was't created (thanks to Stoil Todorov)
//		2004/07/02 [UPD] Changes a GetWndFromPoint mechanism of the window's searching
//		2004/09/01 [ADD] New SetMaxTipWidth method was added
//		2004/10/12 [FIX] Now a tooltip has a different methods to show a menu's tooltip and other 
//							control's tooltip
//------------------------------------------
// 2004/11/19 *** Releases version 2.1 ***
//------------------------------------------
//		2004/11/30 [FIX] Corrected the debug window drawing
//		           [FIX] Changes a GetWndFromPoint mechanism of the window's searching
////////////////////////////////////////////////////////////////////
//
// "SmoothMaskImage" and "GetPartialSums" functions by Denis Sarazhinsky (c)2003
// Modified by Eugene Pustovoyt to use with image's mask instead of full color image.
//
/////////////////////////////////////////////////////////////////////
//

#ifndef _PPTOOLTIP_H
#define _PPTOOLTIP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4786)
#include "ksguicommon/drawer/PPHtmlDrawer.h"
//#include "multimon.h"
#include "atlcrack.h"
#include <vector>
#include <map>

//ENG: Comments a next line if you never use a tooltip for a menu
#define PPTOOLTIP_USE_MENU

//The 
#define UDM_TOOLTIP_FIRST		   (WM_USER + 100)
#define UDM_TOOLTIP_DISPLAY		   (UDM_TOOLTIP_FIRST) //User was changed the data
#define UDM_TOOLTIP_REPAINT		   (UDM_TOOLTIP_FIRST + 1)
#define UDM_TOOLTIP_HIDING		   (UDM_TOOLTIP_FIRST + 2)

//The behaviours
#define PPTOOLTIP_MULTIPLE_SHOW			0x00000001 //Multiple show for single control
#define PPTOOLTIP_TRACKING_MOUSE		0x00000002 //Tracking for mouse
#define PPTOOLTIP_CLOSE_LEAVEWND		0x00000004 //Close tooltip if mouse leave the control
#define PPTOOLTIP_NOCLOSE_OVER			0x00000008 //No close tooltip if mouse over him
#define PPTOOLTIP_DISABLE_AUTOPOP		0x00000010 //Disables autopop tooltip from timer
//#define PPTOOLTIP_NOCLOSE_MOUSEDOWN		0x00000020 //Disables autopop tooltip from timer

//The masks
#define PPTOOLTIP_MASK_STYLES			0x0001	// The styles for the tooltip gets from the structures
#define PPTOOLTIP_MASK_EFFECT			0x0002	// The background's type for the tooltip gets from the structures
#define PPTOOLTIP_MASK_COLORS			0x0004	// The background's colors for the tooltip gets from the structures
#define PPTOOLTIP_MASK_DIRECTION		0x0008  // The align for the tooltip gets from the structures
#define PPTOOLTIP_MASK_BEHAVIOUR		0x0010  // The behaviour for the tooltip gets from the structures
#define PPTOOLTIP_MASK_TRANSPARENCY		0x0020  // 

//The constants of the timers
#define PPTOOLTIP_TIME_INITIAL			TTDT_INITIAL
#define PPTOOLTIP_TIME_AUTOPOP			TTDT_AUTOPOP
#define PPTOOLTIP_TIME_FADEIN			4
#define PPTOOLTIP_TIME_FADEOUT			5
#define PPTOOLTIP_TIME_ANIMATION		6

//Tooltip's directions
#define PPTOOLTIP_TOPEDGE_LEFT			0x00
#define PPTOOLTIP_TOPEDGE_RIGHT			0x01
#define PPTOOLTIP_TOPEDGE_CENTER		0x02
#define PPTOOLTIP_BOTTOMEDGE_LEFT		0x10
#define PPTOOLTIP_BOTTOMEDGE_RIGHT		0x11
#define PPTOOLTIP_BOTTOMEDGE_CENTER		0x12
#define PPTOOLTIP_LEFTEDGE_TOP			0x20
#define PPTOOLTIP_LEFTEDGE_BOTTOM		0x21
#define PPTOOLTIP_LEFTEDGE_VCENTER		0x22
#define PPTOOLTIP_RIGHTEDGE_TOP			0x30
#define PPTOOLTIP_RIGHTEDGE_BOTTOM		0x31
#define PPTOOLTIP_RIGHTEDGE_VCENTER		0x32
//----- Old version (1.x) ---------
#define PPTOOLTIP_LEFT_TOP				PPTOOLTIP_TOPEDGE_LEFT
#define PPTOOLTIP_RIGHT_TOP				PPTOOLTIP_TOPEDGE_RIGHT
#define PPTOOLTIP_LEFT_BOTTOM			PPTOOLTIP_BOTTOMEDGE_LEFT
#define PPTOOLTIP_RIGHT_BOTTOM			PPTOOLTIP_BOTTOMEDGE_RIGHT

#ifdef PPTOOLTIP_USE_MENU
//ENG: Anchor's position about a menu item
#define PPTOOLTIP_MENU_LEFT		0x00
#define PPTOOLTIP_MENU_RIGHT	0x01
#define PPTOOLTIP_MENU_CENTER	0x02
#define PPTOOLTIP_MENU_TOP		0x00
#define PPTOOLTIP_MENU_BOTTOM	0x10
#define PPTOOLTIP_MENU_VCENTER  0x20

#define PPTOOLTIP_MENU_HMASK	0x0F
#define PPTOOLTIP_MENU_VMASK	0xF0
#endif //PPTOOLTIP_USE_MENU

#define COMPILE_MULTIMON_STUBS

#define TIMER_HIDE		0x101 //the identifier of the timer for hide the tooltip
#define TIMER_SHOWING	0x102 //the identifier of the timer for tooltip's fade in
#define TIMER_SHOW		0x100 //the identifier of the timer for show the tooltip
#define TIMER_HIDING	0x103 //the identifier of the timer for tooltip's fade out
#define TIMER_ANIMATION 0x104 //the identifier of the timer for animation

#define PERCENT_STEP_FADEIN		20 //How mush percent will adding during fade in
#define PERCENT_STEP_FADEOUT	20 //How mush percent will adding during fade out
#define PERCENT_MAX_TRANSPARENCY 100 //How mush percent by maximum transparency
#define PERCENT_MIN_TRANSPARENCY 0 //How mush percent by minimum transparency

#define MAX_LENGTH_DEBUG_STRING 25 //


// This info structure
typedef struct tagPPTOOLTIP_INFO
{
    UINT		nIDTool;		// ID of tool   
    CRect		rectBounds;		// Bounding rect for toolinfo to be displayed
	WTL::CString		sTooltip;		// The string of the tooltip
	UINT        nMask;			// The mask 
	UINT		nStyles;		// The tooltip's styles
	UINT        nDirection;		// Direction display the tooltip relate cursor point
	UINT		nEffect;		// The color's type or effects
	UINT        nBehaviour;		// The tooltip's behaviour
	BYTE        nGranularity;	// The effect's granularity
	BYTE        nTransparency;	// The factor of the window's transparency (0-100)
	COLORREF	crBegin;		// Begin Color
	COLORREF    crMid;			// Mid Color
	COLORREF	crEnd;			// End Color
} PPTOOLTIP_INFO;

// This structure sent to PPTooltip parent in a WM_NOTIFY message
typedef struct tagNM_PPTOOLTIP_DISPLAY {
    NMHDR hdr;
	HWND hwndTool;
	LPPOINT pt;
	PPTOOLTIP_INFO * ti;
} NM_PPTOOLTIP_DISPLAY;


//////////////////////////////////////////////////////////////////////////
// Note that windows are enumerated in top-down Z-order, so the menu
// window should always be the first one found.
//
static BOOL CALLBACK MyEnumProc(HWND hwnd, LPARAM lParam)
{
	TCHAR buf[16];
	GetClassName(hwnd, buf, sizeof(buf) / sizeof(TCHAR));
	if (_tcscmp(buf, _T("#32768")) == 0)  // special classname for menus
	{
		*((HWND*)lParam) = hwnd;	 // found it
		return FALSE;
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPPToolTip window
class CPPToolTip : public CWindowImpl<CPPToolTip>
{
//	friend BOOL CALLBACK EnumChildWinF(HWND hwnd, LPARAM lParam);
// Construction
public:
	DECLARE_WND_CLASS(_T("HTML_TOOLTIP_EX"))
	typedef CWindowImpl<CPPToolTip, CWindow>		_Base;

	BOOL Create(HWND hWndParent, BOOL bBalloon  = TRUE) 
	{
		ATLTRACE(_T("Create\n"));

		ASSERT(hWndParent);

		DWORD dwStyle = WS_POPUP; 
		DWORD dwExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
		m_hParentWnd = hWndParent;

		if (!_Base::Create(hWndParent, 0, NULL, dwStyle, dwExStyle))
			return FALSE;

		//
		SetDefaultSizes(bBalloon);
		m_drawer.SetCallbackRepaint(m_hWnd, UDM_TOOLTIP_REPAINT);
		SetDelayTime(PPTOOLTIP_TIME_ANIMATION, 100);

		return TRUE;
	}


	CPPToolTip()
	{
		// Default values
		m_dwTimeAutoPop = 5000;
		m_dwTimeInitial = 500;
		m_dwTimeFadeIn = 500;
		m_dwTimeFadeOut = 500;
		m_dwBehaviour = 0; //PPTOOLTIP_CLOSE_LEAVEWND | PPTOOLTIP_NOCLOSE_OVER;	 //The tooltip's behaviour
		m_dwEffectBk = 0;
		m_dwDirection = 0;
		m_dwStyles = 0;
		m_nGranularity = 0;
		m_nTransparency = 0;
		m_bDelayNextTool = FALSE;
		m_dwShowEffect = SHOWEFFECT_FADEINOUT;
		m_dwHideEffect = SHOWEFFECT_FADEINOUT;

		m_nTooltipState = PPTOOLTIP_STATE_HIDEN;
		m_nTooltipType = PPTOOLTIP_NORMAL;
		m_nNextTooltipType = PPTOOLTIP_NORMAL;

		m_ptOriginal.x = m_ptOriginal.y = 0;

		m_rcCurTool.SetRectEmpty();

		m_hwndDisplayedTool = NULL;

		m_hBitmapBk = NULL;
		m_hUnderTooltipBk = NULL;

		m_hbrBorder = NULL;
		m_hrgnTooltip = NULL;

		SetColorBk(::GetSysColor(COLOR_INFOBK));
		SetBorder(::GetSysColor(COLOR_INFOTEXT));
		EnableHyperlink();
		SetNotify(FALSE);
		SetDefaultSizes();
		SetDirection();
		SetBehaviour();
		SetDebugMode(FALSE);
		SetMaxTipWidth(0);
		//	EnableTextWrap(FALSE);
		SetDelayTime(PPTOOLTIP_TIME_INITIAL, 500);
		SetDelayTime(PPTOOLTIP_TIME_AUTOPOP, 5000);
		SetDelayTime(PPTOOLTIP_TIME_FADEIN, 0);
		SetDelayTime(PPTOOLTIP_TIME_FADEOUT, 0);
		SetTooltipShadow(6, 6);

#ifdef PPTOOLTIP_USE_MENU
		MenuToolPosition();
#endif //PPTOOLTIP_USE_MENU
	}

	virtual ~CPPToolTip()
	{
		_FreeResources();
		RemoveAllTools();
		HideBorder();
	}

// Attributes
public:
	enum {	PPTTSZ_ROUNDED_CX = 0,
			PPTTSZ_ROUNDED_CY,
			PPTTSZ_MARGIN_CX,
			PPTTSZ_MARGIN_CY,
			PPTTSZ_WIDTH_ANCHOR,
			PPTTSZ_HEIGHT_ANCHOR,
			PPTTSZ_MARGIN_ANCHOR,
			PPTTSZ_OFFSET_ANCHOR_CX,
			PPTTSZ_OFFSET_ANCHOR_CY,

			PPTTSZ_MAX_SIZES
		};

	enum {	SHOWEFFECT_NONE = 0,
			SHOWEFFECT_FADEINOUT,
			
			SHOWEFFECT_MAX
		};

	// Operations
private:
	enum TooltipState 
	{  
		PPTOOLTIP_STATE_HIDEN = 0,
		PPTOOLTIP_STATE_SHOWING,
		PPTOOLTIP_STATE_SHOWN,
		PPTOOLTIP_STATE_HIDING
	};

	enum TooltipType
	{
		PPTOOLTIP_NORMAL = 0,
		PPTOOLTIP_HELP,
		PPTOOLTIP_MENU
	};

	CPPHtmlDrawer m_drawer; //HTML drawer object
	
	HWND m_hParentWnd; // The handle of the parent window
	HWND m_hNotifyWnd; // The handle of the notified window

	BOOL m_bHyperlinkEnabled;
	BOOL m_bDebugMode;

	POINT m_ptOriginal;

	// Info about last displayed tool
	HWND  m_hwndDisplayedTool;
	TooltipType m_nTooltipType;
	PPTOOLTIP_INFO m_tiDisplayed; //Info about displayed tooltip

	// Info about last displayed tool
	BOOL  m_bDelayNextTool;
	BOOL  m_bNextToolExist;
	HWND  m_hwndNextTool;
	TooltipType m_nNextTooltipType;
	PPTOOLTIP_INFO m_tiNextTool; //Info about next tooltip
	
	// Info about current tool
	CRect m_rcCurTool;
	DWORD m_dwCurDirection;
	BYTE  m_dwCurTransparency;
	TooltipState  m_nTooltipState;

	//Colors
	COLORREF m_clrBeginBk;
	COLORREF m_clrMidBk;
	COLORREF m_clrEndBk;

	//Background
	HBITMAP m_hBitmapBk; //A bitmap with tooltip's background only
	HBITMAP m_hUnderTooltipBk;

	//Border of the tooltip
	HBRUSH m_hbrBorder;
	SIZE m_szBorder;

	//Shadow of the tooltip
	BOOL m_bGradientShadow;
	SIZE m_szOffsetShadow;
	SIZE m_szDepthShadow;
	BYTE m_nDarkenShadow;

	HRGN m_hrgnTooltip;

	//Default values for the window
	DWORD m_dwTimeAutoPop; //Retrieve the length of time the tool tip window remains visible if the pointer is stationary within a tool's bounding rectangle
	DWORD m_dwTimeInitial; //Retrieve the length of time the pointer must remain stationary within a tool's bounding rectangle before the tool tip window appears
	DWORD m_dwTimeFadeIn;
	DWORD m_dwTimeFadeOut;

	DWORD m_dwBehaviour;   //The tooltip's behaviour
	DWORD m_dwEffectBk;
	DWORD m_dwDirection;   //The default tooltip's direction
	DWORD m_dwStyles;
	BYTE  m_nGranularity;
	BYTE  m_nTransparency; //The current value of transparency
	DWORD m_dwShowEffect; //
	DWORD m_dwHideEffect;
	int  m_nSizes [PPTTSZ_MAX_SIZES]; //All sizes 

#ifdef PPTOOLTIP_USE_MENU
	DWORD m_dwMenuToolPos;
#endif //PPTOOLTIP_USE_MENU

	//
	CRect m_rcTipArea;					//The bound rect around the tip's area in the client coordinates.
	CRect m_rcTooltip;					//The bound rect around the body of the tooltip in the client coordinates.
	CRect m_rcBoundsTooltip;			//The bound rect around a tooltip include an anchor
	CRect m_rcUnderTooltip;				//The bound rect of the window under the tooltip in the screen coordinates

	//Initialize tools
	typedef std::vector<PPTOOLTIP_INFO>	arHotArea;			// array of Tips rectangular spots
	typedef std::map<HWND, arHotArea>::iterator	mapIter;	// simplify reading
	std::map<HWND, arHotArea>	m_ToolMap;

	//Initialize list of toolbars
	typedef std::vector<HWND>	arToolBarWnd;
	arToolBarWnd m_wndToolBars;								// array of HWND of the toolbars

public:
	BEGIN_MSG_MAP(CPPToolTip)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_SETCURSOR(OnSetCursor)
		MSG_WM_ACTIVATEAPP(OnActivateApp)
		MSG_WM_DESTROY(OnDestroy)

//		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage)

		MESSAGE_HANDLER(UDM_TOOLTIP_REPAINT, OnRepaintWindow)
//		REFLECT_NOTIFICATIONS();
	END_MSG_MAP()

	//////////////////////////////////////////////////////////////////////////
	// 消息

	void OnDestroy()
	{
		ASSERT(IsWindow());

		_Pop();
		SetDelayTime(PPTOOLTIP_TIME_ANIMATION, 0);

		SetMsgHandled(FALSE);							//让其窗体实体收到此消息
	}
	void OnActivateApp(BOOL bActive, DWORD hTask)
	{
		//	CWindow::OnActivateApp(bActive, hTask);		// 冰峰

		if (!bActive) 
			_Pop();
	}
	BOOL OnSetCursor(HWND hWnd, UINT nHitTest, UINT message) 
	{
		CPoint ptClient;
		::GetCursorPos(&ptClient);
		ScreenToClient(&ptClient);
		ATLTRACE (_T("OnSetCursor(x=%d, y=%d)\n"), ptClient.x, ptClient.y);
		if (m_drawer.OnSetCursor(&ptClient))
			return TRUE; //The cursor over the hyperlink

		::SetCursor(::LoadCursor(NULL, IDC_ARROW));
		return TRUE;
	} //End of the WM_SETCURSOR handler

	LRESULT OnRepaintWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ATLTRACE (_T("OnRepaintWindow()\n"));
		if (m_bHyperlinkEnabled)
		{
			//Window's repaint enabled
			CDC dc(GetDC());
			_OnRedrawTooltip(dc.m_hDC);
		}
		return TRUE;
	}

	void OnPaint(HDC hdc) 
	{
		ATLTRACE(_T("CPPToolTip::OnPaint()\n"));
		CPaintDC dc(m_hWnd); // device context for painting

		//Copying info about current tool to displayed
		m_hwndDisplayedTool = m_hwndNextTool;
		m_tiDisplayed = m_tiNextTool;
		m_nTooltipType = m_nNextTooltipType;

		_OnRedrawTooltip(dc.m_hDC, m_dwCurTransparency);
	}

	LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		MSG msg = { m_hWnd, uMsg, wParam, lParam };
		msg.pt.x = GET_X_LPARAM(lParam);
		msg.pt.y = GET_Y_LPARAM(lParam);
		ClientToScreen(&msg.pt);

		RelayEvent(&msg);

		bHandled = FALSE;					//让消息流下去
		return 1LL;							//让消息流下去
	}

	void OnTimer(UINT nIDEvent) 
	{
		POINT pt;
		switch (nIDEvent)
		{
		case TIMER_SHOW:
			ATLTRACE(_T("OnTimerShow(%d)\n"), m_nNextTooltipType);
			//ENG: Kill SHOW timer 
			KillTimer(TIMER_SHOW);
			//ENG: Get current mouse coordinates
			if ((PPTOOLTIP_HELP == m_nNextTooltipType) || 
				(PPTOOLTIP_MENU == m_nNextTooltipType))
				pt = m_ptOriginal;
			else GetCursorPos(&pt);

			if ((pt.x != m_ptOriginal.x) || (pt.y != m_ptOriginal.y))
			{
				//ENG: If mouse coordinates was changed
				ATLTRACE(_T("OnTimerShow(HideTooltip)\n"));
				HideTooltip();
			}
			else if (PPTOOLTIP_STATE_HIDEN == m_nTooltipState)
			{
				ATLTRACE(_T("OnTimerShow(Showing)\n"));
				m_nTooltipState = PPTOOLTIP_STATE_SHOWING;
				//Display first step
				_PrepareDisplayTooltip(&m_ptOriginal);

				//Fade In effect
				if ((SHOWEFFECT_FADEINOUT == m_dwShowEffect) && m_dwTimeFadeIn)
				{
					ATLTRACE(_T("OnTimerShow(FadeIn)\n"));
					SetTimer(TIMER_SHOWING, m_dwTimeFadeIn, NULL); //Fade in showing was enabled
				}
				else
				{
					ATLTRACE(_T("OnTimerShow(Shown)\n"));
					m_nTooltipState = PPTOOLTIP_STATE_SHOWN; //Tooltip is already show
					if (m_dwTimeAutoPop && !(m_tiNextTool.nBehaviour & PPTOOLTIP_DISABLE_AUTOPOP))
						SetTimer(TIMER_HIDE, m_dwTimeAutoPop, NULL); //Hiding by timer was enabled
				} //if
			} //if
			break;
		case TIMER_SHOWING:
			ATLTRACE(_T("OnTimerFadeIn(Current Transparency=%d )\n"), m_dwCurTransparency);
			//ENG: If fade-in effect was finished then sets minimum transparency
			if (m_dwCurTransparency > (PERCENT_MIN_TRANSPARENCY + PERCENT_STEP_FADEIN))
				m_dwCurTransparency -= PERCENT_STEP_FADEIN;
			else m_dwCurTransparency = PERCENT_MIN_TRANSPARENCY;

			if (m_dwCurTransparency <= m_tiDisplayed.nTransparency)
			{
				//ENG: Kills showing timer and sets a tooltip's state as SHOWN
				m_dwCurTransparency = m_tiDisplayed.nTransparency;
				KillTimer(TIMER_SHOWING);
				m_nTooltipState = PPTOOLTIP_STATE_SHOWN;
				//ENG: Starts timer to auto pop of the tooltip
				_SetAutoPopTimer();
			} //if
			//ENG: Redraw tooltip with new transparency factor
			_OnRedrawTooltip(NULL, m_dwCurTransparency);
			break;	
		case TIMER_HIDE:
			ATLTRACE(_T("OnTimerHide()\n"));
			//ENG: Kill all timers except HIDING timer
			KillTimer(TIMER_SHOW);
			KillTimer(TIMER_SHOWING);
			KillTimer(TIMER_HIDE);
			//ENG: If hiding timer don't start
			if (PPTOOLTIP_STATE_HIDING != m_nTooltipState)
			{
				m_nTooltipState = PPTOOLTIP_STATE_HIDING;
				if ((SHOWEFFECT_FADEINOUT == m_dwHideEffect) && m_dwTimeFadeOut)
				{
					//ENG: If fade-out effect enabled and setted fade-out timestep
					SetTimer(TIMER_HIDING, m_dwTimeFadeOut, NULL);
				}
				else
				{
					//ENG: Sets a maximum transparency and to stops a hiding of a tooltip
					m_dwCurTransparency = PERCENT_MAX_TRANSPARENCY;
					OnTimer(TIMER_HIDING);
				} //if
			} //if
			break;
		case TIMER_HIDING:
			ATLTRACE(_T("OnTimerFadeOut(Current Transparency=%d)\n"), m_dwCurTransparency);
			//ENG: If fade-out effect was finished then sets maximum transparency
			if (m_dwCurTransparency < (PERCENT_MAX_TRANSPARENCY - PERCENT_STEP_FADEOUT))
				m_dwCurTransparency += PERCENT_STEP_FADEOUT;
			else m_dwCurTransparency = PERCENT_MAX_TRANSPARENCY;

			if (PERCENT_MAX_TRANSPARENCY == m_dwCurTransparency)
			{
				//ENG: Kills hiding timer and hides a tooltip
				KillTimer(TIMER_HIDING);
				if (m_tiDisplayed.nBehaviour & PPTOOLTIP_MULTIPLE_SHOW)
				{
					//If for tool to set a multiple show then reset last window
					m_hwndDisplayedTool = NULL;
					m_tiDisplayed.rectBounds.SetRectEmpty();
				} //if
				ShowWindow(SW_HIDE);
				m_nTooltipState = PPTOOLTIP_STATE_HIDEN;
				if (m_bNextToolExist)
				{
					//ENG: If next tooltip is exist then starts show
					m_bNextToolExist = FALSE;
					//				m_nTooltipType = m_nNextTooltipType;
					//				m_nNextTooltipType = PPTOOLTIP_NORMAL;
					if (m_bDelayNextTool) SetTimer(TIMER_SHOW, m_dwTimeInitial, NULL);
					else OnTimer(TIMER_SHOW);
				}
				else
				{
					//ENG: If next tooltip wasn't exist
					m_nTooltipType = PPTOOLTIP_NORMAL;
					m_nNextTooltipType = PPTOOLTIP_NORMAL;
				} //if
			}
			else 
			{
				//ENG: Redraw tooltip with new transparency factor
				_OnRedrawTooltip(NULL, m_dwCurTransparency);
			} //if
			break;	
		case TIMER_ANIMATION:
			if (_IsVisible() && (PPTOOLTIP_STATE_SHOWN == m_nTooltipState))
			{
				if(m_drawer.OnTimer())
					_OnRedrawTooltip(NULL, m_dwCurTransparency);
			} //if
			break;
		default:
			//		CWindow::OnTimer(nIDEvent);
			break;
		} //switch
	}

	//////////////////////////////////////////////////////////////////////////
	// 导出函数
public:
	BOOL PreTranslateMessage(MSG* pMsg)					//冰峰
	{
		RelayEvent(pMsg);

		//	return CWindow::PreTranslateMessage(pMsg);					//冰峰
		return TRUE;
	}

	BOOL RelayEvent(MSG* pMsg)
	{
		//ENG: Disables a message translation if object was't created (thanks to Stoil Todorov)
		if (NULL == m_hWnd)  
			return FALSE;

		ASSERT(m_hParentWnd);

		HWND hWnd = NULL;
		POINT pt;
		CRect rect;
		PPTOOLTIP_INFO ti;
		WTL::CString strTemp;

		switch(pMsg->message)
		{
		case WM_SETFOCUS:
			rect.left = 0;
			break;
		case WM_LBUTTONDOWN:
			ATLTRACE(_T("CPPToolTip::WM_LBUTTONDOWN\n"));
			if (_IsCursorOverTooltip())
			{
				//Left Button was pressed over the tooltip
				pt = pMsg->pt;
				ScreenToClient(&pt);
				m_drawer.OnLButtonDown(&pt); //
			} //if
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONDBLCLK:
		case WM_NCLBUTTONDOWN:
		case WM_NCLBUTTONDBLCLK:
		case WM_NCRBUTTONDOWN:
		case WM_NCRBUTTONDBLCLK:
		case WM_NCMBUTTONDOWN:
		case WM_NCMBUTTONDBLCLK:
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_MOUSEWHEEL:
			//		// The user has interrupted the current tool - dismiss it
			//		if (!(m_tiDisplayed.nBehaviour & PPTOOLTIP_NOCLOSE_MOUSEDOWN))
			_Pop();
			break;
		case WM_MOUSEMOVE:
			if ((PPTOOLTIP_HELP == m_nTooltipType) || (PPTOOLTIP_HELP == m_nNextTooltipType))
				return FALSE;
			if ((m_ptOriginal.x != pMsg->pt.x) || (m_ptOriginal.y != pMsg->pt.y))
			{
				// The mouse pointer's position was changed

				//Initialize values
				rect.SetRectEmpty();
				m_ptOriginal = pt = pMsg->pt;
				::ScreenToClient(m_hParentWnd, &pt);
				if (m_bDebugMode)
				{
					//Debug mode
					ti.sTooltip = _GetDebugInfoTool(&pt);
					ti.nMask = 0;
					m_hwndDisplayedTool = NULL;
					_SetNewTooltip(m_hWnd, ti);
				}
				else if (_IsCursorOverTooltip() && !(m_tiDisplayed.nBehaviour & PPTOOLTIP_TRACKING_MOUSE)) 
				{
					//ENG: Mouse over a tooltip and tracking mode was disabled
					if (!(m_tiDisplayed.nBehaviour & PPTOOLTIP_NOCLOSE_OVER))
					{
						//ENG: A tooltip don't hides when mouse over him
						HideTooltip();
					}
					else
					{
						//ENG: Resetup autopop timer
						_SetAutoPopTimer();
					} //if
				}
				else
				{
					//ENG: Searching a toolbar's item
//					hWnd = FindToolBarItem(pMsg->pt, ti);				//冰峰
					if (NULL == hWnd)
					{
						//ENG: Searching a hot area of the tooltip
						hWnd = _FindTool(&pt, ti);
					} //if
					if (NULL == hWnd)
					{
						//ENG: An item with a tooltip wasn't found
						m_hwndDisplayedTool = NULL;
						m_tiDisplayed.rectBounds.SetRectEmpty();
						KillTimer(TIMER_SHOW);
						HideTooltip();
					}
					else 
					{
						if ((hWnd != m_hwndDisplayedTool) || (ti.rectBounds != m_tiDisplayed.rectBounds/* m_rcDisplayedTool*/))
						{
							//ENG: Sets new tooltip for the new window or for the new window's item
							_SetNewTooltip(hWnd, ti);
						}
						else
						{
							//ENG: Nothing was changed
							if (m_tiDisplayed.nBehaviour & PPTOOLTIP_TRACKING_MOUSE)
							{
								//ENG: If sets tracking mode
								_SetAutoPopTimer();
								_OutputTooltipOnScreen(&pMsg->pt);
							}
							else if (!(m_tiDisplayed.nBehaviour & PPTOOLTIP_CLOSE_LEAVEWND))
							{
								//ENG: A tooltip must hide at anything mouse move
								if ((hWnd == m_hwndDisplayedTool) && 
									!(m_tiDisplayed.nBehaviour & PPTOOLTIP_MULTIPLE_SHOW))
								{
									//ENG: "Multiple show" mode was disabled
									HideTooltip();
								}
								else
								{
									//ENG: "Multiple show" mode was enabled
									_SetNewTooltip(hWnd, ti);
								} //if
							}
							else
							{
								//ENG: A tooltip don't must when a mouse is over window
								_SetAutoPopTimer();
							} //if
						} //if
					} //if
				} //if
			} //if
			break;
		} //switch

		return FALSE;
	} 


	// HWND FindToolBarItem(POINT point, PPTOOLTIP_INFO & ti)
	// {
	// 	//ENG: Toolbar control was disabled
	// 	if (!m_wndToolBars.size())
	// 		return NULL;
	// 
	// 	//ENG: Gets a window under mouse
	// 	HWND hWnd = ::WindowFromPoint(point);
	// 	if (NULL != hWnd)
	// 	{
	// 		//ENG: A window was found. Searching a coincidence with toolbar windows
	// 		for (int i = 0; i < (int)m_wndToolBars.size(); i++)
	// 		{
	// 			if (m_wndToolBars[i] == hWnd)
	// 			{
	// 				//ENG: A toolbar under mouse
	// 				CToolBar * pBar = (CToolBar*)CToolBar::FromHandle(hWnd);
	// 				pBar->ScreenToClient(&point);
	// 				//ENG: Gets a item's count of the toolbar
	// 				int count = pBar->GetCount();
	// 				CRect rect;
	// 				//ENG: Searching an toolbar's item under mouse
	// 				for (int i = 0; i < count; i++)
	// 				{
	// 					pBar->GetItemRect(i, rect);
	// 					if (rect.PtInRect(point))
	// 					{
	// 						//ENG: Toolbar's item was found
	// 						ti.nIDTool = pBar->GetItemID(i);
	// 						ti.rectBounds = rect;
	// 						ti.nMask = 0;
	// 						ti.sTooltip = m_drawer.GetResCommandPrompt(ti.nIDTool, 1);
	// 						return hWnd;
	// 					} //if
	// 				} //for
	// 				return NULL;
	// 			} //if
	// 		} //for
	// 	} //if
	// 	return NULL;
	// }


	////////////////////////////////////////////////////////////////////////////////////////////
	// *** public methods ***
	////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////
	// AddTool()
	// Registers a tool with the tooltip control.
	//------------------------------------------------------------------
	// Parameters:
	//		pWnd			- Pointer to the window that contains the tool.  
	//		lpszString		- Pointer to the text for the tool. 
	//      dwIdString		- ID of string resource
	//		hIcon			- Handle of the icon
	//		dwIdIcon		- ID of icon resource
	//		szIcon			- Specifies the width and the height, in pixels, of the icon to load.
	//		lpRectBounds	- Pointer to a RECT structure containing coordinates of the tool's bounding rectangle. 
	//						  The coordinates are relative to the upper-left corner of the client area of the window identified by pWnd. 
	//					      NULL if bounding rectangle don't uses for specified window
	//		dwIdTool		- ID of the tool
	//		ti				- Reference to PPTOOLTIP_INFO structure containing the parameters of the tooltip 
	//
	// Remarks:
	//		  A tooltip control can be associated with more than one tool. Call this function to register a tool 
	//		with the tooltip control, so that the information stored in the tooltip is displayed when the cursor is on the tool.
	////////////////////////////////////////////////////////////////////
	void AddTool(HWND hWnd, DWORD dwIdString, HICON hIcon, LPCRECT lpRectBounds = NULL, DWORD dwIDTool = 0)
	{
		WTL::CString str;
		str.LoadString(dwIdString);
		AddTool(hWnd, (LPCTSTR)str, hIcon, lpRectBounds, dwIDTool);
	}

	void AddTool(HWND hWnd, LPCTSTR lpszString, HICON hIcon, LPCRECT lpRectBounds = NULL, DWORD dwIDTool = 0)
	{
		WTL::CString str;
		str.Format(_T("<table><tr><td><icon handle=0x%X></td><td>%s</td></tr></table>"), 
			hIcon, lpszString);
		AddTool(hWnd, str, lpRectBounds, dwIDTool);
	}

	void AddTool(HWND hWnd, DWORD dwIdString, DWORD dwIdIcon, CSize & szIcon  = CSize(0, 0), LPCRECT lpRectBounds = NULL, DWORD dwIDTool = 0)
	{
		WTL::CString str;
		str.LoadString(dwIdString);
		AddTool(hWnd, (LPCTSTR)str, dwIdIcon, szIcon, lpRectBounds, dwIDTool);
	}

	void AddTool(HWND hWnd, LPCTSTR lpszString, DWORD dwIdIcon, CSize & szIcon  = CSize(0, 0), LPCRECT lpRectBounds = NULL, DWORD dwIDTool = 0)
	{
		WTL::CString str;
		str.Format(_T("<table><tr><td><icon idres=%d width=%d height=%d></td><td>%s</td></tr></table>"), 
			dwIdIcon, szIcon.cx, szIcon.cy, lpszString);
		AddTool(hWnd, str, lpRectBounds, dwIDTool);
	}

	void AddTool(HWND hWnd, LPCTSTR lpszString, DWORD dwIdBitmap, COLORREF crMask, CSize & szBitmap = CSize(0, 0), LPCRECT lpRectBounds = NULL, DWORD dwIDTool = 0)
	{
		WTL::CString str;
		str.Format(_T("<table><tr><td><bmp idres=%d mask=0x%X width=%d height=%d></td><td>%s</td></tr></table>"), 
			dwIdBitmap, crMask, szBitmap.cx, szBitmap.cy, lpszString);
		AddTool(hWnd, str, lpRectBounds, dwIDTool);
	}

	void AddTool(HWND hWnd, DWORD dwIdString, LPCRECT lpRectBounds  = NULL, DWORD dwIDTool  = 0)
	{
		WTL::CString str;
		str.LoadString(dwIdString);
		AddTool(hWnd, (LPCTSTR)str, lpRectBounds, dwIDTool);
	}

	void AddTool(HWND hWnd, LPCTSTR lpszString  = NULL, LPCRECT lpRectBounds  = NULL, DWORD dwIDTool  = 0)
	{
		PPTOOLTIP_INFO ti;

		ti.nIDTool = dwIDTool;
		if (NULL != lpRectBounds)
			ti.rectBounds = *lpRectBounds;
		else 
			ti.rectBounds.SetRectEmpty();
		ti.sTooltip = (WTL::CString)lpszString;
		ti.nMask = 0;
		ti.nStyles = 0;
		ti.nDirection = 0;
		ti.nEffect = 0;
		ti.nBehaviour = 0;
		ti.nGranularity = 0;
		ti.crBegin = RGB(0, 0, 0);
		ti.crMid = RGB(0, 0, 0);
		ti.crEnd = RGB(0, 0, 0);

		AddTool(hWnd, ti);
	}

	void AddTool(HWND hWnd, PPTOOLTIP_INFO & ti)
	{
		ATLTRACE(_T("AddTool(hWnd=0x%08X)\n"), hWnd);
		ASSERT (hWnd);

		//ENG: Searching a specified HWND
		mapIter item = m_ToolMap.find(hWnd);

		if (item == m_ToolMap.end())
		{
			//ENG: A tooltip for a specified HWND wasn't found therefore create it
			arHotArea hotarea;
			hotarea.push_back(ti);
			m_ToolMap.insert(std::make_pair(hWnd, hotarea));
			return;
		} //if

		//ENG: Gets parameters of the tooltip
		arHotArea & hotarea = item->second;

		//ENG: A tooltip has more one rectangle areas. Check all theirs
		arHotArea::iterator iter;
		for (iter = hotarea.begin(); iter != hotarea.end(); ++iter)
		{
			if (ti.rectBounds == (*iter).rectBounds)
			{
				//ENG: Specified window's rect already exist and so updates him
				*iter = ti;
				return;
			} //if
		} //for

		//ENG: Adds a new tool 
		hotarea.push_back(ti);
	} //End of AddTool

	////////////////////////////////////////////////////////////////////
	// RemoveTool()
	//   Removes the tool specified by pWnd and lpRectBounds from the collection of 
	// tools supported by a tooltip control.
	//------------------------------------------------------------------
	// Parameters:
	//		pWnd			- Pointer to the window that contains the tool.  
	//		lpRectBounds	- Pointer to a RECT structure containing coordinates of the tool's bounding rectangle. 
	//						  The coordinates are relative to the upper-left corner of the client area of the window identified by pWnd. 
	//					      NULL if bounding rectangle don't uses for specified window
	////////////////////////////////////////////////////////////////////
	void RemoveTool(HWND hWnd, LPCRECT lpRectBounds  = NULL)
	{
		ATLTRACE (_T("CPPToolTip::RemoveTool(hWnd=0x%08X)\n"), hWnd);
		ASSERT(hWnd);

		//ENG: Searching a specified HWND
		mapIter item = m_ToolMap.find(hWnd);

		if (item == m_ToolMap.end())
		{
			//ENG: Specified HWND wasn't found
			return; 
		} //if

		if (NULL == lpRectBounds)
		{
			//ENG: Removes all tools for the specified window
			m_ToolMap.erase(item);
		}
		else
		{
			//ENG: Search the tool to remove
			arHotArea & hotarea = item->second;
			arHotArea::iterator iter;
			for (iter = hotarea.begin(); iter != hotarea.end(); ++iter)
			{
				if ((*iter).rectBounds == *lpRectBounds)
				{
					//ENG: The tool was found
					if (hotarea.size() > 1)
					{
						//ENG: If the specified window has more one rectangle areas then removes the specified area only
						hotarea.erase(iter);
					}
					else
					{
						//ENG: If the specified window has one rectangle area only then removes the tool for specified window
						m_ToolMap.erase(item);
					} //if
					return;
				} //if
			} //for
		} //if
	} //End of RemoveTool

	////////////////////////////////////////////////////////////////////
	// RemoveAllTools()
	//   Removes all tools from the collection of tools supported by a tooltip control.
	////////////////////////////////////////////////////////////////////
	void RemoveAllTools()
	{
		ATLTRACE (_T("CPPToolTip::RemoveAllTools()\n"));

		//ENG: Removes all tools
		if (m_ToolMap.size())
			m_ToolMap.clear();

		//ENG: Removes all toolbars
		if (m_wndToolBars.size())
			m_wndToolBars.clear();
	} //End of RemoveAllTools

	////////////////////////////////////////////////////////////////////
	// AddToolBar()
	// Registers a toolbar to the tooltip control.
	//------------------------------------------------------------------
	// Parameters:
	//		pBar			- Pointer to the toolbar window.  
	////////////////////////////////////////////////////////////////////
	// void AddToolBar(CToolBar * pBar)
	// {
	// 	ATLTRACE (_T("AddToolBar(hWnd=0x%08X)\n"), pBar->GetSafeHwnd());
	// 	ASSERT(pBar);
	// 
	// 	//ENG: Gets HWND toolbar's window
	// 	HWND hWnd = pBar->GetSafeHwnd();
	// 
	// 	//ENG: Searching a clone of a toolbar
	// 	arToolBarWnd::iterator iter;
	// 	for (iter = m_wndToolBars.begin(); iter != m_wndToolBars.end(); ++iter)
	// 	{
	// 		if (*iter == hWnd)
	// 		{
	// 			//ENG: A clone was found
	// 			return;
	// 		} //if
	// 	} //for
	// 
	// 	//ENG: Stores HWND toolbar's window
	// 	m_wndToolBars.push_back(hWnd);
	// 
	// 	//ENG: Disables a standard tooltip for the specified toolbar
	// 	DWORD dwStyles = pBar->GetBarStyle();
	// 	dwStyles &= ~CBRS_TOOLTIPS;
	// 	pBar->SetBarStyle(dwStyles);
	// } //End of AddToolBar

	void HideTooltip()
	{
		ATLTRACE (_T("CPPToolTip::HideTooltip(CurState=%d)\n"), m_nTooltipState);
		switch(m_nTooltipState)
		{
		case PPTOOLTIP_STATE_SHOWING:
			//ENG: Kill showing tooltip
			KillTimer(TIMER_SHOWING);
		case PPTOOLTIP_STATE_SHOWN:
			//ENG: Hiding a tooltip
			OnTimer(TIMER_HIDE);
			break;
		} //switch
	} //End of HideTooltip

	BOOL GetToolInfo(PPTOOLTIP_INFO & ti, HWND hWnd, LPCRECT lpRectBounds  = NULL)
	{
		ASSERT(hWnd);

		//ENG: Searching a specified HWND
		mapIter item = m_ToolMap.find(hWnd);	
		if (item == m_ToolMap.end())
		{
			//ENG: Specified HWND wasn't found
			return FALSE; 
		} //if

		//ENG: Gets parameters of the tooltip
		arHotArea & hotarea = item->second;

		//ENG: A tooltip has more one rectangle areas. Check all theirs
		arHotArea::iterator iter;
		for (iter = hotarea.begin(); iter != hotarea.end(); ++iter)
		{
			if (lpRectBounds == (*iter).rectBounds)
			{
				//ENG: Specified window's rect already exist and so updates him
				ti = *iter;
				return TRUE ;
			} //if
		} //for

		return FALSE;
	}

	BOOL GetToolInfo(PPTOOLTIP_INFO & ti, HWND hWnd, DWORD dwIDTool  = 0)
	{
		ASSERT(hWnd);

		//ENG: Searching a specified HWND
		mapIter item = m_ToolMap.find(hWnd);	
		if (item == m_ToolMap.end())
		{
			//ENG: Specified HWND wasn't found
			return FALSE; 
		} //if

		//ENG: Gets parameters of the tooltip
		arHotArea & hotarea = item->second;

		arHotArea::iterator iter;
		for (iter = hotarea.begin(); iter != hotarea.end(); ++iter)
		{
			if (dwIDTool == (*iter).nIDTool)
			{
				ti = *iter;
				return TRUE ;
			} //if
		} //for

		return FALSE;
	}

	void UpdateTipText(LPCTSTR lpszText, HWND hWnd, DWORD dwIDTool  = 0)
	{
		PPTOOLTIP_INFO ti;
		if (GetToolInfo(ti, hWnd, dwIDTool))
		{
			ti.sTooltip = lpszText;
			AddTool(hWnd, ti);
		}
	}

	void DelTool(HWND hWnd, DWORD dwIDTool)
	{
		PPTOOLTIP_INFO ti;
		if (GetToolInfo(ti, hWnd, dwIDTool))
		{
			RemoveTool(hWnd, ti.rectBounds);
		}
	}

	void SetToolRect(HWND hWnd, DWORD dwIDTool, LPCRECT lpRectBounds)
	{
		PPTOOLTIP_INFO ti;
		if (GetToolInfo(ti, hWnd, dwIDTool))
		{
			ti.rectBounds = *lpRectBounds;
			AddTool(hWnd, ti);
		}
	}

	////////////////////////////////////////////////////////////////////
	// CPPToolTip::EnableHyperlink()
	// Enables redrawing hyperlinks and hot areas.
	////////////////////////////////////////////////////////////////////
	void EnableHyperlink(BOOL bEnable  = TRUE)
	{
		m_bHyperlinkEnabled = bEnable;
	} //End of EnableHyperlink

	////////////////////////////////////////////////////////////////////
	// SetCallbackHyperlink()
	//   Sets the callback message that will be sent to the specified window 
	// if user clicks a hyperlink or hotareas with a msg parameter.
	//------------------------------------------------------------------
	// Parameters:
	//		hWnd			- Handle of the window that will receive the callback message.
	//						  Pass NULL to remove any previously specified callback message.
	//		nMessage		- Callback message to send to window.
	//		lParam			- A 32 bits user specified value that will be passed to the callback function.
	//
	// Remarks:
	//    The callback function must be in the form:
	//  LRESULT On_MenuCallback(WPARAM wParam, LPARAM lParam)
	//		wParam			- Pointer to the string specified as parameter in <a msg=> tag.
	//		lParam			- The 32 bits user specified value.
	////////////////////////////////////////////////////////////////////
	void SetCallbackHyperlink(HWND hWnd, UINT nMessage, LPARAM lParam  = 0)
	{
		ATLTRACE(_T("SetCallbackHyperlink()\n"));

		m_drawer.SetCallbackHyperlink(hWnd, nMessage, lParam);
	} //End of SetCallbackHyperlink

	/////////////////////////////////////////////////////////////////////
	// SetNotify()
	// This function sets or removes the notification messages from the control before display.
	//-------------------------------------------------------------------
	// Parameters:
	//		bParentNotify	- If TRUE the tooltip will be send the notification to parent window
	//						  Else the notification will not send
	//		hWnd			- If non-NULL the tooltip will be send the notification to specified window
	//						  Else the notification will not send
	///////////////////////////////////////////////////////////////////////
	void SetNotify(BOOL bParentNotify  = TRUE)
	{
		HWND hWnd = NULL;

		if (bParentNotify)
			hWnd = m_hParentWnd;

		SetNotify(hWnd);
	} //End of SetNotify

	void SetNotify(HWND hWnd)
	{
		ATLTRACE(_T("CPPToolTip::SetNotify\n"));

		m_hNotifyWnd = hWnd;
	} //End of SetNotify

	/////////////////////////////////////////////////////////////////////////////
	//  SetSize()
	//    Sets the specified size
	//---------------------------------------------------------------------------
	//  Parameters :
	//		nSizeIndex		- index of the size. This parameter can be one 
	//						  of the following values:
	//							PPTTSZ_ROUNDED_CX - The width of the ellipse used 
	//												to draw the rounded corners, in logical units.
	//							PPTTSZ_ROUNDED_CY - The height of the ellipse used 
	//												to draw the rounded corners, in logical units.
	//							PPTTSZ_MARGIN_CX  - The left and right margins of the tooltip's 
	//												text from the tooltip's edges. 
	//							PPTTSZ_MARGIN_CY  - The top and bottom margins of the tooltip's 
	//												text from the tooltip's edges.
	//							PPTTSZ_WIDTH_ANCHOR - The width of the tooltip's anchor
	//							PPTTSZ_HEIGHT_ANCHOR - The height of the tooltip's anchor 
	//							PPTTSZ_MARGIN_ANCHOR - The margin of the tooltip's anchor from 
	//												   his edge.
	//							PPTTSZ_OFFSET_ANCHOR_CX - The horizontal offset of the tooltip's anchor
	//													  from the hot spot of a cursor
	//							PPTTSZ_OFFSET_ANCHOR_CY - The vertical offset of the tooltip's anchor
	//													  from the hot spot of a cursor
	//		nValue			- size's value
	/////////////////////////////////////////////////////////////////////////////
	void SetSize(int nSizeIndex, int nValue)
	{
		ATLTRACE(_T("CPPToolTip::SetSize(nSizeIndex = %d, nValue = %d)\n"), nSizeIndex, nValue);
		if (nSizeIndex >= PPTTSZ_MAX_SIZES)
			return;

		m_nSizes [nSizeIndex] = nValue;
	} //End of SetSize

	/////////////////////////////////////////////////////////////////////////////
	//  CPPTootTip::GetSize()
	//    Gets the specified size
	//---------------------------------------------------------------------------
	//  Parameters :
	//		nSizeIndex		- An index of the sizes. See CPPToolTip::SetSize for a 
	//						  description of the valid values.
	//  Returns :
	//		size's value
	//
	/////////////////////////////////////////////////////////////////////////////
	int GetSize(int nSizeIndex)
	{
		ATLTRACE(_T("CPPToolTip::GetSize(nSizeIndex = %d)\n"), nSizeIndex);
		if (nSizeIndex >= PPTTSZ_MAX_SIZES)
			return 0;

		return m_nSizes [nSizeIndex];
	} //End of GetSize

	/////////////////////////////////////////////////////////////////////////////
	//  CPPToolTip::SetDefaultSizes()
	//    Sets all sizes to default values
	//---------------------------------------------------------------------------
	//  Parameters:
	//		bBalloonSize	- If TRUE all sizes will be sets for balloon tooltip
	//						  otherwise tooltip will look as standard 
	/////////////////////////////////////////////////////////////////////////////
	void SetDefaultSizes(BOOL bBalloonSize  = TRUE)
	{
		ATLTRACE(_T("CPPToolTip::SetDefaultSizes()\n"));

		if (bBalloonSize)
		{
			SetSize(PPTTSZ_ROUNDED_CX, 16);
			SetSize(PPTTSZ_ROUNDED_CY, 16);
			SetSize(PPTTSZ_MARGIN_CX, 12);
			SetSize(PPTTSZ_MARGIN_CY, 12);
			SetSize(PPTTSZ_WIDTH_ANCHOR, 12);
			SetSize(PPTTSZ_HEIGHT_ANCHOR, 16);
			SetSize(PPTTSZ_MARGIN_ANCHOR, 16);
			SetSize(PPTTSZ_OFFSET_ANCHOR_CX, 0);
			SetSize(PPTTSZ_OFFSET_ANCHOR_CY, 0);
		}
		else
		{
			SetSize(PPTTSZ_ROUNDED_CX, 0);
			SetSize(PPTTSZ_ROUNDED_CY, 0);
			SetSize(PPTTSZ_MARGIN_CX, 3);
			SetSize(PPTTSZ_MARGIN_CY, 1);
			SetSize(PPTTSZ_WIDTH_ANCHOR, 0);
			SetSize(PPTTSZ_HEIGHT_ANCHOR, 0);
			SetSize(PPTTSZ_MARGIN_ANCHOR, 0);
			SetSize(PPTTSZ_OFFSET_ANCHOR_CX, 0);
			SetSize(PPTTSZ_OFFSET_ANCHOR_CY, 0);
		} //if
	} //End of SetDefaultSizes

	/////////////////////////////////////////////////////////////////////////////
	// CPPToolTip::SetColorBk()
	//   Sets background's colors 
	//---------------------------------------------------------------------------
	//  Parameters:
	//		color			- A solid color for background's effect 
	//		clrBegin		- A begin color for background's effect
	//		clrMid			- A middle color for background's effect
	//		clrEnd			- A end color for background's effect
	/////////////////////////////////////////////////////////////////////////////
	void SetColorBk(COLORREF color)
	{
		SetColorBk(color, color, color);
	} //End of SetColorBk

	void SetColorBk(COLORREF clrBegin, COLORREF clrEnd)
	{
		SetColorBk(clrBegin, clrBegin, clrEnd);
	} //End of SetColorBk

	void SetColorBk(COLORREF clrBegin, COLORREF clrMid, COLORREF clrEnd)
	{
		m_clrBeginBk = clrBegin;
		m_clrMidBk = clrMid;
		m_clrEndBk = clrEnd;
	} //End of SetColorBk

	/////////////////////////////////////////////////////////////////////////////
	// CPPToolTip::SetEffectBk()
	//   Sets a background's effect 
	//---------------------------------------------------------------------------
	//  Parameters:
	//		dwEffect		- A background's effect 
	//		nGranularity	- Adds an uniform noise to the effect. 
	//						  A good value is from 5 to 20; 0 to disable the effect. 
	//						  The noise has a positive effect because it hides the palette steps.
	/////////////////////////////////////////////////////////////////////////////
	void SetEffectBk(DWORD dwEffect, BYTE nGranularity  = 5)
	{
		m_dwEffectBk = dwEffect;
		m_nGranularity = nGranularity;
	} //End of SetEffectBk

	/////////////////////////////////////////////////////////////////////////////
	// CPPToolTip::SetBehaviour()
	//   Sets a tooltip's behaviour 
	//---------------------------------------------------------------------------
	//  Parameters:
	//		dwBehaviour		- A tooltip's behaviour. 0 for normal tooltip without 
	//						  specific behaviours. This parameter can be any combination 
	//						  of CPPToolTip behaviours:
	//							PPTOOLTIP_MULTIPLE_SHOW		- Multiple show for single control
	//							PPTOOLTIP_TRACKING_MOUSE	- Tracking for mouse
	//							PPTOOLTIP_CLOSE_LEAVEWND	- Close tooltip if mouse leave the control
	//							PPTOOLTIP_NOCLOSE_OVER		- No close tooltip if mouse over him
	//							PPTOOLTIP_DISABLE_AUTOPOP	- Disables autopop tooltip from timer
	/////////////////////////////////////////////////////////////////////////////
	void SetBehaviour(DWORD dwBehaviour  = 0)
	{
		m_dwBehaviour = dwBehaviour;
	} //End of SetBehaviour

	/////////////////////////////////////////////////////////////////////////////
	// CPPToolTip::GetBehaviour()
	//   Gets a tooltip's behaviour 
	//---------------------------------------------------------------------------
	// Return value:
	//		A tooltip's behaviour. See CPPToolTip::SetBehaviour for a description of the 
	//	valid values.
	/////////////////////////////////////////////////////////////////////////////
	DWORD GetBehaviour()
	{
		return m_dwBehaviour;
	} //End of GetBehaviour

	/////////////////////////////////////////////////////////////////////
	//  CPPToolTip::SetDelayTime()
	//   Call this function to set the delay time for a tooltip control. 
	// The delay time is the length of time the cursor must remain on a tool 
	// before the tooltip window appears. The default delay time is 500 milliseconds.
	//-------------------------------------------------------------------
	// Parameters:
	//		dwDuration		- Flag that specifies which duration value will be retrieved. 
	//						  This parameter can be one of the following values:
	//							PPTOOLTIP_TIME_AUTOPOP  - Retrieve the length of time the tooltip 
	//													  window remains visible if the pointer is 
	//													  stationary within a tool's bounding rectangle. 
	//							PPTOOLTIP_TIME_INITIAL  - Retrieve the length of time the pointer 
	//			 										  must remain stationary within a tool's bounding 
	//													  rectangle before the tool tip window appears. 
	//							PPTOOLTIP_TIME_FADEIN	- Retrieve the length of time for each step of
	//													  fade-in effect
	//							PPTOOLTIP_TIME_FADEOUT	- Retrieve the length of time for each step of
	//													  fade-out effect
	//							PPTOOLTIP_TIME_ANIMATION  Retrieve the speed for the animation
	//						  For compatibility with 1.x versions of CPPToolTip a following values
	//						  are available also:
	//							TTDT_AUTOPOP			- Same PPTOOLTIP_TIME_AUTOPOP 
	//							TTDT_INITIAL			- Same PPTOOLTIP_TIME_INITIAL 
	//		nTime			- The specified delay time, in milliseconds.
	/////////////////////////////////////////////////////////////////////
	void SetDelayTime(DWORD dwDuration, DWORD dwTime)
	{
		switch (dwDuration)
		{
		case PPTOOLTIP_TIME_AUTOPOP:
			m_dwTimeAutoPop = dwTime;
			break;
		case PPTOOLTIP_TIME_INITIAL:
			m_dwTimeInitial = dwTime;
			break;
		case PPTOOLTIP_TIME_FADEIN:
			m_dwTimeFadeIn = dwTime;
			break;
		case PPTOOLTIP_TIME_FADEOUT:
			m_dwTimeFadeOut = dwTime;
			break;
		case PPTOOLTIP_TIME_ANIMATION:
			KillTimer(TIMER_ANIMATION);
			if (dwTime)
				SetTimer(TIMER_ANIMATION, dwTime, NULL);
			break;
		}
	} //End of SetDelayTime

	/////////////////////////////////////////////////////////////////////
	// CPPToolTip::GetDelayTime()
	// Retrieves the initial, pop-up, and reshow durations currently set 
	// for a CPPToolTip control
	//-------------------------------------------------------------------
	// Parameters:
	//		dwDuration		- Flag that specifies which duration value will be retrieved. 
	//						  See CPPToolTip::SetDelayTime for a description of the valid values. 
	// Return value:
	//	The specified delay time, in milliseconds
	///////////////////////////////////////////////////////////////////////
	DWORD GetDelayTime(DWORD dwDuration) const
	{
		DWORD dwTime = 0;
		switch (dwDuration)
		{
		case PPTOOLTIP_TIME_AUTOPOP:
			dwTime = m_dwTimeAutoPop;
			break;
		case PPTOOLTIP_TIME_INITIAL:
			dwTime = m_dwTimeInitial;
			break;
		case PPTOOLTIP_TIME_FADEIN:
			dwTime = m_dwTimeFadeIn;
			break;
		case PPTOOLTIP_TIME_FADEOUT:
			dwTime = m_dwTimeFadeOut;
			break;
		}

		return dwTime;
	} //End of GetDelayTime

	/////////////////////////////////////////////////////////////////////////////
	//  CPPToolTip::SetDirection()
	//    Sets a placement of the tooltip's anchor
	//---------------------------------------------------------------------------
	//  Parameters :
	//		dwDirection		- A placement of the tooltip's anchor. This parameter 
	//					      can be one of the following values:
	//							PPTOOLTIP_TOPEDGE_LEFT			- A left corner of the top edge
	//							PPTOOLTIP_TOPEDGE_RIGHT			- A right corner of the top edge
	//							PPTOOLTIP_TOPEDGE_CENTER		- By center of the top edge
	//							PPTOOLTIP_BOTTOMEDGE_LEFT		- A left corner of the bottom edge
	//							PPTOOLTIP_BOTTOMEDGE_RIGHT		- A right corner of the bottom edge
	//							PPTOOLTIP_BOTTOMEDGE_CENTER		- By center of the bottom edge
	//							PPTOOLTIP_LEFTEDGE_TOP			- A top corner of the left edge
	//							PPTOOLTIP_LEFTEDGE_BOTTOM		- A bottom corner of the left edge
	//							PPTOOLTIP_LEFTEDGE_VCENTER		- By center of the left edge
	//							PPTOOLTIP_RIGHTEDGE_TOP			- A top corner of the right edge
	//							PPTOOLTIP_RIGHTEDGE_BOTTOM		- A bottom corner of the right edge
	//							PPTOOLTIP_RIGHTEDGE_VCENTER		- By center of the right edge
	//						  For compatibility with 1.x versions of CPPToolTip a following values
	//						  are available also:
	//							PPTOOLTIP_LEFT_TOP				- Same PPTOOLTIP_TOPEDGE_LEFT
	//							PPTOOLTIP_RIGHT_TOP				- Same PPTOOLTIP_TOPEDGE_RIGHT
	//							PPTOOLTIP_LEFT_BOTTOM			- Same PPTOOLTIP_BOTTOMEDGE_LEFT
	//							PPTOOLTIP_RIGHT_BOTTOM			- Same PPTOOLTIP_BOTTOMEDGE_RIGHT
	/////////////////////////////////////////////////////////////////////////////
	void SetDirection(DWORD dwDirection  = PPTOOLTIP_BOTTOMEDGE_LEFT)
	{
		ATLTRACE(_T("CPPToolTip::SetDirection(nDirection = %d)\n"), dwDirection);

		m_dwDirection = dwDirection;
	} //End of SetDirection	

	/////////////////////////////////////////////////////////////////////////////
	//  CPPToolTip::GetDirection()
	//    Gets a placement of the tooltip's anchor
	//---------------------------------------------------------------------------
	//  Returns :
	//	  A placement of the tooltip's anchor. See CPPToolTip::SetDirection for a description of 
	//	the valid values.
	/////////////////////////////////////////////////////////////////////////////
	DWORD GetDirection()
	{
		ATLTRACE(_T("CPPToolTip::GetDirection()\n"));

		return m_dwDirection;
	} //End of GetDirection

	/////////////////////////////////////////////////////////////////////////////
	//  CPPToolTip::SetTextStyles()
	//    Applies a CSS-like style for the tooltip's HTML
	//---------------------------------------------------------------------------
	//  Parameters:
	//		lpszStyleName	- Pointer to a null-terminated string that specifies
	//						  a name of CSS style
	//		lpszStyleValue  - Pointer to a null-terminated string that specifies 
	//						  CSS-lite style for drawing a tooltip text.
	/////////////////////////////////////////////////////////////////////////////
	void SetTextStyle(LPCTSTR lpszStyleName, LPCTSTR lpszStyleValue)
	{
		m_drawer.SetTextStyle(lpszStyleName, lpszStyleValue);
	}

	/////////////////////////////////////////////////////////////////////////////
	//  CPPToolTip::SetCssStyles()
	//    Applies a CSS-like styles for the tooltip's HTML
	//---------------------------------------------------------------------------
	//  Parameters:
	//		lpszCssStyles	- Pointer to a null-terminated string that specifies 
	//						  CSS-lite styles for drawing a tooltip text.
	/////////////////////////////////////////////////////////////////////////////
	void SetCssStyles(LPCTSTR lpszCssStyles  = NULL)
	{
		m_drawer.SetCssStyles(lpszCssStyles);
	} //End of SetCssStyles

	///////////////////////////////////////////////////////////////////////////// 
	//  CPPToolTip::SetCssStyles() 
	//    Applies a CSS-like styles for the tooltip's HTML 
	//--------------------------------------------------------------------------- 
	//  Parameters: 
	//      dwIdCssStyle    - ID of string resource 
	//      lpszPathDll		- 
	///////////////////////////////////////////////////////////////////////////// 
	void SetCssStyles(DWORD dwIdCssStyle, LPCTSTR lpszPathDll  = NULL) 
	{ 
		m_drawer.SetCssStyles(dwIdCssStyle, lpszPathDll); 
	} //End of SetCssStyles

	/////////////////////////////////////////////////////////////////////////////
	//  CPPToolTip::GetCssStyles()
	//    Applies a CSS-like styles for the tooltip's HTML
	//---------------------------------------------------------------------------
	//  Return value:
	//		Pointer to a null-terminated string that specifies CSS-lite styles 
	//  for drawing a tooltip text.
	/////////////////////////////////////////////////////////////////////////////
	LPCTSTR GetCssStyles()
	{
		return m_drawer.GetCssStyles();
	} //End of GetCssStyles

	/////////////////////////////////////////////////////////////////////////////
	//  CPPToolTip::SetDebugMode()
	//    Sets a debug mode. In this mode tooltip will display for any control
	// and will contain debug info about control.
	//---------------------------------------------------------------------------
	//  Parameters:
	//		bDebug			- TRUE set a debug mode.
	/////////////////////////////////////////////////////////////////////////////
	void SetDebugMode(BOOL bDebug  = TRUE)
	{
		m_bDebugMode = bDebug;
	} //End of SetDebugMode

	////////////////////////////////////////////////////////////////////
	// CPPToolTip::ShowHelpTooltip()
	// Shows the help tooltip in any place of screen.
	//------------------------------------------------------------------
	// Parameters:
	//		pt				- Pointer to a POINT structure that receives the screen coordinates of the tooltip's anchor  
	//		lpszString		- Pointer to the text for the help tooltip. 
	//      dwIdString		- ID of string resource
	//		hIcon			- Handle of the icon
	//		dwIdIcon		- ID of icon resource
	//		szIcon			- Specifies the width and the height, in pixels, of the icon to load.
	//		ti				- Reference to PPTOOLTIP_INFO structure containing the parameters of the tooltip 
	////////////////////////////////////////////////////////////////////
	void ShowHelpTooltip(LPPOINT pt, DWORD dwIdText, HICON hIcon  = NULL)
	{
		WTL::CString str;
		str.LoadString(dwIdText);
		ShowHelpTooltip(pt, (LPCTSTR)str, hIcon);
	} //End ShowHelpTooltip

	void ShowHelpTooltip(LPPOINT pt, DWORD dwIdText, DWORD dwIdIcon, CSize & szIcon  = CSize(0, 0))
	{
		WTL::CString str;
		str.LoadString(dwIdText);
		ShowHelpTooltip(pt, (LPCTSTR)str, dwIdIcon, szIcon);
	} //End ShowHelpTooltip

	void ShowHelpTooltip(LPPOINT pt, LPCTSTR lpszString, HICON hIcon  = NULL)
	{
		PPTOOLTIP_INFO ti;
		if (NULL == hIcon)
		{
			ti.sTooltip = (WTL::CString)lpszString;
		}
		else
		{
			ti.sTooltip.Format(_T("<table><tr><td><icon handle=0x%X></td><td>%s</td></tr></table>"), 
				hIcon, lpszString);
		} //if

		ti.nMask = 0;
		ShowHelpTooltip(pt, ti);
	} //End ShowHelpTooltip

	void ShowHelpTooltip(LPPOINT pt, LPCTSTR lpszString, DWORD dwIdIcon, CSize & szIcon  = CSize(0, 0))
	{
		WTL::CString str;
		str.Format(_T("<table><tr><td><icon idres=%d width=%d height=%d></td><td>%s</td></tr></table>"), 
			dwIdIcon, szIcon.cx, szIcon.cy, lpszString);
		ShowHelpTooltip(pt, (LPCTSTR)str);
	} //End ShowHelpTooltip

	void ShowHelpTooltip(LPPOINT pt, PPTOOLTIP_INFO & ti)
	{
		ATLTRACE(_T("CPPToolTip::ShowHelpTooltip\n"));

		m_ptOriginal = CPoint(pt->x, pt->y);
		ti.nBehaviour = m_dwBehaviour | PPTOOLTIP_DISABLE_AUTOPOP;
		ti.nMask = PPTOOLTIP_MASK_BEHAVIOUR;
		_SetNewTooltip(NULL, ti, FALSE, PPTOOLTIP_HELP);
	} //End ShowHelpTooltip

	////////////////////////////////////////////////////////////////////
	// CPPToolTip::SetBorder()
	// Sets a border of the tooltip.
	//------------------------------------------------------------------
	// Parameters:
	//		color			- Color of the tooltip's border
	//		hbr				- Brush for drawing tooltip's border
	//      nWidth			- A width of the brush
	//		nHeight			- A height of the brush
	////////////////////////////////////////////////////////////////////
	void SetBorder(COLORREF color, int nWidth  = 1, int nHeight  = 1)
	{
		HBRUSH hbr = ::CreateSolidBrush(color);
		SetBorder(hbr, nWidth, nHeight);
	} //End of SetBorder

	void SetBorder(HBRUSH hbr, int nWidth  = 1, int nHeight  = 1)
	{
		HideBorder();
		if (nWidth && nHeight && (NULL != hbr))
		{
			m_hbrBorder = hbr;
			m_szBorder.cx = nWidth;
			m_szBorder.cy = nHeight;
		} //if
	} //End of SetBorder

	////////////////////////////////////////////////////////////////////
	// CPPToolTip::HideBorder()
	// Hides border of the tooltip.
	////////////////////////////////////////////////////////////////////
	void HideBorder()
	{
		if (NULL != m_hbrBorder)
		{
			::DeleteObject(m_hbrBorder);
			m_hbrBorder = NULL;
		} //if
		m_szBorder.cx = 0;
		m_szBorder.cy = 0;
	} //End of HideBorder

	////////////////////////////////////////////////////////////////////////////////
	// Begin of the menu methods block. Build-in support for menu
#ifdef PPTOOLTIP_USE_MENU
	//////////////////
	// Need to handle WM_ENTERIDLE to cancel the tip if the user 
	// moved the mouse off the popup menu. For main menus, Windows 
	// will send a WM_MENUSELECT message for the parent menu when 
	// this happens, but for context menus there's no other way to 
	// know the user moved the mouse off the menu.
	//
	void OnEnterIdle(UINT nWhy, HWND hwndWho)
	{
		if ((MSGF_MENU == nWhy))
		{
			if (m_nTooltipType == PPTOOLTIP_MENU)
			{
				if (PPTOOLTIP_STATE_SHOWN == m_nTooltipState)
				{
					CPoint pt;
					GetCursorPos(&pt);
					if (hwndWho != ::WindowFromPoint(pt)) 
					{
						HideTooltip();
					} //if
				} //if
			} //if
		} //if
	} //End of OnEnterIdle

	void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSubMenu)
	{
		if (((nFlags & 0xFFFF) == 0xFFFF) || (nFlags & MF_POPUP) || (nFlags & MF_SEPARATOR))
		{
			//HideTooltip();
			_Pop();
		} 
		else if (nItemID && hSubMenu) 
		{
			HWND hwndMenu = GetRunningMenuWnd(); //CWnd::WindowFromPoint(pt);
			if (NULL != hwndMenu)
			{
				CRect rcMenu;
				::GetWindowRect(hwndMenu, rcMenu); // whole menu rect

				// find Item Rectangle and position
				int count = ::GetMenuItemCount(hSubMenu);
				int cy = rcMenu.top + GetSystemMetrics(SM_CYEDGE) + 1;
				for(int nItem = 0; nItem < count; nItem++) 
				{
					CRect rect;
					::GetMenuItemRect(m_hParentWnd, hSubMenu, nItem, &rect);
					if(nItemID == ::GetMenuItemID(hSubMenu, nItem)) 
					{
						UINT nState = GetMenuState(hSubMenu, nItemID, MF_BYCOMMAND);
						WTL::CString str;
						if (MF_DISABLED & nState)
							str = m_drawer.GetResCommandPrompt(nItemID, 2); //String for disabled item
						else
							str = m_drawer.GetResCommandPrompt(nItemID, 0);

						CPoint pt;
						// found menu item: adjust rectangle to right and down
						pt.x = rcMenu.left;
						pt.y = cy;
						if (m_dwMenuToolPos & PPTOOLTIP_MENU_CENTER)
							pt.x += rect.Width() / 2;
						else if (m_dwMenuToolPos & PPTOOLTIP_MENU_RIGHT)
							pt.x += rect.Width();

						if (m_dwMenuToolPos & PPTOOLTIP_MENU_VCENTER)
							pt.y += rect.Height() / 2;
						else if (m_dwMenuToolPos & PPTOOLTIP_MENU_BOTTOM)
							pt.y += rect.Height();

						PPTOOLTIP_INFO ti;
						ti.rectBounds = rect;
						ti.nMask = 0;
						ti.sTooltip = str;
						m_nNextTooltipType = PPTOOLTIP_MENU;
						m_ptOriginal = pt;
						_SetNewTooltip(hwndMenu, ti, TRUE, PPTOOLTIP_MENU);

						return;
					} //if
					cy += rect.Height(); // add height
				} //for
			} //if
			//ENG: Menu item was not found
			_Pop();
		} //if
	}

	////////////////////////////////////////////////////////////////////
	// CPPToolTip::GetRunningMenuWnd()
	// Get running menu window.
	////////////////////////////////////////////////////////////////////
	HWND GetRunningMenuWnd()
	{
		HWND hwnd = NULL;
		EnumWindows(MyEnumProc,(LPARAM)&hwnd);
		return hwnd;
	} //End of GetRunningMenuWnd

	////////////////////////////////////////////////////////////////////
	// MenuToolPosition()
	// Sets a position of the tooltip's anchor about menu item.
	//------------------------------------------------------------------
	// Parameters:
	//		nPos			- A tooltip's position. This parameter can be any combination 
	//						  of single horizontal value and single vertical value of CPPToolTip:
	//							--- Horizontal position ---
	//							PPTOOLTIP_MENU_LEFT		0x00
	//							PPTOOLTIP_MENU_RIGHT	0x01
	//							PPTOOLTIP_MENU_CENTER	0x02
	//							--- Vertical position ---
	//							PPTOOLTIP_MENU_TOP		0x00
	//							PPTOOLTIP_MENU_BOTTOM	0x10
	//							PPTOOLTIP_MENU_VCENTER  0x20
	////////////////////////////////////////////////////////////////////
	void MenuToolPosition(DWORD nPos  = PPTOOLTIP_MENU_LEFT | PPTOOLTIP_MENU_TOP)
	{
		m_dwMenuToolPos = nPos;
	} //End of MenuToolPosition

	// End of menu methods block
	///////////////////////////////////////////////////////////
#endif //PPTOOLTIP_USE_MENU

	////////////////////////////////////////////////////////////////////
	// EnableEscapeSequences()
	//		Enables the escape sequences. If the escape sequences was disabled
	//	HTML-lite compiler will ignore the codes less then 0x20 (such \n, \r, \t).
	////////////////////////////////////////////////////////////////////
	void EnableEscapeSequences(BOOL bEnable)
	{
		m_drawer.EnableEscapeSequences(bEnable);
	} //End of EnableEscapeSequences

	////////////////////////////////////////////////////////////////////
	// SetImageList()
	//		Sets an image list for using it into the HTML string with <ilst> tag.
	//------------------------------------------------------------------
	// Parameters:
	//		nIdBitmap		- Resource IDs of the bitmap to be associated with the image list.
	//		hBitmap			- Handle of the bitmap to be associated with the image list.
	//      cx				- Dimensions of each image, in pixels.
	//		cy				- Dimensions of each image, in pixels.
	//		nCount			- The number of images in the image list
	//		crMask			- Color used to generate a mask. Each pixel of this color in 
	//						  the specified bitmap is changed to transparent, and the 
	//						  corresponding bit in the mask is set to one.
	////////////////////////////////////////////////////////////////////
	void SetImageList(UINT nIdBitmap, int cx, int cy, int nCount, COLORREF crMask  = RGB(255, 0, 255))
	{
		m_drawer.SetImageList(nIdBitmap, cx, cy, nCount, crMask);
	} //End of SetImageList

	void SetImageList(HBITMAP hBitmap, int cx, int cy, int nCount, COLORREF crMask  = RGB(255, 0, 255))
	{
		m_drawer.SetImageList(hBitmap, cx, cy, nCount, crMask);
	} //End of SetImageList

	////////////////////////////////////////////////////////////////////
	// SetTransparency()
	//		Sets a transparency of the tooltip.
	//------------------------------------------------------------------
	// Parameters:
	//		nTransparency	- A transparency value to be used on the tooltip. 
	//						  The default 0 assumes that your tooltip is opaque and 0xFF (255) 
	//						  for full transparency of the tooltip.
	////////////////////////////////////////////////////////////////////
	void SetTransparency(BYTE nTransparency  = 0) 
	{
		if (nTransparency <= PERCENT_MAX_TRANSPARENCY)
			m_nTransparency = nTransparency;
	} //End of SetTransparency

	////////////////////////////////////////////////////////////////////
	// SetTooltipShadow()
	//		Sets a tooltip's shadow.
	//------------------------------------------------------------------
	// Parameters:
	//		nOffsetX, 
	//		nOffsetY		- The offsets of the tooltip's shadow from the tooltip's window.
	//		nDarkenPercent	- So far as colors under the shadow will be darken (0 - 100)
	//      bGradient		- TRUE to use a gradient shadow.
	//		nDepthX,
	//		nDepthY			- The gradient depths of the tooltip's shadow.
	////////////////////////////////////////////////////////////////////
	void SetTooltipShadow(int nOffsetX, int nOffsetY, BYTE nDarkenPercent  = 50, 
		BOOL bGradient  = TRUE, int nDepthX  = 7, int nDepthY  = 7)
	{
		m_szOffsetShadow.cx = nOffsetX;
		m_szOffsetShadow.cy = nOffsetY;
		m_szDepthShadow.cx = nDepthX;
		m_szDepthShadow.cy = nDepthY;
		m_nDarkenShadow = min(100, nDarkenPercent);
		m_bGradientShadow = bGradient;
	} //End of SetTooltipShadow

	void SetImageShadow(int nOffsetX, int nOffsetY, BYTE nDarkenPercent  = 50, 
		BOOL bGradient  = TRUE, int nDepthX  = 7, int nDepthY  = 7)
	{
		m_drawer.SetImageShadow(nOffsetX, nOffsetY, nDarkenPercent, bGradient, nDepthX, nDepthY);
	} //End of SetImageShadow

	//Tools
//	void AddToolBar(CToolBar * pBar);
// 	void AddTool(HWND hWnd, DWORD dwIdString, LPCRECT lpRectBounds = NULL, DWORD dwIDTool = 0);
// 	void AddTool(HWND hWnd, LPCTSTR lpszString = NULL, LPCRECT lpRectBounds = NULL, DWORD dwIDTool = 0);
// 	void AddTool(HWND hWnd, PPTOOLTIP_INFO & ti);
// 	//To compatible with old version of CPPToolTip
// 	void AddTool(HWND hWnd, DWORD dwIdString, HICON hIcon, LPCRECT lpRectBounds = NULL, DWORD dwIDTool = 0);
// 	void AddTool(HWND hWnd, DWORD dwIdString, DWORD dwIdIcon, CSize & szIcon = CSize(0, 0), LPCRECT lpRectBounds = NULL, DWORD dwIDTool = 0);
// 	void AddTool(HWND hWnd, LPCTSTR lpszString, HICON hIcon, LPCRECT lpRectBounds = NULL, DWORD dwIDTool = 0);
// 	void AddTool(HWND hWnd, LPCTSTR lpszString, DWORD dwIdIcon, CSize & szIcon = CSize(0, 0), LPCRECT lpRectBounds = NULL, DWORD dwIDTool = 0);
// 	void AddTool(HWND hWnd, LPCTSTR lpszString, DWORD dwIdBitmap, COLORREF crMask, CSize & szBitmap = CSize(0, 0), LPCRECT lpRectBounds = NULL, DWORD dwIDTool = 0);
	
//	void RemoveTool(HWND hWnd, LPCRECT lpRectBounds = NULL);
//	void RemoveAllTools();

// 	BOOL GetToolInfo(PPTOOLTIP_INFO & ti, HWND hWnd, LPCRECT lpRectBounds = NULL);
// 	BOOL GetToolInfo(PPTOOLTIP_INFO & ti, HWND hWnd, DWORD dwIDTool = 0);
// 	void UpdateTipText(LPCTSTR lpszText, HWND hWnd, DWORD dwIDTool = 0);
// 	void DelTool(HWND hWnd, DWORD dwIDTool = 0);
// 	void SetToolRect(HWND hWnd, DWORD dwIDTool, LPCRECT lpRectBounds);
// 
// 	//Help tooltip
// 	void ShowHelpTooltip (LPPOINT pt, DWORD dwIdText, HICON hIcon = NULL);
// 	void ShowHelpTooltip (LPPOINT pt, DWORD dwIdText, DWORD dwIdIcon, CSize & szIcon = CSize(0, 0));
// 	void ShowHelpTooltip (LPPOINT pt, LPCTSTR lpszString, HICON hIcon = NULL);
// 	void ShowHelpTooltip (LPPOINT pt, LPCTSTR lpszString, DWORD dwIdIcon, CSize & szIcon = CSize(0, 0));
// 	void ShowHelpTooltip (LPPOINT pt, PPTOOLTIP_INFO & ti);
// 	void HideTooltip();
// 
// 	// color's functions
// 	void SetColorBk(COLORREF color);
// 	void SetColorBk(COLORREF clrBegin, COLORREF clrEnd);
// 	void SetColorBk(COLORREF clrBegin, COLORREF clrMid, COLORREF clrEnd);
// 	void SetEffectBk(DWORD dwEffect, BYTE nGranularity = 5);
// 
// 	//behaviour's methods
// 	void SetBehaviour(DWORD dwBehaviour = 0);
// 	DWORD GetBehaviour();
// 
// 	void SetDelayTime(DWORD dwDuration, DWORD dwTime);
// 	DWORD GetDelayTime(DWORD dwDuration) const;
// 
// 	void SetImageList(UINT nIdBitmap, int cx, int cy, int nCount, COLORREF crMask = RGB(255, 0, 255));
// 	void SetImageList(HBITMAP hBitmap, int cx, int cy, int nCount, COLORREF crMask = RGB(255, 0, 255));
// 
// 	//functions for sizes
// 	void SetSize(int nSizeIndex, int nValue);
// 	int GetSize(int nSizeIndex);
// 	void SetDefaultSizes(BOOL bBalloonSize = TRUE);
// 
// 	//functions for direction
// 	void SetDirection (DWORD dwDirection = PPTOOLTIP_BOTTOMEDGE_LEFT);
// 	DWORD GetDirection();
// 
// 	void SetCallbackHyperlink(HWND hWnd, UINT nMessage, LPARAM lParam = 0);
// 	
// 	void EnableHyperlink(BOOL bEnable = TRUE);
// 	void SetDebugMode(BOOL bDebug = TRUE);
// 
 	void SetMaxTipWidth(int nWidth = 0){m_drawer.SetMaxWidth(nWidth);};
	inline BYTE GetTransparency() {return m_nTransparency;};

	//////////////////////////////////////////////////////////////////////////
	//
private:
	void _SetNewTooltip(HWND hWnd, const PPTOOLTIP_INFO & ti, BOOL bDisplayWithDelay  = TRUE, TooltipType type  = PPTOOLTIP_NORMAL)
	{
		ATLTRACE (_T("CPPToolTip::SetNewTooltip(hWnd=0x%08X, CRect(left=%d, top=%d, right=%d, bottom=%d), nID=%d)\n"), 
			hWnd, ti.rectBounds.left, ti.rectBounds.top, ti.rectBounds.right, ti.rectBounds.bottom, ti.nIDTool);

		m_bNextToolExist = FALSE;

		//ENG: Hides a tooltip
		if ((PPTOOLTIP_STATE_SHOWING == m_nTooltipState) || 
			(PPTOOLTIP_STATE_SHOWN == m_nTooltipState))
			HideTooltip();

		//ENG: If a tooltip wasn't hidden
		m_nNextTooltipType = type;
		m_hwndNextTool = hWnd;
		m_tiNextTool = ti;
		if (PPTOOLTIP_STATE_HIDEN != m_nTooltipState)
		{
			ATLTRACE(_T("SetNewTooltip2(%d)\n"), m_nNextTooltipType);
			m_bNextToolExist = TRUE;
			if (bDisplayWithDelay && m_dwTimeInitial)
				m_bDelayNextTool = TRUE;
			else 
				m_bDelayNextTool = FALSE;
			return;
		} //if

		//ENG: Start the show timer
		if (bDisplayWithDelay && m_dwTimeInitial)
			SetTimer(TIMER_SHOW, m_dwTimeInitial, NULL);
		else
			OnTimer(TIMER_SHOW);
	} //End of SetNewTooltip


	void _SetAutoPopTimer()
	{
		ATLTRACE (_T("CPPToolTip::SetAutoPopTimer()\n"));
		if (m_dwTimeAutoPop && !(m_tiDisplayed.nBehaviour & PPTOOLTIP_DISABLE_AUTOPOP))
			SetTimer(TIMER_HIDE, m_dwTimeAutoPop, NULL);
	} //End of SetAutoPopTimer

	void _KillTimers(DWORD dwIdTimer  = NULL)
	{
		ATLTRACE (_T("CPPToolTip::KillTimers()\n"));
		if (dwIdTimer == NULL)
		{
			KillTimer(TIMER_SHOW);
			KillTimer(TIMER_HIDE);
			KillTimer(TIMER_SHOWING);
			KillTimer(TIMER_HIDING);
		}
		else 
		{
			KillTimer(dwIdTimer);
		} //if
	}

	void _Pop()
	{
		ATLTRACE (_T("CPPToolTip::Pop()\n"));
		m_nTooltipState = PPTOOLTIP_STATE_HIDEN;
		m_nTooltipType = PPTOOLTIP_NORMAL;
		m_nNextTooltipType = PPTOOLTIP_NORMAL;
		_KillTimers();
		if (_IsVisible())
		{
			if (m_tiDisplayed.nBehaviour & PPTOOLTIP_MULTIPLE_SHOW)
			{
				//If for tool to set a multiple show then reset last window
				m_hwndDisplayedTool = NULL;
				m_tiDisplayed.rectBounds.SetRectEmpty();
			} //if
			ShowWindow(SW_HIDE);
		} //if
	} //End of Pop

	void _PrepareDisplayTooltip(LPPOINT lpPoint)
	{
		ATLTRACE (_T("CPPToolTip::PrepareDisplayTooltip()\n"));

		//Fills default members
		if (!(m_tiNextTool.nMask & PPTOOLTIP_MASK_STYLES))
			m_tiNextTool.nStyles = m_dwStyles;

		if (!(m_tiNextTool.nMask & PPTOOLTIP_MASK_EFFECT))
		{
			m_tiNextTool.nEffect = m_dwEffectBk;
			m_tiNextTool.nGranularity = m_nGranularity;
		} //if

		if (!(m_tiNextTool.nMask & PPTOOLTIP_MASK_COLORS))
		{
			m_tiNextTool.crBegin = m_clrBeginBk;
			m_tiNextTool.crMid = m_clrMidBk;
			m_tiNextTool.crEnd = m_clrEndBk;
		} //if

		if (!(m_tiNextTool.nMask & PPTOOLTIP_MASK_DIRECTION))
			m_tiNextTool.nDirection = m_dwDirection;

		if (!(m_tiNextTool.nMask & PPTOOLTIP_MASK_BEHAVIOUR))
			m_tiNextTool.nBehaviour = m_dwBehaviour;

		if (!(m_tiNextTool.nMask & PPTOOLTIP_MASK_TRANSPARENCY))
			m_tiNextTool.nTransparency = m_nTransparency;

		//Send notify
		POINT pt = *lpPoint; //Pointer in screen coordinates
		_SendNotify(&pt, m_tiNextTool);

		//If string and icon are not exist then exit
		if (m_tiNextTool.sTooltip.IsEmpty())
			return;

		//calculate the width and height of the box dynamically
		CDC dc(GetDC());

		CSize sz (0, 0);
		m_drawer.PrepareOutput(dc.m_hDC, m_tiNextTool.sTooltip, &sz);

		m_rcTipArea.SetRect(0, 0, sz.cx, sz.cy);
		m_rcTooltip = m_rcTipArea;

		//Inflates on MARGIN_CX and MARGIN_CY sizes
		m_rcTipArea.OffsetRect(m_nSizes[PPTTSZ_MARGIN_CX], m_nSizes[PPTTSZ_MARGIN_CY]);
		m_rcTooltip.InflateRect(0, 0, 2 * m_nSizes[PPTTSZ_MARGIN_CX], 2 * m_nSizes[PPTTSZ_MARGIN_CY]);

		//Inflates on 
		//Gets tooltip's rect with anchor
		CPoint ptAnchor;
		m_dwCurDirection = _GetTooltipDirection(m_tiNextTool.nDirection, pt, ptAnchor, m_rcTooltip, m_rcBoundsTooltip, m_rcTipArea);

		//ENG: Clears resources
		_FreeResources();

		//ENG: Creates a new region of the window
		m_hrgnTooltip = _GetTooltipRgn(m_dwCurDirection, ptAnchor.x, ptAnchor.y, m_rcBoundsTooltip.Width(), m_rcBoundsTooltip.Height());

		CRect rect = m_rcBoundsTooltip;

		//ENG: Creates a background bitmap
		m_hBitmapBk = ::CreateCompatibleBitmap(dc.m_hDC, rect.Width(), rect.Height());
		HDC hMemDC = ::CreateCompatibleDC(dc.m_hDC);

		//ENG: Creates a background of the tooltip's body
		HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, m_hBitmapBk);
		m_drawer.GetDrawManager()->FillEffect(hMemDC, 
			m_tiNextTool.nEffect, 
			m_rcTooltip,
			m_tiNextTool.crBegin,
			m_tiNextTool.crMid,
			m_tiNextTool.crEnd,
			m_tiNextTool.nGranularity,
			5);
		//ENG: Fills an anchor
		switch (m_dwCurDirection)
		{
		case PPTOOLTIP_LEFTEDGE_TOP:
		case PPTOOLTIP_LEFTEDGE_VCENTER:
		case PPTOOLTIP_LEFTEDGE_BOTTOM:
			m_drawer.GetDrawManager()->MultipleCopy(hMemDC, m_rcBoundsTooltip.left, m_rcBoundsTooltip.top, m_rcTooltip.left - m_rcBoundsTooltip.left, m_rcBoundsTooltip.Height(),
				hMemDC, m_rcTooltip.left + 1, m_rcBoundsTooltip.top, 1, m_rcBoundsTooltip.Height());
			break;
		case PPTOOLTIP_RIGHTEDGE_TOP:
		case PPTOOLTIP_RIGHTEDGE_VCENTER:
		case PPTOOLTIP_RIGHTEDGE_BOTTOM:
			m_drawer.GetDrawManager()->MultipleCopy(hMemDC, m_rcTooltip.right, m_rcBoundsTooltip.top, m_rcBoundsTooltip.right - m_rcTooltip.right, m_rcBoundsTooltip.Height(),
				hMemDC, m_rcTooltip.right - 1, m_rcBoundsTooltip.top, 1, m_rcBoundsTooltip.Height());
			break;
		case PPTOOLTIP_BOTTOMEDGE_LEFT:
		case PPTOOLTIP_BOTTOMEDGE_CENTER:
		case PPTOOLTIP_BOTTOMEDGE_RIGHT:
			m_drawer.GetDrawManager()->MultipleCopy(hMemDC, m_rcBoundsTooltip.left, m_rcTooltip.bottom, m_rcBoundsTooltip.Width(), m_rcBoundsTooltip.bottom - m_rcTooltip.bottom,
				hMemDC, m_rcBoundsTooltip.left, m_rcTooltip.bottom - 1, m_rcBoundsTooltip.Width(), 1);
			break;
		case PPTOOLTIP_TOPEDGE_LEFT:
		case PPTOOLTIP_TOPEDGE_CENTER:
		case PPTOOLTIP_TOPEDGE_RIGHT:
			m_drawer.GetDrawManager()->MultipleCopy(hMemDC, m_rcBoundsTooltip.left, m_rcBoundsTooltip.top, m_rcBoundsTooltip.Width(), m_rcTooltip.top - m_rcBoundsTooltip.top,
				hMemDC, m_rcBoundsTooltip.left, m_rcTooltip.top + 1, m_rcBoundsTooltip.Width(), 1);
			break;
		} //switch

		::SelectObject(hMemDC, hOldBitmap);
		::DeleteDC(hMemDC);

		//	ReleaseDC(pDC);	//冰峰

		//ENG: Calculate the tooltip's placement on the screen
		rect.left = pt.x - ptAnchor.x;
		rect.top = pt.y - ptAnchor.y;
		rect.right = rect.left + m_rcBoundsTooltip.Width();
		rect.bottom = rect.top + m_rcBoundsTooltip.Height();

		//ENG: If fade-in effect ia available
		if ((SHOWEFFECT_FADEINOUT == m_dwShowEffect) && m_dwTimeFadeIn)
			m_dwCurTransparency = 100;
		else
			m_dwCurTransparency = m_tiNextTool.nTransparency;

		HRGN hrgnWindow = ::CreateRectRgn(0, 0, 0, 0);
		if (m_szOffsetShadow.cx || m_szOffsetShadow.cy)
		{
			//ENG: If shadow will drawn
			HRGN hrgnShadow = ::CreateRectRgn(0, 0, 0, 0);
			::CombineRgn(hrgnShadow, m_hrgnTooltip, hrgnShadow, RGN_COPY);
			::OffsetRgn(hrgnShadow, m_szOffsetShadow.cx, m_szOffsetShadow.cy);
			::CombineRgn(hrgnWindow, m_hrgnTooltip, hrgnShadow, RGN_OR);
			::DeleteObject(hrgnShadow);

			//ENG: Increments the sizes of tooltip to drawing a shadow
			m_rcBoundsTooltip.right += m_szOffsetShadow.cx;
			m_rcBoundsTooltip.bottom += m_szOffsetShadow.cy;
		}
		else
		{
			//ENG: The current window has not a shadow
			::CombineRgn(hrgnWindow, m_hrgnTooltip, NULL, RGN_COPY);
		}//if

		//ENG: Applies a region
		SetWindowRgn(hrgnWindow, FALSE);

		//ENG: Sets a tooltip on the screen
		if (PPTOOLTIP_MENU == m_nTooltipType) 
		{
			SetWindowPos(NULL, 
				rect.left, rect.top,
				m_rcBoundsTooltip.Width(), 
				m_rcBoundsTooltip.Height(),
				SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_NOZORDER/*|SWP_NOCOPYBITS*/);
		}
		else
		{
			SetWindowPos(NULL, 
				rect.left, rect.top,
				m_rcBoundsTooltip.Width(), 
				m_rcBoundsTooltip.Height(),
				SWP_SHOWWINDOW|SWP_NOACTIVATE/*|SWP_NOCOPYBITS*/);
		}
	}

	void _FreeResources()
	{
		if (NULL != m_hrgnTooltip)
		{
			::DeleteObject(m_hrgnTooltip);
			m_hrgnTooltip = NULL;
		} //if

		if (NULL != m_hBitmapBk)
		{
			::DeleteObject(m_hBitmapBk);
			m_hBitmapBk = NULL;
		} //if

		if (NULL != m_hUnderTooltipBk)
		{
			::DeleteObject(m_hUnderTooltipBk);
			m_hUnderTooltipBk = NULL;
		} //if
	} //End of FreeResources

	void _OutputTooltipOnScreen(LPPOINT lpPoint, HDC hDC  = NULL)
	{
		ATLTRACE(_T("OutputTooltipOnScreen()\n"));
		CRect rect = m_rcBoundsTooltip;
		rect.OffsetRect(*lpPoint);
		MoveWindow(rect);
	}

	////////////////////////////////////////////////////////////////////
	// GetTooltipDirection()
	//		Gets a real direction of a tooltip.
	//------------------------------------------------------------------
	// Parameters:
	//		dwDirection		- A default direction of a tooltip. 
	//		lpPoint			- A mouse position in the screen coordinates.
	//		lpAnchor		- An anchor position in the client coordinates
	//      rcBody			- A rectangle of a tooltip's body in the client coordinates
	//		rcFull			- A rectangle of a full tooltip in the client coordinates
	//		rcTipArea		- A rectangle of a tooltip's info area in the client coordinates
	// Return values:
	//		A real direction of a tooltip
	//------------------------------------------------------------------
	// Explanation:
	//    0
	//  0 +------------------------------------+
	//    |                                    |
	//    |             rcBody                 |
	//    |                                    |
	//    |  +------------------------------+  |
	//    |  |                              |  |
	//    |  |         rcTipArea            |  |
	//    |  |                              |  |
	//    |  +------------------------------+  |
	//    |                                    |
	//    +--+...------------------------------+
	//    :  |  /                              :
	//    :  | /        rcFull                 :
	//    :..|/................................:
	//       +- lpAnchor
	//
	////////////////////////////////////////////////////////////////////
	DWORD _GetTooltipDirection(DWORD dwDirection, const CPoint & ptPoint, CPoint & ptAnchor, CRect & rcBody, CRect & rcFull, CRect & rcTipArea)
	{
		//ENG: Get Window's rectangle. The whole virtual desktop .... not only the primary screen.JFN
		CRect rWindow;
		rWindow.left    = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
		rWindow.top     = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
		rWindow.right   = rWindow.left + ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
		rWindow.bottom  = rWindow.top + ::GetSystemMetrics(SM_CYVIRTUALSCREEN);

		//-------------------------------------------
		//ENG: Initializing size of the bounds rect
		rcFull = rcBody;
		switch(dwDirection) 
		{
		case PPTOOLTIP_LEFTEDGE_TOP:
		case PPTOOLTIP_LEFTEDGE_VCENTER:
		case PPTOOLTIP_LEFTEDGE_BOTTOM:
			rcFull.right += m_nSizes [PPTTSZ_HEIGHT_ANCHOR];
			break;
		case PPTOOLTIP_RIGHTEDGE_TOP:
		case PPTOOLTIP_RIGHTEDGE_VCENTER:
		case PPTOOLTIP_RIGHTEDGE_BOTTOM:
			rcFull.right += m_nSizes [PPTTSZ_HEIGHT_ANCHOR];
			break;
		case PPTOOLTIP_BOTTOMEDGE_LEFT:
		case PPTOOLTIP_BOTTOMEDGE_CENTER:
		case PPTOOLTIP_BOTTOMEDGE_RIGHT:
			rcFull.bottom += m_nSizes [PPTTSZ_HEIGHT_ANCHOR];
			break;
		case PPTOOLTIP_TOPEDGE_LEFT:
		case PPTOOLTIP_TOPEDGE_CENTER:
		case PPTOOLTIP_TOPEDGE_RIGHT:
			rcFull.bottom += m_nSizes [PPTTSZ_HEIGHT_ANCHOR];
			break;
		} //switch

		//---------------------------------------------------
		//ENG: If needed change a horizontal direction
		CPoint pt(ptPoint);
		switch(dwDirection) 
		{
		case PPTOOLTIP_LEFTEDGE_TOP:
		case PPTOOLTIP_LEFTEDGE_VCENTER:
		case PPTOOLTIP_LEFTEDGE_BOTTOM:
			pt.x += rcFull.right;
			if (pt.x > rWindow.right)
				dwDirection ^= 0x10;
			break;
		case PPTOOLTIP_RIGHTEDGE_TOP:
		case PPTOOLTIP_RIGHTEDGE_VCENTER:
		case PPTOOLTIP_RIGHTEDGE_BOTTOM:
			pt.x -= rcFull.right;
			if (pt.x < rWindow.left)
				dwDirection ^= 0x10;
			break;
		case PPTOOLTIP_BOTTOMEDGE_LEFT:
		case PPTOOLTIP_TOPEDGE_LEFT:
			pt.x += rcFull.right;
			pt.x -= m_nSizes [PPTTSZ_MARGIN_ANCHOR];
			if (pt.x > rWindow.right)
			{
				pt.x = ptPoint.x - rcFull.right;
				pt.x += m_nSizes [PPTTSZ_MARGIN_ANCHOR];
				if (pt.x < rWindow.left)
					dwDirection |= 0x02;
				else
					dwDirection ^= 0x01;
			} //if
			break;
		case PPTOOLTIP_BOTTOMEDGE_RIGHT:
		case PPTOOLTIP_TOPEDGE_RIGHT:
			pt.x -= rcFull.right;
			pt.x += m_nSizes [PPTTSZ_MARGIN_ANCHOR];
			if (pt.x < rWindow.left)
			{
				pt.x = ptPoint.x + rcFull.right;
				pt.x -= m_nSizes [PPTTSZ_MARGIN_ANCHOR];
				if (pt.x > rWindow.right)
					dwDirection ^= 0x03;
				else
					dwDirection ^= 0x01;
			} //if
			break;
		case PPTOOLTIP_BOTTOMEDGE_CENTER:
		case PPTOOLTIP_TOPEDGE_CENTER:
			if ((ptPoint.x - rWindow.left) <= m_nSizes [PPTTSZ_MARGIN_ANCHOR])
				dwDirection ^= 0x02;
			else if ((rWindow.right - ptPoint.x) <= m_nSizes [PPTTSZ_MARGIN_ANCHOR])
				dwDirection ^= 0x03;
			break;
		} //switch

		//---------------------------------------------------
		//ENG: If needed change a vertical direction
		switch(dwDirection) 
		{
		case PPTOOLTIP_LEFTEDGE_TOP:
		case PPTOOLTIP_RIGHTEDGE_TOP:
			pt.y += rcFull.bottom;
			pt.y -= m_nSizes [PPTTSZ_MARGIN_ANCHOR];
			if (pt.y > rWindow.bottom)
			{
				pt.y = ptPoint.y - rcFull.bottom;
				pt.y += m_nSizes [PPTTSZ_MARGIN_ANCHOR];
				if (pt.y < rWindow.top)
					dwDirection |= 0x02;
				else
					dwDirection ^= 0x01;
			} //if
			break;
		case PPTOOLTIP_LEFTEDGE_BOTTOM:
		case PPTOOLTIP_RIGHTEDGE_BOTTOM:
			pt.y -= rcFull.bottom;
			pt.y += m_nSizes [PPTTSZ_MARGIN_ANCHOR];
			if (pt.y < rWindow.top)
			{
				pt.y = ptPoint.y + rcFull.bottom;
				pt.y -= m_nSizes [PPTTSZ_MARGIN_ANCHOR];
				if (pt.y > rWindow.bottom)
					dwDirection ^= 0x03;
				else
					dwDirection ^= 0x01;
			} //if
			break;
		case PPTOOLTIP_LEFTEDGE_VCENTER:
		case PPTOOLTIP_RIGHTEDGE_VCENTER:
			if ((ptPoint.y - rWindow.top) <= m_nSizes [PPTTSZ_MARGIN_ANCHOR])
				dwDirection ^= 0x02;
			else if ((rWindow.bottom - ptPoint.y) <= m_nSizes [PPTTSZ_MARGIN_ANCHOR])
				dwDirection ^= 0x03;
			break;
		case PPTOOLTIP_BOTTOMEDGE_LEFT:
		case PPTOOLTIP_BOTTOMEDGE_CENTER:
		case PPTOOLTIP_BOTTOMEDGE_RIGHT:
			pt.y -= rcFull.bottom;
			if (pt.y < rWindow.top)
				dwDirection ^= 0x10;
			break;
		case PPTOOLTIP_TOPEDGE_LEFT:
		case PPTOOLTIP_TOPEDGE_CENTER:
		case PPTOOLTIP_TOPEDGE_RIGHT:
			pt.y += rcFull.bottom;
			if (pt.y > rWindow.bottom)
				dwDirection ^= 0x10;
			break;
		} //switch

		//---------------------------------------------------
		//ENG: Set the anchor's point
		switch(dwDirection) 
		{
		case PPTOOLTIP_LEFTEDGE_TOP:
		case PPTOOLTIP_LEFTEDGE_VCENTER:
		case PPTOOLTIP_LEFTEDGE_BOTTOM:
			ptAnchor.x = rcFull.left;
			break;
		case PPTOOLTIP_RIGHTEDGE_TOP:
		case PPTOOLTIP_RIGHTEDGE_VCENTER:
		case PPTOOLTIP_RIGHTEDGE_BOTTOM:
			ptAnchor.x = rcFull.right;
			break;
		case PPTOOLTIP_BOTTOMEDGE_LEFT:
		case PPTOOLTIP_BOTTOMEDGE_CENTER:
		case PPTOOLTIP_BOTTOMEDGE_RIGHT:
			ptAnchor.y = rcFull.bottom;
			break;
		case PPTOOLTIP_TOPEDGE_LEFT:
		case PPTOOLTIP_TOPEDGE_CENTER:
		case PPTOOLTIP_TOPEDGE_RIGHT:
			ptAnchor.y = rcFull.top;
			break;
		} //switch

		//
		switch(dwDirection) 
		{
		case PPTOOLTIP_LEFTEDGE_TOP:
		case PPTOOLTIP_RIGHTEDGE_TOP:
			ptAnchor.y = rcFull.top + m_nSizes [PPTTSZ_MARGIN_ANCHOR];
			break;
		case PPTOOLTIP_LEFTEDGE_BOTTOM:
		case PPTOOLTIP_RIGHTEDGE_BOTTOM:
			ptAnchor.y = rcFull.bottom - m_nSizes [PPTTSZ_MARGIN_ANCHOR];
			break;
		case PPTOOLTIP_LEFTEDGE_VCENTER:
		case PPTOOLTIP_RIGHTEDGE_VCENTER:
			ptAnchor.y = rcFull.bottom / 2;
			if ((ptPoint.y + rcFull.bottom / 2) <= rWindow.bottom)
			{
				if ((ptPoint.y - rcFull.bottom / 2) < rWindow.top)
					ptAnchor.y -= (rcFull.bottom / 2 - ptPoint.y + rWindow.top);
			}
			else if ((ptPoint.y - rcFull.bottom / 2) >= rWindow.top)
			{
				if ((ptPoint.y + rcFull.bottom / 2) > rWindow.bottom)
					ptAnchor.y += (ptPoint.y + rcFull.bottom / 2 - rWindow.bottom);
			} //if
			break;
		case PPTOOLTIP_BOTTOMEDGE_LEFT:
		case PPTOOLTIP_TOPEDGE_LEFT:
			ptAnchor.x = rcFull.left + m_nSizes [PPTTSZ_MARGIN_ANCHOR];
			break;
		case PPTOOLTIP_BOTTOMEDGE_RIGHT:
		case PPTOOLTIP_TOPEDGE_RIGHT:
			ptAnchor.x = rcFull.right - m_nSizes [PPTTSZ_MARGIN_ANCHOR];
			break;
		case PPTOOLTIP_BOTTOMEDGE_CENTER:
		case PPTOOLTIP_TOPEDGE_CENTER:
			ptAnchor.x = rcFull.right / 2;
			if ((ptPoint.x + rcFull.right / 2) <= rWindow.right)
			{
				if ((ptPoint.x - rcFull.right / 2) < rWindow.left)
					ptAnchor.x -= (rcFull.right / 2 - ptPoint.x + rWindow.left);
			}
			else if ((ptPoint.x - rcFull.right / 2) >= rWindow.left)
			{
				if ((ptPoint.x + rcFull.right / 2) > rWindow.right)
					ptAnchor.x += (ptPoint.x + rcFull.right / 2 - rWindow.right);
			} //if
			break;
		} //switch

		//---------------------------------------------------
		//If needed offset anchor
		switch(dwDirection) 
		{
		case PPTOOLTIP_LEFTEDGE_TOP:
		case PPTOOLTIP_RIGHTEDGE_TOP:
		case PPTOOLTIP_LEFTEDGE_VCENTER:
		case PPTOOLTIP_RIGHTEDGE_VCENTER:
			if ((ptPoint.y - ptAnchor.y) < rWindow.top)
				ptAnchor.y = ptPoint.y - rWindow.top;
			break;
		case PPTOOLTIP_LEFTEDGE_BOTTOM:
		case PPTOOLTIP_RIGHTEDGE_BOTTOM:
			if ((ptPoint.y + rcFull.bottom - ptAnchor.y) > rWindow.bottom)
				ptAnchor.y = rcFull.bottom - rWindow.bottom + ptPoint.y;
			break;
		case PPTOOLTIP_BOTTOMEDGE_LEFT:
		case PPTOOLTIP_TOPEDGE_LEFT:
		case PPTOOLTIP_BOTTOMEDGE_CENTER:
		case PPTOOLTIP_TOPEDGE_CENTER:
			if ((ptPoint.x - ptAnchor.x) < rWindow.left)
				ptAnchor.x = ptPoint.x - rWindow.left;
			break;
		case PPTOOLTIP_BOTTOMEDGE_RIGHT:
		case PPTOOLTIP_TOPEDGE_RIGHT:
			if ((ptPoint.x + rcFull.right - ptAnchor.x) > rWindow.right)
				ptAnchor.x = rcFull.right - rWindow.right + ptPoint.x;
			break;
		} //switch

		//*!* I don't know why but without following lines application fails in Release mode!!!!
		WTL::CString str;
		str.Format(_T("0x%08X"), dwDirection);

		//---------------------------------------------
		// Offset the body rectangle
		switch(dwDirection) 
		{
		case PPTOOLTIP_LEFTEDGE_TOP:
		case PPTOOLTIP_LEFTEDGE_VCENTER:
		case PPTOOLTIP_LEFTEDGE_BOTTOM:
			rcTipArea.OffsetRect(m_nSizes [PPTTSZ_HEIGHT_ANCHOR], 0);
			rcBody.OffsetRect(m_nSizes [PPTTSZ_HEIGHT_ANCHOR], 0);
			break;
		case PPTOOLTIP_TOPEDGE_LEFT:
		case PPTOOLTIP_TOPEDGE_CENTER:
		case PPTOOLTIP_TOPEDGE_RIGHT:
			rcTipArea.OffsetRect(0, m_nSizes [PPTTSZ_HEIGHT_ANCHOR]);
			rcBody.OffsetRect(0, m_nSizes [PPTTSZ_HEIGHT_ANCHOR]);
			break;
		} //switch

		return dwDirection;
	}

	HRGN _GetTooltipRgn(DWORD dwDirection, int x, int y, int nWidth, int nHeight)
	{
		HRGN hRgn = NULL;

		HRGN hrgnBody = NULL;
		CRect rcBody(0, 0, nWidth, nHeight);

		HRGN hrgnAnchor = NULL;
		POINT ptAnchor [3];
		ptAnchor [0].x = x;
		ptAnchor [0].y = y;

		HRGN hrgnRect = NULL;

		//------------------------------
		switch(dwDirection) 
		{
		case PPTOOLTIP_LEFTEDGE_TOP:
		case PPTOOLTIP_LEFTEDGE_VCENTER:
		case PPTOOLTIP_LEFTEDGE_BOTTOM:
			rcBody.left += m_nSizes [PPTTSZ_HEIGHT_ANCHOR];
			ptAnchor [1].x = ptAnchor [2].x = rcBody.left;
			break;
		case PPTOOLTIP_RIGHTEDGE_TOP:
		case PPTOOLTIP_RIGHTEDGE_VCENTER:
		case PPTOOLTIP_RIGHTEDGE_BOTTOM:
			rcBody.right -= m_nSizes [PPTTSZ_HEIGHT_ANCHOR];
			ptAnchor [1].x = ptAnchor [2].x = rcBody.right;
			break;
		case PPTOOLTIP_TOPEDGE_LEFT:
		case PPTOOLTIP_TOPEDGE_CENTER:
		case PPTOOLTIP_TOPEDGE_RIGHT:
			rcBody.top += m_nSizes [PPTTSZ_HEIGHT_ANCHOR];
			ptAnchor [1].y = ptAnchor [2].y = rcBody.top;
			break;
		case PPTOOLTIP_BOTTOMEDGE_LEFT:
		case PPTOOLTIP_BOTTOMEDGE_CENTER:
		case PPTOOLTIP_BOTTOMEDGE_RIGHT:
			rcBody.bottom -= m_nSizes [PPTTSZ_HEIGHT_ANCHOR];
			ptAnchor [1].y = ptAnchor [2].y = rcBody.bottom;
			break;
		} //switch

		//------------------------------
		switch(dwDirection) 
		{
		case PPTOOLTIP_LEFTEDGE_TOP:
		case PPTOOLTIP_RIGHTEDGE_TOP:
			ptAnchor [1].y = rcBody.top + m_nSizes [PPTTSZ_MARGIN_ANCHOR];
			ptAnchor [2].y = ptAnchor [1].y + m_nSizes [PPTTSZ_WIDTH_ANCHOR];
			break;
		case PPTOOLTIP_LEFTEDGE_BOTTOM:
		case PPTOOLTIP_RIGHTEDGE_BOTTOM:
			ptAnchor [1].y = rcBody.bottom - m_nSizes [PPTTSZ_MARGIN_ANCHOR];
			ptAnchor [2].y = ptAnchor [1].y - m_nSizes [PPTTSZ_WIDTH_ANCHOR];
			break;
		case PPTOOLTIP_LEFTEDGE_VCENTER:
		case PPTOOLTIP_RIGHTEDGE_VCENTER:
			ptAnchor [1].y = ptAnchor [0].y - m_nSizes [PPTTSZ_WIDTH_ANCHOR] / 2;
			//		ptAnchor [1].y = rcBody.top + (rcBody.Height() - m_nSizes [PPTTSZ_WIDTH_ANCHOR]) / 2;
			ptAnchor [2].y = ptAnchor [1].y + m_nSizes [PPTTSZ_WIDTH_ANCHOR];
			break;
		case PPTOOLTIP_TOPEDGE_LEFT:
		case PPTOOLTIP_BOTTOMEDGE_LEFT:
			ptAnchor [1].x = rcBody.left + m_nSizes [PPTTSZ_MARGIN_ANCHOR];
			ptAnchor [2].x = ptAnchor [1].x + m_nSizes [PPTTSZ_WIDTH_ANCHOR];
			break;
		case PPTOOLTIP_TOPEDGE_RIGHT:
		case PPTOOLTIP_BOTTOMEDGE_RIGHT:
			ptAnchor [1].x = rcBody.right - m_nSizes [PPTTSZ_MARGIN_ANCHOR];
			ptAnchor [2].x = ptAnchor [1].x - m_nSizes [PPTTSZ_WIDTH_ANCHOR];
			break;
		case PPTOOLTIP_TOPEDGE_CENTER:
		case PPTOOLTIP_BOTTOMEDGE_CENTER:
			ptAnchor [1].x = ptAnchor [0].x - m_nSizes [PPTTSZ_WIDTH_ANCHOR] / 2;
			//		ptAnchor [1].x = rcBody.left + (rcBody.Width() - m_nSizes [PPTTSZ_WIDTH_ANCHOR]) / 2;
			ptAnchor [2].x = ptAnchor [1].x + m_nSizes [PPTTSZ_WIDTH_ANCHOR];
			break;
		} //switch

		//------------------------------
		//Gets the tooltip body's region
		hrgnBody = ::CreateRoundRectRgn(rcBody.left, rcBody.top, rcBody.right + 1, rcBody.bottom + 1, 
			m_nSizes[PPTTSZ_ROUNDED_CX], m_nSizes[PPTTSZ_ROUNDED_CY]);

		//Gets the tooltip anchor's region
		if (m_nSizes [PPTTSZ_HEIGHT_ANCHOR] && m_nSizes [PPTTSZ_WIDTH_ANCHOR])
			hrgnAnchor = ::CreatePolygonRgn(ptAnchor, 3, ALTERNATE);
		else
			hrgnAnchor = ::CreateRectRgn(0, 0, 0, 0);

		hRgn = ::CreateRectRgn(0, 0, 0, 0);
		::CombineRgn(hRgn, hrgnBody, hrgnAnchor, RGN_OR);

		if (NULL != hrgnBody)
			::DeleteObject(hrgnBody);
		if (NULL != hrgnAnchor)
			::DeleteObject(hrgnAnchor);

		return hRgn;
	}

	BOOL _IsCursorOverTooltip() const
	{
		ASSERT(m_hParentWnd);

		// Is tooltip visible?
		if (!_IsVisible() || !::IsWindow(m_hWnd))
			return FALSE;

		POINT pt;
		GetCursorPos(&pt);

		HWND hWnd = WindowFromPoint(pt);

		return (hWnd == m_hWnd);
	}

	HWND _GetWndFromPoint(const LPPOINT lpPoint, PPTOOLTIP_INFO & ti, BOOL bCheckTool  = TRUE)
	{
		// the default implementation of tooltips just calls WindowFromPoint
		// which does not work for certain kinds of combo boxes
		CPoint pt = *lpPoint;
		::ClientToScreen(m_hParentWnd, &pt);
		HWND hWnd = ::WindowFromPoint(pt);
		if (NULL != hWnd)
		{
			// try to hit combobox instead of edit control for CBS_DROPDOWN styles
			HWND hWndTemp = ::GetParent(hWnd);
			if (NULL != hWndTemp)
			{
				if (!_IsComboBoxControl(hWndTemp, CBS_DROPDOWN))
				{
					// handle special case of disabled child windows
					::ScreenToClient(hWnd, &pt);
					hWndTemp = ::ChildWindowFromPoint(hWnd, pt);
					if (NULL == hWndTemp)
						return NULL;
					if ((!::IsWindowEnabled(hWndTemp)) && bCheckTool)
						return NULL;
				} //if

				if (_FindTool(hWndTemp, &pt, ti) || !bCheckTool)
					return hWndTemp;
			} //if
		} //if

		return NULL;
	}

	BOOL _IsComboBoxControl(HWND hWnd, UINT nStyle)
	{
		if (hWnd == NULL)
			return FALSE;
		// do cheap style compare first
		if ((UINT)(::GetWindowLong(hWnd, GWL_STYLE) & 0x0F) != nStyle)
			return FALSE;

		// do expensive classname compare next
		TCHAR szCompare[9];
		::GetClassName(hWnd, szCompare, 9);
		return lstrcmpi(szCompare, _T("combobox")) == 0;
	}

	WTL::CString _GetDebugInfoTool(LPPOINT lpPoint)
	{
		PPTOOLTIP_INFO ti;
		HWND hWnd = _GetWndFromPoint(lpPoint, ti, FALSE);
		HWND hParent = ::GetParent (hWnd);

		_TCHAR ch[128];
		WTL::CString str, strTemp;
		WTL::CString strOutput = _T("<table>");

		///////////////////////////////////////////////////////////////////
		//Table of a window
		strOutput += _T("<tr><td><font color=darkblue>Window</font><table border=1>");

		//1. Window's class name and Window Owner's class name
		::GetClassName (hWnd, ch, 128);
		strOutput += _CreateDebugCell(_T("Class name"), ch);

		//2. Window's title and Window Owner's title
		::GetWindowText (hWnd, ch, 128);
		strOutput += _CreateDebugCell(_T("Title"), ch);

		//3. Window's handle and Window Owner's handle
		str.Format(_T("0x%08X"), hWnd);
		strOutput += _CreateDebugCell(_T("Handle"), str);

		//4. Window's ID
		str.Format(_T("%d"), ::GetWindowLong(hWnd, GWL_ID));
		strOutput += _CreateDebugCell(_T("Control ID"), str);

		//5. Window's styles
		str.Format(_T("0x%08X"), (DWORD)::GetWindowLong (hWnd, GWL_STYLE));
		strOutput += _CreateDebugCell(_T("Styles"), str);

		//6. Window's rect
		RECT rc; 
		::GetWindowRect(hWnd, &rc);
		str.Format(_T("(%d, %d)-(%d, %d)"), rc.left, rc.top, rc.right, rc.bottom);
		strOutput += _CreateDebugCell(_T("RECT"), str);

		//7. Window's width
		str.Format(_T("%d"), rc.right - rc.left);
		strOutput += _CreateDebugCell(_T("Width"), str);

		//8. Window's height
		str.Format(_T("%d"), rc.bottom - rc.top);
		strOutput += _CreateDebugCell(_T("Height"), str);

		//9. Window's has tooltip
		HWND hwndTool = _FindTool(lpPoint, ti);
		str = (NULL != hwndTool) ? _T("Yes") : _T("No");
		strOutput += _CreateDebugCell(_T("Has Tooltip"), str);

		strOutput += _T("</table></td>");

		///////////////////////////////////////////////////////////////////
		//Table of a window owner
		strOutput += _T("<td><font color=darkblue>Window Owner</font><table border=1>");

		//1. Window's class name and Window Owner's class name
		if (NULL != hParent)
		{
			::GetClassName (hParent, ch, 128);
			str = _GetMaxDebugString((WTL::CString)ch);
		} //if
		else str = _T("N/A");
		strOutput += _CreateDebugCell(_T("Class name"), str);

		//2. Window's title and Window Owner's title
		if (NULL != hParent)
		{
			::GetWindowText (hParent, ch, 128);
			str = _GetMaxDebugString((WTL::CString)ch);
		} //if
		else str = _T("N/A");
		strOutput += _CreateDebugCell(_T("Title"), str);

		//3. Window's handle and Window Owner's handle
		str.Format(_T("0x%08X"), hParent);
		strOutput += _CreateDebugCell(_T("Handle"), str);

		strOutput += _T("</table>");

		///////////////////////////////////////////////////////////////////
		//Table of a window owner
		strOutput += _T("<br><font color=darkblue>Mouse Cursor</font><table border=1>");

		//1.
		str.Format(_T("%d"), lpPoint->x);
		strOutput += _CreateDebugCell(_T("X"), str);

		//2.
		str.Format(_T("%d"), lpPoint->y);
		strOutput += _CreateDebugCell(_T("Y"), str);

		strOutput += _T("</table></td></tr></table>");

		///////////////////////////////////////////////////////////////////////////
		return strOutput;
	}

	WTL::CString _CreateDebugCell(WTL::CString sTitle, LPCTSTR lpszDescription)
	{
		WTL::CString str;
		str.Format(_T("<tr><td width=70 bgcolor=buttonface>%s</td><td width=130 bgcolor=window>%s</td></tr>"), 
			sTitle, _GetMaxDebugString(lpszDescription));
		return str;
	}

	WTL::CString _GetMaxDebugString(LPCTSTR lpszText)
	{
		WTL::CString str = (WTL::CString)lpszText;
		str.Replace(_T("<"), _T("?")); //Replaces the begins of the tags
		if (str.GetLength() > MAX_LENGTH_DEBUG_STRING)
		{
			str = str.Left(MAX_LENGTH_DEBUG_STRING - 4);
			str += _T(" ...");
		} //if

		return str;
	}

	HWND _FindTool(const LPPOINT lpPoint, PPTOOLTIP_INFO & ti)
	{
		return _GetWndFromPoint(lpPoint, ti, TRUE);
	}

	BOOL _FindTool(HWND hWnd, const LPPOINT lpPoint, PPTOOLTIP_INFO & ti)
	{
		//ENG: Searching a specified HWND
		mapIter item = m_ToolMap.find(hWnd);
		if (item == m_ToolMap.end())
		{
			//ENG: Specified HWND wasn't found
			return FALSE; 
		} //if

		//ENG: Gets the array with the hotarea's parameters
		arHotArea & hotarea = item->second;
		if ((hotarea.size() == 1) && hotarea[0].rectBounds.IsRectEmpty())
		{
			//ENG: If a bounding rectangle of a hotarea wasn't define
			ti = hotarea[0];
			return TRUE;
		} //if

		POINT ptClient = *lpPoint;
		if (hWnd != m_hParentWnd)
		{
			//ENG: If HWND specified window isn't a parent
			::ScreenToClient(hWnd, &ptClient);
		} //if

		//  	CScrollView * pScroll = (CScrollView*)CScrollView::FromHandle(hWnd);
		//  	if (pScroll->IsKindOf(RUNTIME_CLASS(CScrollView))) 
		//  	{
		//  		//ENG: If HWND of CScrollView or derived class then corrects the coordinates
		//  		CPoint ptScroll = pScroll->GetScrollPosition();
		//  		ptClient.x += ptScroll.x;
		//  		ptClient.y += ptScroll.y;
		//  	} //if

		//ENG: Search a hotarea under the mouse
		arHotArea::iterator iter;
		for (iter = hotarea.begin(); iter != hotarea.end(); ++iter)
		{
			ti = *iter;
			if (ti.rectBounds.PtInRect(ptClient))
			{
				//ENG: A hotarea was found
				return TRUE;
			} //if
		} //for

		return FALSE;
	}


	inline BOOL _IsVisible() const {return ((GetStyle() & WS_VISIBLE) == WS_VISIBLE);}
	LRESULT _SendNotify(LPPOINT pt, PPTOOLTIP_INFO & ti) 
	{ 
		// Make sure this is a valid window  
		if (!IsWindow()) 
			return 0L; 
		// See if the user wants to be notified  
		if (!_IsNotify()) 
			return 0L; 

		NM_PPTOOLTIP_DISPLAY lpnm; 
		lpnm.hwndTool = m_hwndNextTool;
		lpnm.pt = pt;  
		lpnm.ti = &ti; 
		lpnm.hdr.hwndFrom = m_hWnd; 
		lpnm.hdr.idFrom   = GetDlgCtrlID(); 
		lpnm.hdr.code     = UDM_TOOLTIP_DISPLAY; 

		::SendMessage(m_hNotifyWnd, WM_NOTIFY, lpnm.hdr.idFrom, (LPARAM)&lpnm);  

		return 0L;
	}

	/////////////////////////////////////////////////////////////////////
	// IsNotify()
	//		This function determines will be send the notification messages from 
	//	the control or not before display.
	//-------------------------------------------------------------------
	// Return value:
	//	TRUE if the control will be notified the specified window
	///////////////////////////////////////////////////////////////////////
	BOOL _IsNotify()
	{
		return (BOOL)(m_hNotifyWnd != NULL);
	}
	void _OnDrawBorder(HDC hDC, HRGN hRgn)
	{
		ASSERT (hDC);
		ASSERT (hRgn);

		::FrameRgn(hDC, hRgn, m_hbrBorder, m_szBorder.cx, m_szBorder.cy);
	} //End OnDrawBorder

	////////////////////////////////////////////////////////////////////////
	//
	//      +-----------------+    +-------------------+   +-----------------+  
	//   +->|     Screen      +--->| m_hUnderTooltipBk |   |   m_hBitmapBk   |
	//   |  +--------+--------+    +-------------------+   +--------+--------+
	//   |           |                                            |
	//   |  +--------V--------+                          +--------V--------+
	//   |  |                 |     +--------------+     |                 |
	//   |  |                 |     |   DrawHTML   |---->|                 |
	//   |  |                 |     +--------------+     |                 |
	//   |  |                 |                          |     MemDC       |
	//   |  |                 |     +--------------+     |                 |
	//   |  |                 |     | OnDrawBorder |---->|                 |
	//   |  |     TempDC      |     +--------------+     +--------+--------+
	//   |  |                 |                                   |         
	//   |  |                 |     +--------------+              |         
	//   |  |                 |<----+  DrawShadow  |              |         
	//   |  |                 |     +--------------+              |         
	//   |  |                 |                                   |         
	//   |  |                 |<--------ALPHA---------------------+         
	//   |  |                 |
	//   |  +--------+--------+
	//   |           |          
	//   +-----------+
	//
	////////////////////////////////////////////////////////////////////////
	void _OnRedrawTooltip(HDC hDC, BYTE nTransparency  = 0)
	{
		ATLTRACE (_T("CPPToolTip::OnRedrawTooltip(Transparency = %d)\n"), nTransparency);

		//ENG: If a transparency more then max value
		if (nTransparency > PERCENT_MAX_TRANSPARENCY)
			nTransparency = PERCENT_MAX_TRANSPARENCY;

		//ENG: If device context not passed
		BOOL bAutoReleaseDC = FALSE;
		if (NULL == hDC)
		{
			hDC = ::GetDC(m_hWnd);
			bAutoReleaseDC = TRUE;
		} //if

		//ENG: Creates memory context
		HDC hMemDC = ::CreateCompatibleDC(hDC);
		HDC hBkDC = ::CreateCompatibleDC(hDC);
		HDC hTempDC = ::CreateCompatibleDC(hDC);
		HBITMAP hOldBkBitmap = (HBITMAP)::SelectObject(hBkDC, m_hBitmapBk);
		HBITMAP hBitmap = ::CreateCompatibleBitmap(hDC, m_rcBoundsTooltip.Width(), m_rcBoundsTooltip.Height());
		HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);
		HBITMAP hTempBitmap = ::CreateCompatibleBitmap(hDC, m_rcBoundsTooltip.Width(), m_rcBoundsTooltip.Height());
		HBITMAP hOldTempBitmap = (HBITMAP)::SelectObject(hTempDC, hTempBitmap);

		//ENG: Gets the rectangle of the tooltip without a shadow
		CRect rect = m_rcBoundsTooltip;
		rect.DeflateRect(0, 0, m_szOffsetShadow.cx, m_szOffsetShadow.cy);

		//ENG: Copy background to the temporary bitmap
		::BitBlt(hMemDC, 0, 0, rect.Width(), rect.Height(),
			hBkDC, 0, 0, SRCCOPY);

		//ENG: Draw HTML string
		m_drawer.DrawPreparedOutput(hMemDC, &m_rcTipArea);

		//ENG: Draw border of the tooltip
		if ((NULL != m_hbrBorder) && (m_szBorder.cx) && (m_szBorder.cy))
			_OnDrawBorder(hMemDC, m_hrgnTooltip);

		if (NULL == m_hUnderTooltipBk)
		{
			//ENG: Stores a background under the tooltip to a bitmap
			m_hUnderTooltipBk = ::CreateCompatibleBitmap(hDC, m_rcBoundsTooltip.Width(), m_rcBoundsTooltip.Height());
			::SelectObject(hBkDC, m_hUnderTooltipBk);
			::BitBlt(hBkDC, 0, 0, m_rcBoundsTooltip.Width(), m_rcBoundsTooltip.Height(),
				hDC, m_rcBoundsTooltip.left, m_rcBoundsTooltip.top, SRCCOPY);
		}
		else
		{
			//ENG: Restores a background from a bitmap
			::SelectObject(hBkDC, m_hUnderTooltipBk);
		} //if

		//ENG: Copy a original background bitmap to the temporary DC
		::BitBlt(hTempDC, m_rcBoundsTooltip.left, m_rcBoundsTooltip.top, m_rcBoundsTooltip.Width(), m_rcBoundsTooltip.Height(),
			hBkDC, 0, 0, SRCCOPY);

		//ENG: Draws a shadow
		if (m_szOffsetShadow.cx || m_szOffsetShadow.cy)
		{
			//ENG: Creates a mask of the tooltip for shadow
			HBITMAP hMask = ::CreateCompatibleBitmap(hDC, rect.Width(), rect.Height());
			HDC hMaskDC = ::CreateCompatibleDC(hDC);
			//ENG: Creates a mask of the tooltip
			BYTE nColor = LOBYTE(::MulDiv(255, 100 - m_nDarkenShadow, 100));
			nColor += ((255 - nColor) * nTransparency) / 100;
			HBRUSH hBrush = ::CreateSolidBrush(RGB(nColor, nColor, nColor));
			HBITMAP hOldMask = (HBITMAP)::SelectObject(hMaskDC, hMask);
			::BitBlt(hMaskDC, 0, 0, rect.Width(), rect.Height(), NULL, 0, 0, WHITENESS);
			::FillRgn(hMaskDC, m_hrgnTooltip, hBrush);
			::DeleteObject(hBrush);
			::SelectObject(hMaskDC, hOldMask);
			::DeleteDC(hMaskDC);

			//
			m_drawer.GetDrawManager()->DrawShadow(hTempDC, 
				m_szOffsetShadow.cx, 
				m_szOffsetShadow.cy,
				rect.Width(), rect.Height(), hMask,
				m_bGradientShadow, 
				m_szDepthShadow.cx, m_szDepthShadow.cy);
			::DeleteObject(hMask);
		} //if

		//ENG: Merges a tooltip on with the client area 
		::SelectClipRgn(hTempDC, m_hrgnTooltip);
		m_drawer.GetDrawManager()->AlphaBitBlt(hTempDC, m_rcBoundsTooltip.left, m_rcBoundsTooltip.top,
			rect.Width(), rect.Height(),
			hMemDC, 0, 0, 100 - nTransparency);
		::SelectClipRgn(hTempDC, NULL);

		//ENG: Output a tooltip to the screen
		::BitBlt(hDC, m_rcBoundsTooltip.left, m_rcBoundsTooltip.top,
			m_rcBoundsTooltip.Width(), m_rcBoundsTooltip.Height(),
			hTempDC, 0, 0, SRCCOPY);

		//ENG: Free resources
		::SelectObject(hBkDC, hOldBkBitmap);
		::SelectObject(hMemDC, hOldBitmap);
		::SelectObject(hTempDC, hOldTempBitmap);
		::DeleteObject(hBitmap);
		::DeleteObject(hTempBitmap);
		::DeleteDC(hBkDC);
		::DeleteDC(hMemDC);
		::DeleteDC(hTempDC);

		if (bAutoReleaseDC)
			::ReleaseDC(m_hWnd, hDC);
	}

};

/////////////////////////////////////////////////////////////////////////////
#endif

