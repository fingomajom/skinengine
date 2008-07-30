///////////////////////////////////////////////////////////////
//
//	Filename: 	skinheaderctrl.h
//	Creator:	冰峰 & lichenglin
//	Date:		2008-7-24  15:24
//	Comment:	
//
///////////////////////////////////////////////////////////////


#ifndef _skinheaderctrl_h_
#define _skinheaderctrl_h_
#include "../ksguicommon/drawer/KFormatDrawer.h"


namespace KSGUI{



//////////////////////////////////////////////////////////////////////////
// Header Ctrl 扩展风格
#define HDS_CT_DRAWFRAME            0x00000001
#define HDS_CT_FIXWIDTH             0x00000002
#define HDS_CT_TEXT_TRANSPARENT     0x00000004




//////////////////////////////////////////////////////////////////////////
// 自绘Header Ctrl
template<class T>
class CSkinHeaderCtrlExImpl : public SkinWindowImpl<T, CHeaderCtrl>
{
public:
	typedef SkinWindowImpl<T, CHeaderCtrl>			                _Base;
	typedef CSkinHeaderCtrlExImpl									_Self;

private:
	typedef std::vector<KSingleLineHTMLControl*>					_Container;
	typedef _Container::iterator									_Iter;

	_Container					m_Container;
	UINT						m_nCustomStyle;				//自定义样式

	int							m_nHeight;					//高度
	COLORREF					m_rGridLine;				//网格线颜色
	COLORREF					m_rBkColor;

	//////////////////////////////////////////////////////////////////////////
	// Init
public:
	CSkinHeaderCtrlExImpl()
	{
		m_nCustomStyle = 0;
		m_rGridLine = RGB(128, 128, 128);
		m_rBkColor = RGB(255, 255, 255);
		m_nHeight = 30;
	}

	~CSkinHeaderCtrlExImpl()
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
	void SetHeight(int nHeight)
	{
		m_nHeight = nHeight;
		CRect rc;
		GetClientRect(&rc);
		SetWindowPos(NULL, 0, 0, rc.Width(), nHeight, SWP_NOMOVE);
	}
	void SetGridLineColor(COLORREF rColor)
	{
		m_rGridLine = rColor;
	}
	void SetBkColor(COLORREF rColor)
	{
		m_rBkColor = rColor;
	}
	void SetCustomStyle(UINT nStyle)
	{
		m_nCustomStyle = nStyle;
	}
	BOOL SetRichText(int nItem, LPCTSTR szRichText, int nWidth = - 1, int nIndent = 0, int nAlignType = AT_LEFT)
	{
		if (nItem >= (int)m_Container.size())
		{
			_InsertTakePlaceItem(GetItemCount(), nItem);
			m_Container.resize(nItem + 1);
		}

		if (m_Container[nItem] == 0)
			m_Container[nItem] = new KSingleLineHTMLControl;

		// Setting
		if (nWidth != -1)
			SetWidth(nItem, nWidth);

		KSingleLineHTMLControl *pDrawer = m_Container[nItem];
		pDrawer->SetIndent(nIndent);
		pDrawer->SetAlignStyle(nAlignType);
		pDrawer->SetPageSize(nWidth, m_nHeight);
		return pDrawer->SetRichText(szRichText);
	}
	BOOL SetWidth(int nItem, int nWidth)
	{
		ASSERT(nItem < (int)m_Container.size());
		if (nItem < (int)m_Container.size())
		{
			m_Container[nItem]->SetPageSize(nWidth);
		}

		//
		HDITEM hditem = { 0 };
		hditem.mask = HDI_WIDTH;
		hditem.cxy	= nWidth;

		return _Base::SetItem(nItem, &hditem);
	}

	//////////////////////////////////////////////////////////////////////////
	// 消息
public:

	BEGIN_MSG_MAP(CSkinHeaderCtrlExImpl<T>)
		MESSAGE_HANDLER(WM_CREATE , OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(HDM_LAYOUT, OnLayOut)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)

		//
		REFLECTED_NOTIFY_CODE_HANDLER(HDN_BEGINTRACK,	OnBeginTrack)	//当它作为ListView的标头时，是收不到此消息的
		REFLECTED_NOTIFY_CODE_HANDLER(HDN_ITEMCHANGINGW, OnColumnWidthChange)
		REFLECTED_NOTIFY_CODE_HANDLER(HDN_ITEMCHANGINGA, OnColumnWidthChange)
		REFLECTED_NOTIFY_CODE_HANDLER(HDN_BEGINTRACK,	OnBeginChangeColumnWidth)
		REFLECTED_NOTIFY_CODE_HANDLER(HDN_ENDTRACK,		OnEndChangeColumnWidth)

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
		for (int i = (int)m_Container.size() - 1; i >= 0; --i)
		{
			ASSERT(m_Container[i]);
			if (m_Container[i])
				delete m_Container[i];
		}

		m_Container.clear();

		bHandled = FALSE;
		return 1L;
	}

	LRESULT OnLayOut(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LPHDLAYOUT pLayout = (LPHDLAYOUT)lParam;
		pLayout->pwpos->hwnd			= m_hWnd;
		pLayout->pwpos->hwndInsertAfter = NULL;
		pLayout->pwpos->flags			= SWP_FRAMECHANGED;

		pLayout->pwpos->x				= pLayout->prc->left;
		pLayout->pwpos->y				= 0;
		pLayout->pwpos->cx				= pLayout->prc->right - pLayout->prc->left;
		pLayout->pwpos->cy				= m_nHeight;
		pLayout->prc->top				= m_nHeight; 
		return 1L;
	};

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		CPaintDC dc(m_hWnd);

		HFONT hFont = _Base::GetFont();
		HFONT hOldFont = dc.SelectFont(hFont);

		int nColumns = GetItemCount();
		for (int i = 0; i < nColumns; ++i)
		{
			_DrawItem(dc.m_hDC, i);
		}

		dc.SelectFont(hOldFont);

		// Draw Frame
		if (m_nCustomStyle & HDS_CT_DRAWFRAME)
		{
			HBRUSH brush = (HBRUSH)GetStockObject(NULL_BRUSH);
			HBRUSH oldBrush = dc.SelectBrush(brush);
			CPen pen;
			pen.CreatePen(PS_SOLID, 1, m_rGridLine);
			HPEN oldPen = dc.SelectPen(pen);
			CRect rc;
			GetClientRect(&rc);
			dc.Rectangle(&rc);
			dc.SelectPen(oldPen);
			dc.SelectBrush(oldBrush);
		}

		return 0;
	}

	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		GetParent().Invalidate();
		bHandled = FALSE;
		return 0;
	}
	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (m_nCustomStyle & HDS_CT_FIXWIDTH)
			return TRUE;

		bHandled = FALSE;
		return 0;
	}
	LRESULT OnBeginTrack(int /*wParam*/, LPNMHDR /*lParam*/, BOOL& bHandled)
	{
		if (m_nCustomStyle & HDS_CT_FIXWIDTH)
			return 1L;											//禁止用户拖动标头

		return 0L;
	};

	LRESULT OnColumnWidthChange(int /*wParam*/, LPNMHDR lParam, BOOL& bHandled)
	{
		return 0;

		NMHEADER *pItem = (NMHEADER*)lParam;
		int nItem = pItem->iItem;
		int nCount = _Base::GetItemCount();

		CRect rc, rcTemp;
		_Base::GetClientRect(&rc);
		int nSpace = 0;
		for (int i = 0; i <= nItem; ++i)
		{
			_Base::GetItemRect(i, &rcTemp);
			nSpace += rcTemp.Width();
		}

		nSpace += 20 * (nCount - nItem - 1);
		if (nSpace > rc.Width())
		{
			return TRUE;
		}

		return FALSE;
	}

	LRESULT OnBeginChangeColumnWidth(int /*wParam*/, LPNMHDR lParam, BOOL& bHandled)
	{

		return 0;
	}
	LRESULT OnEndChangeColumnWidth(int /*wParam*/, LPNMHDR lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;

		NMHEADER *pItem = (NMHEADER*)lParam;
		int nItem = pItem->iItem;
		int nCount = _Base::GetItemCount();

		CRect rc, rcTemp;
		_Base::GetClientRect(&rc);
		rc.right -= 15;
		int nSpace = 0;
		for (int i = 0; i < nCount; ++i)
		{
			_Base::GetItemRect(i, &rcTemp);
			nSpace += rcTemp.Width();
		}

		nSpace = nSpace - rc.Width();							// 多出的Space
		if (nCount - nItem - 1 == 0)
			return 0;

		int nvSpace = nSpace / (nCount - nItem - 1);	// 后面项平均每项要减少的Sapce

		for (int i = nItem + 1; i < nCount; ++i)
		{
			_Base::GetItemRect(i, &rcTemp);
			//			if (i < nCount - 1)
			SetWidth(i, rcTemp.Width() - nvSpace);
			//			else
			//				SetWidth(i, rcTemp.Width() - nvSpace + 15);

			//			nSpace -= nvSpace;
		}
		return 0;
	}

	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
	{
		int nColumns = GetItemCount();

		CDCHandle dc(lpDrawItemStruct->hDC);
		int nDC = dc.SaveDC();

		CRect rc;
		for (int i = 0; i < nColumns; ++i)
		{
			_DrawItem(lpDrawItemStruct->hDC, lpDrawItemStruct->itemID);
		}

		dc.RestoreDC(nDC);
		return;
	}

	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
	{
		return;
	}

	void DeleteItem(LPDELETEITEMSTRUCT /*lpDeleteItemStruct*/)
	{
		return;
	}


	//////////////////////////////////////////////////////////////////////////
	// Helper
private:
	void _Init()
	{
		ModifyStyle(0, HDS_FLAT);
	}

	inline void _DrawItem(HDC hdc, int nItem)
	{
		CDCHandle dc(hdc);
		CRect rc;
		GetItemRect(nItem, &rc);

		// Draw Bk
		CPen pen;
		pen.CreatePen(PS_SOLID, 1, m_rGridLine);
		HPEN oldPen = dc.SelectPen(pen);

		dc.FillSolidRect(rc.left, rc.top, rc.right, rc.bottom, m_rBkColor);
		if (nItem > 0)
			_DrawLine(dc, rc.left, rc.top, rc.left, rc.bottom);

		_DrawLine(dc, rc.right, rc.top, rc.right, rc.bottom);
		_DrawLine(dc, rc.left, rc.bottom - 1, rc.right, rc.bottom - 1);

		ASSERT(nItem < (int)m_Container.size());
		if (nItem < (int)m_Container.size())
		{
			int bm = dc.GetBkMode();
			if (m_nCustomStyle & HDS_CT_TEXT_TRANSPARENT)
				dc.SetBkMode(TRANSPARENT);

			m_Container[nItem]->Draw(dc, &rc);
			dc.SetBkMode(bm);
		}

		dc.SelectPen(oldPen);
		return;
	}

	inline void _DrawLine(CDCHandle &dc, int x1, int y1, int x2, int y2)
	{
		dc.MoveTo(x1, y1);
		dc.LineTo(x2, y2);
	}

	inline void _InsertTakePlaceItem(int nStart, int nEnd)
	{
		HDITEM hditem = { 0 };
		hditem.pszText = _T("");
		hditem.cchTextMax = 0;
		hditem.mask = HDI_TEXT;

		for (int i = nStart; i <= nEnd; ++i)
		{
			AddItem(&hditem);
		}
	}
};





class CSkinHeaderCtrlEx : public CSkinHeaderCtrlExImpl<CSkinHeaderCtrlEx>
{

};

}

#endif // _skinheaderctrl_h_