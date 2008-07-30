///////////////////////////////////////////////////////////////
//
//	Filename: 	skinlistctrlse.h
//	Creator:	���� & lichenglin 
//	Date:		2008-7-24  15:51
//	Comment:	
//
///////////////////////////////////////////////////////////////


#ifndef _skinlistctrlse_h_
#define _skinlistctrlse_h_
#include <algorithm>
#include <atlframe.h>
#include <vector>
#include "skinheaderctrl.h"

#pragma warning(push)
#pragma warning (disable : 4267 4995 4018)

namespace KSGUI{

//////////////////////////////////////////////////////////////////////////
// ListViewEx�Զ�����ʽ���
typedef enum
{
	// �������
	LVS_CT_NOSELECT				= 0x00000001,				//����ѡ��
	LVS_CT_HEADEREX				= 0x00000004,				//��ǿ��Header
	LVS_CT_FIXWIDTH				= 0x00000008,				//�̶��п������û��϶�
	LVS_CT_KEEP_COL_SUM_WIDTH	= 0x00000010,				//�������п�Ч��Ϊ�϶�ͷʱ�Զ��ı������п�ʼ�ղ�����ˮƽ������

	// ���Ʒ��
	LVS_CT_DRAWTITLE			= 0x00001000,				//��Title
	LVS_CT_DRAWFRAME			= 0x00002000,				//���Ʊ߿�
	LVS_CT_TEXT_TRANSPARENT		= 0x00004000,				//���ֱ���
	LVS_CT_HGRIDLINES			= 0x00008000,				//ˮƽ������
	LVS_CT_VGRIDLINES			= 0x00010000,				//��ֱ������

	// ע�⣺����չ�ؼ�ʹ�� ��ǰ��3���ֽڸ�  0x00011111

} _CustomStyle;



//////////////////////////////////////////////////////////////////////////
// ���ӵ�ListViewCtrl����������ݿ���ֱ������Htmlָ��
template<class T>
class CSkinListViewCtrlSeImpl : 
	public SkinWindowImpl<T, CSkinListViewCtrl>,
	public COwnerDraw<T>
{
public:
	//DECLARE_WND_SUPERCLASS(_T("KSG_SkinListViewCtrlEx"), GetWndClassName())

	// ��ʾ��ʽ
	typedef enum
	{
		ST_NORMAL,
		ST_ONELINE

	} ShowType;

	typedef SkinWindowImpl<T, CSkinListViewCtrl>     			_Base;
	typedef CSkinListViewCtrlSeImpl<T>   						_Self;

	//////////////////////////////////////////////////////////////////////////
	// ˽������
private:

	// ��Ԫ������
	typedef enum
	{
		CT_DrawerCtrl,
		CT_Preset,

	} _CellType;

	// ��Ԫ��
	typedef struct tagCell
	{
		_CellType					eCellType;						//��Ԫ���ݵ�����
		union
		{
			int						nPresetIndex;					//Ԥ��DrawerCtrl������
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
		DWORD_PTR							ItemData;				//ItemData��Ҳ���Ǹ��ؼ�ʹ�������õ�)
		//int								m_nGroupLevel;			//��ļ����������Ϳؼ��ļ��𣬿����۵�ĳ����
		//BOOL								m_bExpand;				//��չ��/�۵�״̬
		ShowType							m_eShowtype;			//��ʾ��ʽ
		COLORREF							m_rbkColor;				//����ɫ
		std::vector<_Cell>					vecSubItems;			//����
		tagItemData()
		{
			ItemData		= NULL;
			//m_nGroupLevel	= 0;
			m_eShowtype	= ST_NORMAL;
			//m_bExpand		= TRUE;
			m_rbkColor		= Undefine;
		}

	} _ItemData;



	typedef typename std::vector<_ItemData*>        _Items;
	typedef typename _Items::iterator               _RowIter;	// �е�����
	typedef typename std::vector<_Cell>::iterator   _ColIter;	// �е�����

	typedef std::vector<KSingleLineHTMLControl*>	_PresetContainer;
	typedef _PresetContainer::iterator				_Preset_Iter;

	_Items						m_Items;
	_PresetContainer			m_vecPresetDrawerCtrl;		//Ԥ������Ҫ�����������ظ�ʱ���Ż�

	CImageList					m_EmptyImageList;			//���������и��õ�

	BOOL						m_bSel3D;
	COLORREF					m_rSelect;					//ѡ�������ɫ
	COLORREF					m_rSelBound;				//ѡ����߿���ɫ
	int							m_nSelLineStyle;			//ѡ����߿���ʽ

	COLORREF					m_rHGridLine;				//ˮƽ��������ɫ
	int							m_nHGridLineStyle;			//ˮƽ��������ʽ
	COLORREF					m_rVGridLine;				//��ֱ��������ɫ
	int							m_nVGridLineStyle;			//��ֱ��������ʽ

	// Header Ctrl
	CSkinHeaderCtrlEx			m_ctrlHeader;

protected:
	int							m_nCustomStyle;

public:
	CSkinListViewCtrlSeImpl()
	{
		m_nCustomStyle			= 0;
		m_rSelect				= RGB(170, 190, 220);
		m_rSelBound				= Undefine;

		m_rHGridLine			= RGB(192, 192, 192);
		m_nHGridLineStyle		= PS_SOLID;
		m_rVGridLine			= RGB(192, 192, 192);
		m_nVGridLineStyle		= PS_SOLID;
	};

	virtual ~CSkinListViewCtrlSeImpl()
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
		T *pSkinWindow = new T;

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
	// ��������
public:


	//////////////////////////////////////////////////////////////////////////
	// �����Զ�����ʽ
	void SetCustomStyle(int nCustomStyle)
	{
		m_nCustomStyle = nCustomStyle;
		if (!m_ctrlHeader.IsWindow())
		{	
			if (nCustomStyle & LVS_CT_HEADEREX)				//��ǿ��Header
			{
				m_ctrlHeader.SubclassWindow(GetHeader());

				// ��ʼ��
				SIZE size;
				if (m_EmptyImageList.IsNull())
					ASSERT(!_T("Set LineHeight First!"));

				m_EmptyImageList.GetIconSize(size);
				m_ctrlHeader.SetHeight(size.cy);
				m_ctrlHeader.SetGridLineColor(m_rHGridLine);
				m_ctrlHeader.SetBkColor(_Base::GetBkColor());

				INT nHeaderStyle = 0;
				if (nCustomStyle & LVS_CT_TEXT_TRANSPARENT)
					nHeaderStyle |= HDS_CT_TEXT_TRANSPARENT;
				if (nCustomStyle & LVS_CT_FIXWIDTH)
					nHeaderStyle |= HDS_CT_FIXWIDTH;
				if( nCustomStyle & LVS_CT_DRAWFRAME )
					nHeaderStyle |= HDS_CT_DRAWFRAME;

				m_ctrlHeader.SetCustomStyle(nHeaderStyle);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// �����и߶ȣ�ע��ListView��һ����ģ�������ListBox���Ե�����ĳ����
	void SetLineHeight(int nHeight)
	{
		SetImageList(NULL, LVSIL_SMALL);
		if (!m_EmptyImageList.IsNull())
		{
			m_EmptyImageList.Destroy();							//��֪���ǲ���û�嶫����ȥ��Destroy���ɹ�
			m_EmptyImageList.Detach();							//ֱ��Detach����
		}

		m_EmptyImageList.Create(1, nHeight, 0, 0, 0);
		SetImageList(m_EmptyImageList, LVSIL_SMALL);
	}


	//////////////////////////////////////////////////////////////////////////
	// ����������ɫ
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
	// ����Select����ɫ
	void SetSelectColor(COLORREF rSelect, COLORREF rBound = Undefine, BOOL b3D = FALSE)
	{
		m_rSelect	= rSelect;
		m_rSelBound = rBound;
		m_bSel3D	= b3D;
	}


	//////////////////////////////////////////////////////////////////////////
	// ����Ԥ������ݣ���Ҫ���������ظ�ʱ���Ż�
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
		pDrawerCtrl->SetAlignStyle(nAlignType);				//���뷽ʽ
		pDrawerCtrl->SetIndent(nIndent);					//����

		return TRUE;
	}

	//////////////////////////////////////////////////////////////////////////
	// �������ݣ�bKeepָʾ�Ƿ����ListCtrl��Text��
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

		// ����Drawer
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
		pSimpleHtmlCtrl->SetAlignStyle(nAlignType);				//���뷽ʽ
		pSimpleHtmlCtrl->SetIndent(nIndent);					//����
		pSimpleHtmlCtrl->SetOmitType(nOmitType);				//ʡ�Է�ʽ
		BOOL bRet = pSimpleHtmlCtrl->SetRichText(szRichText);
		ASSERT(bRet); 


		CRect rc;
		GetSubItemRect(0, nSubItem, LVIR_BOUNDS, &rc);			//ÿ�ж���һ���ģ�ȡ��һ�к���
		if (nSubItem == 0 && pItemData->m_eShowtype != ST_ONELINE)	//GetSubItemRectȡ��һ��ʱȡ��������Item��Rect��Ҫ����
			rc.right = GetColumnWidth(0);
		pSimpleHtmlCtrl->SetPageSize(rc.Width(), rc.Height());

		// ˢ�´�Item
		int nDrawItem = _GetDrawItemMap(nItem, FALSE);
		_Base::RedrawItems(nDrawItem, nDrawItem);

		return bRet;
	}
	//////////////////////////////////////////////////////////////////////////
	// ����Ԥ������
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

		// ����Drawer
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
		if (nSubItem == 0 && pItemData->m_eShowtype != ST_ONELINE)	//GetSubItemRectȡ��һ��ʱȡ��������Item��Rect��Ҫ����
			rc.right = GetColumnWidth(0);
		pSimpleHtmlCtrl->SetPageSize(INT_MAX, rc.Height());			//��ֱ������Ҫ����߶�

		// ˢ�´�Item
		int nDrawItem = _GetDrawItemMap(nItem, FALSE);
		_Base::RedrawItems(nDrawItem, nDrawItem);

		return TRUE;
	}

	//////////////////////////////////////////////////////////////////////////
	// �����е���ʽ
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
		//pItem->m_nGroupLevel = nGroupLevel;
		pItem->m_eShowtype	 = eShowType;
	}

	//////////////////////////////////////////////////////////////////////////
	// ���ص�ԭ���ĺ�����������
	int AddItem(int nItem, int nSubItem, LPCTSTR strItem, int nImageIndex = -1)
	{
		ASSERT(!_T("��ʹ�� SetItemRichText ������"));
		return 0;
	}
	int InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState, UINT nStateMask, int nImage, LPARAM lParam)
	{
		ASSERT(!_T("��ʹ�� SetItemRichText ������"));
		return 0;
	}

	int InsertItem(const LVITEM* pItem)
	{
		ASSERT(!_T("��ʹ�� SetItemRichText ������"));
		return 0;
	}

	int InsertItem(int nItem, LPCTSTR lpszItem)
	{
		ASSERT(!_T("��ʹ�� SetItemRichText ������"));
		return 0;
	}

	int InsertItem(int nItem, LPCTSTR lpszItem, int nImage)
	{
		ASSERT(!_T("��ʹ�� SetItemRichText ������"));
		return 0;
	}
	int SetItemText(int nItem, int nSubItem, LPCTSTR lpszText)
	{
		ASSERT(!_T("��ʹ�� SetItemRichText ������"));
		return 0;
	}


	//////////////////////////////////////////////////////////////////////////
	// ����������Item��
	int GetItemCount()
	{
		return m_Items.size();
	}


	//////////////////////////////////////////////////////////////////////////
	// ����GetItemData
	DWORD_PTR GetItemData(int nItem)
	{
		_ItemData *pItemData = _GetItemData(nItem);
		return pItemData->ItemData;
	}

	//////////////////////////////////////////////////////////////////////////
	// ����SetItemData
	BOOL SetItemData(int nItem, DWORD_PTR dwData)
	{
		_ItemData *pItemData = _GetItemData(nItem);
		if (!pItemData)
			pItemData = new _ItemData;

		pItemData->ItemData	= dwData; 
		return _SetItemData(nItem, pItemData);
	}


	//////////////////////////////////////////////////////////////////////////
	// ����Image
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
	// MESSAGE
public:

	BEGIN_MSG_MAP(CSkinListViewCtrlSeImpl<T>)
		MESSAGE_HANDLER(WM_CREATE ,		OnCreate)
		MESSAGE_HANDLER(WM_DESTROY,		OnDestroy)
		MESSAGE_HANDLER(WM_MOUSEMOVE,	OnMouseMove)
		MESSAGE_HANDLER(WM_NCPAINT   ,	OnNcPaint)
		MESSAGE_HANDLER(WM_NCCALCSIZE,	OnNcCalcSize)

		NOTIFY_CODE_HANDLER(HDN_BEGINTRACK,		OnBeginTrack)
		NOTIFY_CODE_HANDLER(HDN_ENDTRACK,		OnEndChangeColumnWidth)

		REFLECTED_NOTIFY_CODE_HANDLER(NM_CLICK, OnListViewClick)

		CHAIN_MSG_MAP_ALT(COwnerDraw<T>, 1)
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
			return 1L;											//��ֹ�û��϶���ͷ

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

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;

		_Init();
		return 0L;
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

		bHandled = FALSE;											//��������������ʵ���յ�
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

		// ����Ϣ����ȥ
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

		// �߿�
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

		// �߿�
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
	// ���ص�HitTest��pIndex���ص�����Ԫ��index
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
				lpInfo->pt.x -= rc.left;					//�������Cell������
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

			//��������
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

		SetLineHeight( 20 );
	}

	inline void _DrawSubItem(HDC hdc, int nDrawItem, int nCol, int nItemState)
	{
		//Prepare
		int nItem = _GetDrawItemMap(nDrawItem);
		CRect rc;
		GetSubItemRect(nDrawItem, nCol, LVIR_BOUNDS, &rc);
		if (nCol == 0)				//GetSubItemRectȡ��һ��ʱȡ��������Item��Rect��Ҫ����
			rc.right = GetColumnWidth(0);

		//��ʼ����
		CDCHandle dc(hdc);

		//���Ʊ���
		_DrawBkGround(dc, nItem, nCol, nItemState, &rc);
		_DrawSelect(dc, nDrawItem, nCol, nItemState, &rc);

		//��������
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

		// ����

		dc.FillSolidRect(*lprc, _GetBkColor(nItem, nItemState));

		// ������
		if (m_nCustomStyle & LVS_CT_HGRIDLINES)							// ˮƽ��
		{
			CPen pen;
			pen.CreatePen(m_nHGridLineStyle, 1, m_rHGridLine);
			dc.SelectPen(pen);
			_DrawLine(dc, lprc->left, lprc->bottom - 1, lprc->right, lprc->bottom - 1);	
		}

		if (m_nCustomStyle & LVS_CT_VGRIDLINES)							// ��ֱ��
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
		if (_Base::GetStyle() & LVS_SHOWSELALWAYS)					//�����ô˷��ﲻ���ҵ�Ҫ�������Լ�����
			bSelected = _Base::GetItemState(nDrawItem, LVIS_SELECTED) == LVIS_SELECTED;
		else
			bSelected = nItemState & CDIS_SELECTED;

		//
		if (
			bSelected	&&											//��ǰ����ѡ�е�
			!(m_nCustomStyle & LVS_CT_NOSELECT)						//����ʾѡ��
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
				if (nSubItem == _Base::GetHeader().GetItemCount() - 1)					//ע��ֻ��һ������
				{
					_DrawLine(dc, lprc->right - 1, lprc->top, lprc->right - 1, lprc->bottom);
				}

				// ˮƽ��
				_DrawLine(dc, lprc->left, lprc->top, lprc->right, lprc->top);
				_DrawLine(dc, lprc->left, lprc->bottom - 1, lprc->right, lprc->bottom - 1);

				dc.SelectPen(oldPen);
			}
		}
	}

	inline COLORREF _GetBkColor(int nItem, int nItemState)
	{
		if (
			(nItemState & CDIS_SELECTED)	&&						//��ǰ����ѡ�е�
			!(m_nCustomStyle & LVS_CT_NOSELECT)						//����ʾѡ��
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
			(nItemState & CDIS_SELECTED)	&&						//��ǰ����ѡ�е�
			!(m_nCustomStyle & LVS_CT_NOSELECT) 					//����ʾѡ��
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
	// 		// ����ǰ���������岻����
	// 		int nItemCount = GetItemCount();
	// 		for (int i = GetItemCount(); i < nItem + 1; ++i)
	// 			_Base::AddItem(i, 0, _T(""));
	// 
	// 		_Base::SetItemText(nItem, nSubItem, szText);
	// 	}

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



	// ȡ��Draw��Doc����֮���ӳ��
	inline int _GetDrawItemMap(int nItem2, BOOL bDrawItem2Item = TRUE)
	{
		return nItem2;
		//int nItem = 0;
		//int i;
		//int nCount = bDrawItem2Item ? m_Items.size() : nItem2 + 1;
		//for (i = 0; i < nCount; ++i)
		//{
		//	if (++nItem > nItem2)
		//		break;

		//	_ItemData *pItem = m_Items[i];
		//	if (!pItem->m_bExpand)
		//		i += _GetGroupItemCount(i);
		//}

		//return bDrawItem2Item ? i : nItem - 1;
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



class CSkinListViewCtrlSe : public CSkinListViewCtrlSeImpl<CSkinListViewCtrlSe>
{
};

};



#pragma warning(pop)

#endif // _skinlistctrlse_h_
