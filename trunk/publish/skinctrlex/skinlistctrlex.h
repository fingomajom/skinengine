/* -------------------------------------------------------------------------
// 文件名		:	ksgui/skinctrlex/skinlistctrlex.h
// 创建人		:	冰峰
// 创建时间		:	2008-4-22 17:28:40
// 功能描述     :	ListCtrlEx
//
// $Id: $
// -----------------------------------------------------------------------*/
#ifndef __KSGUI_SKINCTRLEX_SKINLISTCTRLEX_H__
#define __KSGUI_SKINCTRLEX_SKINLISTCTRLEX_H__

#include "../ksguicommon/drawer/KFormatDrawer.h"
#include <algorithm>

#pragma warning(push)
#pragma warning (disable : 4267 4995 4018)

namespace KSGUI{

//////////////////////////////////////////////////////////////////////////
// ListViewEx自定义样式风格
typedef enum
{
	// 特征风格
	LVS_CT_NOSELECT				= 0x00000001,				//不绘选择
	LVS_CT_HEADEREX				= 0x00000004,				//增强的Header
	LVS_CT_FIXWIDTH				= 0x00000008,				//固定列宽，不让用户拖动
	LVS_CT_KEEP_COL_SUM_WIDTH	= 0x00000010,				//保持总列宽，效果为拖动头时自动改变其它列宽，始终不出现水平滚动条

	// 绘制风格
	LVS_CT_DRAWTITLE			= 0x00001000,				//绘Title
	LVS_CT_DRAWFRAME			= 0x00002000,				//绘制边框
	LVS_CT_TEXT_TRANSPARENT		= 0x00004000,				//文字背景
	LVS_CT_HGRIDLINES			= 0x00008000,				//水平网络线
	LVS_CT_VGRIDLINES			= 0x00010000,				//垂直网络线

	// 注意：给扩展控件使用 最前面3个字节给  0x00011111

} _CustomStyle;

//////////////////////////////////////////////////////////////////////////
// 自绘Header Ctrl
class CSkinHeaderCtrlEx : 
	public SkinWindowImpl<CSkinHeaderCtrlEx, CHeaderCtrl>
{
public:
	typedef SkinWindowImpl<CSkinHeaderCtrlEx, CHeaderCtrl>			_Base;
	typedef CSkinHeaderCtrlEx										_Self;

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
	CSkinHeaderCtrlEx()
	{
		m_nCustomStyle = 0;
		m_rGridLine = RGB(128, 128, 128);
		m_rBkColor = RGB(255, 255, 255);
		m_nHeight = 30;
	}

	~CSkinHeaderCtrlEx()
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
		if (nItem >= m_Container.size())
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
		ASSERT(nItem < m_Container.size());
		if (nItem < m_Container.size())
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

	BEGIN_MSG_MAP(CSkinHeaderCtrlEx)
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
		for (int i = m_Container.size() - 1; i >= 0; --i)
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
		if (m_nCustomStyle & LVS_CT_DRAWFRAME)
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
		if (m_nCustomStyle & LVS_CT_FIXWIDTH)
			return TRUE;

		bHandled = FALSE;
		return 0;
	}
	LRESULT OnBeginTrack(int /*wParam*/, LPNMHDR /*lParam*/, BOOL& bHandled)
	{
		if (m_nCustomStyle & LVS_CT_FIXWIDTH)
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

		ASSERT(nItem < m_Container.size());
		if (nItem < m_Container.size())
		{
			int bm = dc.GetBkMode();
			if (m_nCustomStyle & LVS_CT_TEXT_TRANSPARENT)
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

typedef BOOL _Fun_Enum(KSingleLineHTMLControl *pCell, WPARAM wParam, LPARAM lParam);
//////////////////////////////////////////////////////////////////////////
// 增加的ListViewCtrl，里面的内容可以直接输入Html指定
class CSkinListViewCtrlEx : 
	public SkinWindowImpl<CSkinListViewCtrlEx, CSkinListViewCtrl>,
	public COwnerDraw<CSkinListViewCtrlEx>
{
public:
	//DECLARE_WND_SUPERCLASS(_T("KSG_SkinListViewCtrlEx"), GetWndClassName())

	// 显示样式
	typedef enum
	{
		ST_NORMAL,
		ST_ONELINE

	} ShowType;

	typedef SkinWindowImpl<CSkinListViewCtrlEx, CSkinListViewCtrl>	_Base;
	typedef CSkinListViewCtrlEx										_Self;

	//////////////////////////////////////////////////////////////////////////
	// 私有数据
private:

	// 单元格类型
	typedef enum
	{
		CT_DrawerCtrl,
		CT_Preset,

	} _CellType;

	// 单元格
	typedef struct tagCell
	{
		_CellType					eCellType;						//单元数据的类型
		union
		{
			int						nPresetIndex;					//预设DrawerCtrl的索引
			KSingleLineHTMLControl	*pDrawerCtrl;
		};
		tagCell()
		{
			eCellType = CT_DrawerCtrl;
			pDrawerCtrl = NULL;
		}
	} _Cell;

	typedef struct tagItemData 
 	{
 		DWORD_PTR							ItemData;				//ItemData（也就是给控件使用者设置的)
		int									m_nGroupLevel;			//组的级别，类似树型控件的级别，可能折叠某级别
		BOOL								m_bExpand;				//组展开/折叠状态
		ShowType							m_eShowtype;			//显示样式
		COLORREF							m_rbkColor;				//背景色
 		std::vector<_Cell>					vecSubItems;			//子项
		tagItemData()
		{
			ItemData		= NULL;
			m_nGroupLevel	= 0;
			m_eShowtype		= ST_NORMAL;
			m_bExpand		= TRUE;
			m_rbkColor		= Undefine;
		}

 	} _ItemData;
	typedef std::vector<_Cell>::iterator			_ColIter;	// 列迭代器
	
	typedef std::vector<_ItemData*>					_Items;
	typedef _Items::iterator						_RowIter;	// 行迭代器

	typedef std::vector<KSingleLineHTMLControl*>	_PresetContainer;
	typedef _PresetContainer::iterator				_Preset_Iter;

	_Items						m_Items;
	_PresetContainer			m_vecPresetDrawerCtrl;		//预设的项，主要用来作多项重复时的优化

	CImageList					m_EmptyImageList;			//用来设置行高用的

	BOOL						m_bSel3D;
	COLORREF					m_rSelect;					//选中项的着色
	COLORREF					m_rSelBound;				//选中项边框颜色
	int							m_nSelLineStyle;			//选中项边框样式

	COLORREF					m_rHGridLine;				//水平网格线颜色
	int							m_nHGridLineStyle;			//水平网格线样式
	COLORREF					m_rVGridLine;				//垂直网格线颜色
	int							m_nVGridLineStyle;			//垂直网格线样式

	// Header Ctrl
	CSkinHeaderCtrlEx			m_ctrlHeader;

protected:
	int							m_nCustomStyle;

public:
	CSkinListViewCtrlEx()
	{
		m_nCustomStyle			= 0;
		m_rSelect				= RGB(170, 190, 220);
		m_rSelBound				= Undefine;

		m_rHGridLine			= RGB(192, 192, 192);
		m_nHGridLineStyle		= PS_SOLID;
		m_rVGridLine			= RGB(192, 192, 192);
		m_nVGridLineStyle		= PS_SOLID;
	};

	virtual ~CSkinListViewCtrlEx()
	{

	};

	BOOL SubclassWindow(HWND hWnd)
	{
		ATLASSERT(m_hWnd == NULL);
		ATLASSERT(::IsWindow(hWnd));

		BOOL bRet = _Base::SubclassWindow(hWnd);
		if(bRet)
			_Init();

		return bRet;
	}

	HWND SkinCreate( 
		const SkinXmlElement& xmlElement,
		HWND hWndParent, _U_MENUorID MenuOrID = 0U ) throw()
	{
		BOOL result;

		ATLASSUME(m_hWnd == NULL);

		// Allocate the thunk structure here, where we can fail gracefully.
		result = m_thunk.Init(NULL, NULL);
		if (result == FALSE) {
			SetLastError(ERROR_OUTOFMEMORY);
			return NULL;
		}

		ATOM atom = GetWndClassInfo().Register(&m_pfnSuperWindowProc);

		_AtlWinModule.AddCreateWndData(&m_thunk.cd, this);

		HWND hWndResult = SkinWindow::SkinCreate(xmlElement, 
			hWndParent,              
			GetWndClassInfo().m_wc.lpszClassName,
			MenuOrID);

		return hWndResult;
	}

	static SkinWindow* SkinCreate_Static(
		const SkinXmlElement& xmlElement,
		HWND hWndParent, _U_MENUorID MenuOrID = 0U )
	{
		CSkinListViewCtrlEx *pSkinWindow = new CSkinListViewCtrlEx;

		if (pSkinWindow == NULL)
			return pSkinWindow;


		if (pSkinWindow->SkinCreate(xmlElement, 
			hWndParent, 
			MenuOrID) == NULL)
		{
			delete pSkinWindow;

			pSkinWindow = NULL;
		}

		return pSkinWindow;
	}



	//////////////////////////////////////////////////////////////////////////
	// 导出函数
public:

	//////////////////////////////////////////////////////////////////////////
	// 给某一个格子指定对齐方式
	void SetItemAlignType(int nAlignType, int nItem = -1, int nSubItem = -1)
	{
		// 如果行为-1，表示本列所有行，列为-1则表示本行所有列
		_EnumItem(nItem, nSubItem, _Enum_SetAlignType, nAlignType);
	}

	//////////////////////////////////////////////////////////////////////////
	// 设置自定义样式
	void SetCustomStyle(int nCustomStyle)
	{
		m_nCustomStyle = nCustomStyle;
		if (!m_ctrlHeader.IsWindow())
		{	
			if (nCustomStyle & LVS_CT_HEADEREX)				//增强型Header
			{
				m_ctrlHeader.SubclassWindow(GetHeader());

				// 初始化
				SIZE size;
				if (m_EmptyImageList.IsNull())
					ASSERT(!_T("Set LineHeight First!"));

				m_EmptyImageList.GetIconSize(size);
				m_ctrlHeader.SetHeight(size.cy);
				m_ctrlHeader.SetGridLineColor(m_rHGridLine);
				m_ctrlHeader.SetBkColor(_Base::GetBkColor());

				INT nHeaderStyle = 0;
				if (nCustomStyle & LVS_CT_TEXT_TRANSPARENT)
					nHeaderStyle |= LVS_CT_TEXT_TRANSPARENT;
				if (nCustomStyle & LVS_CT_FIXWIDTH)
					nHeaderStyle |= LVS_CT_FIXWIDTH;

				m_ctrlHeader.SetCustomStyle(nHeaderStyle);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 设置行高度，注意ListView是一起设的，不能像ListBox可以单独给某行设
	void SetLineHeight(int nHeight)
	{
		SetImageList(NULL, LVSIL_SMALL);
		if (!m_EmptyImageList.IsNull())
		{
			m_EmptyImageList.Destroy();							//不知道是不是没插东西进去，Destroy不成功
			m_EmptyImageList.Detach();							//直接Detach好了
		}

		m_EmptyImageList.Create(1, nHeight, 0, 0, 0);
		SetImageList(m_EmptyImageList, LVSIL_SMALL);
	}


	//////////////////////////////////////////////////////////////////////////
	// 设置网格颜色
	void SetGridLineColor(COLORREF rHColor, int nHStyle = PS_SOLID, COLORREF rVColor = Undefine, int nVStyle = PS_SOLID)
	{
		m_rHGridLine		= rHColor;
		m_nHGridLineStyle	= nHStyle;
		
		if (rVColor == Undefine)
			m_rVGridLine	= m_rHGridLine;
		else
			m_rVGridLine	= rVColor;

		m_nVGridLineStyle	= nVStyle;
	}


	//////////////////////////////////////////////////////////////////////////
	// 设置Select的颜色
	void SetSelectColor(COLORREF rSelect, COLORREF rBound = Undefine, BOOL b3D = FALSE)
	{
		m_rSelect	= rSelect;
		m_rSelBound = rBound;
		m_bSel3D	= b3D;
	}

	//////////////////////////////////////////////////////////////////////////
	// 设置缩进
	void SetItemIndent(int nIndent, int nItem = -1, int nSubItem = -1)
	{
		_EnumItem(nItem, nSubItem, _Enum_SetIndent, nIndent);
	}

	//////////////////////////////////////////////////////////////////////////
	// 设置预设的内容，主要是作多项重复时的优化
	BOOL SetPresetContent(int nIndex, LPCTSTR szRichText, int nWidth = INT_MAX, int nHeight = INT_MAX, int nAlignType = AT_DEFAULT, int nIndent = 0)
	{
		KSingleLineHTMLControl *pDrawerCtrl;
		if (nIndex < m_vecPresetDrawerCtrl.size())
			pDrawerCtrl = m_vecPresetDrawerCtrl[nIndex];
		else
		{
			m_vecPresetDrawerCtrl.resize(nIndex + 1);
			pDrawerCtrl = new KSingleLineHTMLControl;
			m_vecPresetDrawerCtrl[nIndex] = pDrawerCtrl;
		}

		//
		pDrawerCtrl->SetPageSize(nWidth, nHeight);
		pDrawerCtrl->SetRichText(szRichText);
		pDrawerCtrl->SetAlignStyle(nAlignType);				//对齐方式
		pDrawerCtrl->SetIndent(nIndent);					//缩进

		return TRUE;
	}

	//////////////////////////////////////////////////////////////////////////
	// 设置内容，bKeep指示是否设进ListCtrl的Text中
	BOOL SetItemRichText(int nItem, int nSubItem, LPCTSTR szRichText,  int nIndent = 0, int nAlignType = AT_DEFAULT, int nOmitType = 0)
	{
		if (nItem >= m_Items.size())
			_Base::SetItemCountEx(m_Items.size() + 1, LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL);

		// SetItemData
		KSingleLineHTMLControl *pSimpleHtmlCtrl = NULL;
		_ItemData *pItemData = _GetItemData(nItem);
		if (!pItemData)
		{
			pItemData = new _ItemData;							//Insert Item Data
			_SetItemData(nItem, pItemData);
		}

		// 挂上Drawer
		if (pItemData->vecSubItems.size() == 0)
			pItemData->vecSubItems.resize(GetHeader().GetItemCount());

		pSimpleHtmlCtrl = _GetDrawerCtrl(pItemData->vecSubItems[nSubItem]);
		if (!pSimpleHtmlCtrl)
		{
			pItemData->vecSubItems[nSubItem].eCellType = CT_DrawerCtrl;
			pSimpleHtmlCtrl = pItemData->vecSubItems[nSubItem].pDrawerCtrl = new KSingleLineHTMLControl;
		}

		ASSERT(pSimpleHtmlCtrl);

		//Setting
		pSimpleHtmlCtrl->SetMsgReviecer(GetParent());
		pSimpleHtmlCtrl->SetAlignStyle(nAlignType);				//对齐方式
		pSimpleHtmlCtrl->SetIndent(nIndent);					//缩进
		pSimpleHtmlCtrl->SetOmitType(nOmitType);				//省略方式
		BOOL bRet = pSimpleHtmlCtrl->SetRichText(szRichText);
		ASSERT(bRet); 


		CRect rc;
		GetSubItemRect(0, nSubItem, LVIR_BOUNDS, &rc);			//每行都是一样的，取第一行好了
		if (nSubItem == 0 && pItemData->m_eShowtype != ST_ONELINE)	//GetSubItemRect取第一列时取的是整个Item的Rect，要修正
			rc.right = GetColumnWidth(0);
		pSimpleHtmlCtrl->SetPageSize(rc.Width(), rc.Height());

		// 刷新此Item
		int nDrawItem = _GetDrawItemMap(nItem, FALSE);
		_Base::RedrawItems(nDrawItem, nDrawItem);

		return bRet;
	}
	//////////////////////////////////////////////////////////////////////////
	// 设置预设内容
	BOOL SetItemRichText(int nItem, int nSubItem, int nPresetIndex)
	{
		if (nItem >= m_Items.size())
			_Base::SetItemCountEx(m_Items.size() + 1, LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL);

		// SetItemData
		KSingleLineHTMLControl *pSimpleHtmlCtrl = NULL;
		_ItemData *pItemData = _GetItemData(nItem);
		if (!pItemData)
		{
			pItemData = new _ItemData;					//Insert Item Data
			_SetItemData(nItem, pItemData);
		}

		if (pItemData->vecSubItems.size() == 0)
			pItemData->vecSubItems.resize(GetHeader().GetItemCount());

		// 挂上Drawer
		ASSERT(nPresetIndex < m_vecPresetDrawerCtrl.size());
		if (nPresetIndex < m_vecPresetDrawerCtrl.size())
		{	
			pSimpleHtmlCtrl = _GetDrawerCtrl(pItemData->vecSubItems[nSubItem]);
			if (pSimpleHtmlCtrl && pItemData->vecSubItems[nSubItem].eCellType == CT_DrawerCtrl)
				delete pSimpleHtmlCtrl;

			pItemData->vecSubItems[nSubItem].eCellType		= CT_Preset;
			pItemData->vecSubItems[nSubItem].nPresetIndex	= nPresetIndex;
			pSimpleHtmlCtrl = m_vecPresetDrawerCtrl[nPresetIndex];
		}

		ASSERT(pSimpleHtmlCtrl);

		CRect rc;
		GetSubItemRect(0, nSubItem, LVIR_BOUNDS, &rc);
		if (nSubItem == 0 && pItemData->m_eShowtype != ST_ONELINE)	//GetSubItemRect取第一列时取的是整个Item的Rect，要修正
			rc.right = GetColumnWidth(0);
		pSimpleHtmlCtrl->SetPageSize(INT_MAX, rc.Height());			//垂直居中需要这个高度

		// 刷新此Item
		int nDrawItem = _GetDrawItemMap(nItem, FALSE);
		_Base::RedrawItems(nDrawItem, nDrawItem);

		return TRUE;
	}

	//////////////////////////////////////////////////////////////////////////
	// 设置行的样式
	void SetItemStyle(int nItem, COLORREF rbkColor = Undefine, int nGroupLevel = 0, ShowType eShowType = ST_NORMAL)
	{
		_ItemData *pItem = _GetItemData(nItem);
		if (eShowType == ST_ONELINE && pItem->vecSubItems[0].pDrawerCtrl)
		{
			CRect rc;
			_Base::GetItemRect(_GetDrawItemMap(nItem, FALSE), &rc, LVIR_BOUNDS);
			pItem->vecSubItems[0].pDrawerCtrl->SetPageSize(rc.Width(), rc.Height());
		}

		pItem->m_rbkColor	 = rbkColor;
		pItem->m_nGroupLevel = nGroupLevel;
		pItem->m_eShowtype	 = eShowType;
	}

	//////////////////////////////////////////////////////////////////////////
	// 重载掉原来的函数，不让用
	int AddItem(int nItem, int nSubItem, LPCTSTR strItem, int nImageIndex = -1)
	{
		ASSERT(!_T("请使用 SetItemRichText 函数！"));
		return 0;
	}
	int InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState, UINT nStateMask, int nImage, LPARAM lParam)
	{
		ASSERT(!_T("请使用 SetItemRichText 函数！"));
		return 0;
	}

	int InsertItem(const LVITEM* pItem)
	{
		ASSERT(!_T("请使用 SetItemRichText 函数！"));
		return 0;
	}

	int InsertItem(int nItem, LPCTSTR lpszItem)
	{
		ASSERT(!_T("请使用 SetItemRichText 函数！"));
		return 0;
	}

	int InsertItem(int nItem, LPCTSTR lpszItem, int nImage)
	{
		ASSERT(!_T("请使用 SetItemRichText 函数！"));
		return 0;
	}
	int SetItemText(int nItem, int nSubItem, LPCTSTR lpszText)
	{
		ASSERT(!_T("请使用 SetItemRichText 函数！"));
		return 0;
	}

	
	//////////////////////////////////////////////////////////////////////////
	// 返回真正的Item数
	int GetItemCount()
	{
		return m_Items.size();
	}

	//////////////////////////////////////////////////////////////////////////
	// 返回当前展开的Item数
	int GetExpandItemCount()
	{
		return _GetVisibleItemCount();
	}

	//////////////////////////////////////////////////////////////////////////
	// 重载GetItemData
	DWORD_PTR GetItemData(int nItem)
	{
		_ItemData *pItemData = _GetItemData(nItem);
		return pItemData->ItemData;
	}

	//////////////////////////////////////////////////////////////////////////
	// 重载SetItemData
	BOOL SetItemData(int nItem, DWORD_PTR dwData)
	{
		_ItemData *pItemData = _GetItemData(nItem);
		if (!pItemData)
			pItemData = new _ItemData;

		pItemData->ItemData	= dwData; 
		return _SetItemData(nItem, pItemData);
	}

	//////////////////////////////////////////////////////////////////////////
	// 插入项，注意一定要用此重载函数
	BOOL InsertItem(int nItemBefore, int nCount = 1)
	{
		ASSERT(nItemBefore <= m_Items.size());
		if (nItemBefore > m_Items.size())
			return FALSE;

		// 插入
		_RowIter it = m_Items.begin() + nItemBefore;
		for (int i = 0; i < nCount; ++i)
			m_Items.insert(it + i, new _ItemData);

		// 设置可见Item数
		_Base::SetItemCountEx(_GetVisibleItemCount(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL);
		RedrawWindow();

		return TRUE;
	}


	//////////////////////////////////////////////////////////////////////////
	// 删除行
	BOOL DeleteItem(int nItem, int nCount = 1)
	{
		for (int i = nItem + nCount - 1; i >= nItem; --i)
		{
			_DeleteItemData(i);
		}

		// 设置可见Item数
		_Base::SetItemCountEx(_GetVisibleItemCount(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL);
		RedrawWindow();

		return TRUE;
	}

	//////////////////////////////////////////////////////////////////////////
	// 设置Image
	BOOL SetImage(LPCTSTR szName, HIMAGELIST hImageList)
	{
		return KImageListManager::Instance()->AddImageList(szName, hImageList);
	}
	BOOL SetImage(LPCTSTR szName, HBITMAP hBitmap)
	{
		return KImageListManager::Instance()->AddBitmap(szName, hBitmap);
	}
	BOOL RemoveImage(LPCTSTR szName)
	{
		return KImageListManager::Instance()->RemoveImage(szName);
	}

	//////////////////////////////////////////////////////////////////////////
	// Header Ctrl
	BOOL SetHeaderHeight(int nHeight)
	{
		ASSERT(m_ctrlHeader.IsWindow());
		if (m_ctrlHeader.IsWindow())
		{
			m_ctrlHeader.SetHeight(nHeight);
			return TRUE;
		}

		return FALSE;
	}
	BOOL SetHeaderLineColor(COLORREF rColor)
	{
		ASSERT(m_ctrlHeader.IsWindow());
		if (m_ctrlHeader.IsWindow())
		{
			m_ctrlHeader.SetGridLineColor(rColor);
			return TRUE;
		}

		return FALSE;
	}
	BOOL SetHeaderBkColor(COLORREF rColor)
	{
		ASSERT(m_ctrlHeader.IsWindow());
		if (m_ctrlHeader.IsWindow())
		{
			m_ctrlHeader.SetBkColor(rColor);
			return TRUE;
		}

		return FALSE;
	}
	BOOL SetHeaderRichText(int nItem, LPCTSTR szRichText, int nWidth = - 1, int nIndent = 0, int nAlignType = AT_LEFT)
	{
		for (int i = GetHeader().GetItemCount(); i <= nItem; ++i)
		{
 			InsertColumn(i, _T(""), LVCFMT_LEFT, nWidth);
		}
		
		return m_ctrlHeader.SetRichText(nItem, szRichText, nWidth, nIndent, nAlignType);
	}

	BOOL SetColumnWidth(int nCol, int nWidth)
	{
		if (m_nCustomStyle & LVS_CT_HEADEREX)
			m_ctrlHeader.SetWidth(nCol, nWidth);

		int nCount = m_Items.size();
		for (int i = 0; i < nCount; ++i)
		{
			KSingleLineHTMLControl *pDawer = _GetDrawerCtrl(i, nCol);
			if (!pDawer)
				continue;

			//
			if (m_Items[i]->m_eShowtype == ST_ONELINE)
			{
				if (nCol > 0)
					continue;

				CRect rc;
				_Base::GetItemRect(0, &rc, LVIR_BOUNDS);
				pDawer->SetPageSize(rc.Width());
			}
			else
			{
				pDawer->SetPageSize(nWidth);
			}
		}

		return _Base::SetColumnWidth(nCol, nWidth);
	}

	//////////////////////////////////////////////////////////////////////////
	// 组相关操作
	int GetRealItemIndex(int nDrawItem)
	{
		return _GetDrawItemMap(nDrawItem);
	}

	int GetGroupLevel(int nItem)
	{
		_ItemData *pItem = _GetItemData(nItem);
		return pItem->m_nGroupLevel;
	}
	BOOL IsGroupExpand(int nItem)
	{
		_ItemData *pItem = _GetItemData(nItem);
		return pItem->m_bExpand;
	}
	int GetParentGroup(int nItem)
	{
		return _GetParentGroup(nItem);
	}
	int GetGroupItemCount(int nItem)
	{
		return _GetGroupItemCount(nItem);
	}


	//////////////////////////////////////////////////////////////////////////
	// 让派生类重载的函数
public:
	virtual int ExpandGroup(int nItem, BOOL bExpand = TRUE)
	{
		_ItemData *pItem = _GetItemData(nItem);
		if (pItem->m_bExpand == bExpand)
			return 0;

		// Normal
		pItem->m_bExpand = bExpand;

		int nGroupItemCount = _GetGroupItemCount(nItem);
		int nItemCount = _GetVisibleItemCount();
		_Base::SetItemCountEx(nItemCount, LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL);

		Invalidate();
		return nGroupItemCount;
	}

	//////////////////////////////////////////////////////////////////////////
	// MESSAGE
public:

	BEGIN_MSG_MAP(CSkinListViewCtrlEx)
		MESSAGE_HANDLER(WM_CREATE ,		OnCreate)
		MESSAGE_HANDLER(WM_DESTROY,		OnDestroy)
		MESSAGE_HANDLER(WM_MOUSEMOVE,	OnMouseMove)
		MESSAGE_HANDLER(WM_NCPAINT   ,	OnNcPaint)
		MESSAGE_HANDLER(WM_NCCALCSIZE,	OnNcCalcSize)
		MESSAGE_HANDLER(WM_KEYDOWN,		OnKeyDown)

		NOTIFY_CODE_HANDLER(HDN_BEGINTRACK,		OnBeginTrack)
		NOTIFY_CODE_HANDLER(HDN_ENDTRACK,		OnEndChangeColumnWidth)

		REFLECTED_NOTIFY_CODE_HANDLER(NM_CLICK, OnListViewClick)
		
		CHAIN_MSG_MAP_ALT(COwnerDraw<_Self>, 1)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	LRESULT OnEndChangeColumnWidth(int /*wParam*/, LPNMHDR lParam, BOOL& bHandled)
	{
		NMHEADER *pItem = (NMHEADER*)lParam;
		SetColumnWidth(pItem->iItem, pItem->pitem->cxy);

		return 0;
	}

	LRESULT OnBeginTrack(int /*wParam*/, LPNMHDR /*lParam*/, BOOL& bHandled)
	{
		if (m_nCustomStyle & LVS_CT_FIXWIDTH)
			return 1L;											//禁止用户拖动标头

		return 0L;
	};

	LRESULT OnListViewClick(int wParam, LPNMHDR lParam, BOOL bHandled)
	{
		DWORD dwPos = GetMessagePos();
		LVHITTESTINFO ht = { 0 };
		ht.pt.x = GET_X_LPARAM(dwPos);
		ht.pt.y = GET_Y_LPARAM(dwPos);
		ScreenToClient(&ht.pt);

		int n = _Base::SubItemHitTest(&ht);
		if (ht.flags & LVHT_ONITEM)
		{
			CRect rc;
			GetSubItemRect(ht.iItem, ht.iSubItem, LVIR_BOUNDS, &rc);
			int nItem = _GetDrawItemMap(ht.iItem);
			KSingleLineHTMLControl *pDrawerCtrl = _GetDrawerCtrl(nItem, ht.iSubItem);
			if (pDrawerCtrl)
			{
				CPoint pt(ht.pt.x - rc.left, ht.pt.y - rc.top);
				pDrawerCtrl->OnMouse(WM_LBUTTONUP, pt, MAKELONG(nItem, ht.iSubItem), NULL);
			}
		}

		return 0L;
	}
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// 处理组逻辑
		int nItem = GetSelectedIndex();
		nItem = GetRealItemIndex(nItem);
		if (wParam == VK_LEFT)
		{
			if (GetGroupLevel(nItem) > 0)
				ExpandGroup(nItem, FALSE);											// 展开组
			else
				_Base::SelectItem(_GetDrawItemMap(GetParentGroup(nItem), FALSE));	// 选中组
		}
		else if (wParam == VK_RIGHT)
		{
			if (GetGroupLevel(nItem) > 0)
				ExpandGroup(nItem, TRUE);
		}

		return DefWindowProc(uMsg, wParam, lParam);
	}
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;

		_Init();
		return 1L;
	}


	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		for (int i = m_Items.size() - 1; i >= 0; --i)
		{
			_DeleteItemData(i);
		}

		m_Items.clear();

		for (int i = m_vecPresetDrawerCtrl.size() - 1; i >= 0; --i)
		{
			delete m_vecPresetDrawerCtrl[i];
		}

		m_vecPresetDrawerCtrl.clear();

		bHandled = FALSE;											//让它的真正窗口实体收到
		return 1L;
	}
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LVHITTESTINFO ht = { 0 };
		ht.pt.x = GET_X_LPARAM(lParam);
		ht.pt.y = GET_Y_LPARAM(lParam);
		int n = _Base::SubItemHitTest(&ht);
		if (ht.flags & LVHT_ONITEM)
		{
			CRect rc;
			int nItem = _GetDrawItemMap(ht.iItem);
			GetSubItemRect(ht.iItem, ht.iSubItem, LVIR_BOUNDS, &rc);
			KSingleLineHTMLControl *pDrawerCtrl = _GetDrawerCtrl(nItem, ht.iSubItem);
			if (pDrawerCtrl)
			{
				CPoint pt(ht.pt.x - rc.left, ht.pt.y - rc.top);
				int nRet = pDrawerCtrl->OnMouse(uMsg, pt, 0, 0);
				if (nRet & Action::OMR_NeedRedraw)
					InvalidateRect(&rc, FALSE);
			}
		}

		// 让消息流下去
		bHandled = FALSE;
		return 1L;
	}

	LRESULT OnEraseBKGnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 1L;
	}
	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lResult = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);

		// 边框
		if (m_nCustomStyle & LVS_CT_DRAWFRAME)
		{
			LPNCCALCSIZE_PARAMS pNCParams = (LPNCCALCSIZE_PARAMS)lParam;
			RECT rcWindow = pNCParams->rgrc[0];
			if (wParam)
			{
				pNCParams->rgrc[0].left		+= 1;
				pNCParams->rgrc[0].top		+= 1;
				pNCParams->rgrc[0].right	-= 1;
				pNCParams->rgrc[0].bottom	-= 1;
			}
		}

		return lResult;
	}
	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lResult = DefWindowProc();

		// 边框
		if (m_nCustomStyle & LVS_CT_DRAWFRAME)
		{
			CWindowDC dc(m_hWnd);
			CRect rc;		
			GetWindowRect(&rc);
			rc.right  = rc.right - rc.left;
			rc.bottom = rc.bottom - rc.top;
			rc.left   = rc.top = 0;

			HBRUSH brush = (HBRUSH)GetStockObject(NULL_BRUSH);
			HBRUSH oldBrush = dc.SelectBrush(brush);
			CPen pen;
			pen.CreatePen(PS_SOLID, 1, m_rHGridLine);
			HPEN oldPen = dc.SelectPen(pen);
			dc.Rectangle(&rc);
			dc.SelectPen(oldPen);
			dc.SelectBrush(oldBrush);
		}

		return lResult;
	}



	//////////////////////////////////////////////////////////////////////////
	// 重载的HitTest，pIndex返回的是子元素index
	int HitTest(LPLVHITTESTINFO lpInfo, int *phtCode = NULL, int *pIndex = NULL)
	{
		return SubItemHitTest(lpInfo, phtCode, pIndex);	
	}
	int SubItemHitTest(LPLVHITTESTINFO lpInfo, int *phtCode = NULL, int *pIndex = NULL)
	{
		int nRet = CSkinListViewCtrl::SubItemHitTest(lpInfo);
		if (nRet != -1 && (pIndex || phtCode))
		{
			int nItem = _GetDrawItemMap(lpInfo->iItem);
			KSingleLineHTMLControl *pDrawerCtrl = _GetDrawerCtrl(nItem, lpInfo->iSubItem);
			if (pDrawerCtrl)
			{
				CRect rc;
				GetSubItemRect(lpInfo->iItem, lpInfo->iSubItem, LVIR_BOUNDS, &rc);
				lpInfo->pt.x -= rc.left;					//换成相对Cell的坐标
				lpInfo->pt.y -= rc.top;
				int nIndex, htCode;
				htCode = pDrawerCtrl->HitTest(lpInfo->pt, &nIndex);
				if (phtCode)
					*phtCode = htCode;
				if (pIndex)
					*pIndex = nIndex;
			}
		}

		return nRet;
	}


	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
	{
		int nColumns = GetHeader().GetItemCount();

		CDCHandle dc(lpDrawItemStruct->hDC);
		int nDC = dc.SaveDC();

		// Normal
		int nItem = _GetDrawItemMap(lpDrawItemStruct->itemID);
		_ItemData *pItem = _GetItemData(nItem);
		if (pItem->m_eShowtype == ST_ONELINE)
 		{
 			dc.FillSolidRect(&lpDrawItemStruct->rcItem, _GetBkColor(nItem, lpDrawItemStruct->itemState));
			CPen pen;
			pen.CreatePen(m_nHGridLineStyle, 1, m_rHGridLine);
			dc.SelectPen(pen);
			_DrawLine(dc, lpDrawItemStruct->rcItem.left, lpDrawItemStruct->rcItem.bottom - 1, 
				lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom - 1);
			
			//绘制内容
			dc.SetBkMode(TRANSPARENT);
			KSingleLineHTMLControl *pDrawerCtrl = _GetDrawerCtrl(nItem, 0);
			if (pDrawerCtrl)
				pDrawerCtrl->Draw(dc, &lpDrawItemStruct->rcItem);
 		}
 		else
		{
			for (int i = 0; i < nColumns; ++i)
			{
				_DrawSubItem(lpDrawItemStruct->hDC, lpDrawItemStruct->itemID, i, lpDrawItemStruct->itemState);
			}
		}

		dc.RestoreDC(nDC);
		return;
	}

	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
	{
		return;
	}

	int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct)
	{
		return 0;
	}

	void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct)
	{
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// Helper
private:
	void _Init()
	{
		ModifyStyle(0, LVS_OWNERDRAWFIXED | LVS_OWNERDATA);
		SetViewType(LVS_REPORT);
	}

	inline void _DrawSubItem(HDC hdc, int nDrawItem, int nCol, int nItemState)
	{
		//Prepare
		int nItem = _GetDrawItemMap(nDrawItem);
		CRect rc;
		GetSubItemRect(nDrawItem, nCol, LVIR_BOUNDS, &rc);
		if (nCol == 0)				//GetSubItemRect取第一列时取的是整个Item的Rect，要修正
			rc.right = GetColumnWidth(0);

		//开始绘制
		CDCHandle dc(hdc);

		//绘制背景
		_DrawBkGround(dc, nItem, nCol, nItemState, &rc);
		_DrawSelect(dc, nDrawItem, nCol, nItemState, &rc);

		//绘制内容
		dc.SetBkMode(TRANSPARENT);
		KSingleLineHTMLControl *pDrawerCtrl = _GetDrawerCtrl(nItem, nCol);
		if (pDrawerCtrl)
			pDrawerCtrl->Draw(dc, &rc);

		return;
	}
	inline void _DrawBkGround(CDCHandle &dc, int nItem, int nSubItem, UINT nItemState, CRect *lprc)
	{		
		//Normal
		int nSaveDC = dc.SaveDC();
		int nColumns = GetHeader().GetItemCount();

		// 背景

		dc.FillSolidRect(*lprc, _GetBkColor(nItem, nItemState));

		// 网络线
		if (m_nCustomStyle & LVS_CT_HGRIDLINES)							// 水平线
		{
			CPen pen;
			pen.CreatePen(m_nHGridLineStyle, 1, m_rHGridLine);
			dc.SelectPen(pen);
			_DrawLine(dc, lprc->left, lprc->bottom - 1, lprc->right, lprc->bottom - 1);	
		}

		if (m_nCustomStyle & LVS_CT_VGRIDLINES)							// 垂直线
		{
			CPen pen;
			pen.CreatePen(m_nVGridLineStyle, 1, m_rVGridLine);
			dc.SelectPen(pen);

			if ((m_nCustomStyle & LVS_CT_DRAWFRAME) == 0 || nSubItem > 0)
				_DrawLine(dc, lprc->left, lprc->top, lprc->left, lprc->bottom);

 			_DrawLine(dc, lprc->right, lprc->top, lprc->right, lprc->bottom);
		}

		dc.RestoreDC(nSaveDC);
	}
	inline void _DrawSelect(CDCHandle &dc, int nDrawItem, int nSubItem, UINT nItemState, CRect *lprc)
	{
		BOOL bSelected;
		if (_Base::GetStyle() & LVS_SHOWSELALWAYS)					//简单设置此风格达不到我的要求，所以自己处理
			bSelected = _Base::GetItemState(nDrawItem, LVIS_SELECTED) == LVIS_SELECTED;
		else
			bSelected = nItemState & CDIS_SELECTED;

		//
		if (
			bSelected	&&											//当前项是选中的
			!(m_nCustomStyle & LVS_CT_NOSELECT)						//不显示选中
			)
		{
			// Bk
			if (m_bSel3D)
			{
				CSkinDCHandle dc3D(dc.m_hDC);
				CRect rc(lprc->left + 1, lprc->top + 1, lprc->right - 1, lprc->bottom - 1);
				dc3D.SkinDrawGradualColorRect(*lprc, RGB(255, 255, 255), m_rSelBound);
			}
			else
			{
				CRect rc(lprc->left + 1, lprc->top + 1, lprc->right, lprc->bottom - 1);
				if (m_nCustomStyle & LVS_CT_VGRIDLINES)
					--rc.right;

				dc.FillSolidRect(&rc, m_rSelect);
			}

			// Bound
			if (m_rSelBound != Undefine)
			{
				CPen pen;
				pen.CreatePen(PS_SOLID, 1, m_rSelBound);
				HPEN oldPen = dc.SelectPen(pen);
				if (nSubItem == 0)
				{
					_DrawLine(dc, lprc->left, lprc->top, lprc->left, lprc->bottom);
				}
				if (nSubItem == _Base::GetHeader().GetItemCount() - 1)					//注意只有一项的情况
				{
					_DrawLine(dc, lprc->right - 1, lprc->top, lprc->right - 1, lprc->bottom);
				}

				// 水平线
				_DrawLine(dc, lprc->left, lprc->top, lprc->right, lprc->top);
				_DrawLine(dc, lprc->left, lprc->bottom - 1, lprc->right, lprc->bottom - 1);

				dc.SelectPen(oldPen);
			}
		}
	}

	inline COLORREF _GetBkColor(int nItem, int nItemState)
	{
		if (
			(nItemState & CDIS_SELECTED)	&&						//当前项是选中的
			!(m_nCustomStyle & LVS_CT_NOSELECT)						//不显示选中
			)
		{
			return m_rSelect;
		}
		else
		{
			_ItemData *pItem = _GetItemData(nItem);
			if (pItem->m_rbkColor != Undefine)
				return pItem->m_rbkColor;
			else
				return _Base::GetBkColor();
		}
	}
	inline RECT* _GetAjustBkRect(int nItem, int nSubItem, int nItemState, CRect &rc)
	{
		if (
			(nItemState & CDIS_SELECTED)	&&						//当前项是选中的
			!(m_nCustomStyle & LVS_CT_NOSELECT) 					//不显示选中
			)
		{
			++rc.top;
			--rc.bottom;
			if (nSubItem == 0)
				++rc.left;
			rc.DeflateRect(1, 1, 1, 1);
		}
		
		return &rc;
	}
	inline void _DrawLine(CDCHandle &dc, int x1, int y1, int x2, int y2)
	{
		dc.MoveTo(x1, y1);
		dc.LineTo(x2, y2);
	}
// 	inline void _SetTextToCell(int nItem, int nSubItem, LPCTSTR szText)
// 	{
// 		// 插入前面空项，否则会插不到的
// 		int nItemCount = GetItemCount();
// 		for (int i = GetItemCount(); i < nItem + 1; ++i)
// 			_Base::AddItem(i, 0, _T(""));
// 
// 		_Base::SetItemText(nItem, nSubItem, szText);
// 	}
	inline BOOL _EnumItem(int nItem, int nSubItem, _Fun_Enum pFun, WPARAM wParam = 0, LPARAM lParam = NULL)
	{
		int nRows = _Base::GetItemCount();
		int nCols = _Base::GetHeader().GetItemCount();
		for (int i = 0; i < nRows; ++i)
		{
			_ItemData *pItemData = (_ItemData*)GetItemData(i);
			for (int j  = 0; j < nCols; ++j)
			{
				if ((nItem == -1 || nItem == i) && 
					(nSubItem == -1 || nSubItem == j))
				{
					if (!pFun(_GetDrawerCtrl(pItemData->vecSubItems[j]), wParam, lParam))
						return FALSE;
				}
			}
		}

		return TRUE;
	}

	static BOOL _Enum_SetAlignType(KSingleLineHTMLControl *pCell, WPARAM wParam, LPARAM lParam)
	{
		pCell->SetAlignStyle((UINT)wParam);
		return TRUE;
	}
	static BOOL _Enum_SetIndent(KSingleLineHTMLControl *pCell, WPARAM wParam, LPARAM lParam)
	{
		ASSERT(!L"暂时有点小bug");
		pCell->SetIndent((int)wParam);
		return TRUE;
	}

	inline _ItemData* _GetItemData(int nItem)
	{
		if (nItem < m_Items.size())
			return m_Items[nItem];
		else
			return NULL;
	}
	inline BOOL _SetItemData(int nItem, _ItemData *pItemData)
	{
		if (nItem >= m_Items.size())
			m_Items.resize(nItem + 1);

		m_Items[nItem] = pItemData;
		return TRUE;
	}

	inline void _DeleteItemData(int nItem)
	{
		_ItemData *pItemData = _GetItemData(nItem);
		if (pItemData)
		{
			for (int j = pItemData->vecSubItems.size() - 1; j >= 0; --j)
			{
				_Cell &cell = pItemData->vecSubItems[j];
				if (cell.eCellType == CT_DrawerCtrl && cell.pDrawerCtrl)
					delete cell.pDrawerCtrl;
			}

			delete pItemData;
		}

		m_Items.erase(std::find(m_Items.begin(), m_Items.end(), pItemData));
	}

	// 返回本组的子Item数
	inline int _GetGroupItemCount(int nItem)
	{
		int nGroupLevel = m_Items[nItem]->m_nGroupLevel;
		int i;
		for (i = nItem + 1; i < m_Items.size(); ++i)
		{
			if (m_Items[i]->m_nGroupLevel >= nGroupLevel)
				break;
		}

		return i - nItem - 1;
	}
	
	// 取得当前Item数
	inline int _GetVisibleItemCount()
	{
		int nHideCount = 0;
		for (int i = 0; i < m_Items.size(); ++i)
		{
			_ItemData *pItem = m_Items[i];
			if (!pItem->m_bExpand)
			{
				int nGroupItemCount = _GetGroupItemCount(i);
				nHideCount += nGroupItemCount;
				i += nGroupItemCount;
			}
		}

		return m_Items.size() - nHideCount;
	}

	// 取得Draw和Doc数据之间的映射
	inline int _GetDrawItemMap(int nItem2, BOOL bDrawItem2Item = TRUE)
	{
		int nItem = 0;
		int i;
		int nCount = bDrawItem2Item ? m_Items.size() : nItem2 + 1;
		for (i = 0; i < nCount; ++i)
		{
			if (++nItem > nItem2)
				break;

			_ItemData *pItem = m_Items[i];
			if (!pItem->m_bExpand)
				i += _GetGroupItemCount(i);
		}

		return bDrawItem2Item ? i : nItem - 1;
	}

	// 取得ParentGroup
	inline int _GetParentGroup(int nItem)
	{
		int nLevel = m_Items[nItem]->m_nGroupLevel;
		while (--nItem >= 0)
		{
			if (m_Items[nItem]->m_nGroupLevel > nLevel)
				break;
		}

		return nItem;
	}

	inline KSingleLineHTMLControl* _GetDrawerCtrl(_Cell &cell)
	{
		if (cell.eCellType == CT_DrawerCtrl)
			return cell.pDrawerCtrl;
		else if (cell.eCellType == CT_Preset)
			return m_vecPresetDrawerCtrl.at(cell.nPresetIndex);
		else
			ASSERT(FALSE);

		return NULL;
	}

protected:
	inline KSingleLineHTMLControl* _GetDrawerCtrl(int nItem, int nSubItem)
	{
		_ItemData *pItemData = _GetItemData(nItem);
		if (pItemData && nSubItem < pItemData->vecSubItems.size())
			return _GetDrawerCtrl(pItemData->vecSubItems[nSubItem]);
		else
			return NULL;
	}
};

};

#pragma warning(pop)
// -------------------------------------------------------------------------
// $Log: $

#endif /* __KSGUI_SKINCTRLEX_SKINLISTCTRLEX_H__ */
