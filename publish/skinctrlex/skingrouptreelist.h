/* -------------------------------------------------------------------------
// �ļ���		:	ksgui/skinctrlex/skingrouptreelist.h
// ������		:	����
// ����ʱ��		:	2008-6-20 11:19:04
// ��������     :	�з�������ListView����Ҫ�Ǵ�������չ��/�۵�������/ɾ��Item�߼�
//
// $Id: $
// -----------------------------------------------------------------------*/
#ifndef __KSGUI_SKINCTRLEX_SKINGROUPTREELIST_H__
#define __KSGUI_SKINCTRLEX_SKINGROUPTREELIST_H__

#include "skinlistctrlex.h"

#define _defTreeStateIconName					_T("_grouptree_tree_state")
#define _defCheckBoxIconName					_T("_grouptree_checkbox_state")
#define _defGoodStateIconName					_T("_grouptree_good_state")
enum
{
	LVS_CT_CHECKBOX			= 0x00100000				// CheckBox ��ʽ
};

//////////////////////////////////////////////////////////////////////////
//
class CSkinGroupTreeListView : public CSkinListViewCtrlEx
{
	typedef	CSkinListViewCtrlEx									_Base;

	//////////////////////////////////////////////////////////////////////////
	// ˽�б���
private:
	typedef struct tagGroupItem
	{
		KSGUI::CString		strText;
		COLORREF			rBack;
		tagGroupItem(){};
		tagGroupItem(LPCTSTR szText, COLORREF _rBack)
		{
			strText	= szText;
			rBack	= _rBack;
		}

	} _GroupItem;

	enum { _CS_UnChecked, _CS_Checked, _CS_3rdState };
	typedef struct tagItem 
	{
		DWORD_PTR		oldItemData;
		int				nCheck;
		int				nGroupIndex;
		tagItem(int _nCheck = _CS_UnChecked, int _nGroupIndex = -1, DWORD_PTR _oldItemData = 0)
		{
			nCheck		= _nCheck;
			nGroupIndex	= _nGroupIndex;
			oldItemData	= _oldItemData;
		}
	} _ItemData;

	typedef std::vector<_GroupItem>									Container_Group;
	typedef Container_Group::iterator								Iter_Group;

	Container_Group			m_vecGroups;

	KSGUI::CString			m_strTreeFold;
	KSGUI::CString			m_strTreeExpand;
	KSGUI::CString			m_strTreeItem;
	KSGUI::CString			m_strTreeLastItem;
	int						m_nIconIndexTreeFold : 8;
	int						m_nIconIndexTreeExpand : 8;
	int						m_nIconIndexTreeItem : 8;
	int						m_nIconIndexTreeLastItem : 8;

	KSGUI::CString			m_strCheckBoxChecked;
	KSGUI::CString			m_strCheckBoxUnChecked;
	int						m_nIconIndexChecked : 8;				// ѡ��/��ѡ��ͼ������
	int						m_nIconIndexUnChecked : 8;
	int						m_nIconIndex3rdState: 8;				// ����̬��ͼ������
	KSGUI::CString			m_strCheckBox3rdState;					// ����̬��ʾ�ִ�

public:
	CSkinGroupTreeListView()
	{
		m_nIconIndexTreeFold	= 0;
		m_nIconIndexTreeExpand	= 0;
		m_nIconIndexTreeItem	= 0;
		m_nIconIndexTreeLastItem= 0;
		m_nIconIndexChecked		= 0;
		m_nIconIndexUnChecked	= 0;
		m_nIconIndex3rdState	= 0;
	}
	~CSkinGroupTreeListView()
	{

	}

	//////////////////////////////////////////////////////////////////////////
	// MESSAGE
public:

	BEGIN_MSG_MAP(CSkinGroupTreeListView)
		MESSAGE_HANDLER(WM_DESTROY,	OnDestroy)
		MESSAGE_HANDLER(WM_KEYDOWN,	OnKeyDown)
		REFLECTED_NOTIFY_CODE_HANDLER(NM_CLICK,		OnListViewClick)
		REFLECTED_NOTIFY_CODE_HANDLER(NM_DBLCLK,	OnListViewDBCLick)

		CHAIN_MSG_MAP(CSkinListViewCtrlEx)
	END_MSG_MAP()

	//////////////////////////////////////////////////////////////////////////
	// �ͷ���Դ
	LRESULT OnDestroy(UINT , WPARAM , LPARAM , BOOL& bHandled)
	{
		_DeleteItem(0, _Base::GetItemCount());

		bHandled = FALSE;
		return 0;
	}
	LRESULT OnListViewClick(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
		LPNMITEMACTIVATE pItem = (LPNMITEMACTIVATE)pnmh;
		int nItem = _Base::GetRealItemIndex(pItem->iItem);

		DWORD dwPos = GetMessagePos();
		LVHITTESTINFO ht = { 0 };
		ht.pt.x = GET_X_LPARAM(dwPos);
		ht.pt.y = GET_Y_LPARAM(dwPos);
		_Base::ScreenToClient(&ht.pt);

		KSGUI::CString str;
		int nIndex = -1;
		_Base::SubItemHitTest(&ht, NULL, &nIndex);

		if (_Base::GetGroupLevel(nItem) > 0)							// ��
		{
			if (nIndex == 0)											// �������ڵ�
			{
				BOOL bExpand = _Base::IsGroupExpand(nItem);
				ExpandGroup(nItem, !bExpand);
			}
		}
		else															// ��
		{
			if (nIndex == 2)											// ����Checkbox
			{
				_ItemData *pItemData = (_ItemData*)_Base::GetItemData(nItem);
				if (pItemData->nCheck != _CS_3rdState)		// ����̬��Ͳ�������
					SetCheck(nItem, !GetCheck(nItem));
			}
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnListViewDBCLick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
	{
		LPNMITEMACTIVATE pItem = (LPNMITEMACTIVATE)pnmh;
		int nItem = _Base::GetRealItemIndex(pItem->iItem);

		if (_Base::GetGroupLevel(nItem) > 0)							// ��
		{
			BOOL bExpand = _Base::IsGroupExpand(nItem);
			ExpandGroup(nItem, !bExpand);
		}
		else															// ��
		{
			_ItemData *pItemData = (_ItemData*)_Base::GetItemData(nItem);
			if (pItemData->nCheck != _CS_3rdState)		// ����̬��Ͳ�������
				SetCheck(nItem, !GetCheck(nItem));
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// �����ո���߼�
		if (wParam == VK_SPACE)
		{
			int nItem = _Base::GetRealItemIndex(GetSelectedIndex());
			if (_Base::GetGroupLevel(nItem) == 0)
				SetCheck(nItem, !GetCheck(nItem));
		}

		bHandled = FALSE;
		return 0L;
	}
	//////////////////////////////////////////////////////////////////////////
	// ��������
public:

	//////////////////////////////////////////////////////////////////////////
	// ������������õ���ͼ��
	BOOL SetTreeIcon(HIMAGELIST hImageList, int nFold = 0, int nExpand = 1, int nItem = 2, int nLastItem = 3)
	{
		m_nIconIndexTreeFold		= nFold;
		m_nIconIndexTreeExpand		= nExpand;
		m_nIconIndexTreeItem		= nItem;
		m_nIconIndexTreeLastItem	= nLastItem;
		m_strTreeFold.Format(_T("<img srcid='%s' index='%d'/> "),		_defTreeStateIconName, nFold);
		m_strTreeExpand.Format(_T("<img srcid='%s' index='%d'/> "),		_defTreeStateIconName, nExpand);
		m_strTreeItem.Format(_T("<img srcid='%s' index='%d'/> "),		_defTreeStateIconName, nItem);
		m_strTreeLastItem.Format(_T("<img srcid='%s' index='%d'/> "),	_defTreeStateIconName, nLastItem);
		return _Base::SetImage(_defTreeStateIconName, hImageList);
	}

	//////////////////////////////////////////////////////////////////////////
	// ����Checkboxͼ��
	BOOL SetCheckBoxIcon(HIMAGELIST hImageList, int nUnChecked = 0, int nChecked = 1)
	{
		m_nIconIndexUnChecked	= nUnChecked;
		m_nIconIndexChecked		= nChecked;
		m_strCheckBoxUnChecked.Format(_T("<img srcid='%s' index='%d'/> "),	_defCheckBoxIconName, nUnChecked);
		m_strCheckBoxChecked.Format(_T("<img srcid='%s' index='%d'/> "),	_defCheckBoxIconName, nChecked);
		return _Base::SetImage(_defCheckBoxIconName, hImageList);
	}

	//////////////////////////////////////////////////////////////////////////
	// ���õ���̬����ʾ����
	BOOL Set3rdStateIcon(HIMAGELIST hImageList, int nIndex)
	{
		m_nIconIndex3rdState = nIndex;
		m_strCheckBox3rdState.Format(_T("<img srcid='%s' index='%d'/> "),	_defGoodStateIconName, nIndex);
		return _Base::SetImage(_defGoodStateIconName, hImageList);
	}

	//////////////////////////////////////////////////////////////////////////
	// ����飬����������Index
	int AddGroup(LPCTSTR szText, COLORREF rBack)
	{
		m_vecGroups.push_back(_GroupItem(szText, rBack));
		return (int)m_vecGroups.size() - 1;
	}

	//////////////////////////////////////////////////////////////////////////
	// ��������ز��������ڵ�Item���˺���ֻ�ܵ�һ�У��������Դ���SetItemRichText����
	int InsertItem(int nGroupIndex, LPCTSTR szRichText, int nIndent = 0, BOOL bInsertFirst = FALSE, int nOmitType = 0)
	{
		BOOL bRet;
		KSGUI::CString str;
		int nInsert;
		int nGroupCount;
		int nGroupItem = _FindGroup(nGroupIndex);
		if (nGroupItem == -1)								// ��ʱ�������ڸ��飬��Ҫ����
		{
			int i = nGroupIndex + 1;
			int nCount = _Base::GetItemCount();
			while ((nGroupItem = _FindGroup(i++)) == -1 && i < nCount); // ������������һ��ǰ��
			if (nGroupItem == -1)
				nGroupItem = _Base::GetItemCount();
			bRet = _InsertItem(nGroupItem);
			if (!bRet)
				goto Exit0;

			// ���ø���
			_ItemData *pItemData = _GetItemData(nGroupItem);
			pItemData->nGroupIndex = nGroupIndex;
			_Base::SetItemData(nGroupItem, (DWORD_PTR)pItemData);
			bRet = _Base::SetItemRichText(nGroupItem, 0, m_strTreeExpand + m_vecGroups[nGroupIndex].strText);
			ASSERT(bRet);
			_Base::SetItemStyle(nGroupItem, m_vecGroups[nGroupIndex].rBack, 1, ST_ONELINE);
		}
		
		// �ڸ����в���Item
		nGroupCount = _Base::GetGroupItemCount(nGroupItem);
		nInsert = bInsertFirst ? nGroupItem : nGroupItem + nGroupCount;
		++nInsert;
		bRet = _InsertItem(nInsert);
		if (!bRet)
			goto Exit0;

		// CheckBox
		if (_Base::m_nCustomStyle & LVS_CT_CHECKBOX)
			str = m_strCheckBoxUnChecked + szRichText;
		else
			str = szRichText;

		// ��������״̬ͼ��
		if (bInsertFirst)
		{
			if (nGroupCount > 0)							// ԭ����������Item��
			{
				_Base::SetItemRichText(nInsert, 0, m_strTreeItem + str, nIndent, AT_DEFAULT, nOmitType);
				if (nGroupCount == 1)
				{
					KSingleLineHTMLControl *pDrawer = _Base::_GetDrawerCtrl(nInsert + 1, 0);
					pDrawer->ChangeImagePiece(0, _defTreeStateIconName, m_nIconIndexTreeLastItem);
				}
			}
			else
			{
				_Base::SetItemRichText(nInsert, 0, m_strTreeLastItem + str, nIndent, AT_DEFAULT, nOmitType);
			}
		}
		else
		{
			_Base::SetItemRichText(nInsert, 0, m_strTreeLastItem + str, nIndent, AT_DEFAULT, nOmitType);
			if (nGroupCount > 0)							// ԭ����������Item��
			{
				KSingleLineHTMLControl *pDrawer = _Base::_GetDrawerCtrl(nInsert - 1, 0);
				pDrawer->ChangeImagePiece(0, _defTreeStateIconName, m_nIconIndexTreeItem);
			}
		}

	Exit0:
		ASSERT(bRet);
		return bRet ? nInsert : -1;
	}

	//////////////////////////////////////////////////////////////////////////
	// ����SetItemRichText
	BOOL SetItemRichText(int nItem, int nSubItem, LPCTSTR szRichText, int nIndent = 0, int nAlignType = AT_DEFAULT, int nOmitType = 0)
	{
		if (nSubItem > 0)
			return _Base::SetItemRichText(nItem, nSubItem, szRichText, nIndent, nAlignType, nOmitType);

		//
		ATL::CString strRichText;
		int nGroupItem = GetParentGroup(nItem);
		int nLastGroupItem = nGroupItem + GetGroupItemCount(nGroupItem);
		strRichText += nItem == nLastGroupItem ? m_strTreeLastItem : m_strTreeItem;

		if (m_nCustomStyle & LVS_CT_CHECKBOX)
		{
			_ItemData *pItemData = (_ItemData*)_Base::GetItemData(nItem);
			strRichText += _GetCheckBoxState(pItemData->nCheck);
		}

		return _Base::SetItemRichText(nItem, nSubItem, strRichText + szRichText, nIndent, nAlignType, nOmitType);
	}

	//////////////////////////////////////////////////////////////////////////
	// ɾ����
	BOOL DeleteItem(int nGroupIndex, int nIndexInGroup)
	{
		int nGroupItem = _FindGroup(nGroupIndex);
		return DeleteItem(nGroupItem + nIndexInGroup + 1);
	}
	BOOL DeleteItem(int nItem)
	{
		BOOL bRet;
		int nGroupItem = _Base::GetParentGroup(nItem);
		int nGroupItemCount = _Base::GetGroupItemCount(nGroupItem);

		if ( nGroupItemCount == 1)			// ֻ��һ��
		{
			bRet = _DeleteItem(nGroupItem, 2);
		}
		else
		{
			bRet = _DeleteItem(nItem);
			if (nItem == nGroupItem + nGroupItemCount)		// ɾ���Ǹ������һ��
			{
				KSingleLineHTMLControl *pDrawer = _Base::_GetDrawerCtrl(nItem - 1, 0);
				pDrawer->ChangeImagePiece(0, _defTreeStateIconName, m_nIconIndexTreeLastItem);
				_Base::RedrawItems(nItem - 1, nItem - 1);
			}
		}

		return bRet;
	}

	BOOL DeleteAllItems()
	{
		_Base::SetRedraw(FALSE);
		BOOL bRet = _DeleteItem(0, _Base::GetItemCount());
		_Base::SetRedraw(TRUE);

		return bRet;
	}

	//////////////////////////////////////////////////////////////////////////
	// ����Checkbox״̬
	void SetCheck(int nItem, BOOL bChecked = TRUE)
	{
		ASSERT(_Base::m_nCustomStyle & LVS_CT_CHECKBOX);
		_ItemData *pItemData = _GetItemData(nItem);
		pItemData->nCheck = bChecked ? _CS_Checked : _CS_UnChecked;
		KSingleLineHTMLControl *pDrawer = _Base::_GetDrawerCtrl(nItem, 0);
		pDrawer->ChangeImagePiece(2, _defCheckBoxIconName, bChecked ? m_nIconIndexChecked : m_nIconIndexUnChecked);
		_Base::RedrawItems(nItem, nItem);
	}
	BOOL GetCheck(int nItem)
	{
		ASSERT(_Base::m_nCustomStyle & LVS_CT_CHECKBOX);
		return _GetItemData(nItem)->nCheck == _CS_Checked;
	}

	//////////////////////////////////////////////////////////////////////////
	// ���õ���̬����Ҫ�ǿ��ǵ��������̫�ռ��ˣ��ż����
	void Set3rdState(int nItem)
	{
		_ItemData *pItemData = _GetItemData(nItem);
		pItemData->nCheck = _CS_3rdState;
		KSingleLineHTMLControl *pDrawer = _Base::_GetDrawerCtrl(nItem, 0);
		pDrawer->ChangeImagePiece(2, _defGoodStateIconName, m_nIconIndex3rdState);
		_Base::RedrawItems(nItem, nItem);
	}

	//////////////////////////////////////////////////////////////////////////
	// ��Ϊ���������õ���ItemData������ҲҪ���ص�
	DWORD_PTR GetItemData(int nItem)
	{
		return _GetItemData(nItem)->oldItemData;
	}
	BOOL SetItemData(int nItem, DWORD_PTR dwData)
	{
		_ItemData *pItemData = _GetItemData(nItem);
		pItemData->oldItemData = dwData;
		return _Base::SetItemData(nItem, (DWORD_PTR)pItemData);
	}

	//////////////////////////////////////////////////////////////////////////
	//
	int SubItemHitTest(LPLVHITTESTINFO lpInfo, int *phtCode, int *pIndex = NULL)
	{
		int nDrawItem = _Base::SubItemHitTest(lpInfo, phtCode, pIndex);
		if (nDrawItem == -1 || lpInfo->iSubItem != 0)				// ֻ�����һ��
			return nDrawItem;

		// ����
		
		int nItem = _Base::GetRealItemIndex(nDrawItem);
		if (_Base::GetGroupLevel(nItem) > 0)						// ��
			*pIndex -= 2;			// һ��ͼ��ͺ����һ���ո�
		else														// ��
		{
			if (_Base::m_nCustomStyle & LVS_CT_CHECKBOX)
				*pIndex -= 3;
			else
				*pIndex -= 1;
			
			// ��Ҫ����һ������
			KSingleLineHTMLControl *pDrawer = _Base::_GetDrawerCtrl(nItem, 0);
			if (pDrawer->GetPieceInfo(*pIndex + 1, NULL) == ST_IMG)
				*pIndex -= 1;
		}

		return nDrawItem;
	}


	//////////////////////////////////////////////////////////////////////////
	// ���ػ��ຯ��
public:
	
	// չ��/�������ڵ�
	virtual int ExpandGroup(int nItem, BOOL bExpand = TRUE)
	{
		KSingleLineHTMLControl *pDrawer = _Base::_GetDrawerCtrl(nItem, 0);
		pDrawer->ChangeImagePiece(0, _defTreeStateIconName, bExpand ? m_nIconIndexTreeExpand : m_nIconIndexTreeFold);
		_Base::RedrawItems(nItem, nItem);

		return _Base::ExpandGroup(nItem, bExpand);
	}

	//////////////////////////////////////////////////////////////////////////
	// Helper
private:
	inline int _FindGroup(int nGroupIndex)
	{
		int nCount = _Base::GetItemCount();
		for (int i = 0; i < nCount; ++i)
		{
			_ItemData *pItemData = _GetItemData(i);
			if (pItemData->nGroupIndex == nGroupIndex)
				return i;
		}

		return -1;
	}
	inline BOOL _InsertItem(int nItem)
	{
		_ItemData *pItemData = new _ItemData;
		ASSERT(pItemData);

		BOOL bRet = _Base::InsertItem(nItem);
		if (!bRet)
			return FALSE;

		return _Base::SetItemData(nItem, (DWORD_PTR)pItemData);
	}
	inline BOOL _DeleteItem(int nItem, int nCount = 1)
	{
		for (int i = nItem + nCount - 1; i >= nItem; --i)
		{
			_ItemData *pItemData = _GetItemData(i);
			if (pItemData)
				delete pItemData;

			BOOL bRet = _Base::DeleteItem(i);
			ASSERT(bRet);
			if (!bRet)
				return FALSE;
		}

		return TRUE;
	}

	inline _ItemData* _GetItemData(int nItem)
	{
		return (_ItemData*)_Base::GetItemData(nItem);
	}
	inline KSGUI::CString _GetCheckBoxState(int nCheck)
	{
		switch(nCheck)
		{
		case _CS_Checked:
			return m_strCheckBoxChecked;
		case _CS_UnChecked:
		    return m_strCheckBoxUnChecked;
		case _CS_3rdState:
			return m_strCheckBox3rdState;
		default:
		    ASSERT(FALSE);
		}
	}
};

// -------------------------------------------------------------------------
// $Log: $

#endif /* __KSGUI_SKINCTRLEX_SKINGROUPTREELIST_H__ */
