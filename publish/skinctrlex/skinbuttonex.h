/* -------------------------------------------------------------------------
// 文件名		:	ksgui/skinctrlex/skinbuttonex.h
// 创建人		:	冰峰
// 创建时间		:	2008-4-28 16:24:25
// 功能描述     :	增强自由的Button
//
// $Id: $
// -----------------------------------------------------------------------*/
#ifndef __KSGUI_SKINCTRLEX_SKINBUTTONEX_H__
#define __KSGUI_SKINCTRLEX_SKINBUTTONEX_H__

#include "../ksguicommon/drawer/KFormatDrawer.h"

namespace KSGUI{

#define _INNER_SPACE_AFTER_PREFIX									4

//////////////////////////////////////////////////////////////////////////
// 增强自由的ButtonEx
class CSkinButtonEx : public SkinWindowImpl<CSkinButtonEx, CButton>,
					  public WTL::COwnerDraw<CSkinButtonEx>
{
public:
	typedef SkinWindowImpl<CSkinButtonEx, CButton>					_Base;
	typedef CSkinButtonEx											_Self;

private:
	KSingleLineHTMLControl		m_Drawer;
	CImageList					m_imPrefixState;			//单选框/复选框的状态
	CToolTipCtrl				m_ToolTip;
	WTL::CString				m_strToolTip;				//Tooltip String
	UINT						m_nCustomStyle;				//自定义样式

	// Internal states
	unsigned m_nChecked:4;
	unsigned m_bMouseOver:1;
	unsigned m_bFocus:1;
	unsigned m_bPressed:1;

	//////////////////////////////////////////////////////////////////////////
	// Init
public:
	CSkinButtonEx()
	{
		m_bMouseOver	= 0;
		m_bPressed		= 0;

	}

	~CSkinButtonEx()
	{

	}

	BOOL SubclassWindow(HWND hWnd)
	{
		ATLASSERT(m_hWnd == NULL);
		ATLASSERT(::IsWindow(hWnd));

		BOOL bRet = _Base::SubclassWindow(hWnd);
		if(bRet)
			_Init();

		return bRet;
	}


	//////////////////////////////////////////////////////////////////////////
	// 导出函数
public:
	void SetCustomStyle(UINT nStyle)
	{
		m_nCustomStyle = nStyle;
	}
	BOOL SetRichText(LPCTSTR szRichText, int nAlignType = AT_DEFAULT, int nIndent = 0)
	{
		//Setting
		CRect rc;
		GetClientRect(&rc);
		m_Drawer.SetPageSize(rc.Width() - _GetLeftInnerWidth(), rc.Height());
		m_Drawer.SetAlignStyle(nAlignType);
		m_Drawer.SetIndent(nIndent);
		m_Drawer.SetRichText(szRichText);
		return TRUE;
	}
	BOOL SetPrefixState(HBITMAP hBitmap, COLORREF rMask, int cx = -1, int cy = -1)
	{
		CBitmapHandle bitmap(hBitmap);
		CSize size;
		bitmap.GetSize(size);
		if (cx == -1)
			cx = size.cx;
		if (cy == -1)
			cy = size.cy;

		m_imPrefixState.Create(cx, cy, ILC_COLOR16 | ILC_MASK, 0, 0);
		m_imPrefixState.Add(hBitmap, rMask);

		return TRUE;
	}
	BOOL SetImage(LPCTSTR szName, HIMAGELIST hImageList)
	{
		return KImageListManager::Instance()->AddImageList(szName, hImageList);
	}

	BOOL SetToolTipText(LPCTSTR szToolTipText)
	{
		if (szToolTipText == NULL)								//清掉
		{
			m_ToolTip.Activate(FALSE);
			return TRUE;
		}

		//
		if(!m_ToolTip.IsWindow())
		{
			m_strToolTip = szToolTipText;
			if(m_ToolTip.Create(m_hWnd))
			{
				CRect rc;
				GetClientRect(&rc);
				if(m_ToolTip.AddTool(m_hWnd, (LPCTSTR)m_strToolTip), &rc)
				{
					m_ToolTip.Activate(TRUE);
				}
			}
		} 
		else 
		{
			m_ToolTip.UpdateTipText(szToolTipText, m_hWnd);
		}

		return TRUE;
	}

	//////////////////////////////////////////////////////////////////////////
	// 消息
public:
	BEGIN_MSG_MAP(CSkinButtonEx)
		MESSAGE_HANDLER(WM_CREATE ,		OnCreate)
		MESSAGE_HANDLER(WM_DESTROY,		OnDestroy)
		MESSAGE_HANDLER(BM_SETCHECK,	OnSetCheck)
		MESSAGE_HANDLER(BM_GETCHECK,	OnGetCheck)

		MESSAGE_HANDLER(WM_ERASEBKGND,	OnEraseBackground)

		MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)

		CHAIN_MSG_MAP_ALT(COwnerDraw<_Self>, 1)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;

		_Init();
		return 1L;
	}
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		//放掉数据
		m_imPrefixState.Destroy();

		if (m_ToolTip.IsWindow())
			m_ToolTip.DestroyWindow();

		bHandled = FALSE;
		return 1L;
	}
	LRESULT OnSetCheck(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		m_nChecked = wParam;
		
		// 组中其它项都清掉
		HWND hwndParent = GetParent();
		ASSERT(hwndParent);
		HWND hwndNext = m_hWnd;
		for ( ; wParam == BST_CHECKED; )
		{
			hwndNext = ::GetNextDlgGroupItem(hwndParent, hwndNext, FALSE);
			if (hwndNext == m_hWnd)
				break;
			::SendMessage(hwndNext, BM_SETCHECK, BST_UNCHECKED, 0);
		}

		if (m_nChecked == BST_UNCHECKED)
			Invalidate();

		return 1L;
	}
	LRESULT OnGetCheck(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return m_nChecked;
	}

	LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
 		MSG msg = { m_hWnd, uMsg, wParam, lParam };
 		if(m_ToolTip.IsWindow())
 			m_ToolTip.RelayEvent(&msg);
 		bHandled = FALSE;
		return 1L;
	}
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		LRESULT lRet = DefWindowProc(uMsg, wParam, lParam);

 		m_bPressed = TRUE;
 		Invalidate();
 		UpdateWindow();

		return lRet;
	}

	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		LRESULT lRet = DefWindowProc(uMsg, wParam, lParam);

		if(m_bPressed)
		{
			m_bPressed = FALSE;
			::SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
		}

		_Base::SetCheck(BST_CHECKED);

		m_ToolTip.Activate(TRUE);
		return lRet;
	}

	LRESULT OnEnable(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		Invalidate();
		UpdateWindow();
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		if(m_bMouseOver == 0)
		{
			m_bMouseOver = 1;
			Invalidate();
			UpdateWindow();
			StartTrackMouseLeave();
		}

		bHandled = FALSE;
		return 1;
	}

	LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(m_bMouseOver == 1)
		{
			m_bMouseOver = 0;
			Invalidate();
			UpdateWindow();
		}

		bHandled = FALSE;
		return 1;
	}

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 1;   // no background needed
	}

	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
	{
		CDCHandle dc(lpDrawItemStruct->hDC);
		CRect rc(lpDrawItemStruct->rcItem);

		if (_HasPrefixState())
		{
			int nImageIndex;
			SIZE size;
			m_imPrefixState.GetIconSize(size);
			int nOffsetY = (rc.Height() - size.cy) / 2;
			if (m_nChecked == BST_CHECKED /*&& !m_bPressed*/)
				nImageIndex = 3;
			else
				nImageIndex = 0;

			if (m_bMouseOver)
				nImageIndex += 1;
//			if (m_bPressed)
//				nImageIndex += 2;

			m_imPrefixState.Draw(dc, nImageIndex, 0, nOffsetY, ILD_TRANSPARENT);
			rc.left += _GetLeftInnerWidth();
		}

		dc.SetBkMode(TRANSPARENT);
		m_Drawer.Draw(dc, &rc);
	}

	//////////////////////////////////////////////////////////////////////////
	// Helper
private:
	void _Init()
	{
		ModifyStyle(0, BS_OWNERDRAW);
	}
	inline BOOL _IsRadioButton()
	{
		DWORD dwStyle = GetStyle();
		return dwStyle & BS_RADIOBUTTON || dwStyle & BS_AUTORADIOBUTTON;
	}
	inline BOOL _IsCheckBoxButton()
	{
		DWORD dwStyle = GetStyle();
		return dwStyle & BS_CHECKBOX || dwStyle & BS_AUTOCHECKBOX;
	}
	inline BOOL _HasPrefixState()
	{
		return _IsRadioButton() || _IsCheckBoxButton();
	}
	inline int _GetLeftInnerWidth()
	{
		CSize size;
		if (_IsRadioButton() || _IsCheckBoxButton())
		{
			m_imPrefixState.GetIconSize(size);
			return size.cx + _INNER_SPACE_AFTER_PREFIX;
		}
		else
		{
			return 0;
		}
	}

	BOOL StartTrackMouseLeave()
	{
		TRACKMOUSEEVENT tme = { 0 };
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_hWnd;
		return _TrackMouseEvent(&tme);
	}

};

};

// -------------------------------------------------------------------------
// $Log: $

#endif /* __KSGUI_SKINCTRLEX_SKINBUTTONEX_H__ */
