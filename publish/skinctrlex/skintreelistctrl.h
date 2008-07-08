/********************************************************************
* CreatedOn: 2008-5-20   10:11
* FileName: skintreelistctrl.h
* CreatedBy: ichenglin  <lichenglin@kingsoft.net>
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/
#pragma once

namespace KSGUI{

#include <ksguicommon/drawer/StandardDrawer.h>
#include <map>
#include <atlframe.h>

//////////////////////////////////////////////////////////////////////////

#define WM_SUBITEMCLICKED   (WM_USER + 0x221)

template<class T>
class KTreeItem;

class KTreeItemData;

typedef KTreeItem<KTreeItemData>*  HTREELISTITEM;

class ITreeListItemData
{
public:

    virtual LPCTSTR GetItemText  ( HTREELISTITEM hItem, int nsubindex ) = 0;
    virtual int     GetImageIndex( HTREELISTITEM hItem, BOOL bSelected = FALSE ) = 0;    
    virtual HICON   GetIcon      ( HTREELISTITEM hItem, BOOL bSelected = FALSE ) = 0;
    virtual HBITMAP GetBitmap    ( HTREELISTITEM hItem, BOOL bSelected = FALSE ) = 0;
    virtual BOOL    Filter       ( HTREELISTITEM hItem, LPCTSTR pszField ) = 0;

    virtual BOOL    OnClicked    ( HTREELISTITEM hItem, int nsubIndex) = 0;

};

class KTreeListItemDataBase: public ITreeListItemData
{
public:
	virtual ~KTreeListItemDataBase() {}

	virtual LPCTSTR GetItemText  ( HTREELISTITEM hItem, int nsubindex )
	{
		return TEXT("");
	}
	virtual int     GetImageIndex( HTREELISTITEM hItem, BOOL bSelected = FALSE )
	{
		return -1;
	}
	virtual HICON   GetIcon      ( HTREELISTITEM hItem, BOOL bSelected = FALSE )
	{
		return NULL;
	}
	virtual HBITMAP GetBitmap    ( HTREELISTITEM hItem, BOOL bSelected = FALSE )
	{
		return NULL;
	}
	virtual BOOL    Filter       ( HTREELISTITEM hItem, LPCTSTR pszField )
	{
		return TRUE;
	}

	virtual BOOL    OnClicked    ( HTREELISTITEM hItem, int nsubIndex)
	{
		return TRUE;
	}
};
//////////////////////////////////////////////////////////////////////////



//  树结点包含以下数据:
//  1. 	 m_data         用于存放数据
//  2.   m_pParent      指向该结点的父结点, 如果该结点为根结点, m_pParent为空
//  3.   m_pFirstChild  指向该结点的第一个子结点, 如果没有子结点, m_pFirstChild为空
//  4.   m_pNextSibling 指向该结点的下一个兄弟结点, 如果后面没有兄弟, m_pNextSibling为空
//
//  树 函数简介:
//  1.   Clear          清空树, 删除所有结点
//  2.   InsertItem     在指定的父结点的所有子结点末尾 插入一个结点
//  3.   DeleteItem     删除指定结点 以及该节点的所有子结点
//	4.   GetParent      获取指定结点的父结点, 根结点的父结点为NULL
//  5.   GetNextSibling 获取指定结点的下一个兄弟结点
//  6.   GetFirstChild  获取指定结点的第一个子结点
//  7.   HasChildren    查看指定结点是否有子结点
//  8.   Size           获取当前结点总数
//
//



template<class T>
class KTree
{ 
public:
    KTree()
    {
        m_nItemCount = 0;
    }

    ~KTree()
    {
        Clear();
    }

    KTreeItem<T>* GetRootItem() const
    {
        return const_cast<KTreeItem<T>*>(&m_root);
    }

    void Clear()
    {
        DeleteItem( &m_root );

        assert( m_nItemCount == 0 );
    }

    KTreeItem<T>* InsertItem( const T& data, KTreeItem<T>* pParent )
    {
        if( pParent == NULL )
        {
            return InsertItem(data, &m_root);
        }

        KTreeItem<T>* pItem = new KTreeItem<T>;
        if( pItem == NULL )
        {
            return NULL;
        }

        m_nItemCount++;

        pItem->m_data    = data;
        pItem->m_pParent = pParent;

        if (true)
        {
            KTreeItem<T>* pIter = pParent->m_pFirstChild;

            if( pIter == NULL )
            {
                pParent->m_pFirstChild = pItem;
            }
            else
            {
                while( pIter->m_pNextSibling != NULL )
                {
                    pIter = pIter->m_pNextSibling;
                }

                pIter->m_pNextSibling = pItem;
            }
        }

        return pItem;
    }


    void DeleteItem( KTreeItem<T>* pItem )
    {
        assert( pItem != NULL );

        DeleteChildren( pItem );

        if( pItem != &m_root )
        {
            KTreeItem<T>*  pParent = GetParent( pItem );
            KTreeItem<T>*  pIter   = pParent->m_pFirstChild;

            if( pIter == pItem )
            {
                pParent->m_pFirstChild = pIter->m_pNextSibling;
            }
            else
            {
                while( pIter->m_pNextSibling != pItem )
                {
                    pIter = pIter->m_pNextSibling;
                    assert( pIter != NULL );
                }

                pIter->m_pNextSibling = pItem->m_pNextSibling;
            }

        }
        else
        {
            m_root.m_pFirstChild  = NULL;
            m_root.m_pNextSibling = NULL;
            m_root.m_pParent      = NULL;
        }

        if (pItem != &m_root)
        {
            delete pItem;
            m_nItemCount--;
        }
    }



    KTreeItem<T>* GetParent( const KTreeItem<T>* pItem ) const
    {
        assert( pItem != NULL );
        return pItem->m_pParent;
    }

    KTreeItem<T>* GetNextSibling( const KTreeItem<T>* pItem ) const
    {
        assert( pItem != NULL );
        return pItem->m_pNextSibling;
    }


    KTreeItem<T>* GetFirstChild( const KTreeItem<T>* pItem ) const
    {
        assert( pItem != NULL );
        return pItem->m_pFirstChild;
    }


    bool HasChildren( const KTreeItem<T>* pItem ) const
    {
        return ( pItem->m_pFirstChild != NULL );
    }


    int Size() const
    {
        return m_nItemCount;
    }

private:
    void DeleteChildren( KTreeItem<T>* pItem )
    {
        KTreeItem<T>* pNext = NULL;
        KTreeItem<T>* pIter = NULL;

        for( pIter = pItem->m_pFirstChild; pIter != NULL; pIter = pNext )
        {
            pNext = pIter->m_pNextSibling;

            DeleteChildren( pIter );
            delete pIter;
            pIter = NULL;
            m_nItemCount--;
        }
    }


private: 
    KTreeItem<T>  m_root;
    int           m_nItemCount;
};


//////////////////////////////////////////////////////////////////////////
//
// 树结点
//
//////////////////////////////////////////////////////////////////////////


template<class T>
class KTreeItem
{
    friend class KTree<T>;
public:
    KTreeItem()
    {
        m_pParent      = NULL;
        m_pFirstChild  = NULL;
        m_pNextSibling = NULL;
    }

    ~KTreeItem()
    {
    }

    const T& GetData() const
    {
        return m_data;
    }

    T& GetData()
    {
        return m_data;
    }

    void SetData( const T& data )
    {
        m_data = data;
    }


    T      m_data;

    KTreeItem<T>*  m_pParent;  
    KTreeItem<T>*  m_pFirstChild;
    KTreeItem<T>*  m_pNextSibling;              
};


class KTreeItemData
{
public:
    KTreeItemData() :
        m_piItemData(NULL),
        m_bVisual(TRUE),
        m_bExpand(FALSE),
        m_bChecked(FALSE),
        m_dwData(0)
    {
    }

    ~KTreeItemData()
    {
    }

    LPCTSTR GetItemText  ( HTREELISTITEM hItem, int nsubindex )
    {
        if (m_piItemData == NULL)
            return _T("");

        return m_piItemData->GetItemText(hItem, nsubindex);
    }

    int GetImageIndex( HTREELISTITEM hItem, BOOL bSelected = FALSE )   
    {
        if (m_piItemData == NULL)
            return -1;

        return m_piItemData->GetImageIndex(hItem, bSelected);
    }

    HICON GetIcon( HTREELISTITEM hItem, BOOL bSelected = FALSE )
    {
        if (m_piItemData == NULL)
            return NULL;

        return m_piItemData->GetIcon(hItem, bSelected);
    }
    HBITMAP GetBitmap( HTREELISTITEM hItem, BOOL bSelected = FALSE )
    {
        if (m_piItemData == NULL)
            return NULL;

        return m_piItemData->GetBitmap(hItem, bSelected);
    }


    void Filter( HTREELISTITEM hItem , LPCTSTR pszField)
    {
        if (m_piItemData == NULL)
        {
            m_bVisual = TRUE;
        }
        else
            m_bVisual = m_piItemData->Filter(hItem, pszField);
    }

    BOOL OnClicked( HTREELISTITEM hItem, int nsubIndex)
    {
        if (m_piItemData != NULL)
            return m_piItemData->OnClicked(hItem, nsubIndex);

        return FALSE;            
    }


public:

    ITreeListItemData* m_piItemData;

    BOOL    m_bVisual;
    BOOL    m_bExpand;
    BOOL    m_bChecked;
    DWORD   m_dwData;
};


class KExpandableTree : public KTree<KTreeItemData>
{
public:
    KExpandableTree()
    {
    }
    virtual ~KExpandableTree()
    {
    }

    BOOL KExpandableTree::IsItemExpanded( const HTREELISTITEM pItem ) const
    {
        ATLASSERT( pItem != NULL );
        return pItem->GetData().m_bExpand;
    }


    BOOL KExpandableTree::IsItemVisible( const HTREELISTITEM pItem ) const
    {
        ATLASSERT( pItem != NULL );

        HTREELISTITEM pParent  = pItem;
        BOOL bVisible = TRUE;

        if ( !pItem->GetData().m_bVisual )
            return FALSE;

        do
        {
            pParent = GetParent( pParent );
            if( pParent == NULL )
            {
                break;
            }

            bVisible = IsItemExpanded( pParent ) && pParent->GetData().m_bVisual;
        } 
        while( bVisible );

        return bVisible;
    }


    BOOL KExpandableTree::ExpandItem( HTREELISTITEM pItem, BOOL bExpand )
    {
        assert( pItem != NULL );

        HTREELISTITEM pParent = GetParent( pItem );
        if( pParent != NULL && !IsItemExpanded( pParent ) )
        {
            return FALSE;
        }

        if( !HasChildren( pItem ) )
        {
            return FALSE;
        }

        pItem->GetData().m_bExpand = bExpand;

        return TRUE;
    }


    void KExpandableTree::ExpandTo( HTREELISTITEM pItem )
    {
        pItem = GetParent( pItem );

        while( pItem != NULL && !IsItemExpanded( pItem ) )
        {
            pItem->GetData().m_bExpand = TRUE;
            pItem = GetParent( pItem );
        }
    }


    int KExpandableTree::GetVisibleItemCount() const
    {
        int nVisibleItemCount = 0;

        HTREELISTITEM pItem = GetRootItem();

        if (pItem != NULL)
            pItem = GetNextVisibleItem( pItem );

        while( pItem != NULL )  
        {
            nVisibleItemCount++;
            pItem = GetNextVisibleItem( pItem );
        }

        return nVisibleItemCount;
    }


    HTREELISTITEM KExpandableTree::GetNextVisibleItem( const HTREELISTITEM pItem ) const
    {
        if( pItem == NULL )
        {
            return GetRootItem() != NULL ? GetNextVisibleItem(GetRootItem()) : NULL;
        }

        if( IsItemExpanded( pItem ) && HasChildren( pItem ) && pItem->GetData().m_bVisual )
        {
            return (GetFirstChild( pItem )->GetData().m_bVisual) ? 
                GetFirstChild( pItem ) : GetNextVisibleItem(GetFirstChild( pItem ));
        }
        else 
        {
            HTREELISTITEM pNextItem = GetNextSibling( pItem );
            HTREELISTITEM pParent   = pItem;

            while( true )  
            {
                while (pNextItem != NULL)
                {
                    if (pNextItem->GetData().m_bVisual)
                        return pNextItem;
                    pNextItem = GetNextSibling( pNextItem );
                }

                pParent = GetParent( pParent );
                if( pParent == NULL || pParent == GetRootItem() )
                {
                    break;
                }

                pNextItem = GetNextSibling( pParent );
            }

            return pNextItem;
        }

    }


    HTREELISTITEM KExpandableTree::GetVisibleItem( int nIndex ) const
    {
        ATLASSERT( nIndex >= 0 );

        HTREELISTITEM pItem = GetRootItem();
        if (pItem != NULL)
            pItem = GetNextVisibleItem( pItem );

        for( int i = 0; i < nIndex; i++ )
        {
            pItem = GetNextVisibleItem( pItem );
            if( pItem == NULL )
            {
                break;
            }
        }

        return pItem;
    }



    int KExpandableTree::GetVisibleItemIndex( const HTREELISTITEM pItem ) const
    {
        int nIndex = 0;

        HTREELISTITEM pTargetItem = GetRootItem();
        pTargetItem = GetNextVisibleItem( pTargetItem );

        while( pTargetItem != NULL )  
        {
            if( pTargetItem == pItem )
            {
                break;
            }

            pTargetItem = GetNextVisibleItem( pTargetItem );
            nIndex++;
        }

        return ( pTargetItem == NULL ) ? -1 : nIndex;
    }



    int KExpandableTree::GetItemLayer( HTREELISTITEM pItem ) const
    {
        int nLayer = -1;

        HTREELISTITEM pParent = pItem;
        while( pParent != NULL )  
        {
            pParent = GetParent( pParent );
            nLayer++;
        }

        return nLayer;
    }

private:

};



class CSkinTreeListCtrl : 
    public KSGUI::SkinWindowImpl<CSkinTreeListCtrl, KSGUI::CSkinListViewCtrl>,
    public WTL::COwnerDraw<CSkinTreeListCtrl>,
    public WTL::CDoubleBufferImpl<CSkinTreeListCtrl>
{
private:

    typedef KSGUI::SkinWindowImpl<CSkinTreeListCtrl, KSGUI::CSkinListViewCtrl> theBaseClass;

public:
    //------------------------------------------------------------------------------------------
    // 构造 析构
    //------------------------------------------------------------------------------------------

    CSkinTreeListCtrl(void)
    {
        m_bTreeView      = TRUE;
        m_nTreeIndent    = 19;
        m_crSelectedItem = ::GetSysColor( COLOR_HIGHLIGHT );
        m_tree.GetRootItem()->GetData().m_bExpand = TRUE;
    }
    ~CSkinTreeListCtrl(void)
    {  }

    BOOL SubclassWindow( HWND hWnd )
    {    
        BOOL bRet = theBaseClass::SubclassWindow( hWnd );

        if( bRet )
        {
            _Init();
        }

        return bRet;
    }

    BOOL SetTreeViewStyle(BOOL bTreeView = TRUE)
    {
        BOOL bResult = m_bTreeView;
        m_bTreeView = bTreeView;
        return bResult;
    }


    //------------------------------------------------------------------------------------------
    // 操作
    //------------------------------------------------------------------------------------------

    /**
    * @brief   插入结点
    * @param   [in] lpszItem,       结点名称
    * @param   [in] nImage,         结点图标
    * @param   [in] nSelectedImage, 选择时的图标
    * @param   [in] pParent,        父结点, 如果父结点为NULL, 则插入根结点
    * @return  指向该结点的指针, 可以当成标示该结点的句柄
    * @remark  
    */
    HTREELISTITEM InsertItem(
        LPCTSTR   lpszItem,
        int		  nImage,
        int		  nSelectedImage,
        HTREELISTITEM pParent  = NULL
        )
    {
        ATLASSERT(FALSE);
        return NULL;
    }

    /**
    * @brief   插入结点
    * @param   [in] lpszItem,    结点名称
    * @param   [in] pParent,     父结点, 如果父结点为NULL, 则插入根结点
    * @return  指向该结点的指针, 可以当成标示该结点的句柄
    * @remark  
    */
    HTREELISTITEM InsertItem( ITreeListItemData* piData, HTREELISTITEM pParent  = NULL )
    {
        KTreeItemData data;

        data.m_bExpand    = FALSE;
        data.m_piItemData = piData;

        HTREELISTITEM pItem = m_tree.InsertItem( data, pParent );

        if( ( pItem != NULL ) && m_tree.IsItemVisible( pItem ) ) 
        {
            _Refresh( pItem, pItem );
        }

        return pItem;
    }

    /**
    * @brief   删除结点( 包含该结点的所有子结点 )
    * @param   [in] pItem,  该节点的地址( 由InsertItem返回 )
    * @remark  
    */
    void DeleteItem( HTREELISTITEM pItem )
    {
        ATLASSERT( pItem != NULL );
        int nDeletedItem = m_tree.GetVisibleItemIndex( pItem );

        m_tree.DeleteItem( pItem );
        pItem = NULL;

        if( nDeletedItem != -1 )
        {
            _Refresh( nDeletedItem - 1, -1 );
        }
    }

    /**
    * @brief   删除结点( 包含该结点的所有子结点 )
    * @param   [in] nItem,  目前(展开可见的)第nItem个结点
    * @remark  
    */
    void DeleteItem( int nItem )
    {
        ATLASSERT( nItem >= 0 );

        HTREELISTITEM pItem = m_tree.GetVisibleItem( nItem );
        DeleteItem( pItem );
    }

    void SetItemVisual( HTREELISTITEM pItem, BOOL bVisual = TRUE )
    {
        ATLASSERT( pItem != NULL );
        int nItemIndex = m_tree.GetVisibleItemIndex( pItem );
        
        pItem->GetData().m_bVisual = bVisual;

        if( nItemIndex != -1 )
        {
            _Refresh( nItemIndex - 1, -1 );
        }
    }

	HTREELISTITEM GetVisibleItem( int nIndex ) const
	{
		return m_tree.GetVisibleItem(nIndex);
	}

	int GetVisibleItemCount() const
	{
		return m_tree.GetVisibleItemCount();
	}

    void SetFilter( LPCTSTR pszFilter , HTREELISTITEM pItem = NULL)
    {
		int nSelIndex = GetSelectedIndex();

		if ( nSelIndex >= 0 )
		{
			SetItemState(nSelIndex, 0, LVIS_SELECTED | LVIS_FOCUSED);
		}

        _SetFilter(pszFilter, pItem);

        Refresh();
    }


    void _SetFilter( LPCTSTR pszFilter , HTREELISTITEM pItem )
    {
        HTREELISTITEM hNextItem = m_tree.GetFirstChild( m_tree.GetRootItem() );
        while (hNextItem != NULL)
        {
            hNextItem->GetData().Filter(hNextItem, pszFilter);

            hNextItem = m_tree.GetNextSibling( hNextItem );
        }


        //if (pItem == NULL)
        //{
        //    _SetFilter(pszFilter, m_tree.GetRootItem());
        //    return;
        //}

        //pItem->GetData().Filter(pItem, pszFilter);

        //HTREELISTITEM hNextItem = m_tree.GetFirstChild( pItem );
        //while (hNextItem != NULL)
        //{
        //    _SetFilter( pszFilter, hNextItem );

        //    hNextItem = m_tree.GetNextSibling( hNextItem );
        //}
    }

    /**
    * @brief   展开结点
    * @param   [in] pItem,   待展开的节点的地址( 由InsertItem返回 )
    * @param   [in] bExpand, TRUE表示展开 FALSE表示收拢
    * @remark  
    */
    void Expand( HTREELISTITEM pItem, BOOL bExpand )
    {
        ATLASSERT( pItem != NULL );

        if( m_tree.ExpandItem( pItem, bExpand ) )
        {
            _Refresh( pItem );
        }
    }

    /**
    * @brief   展开pItem路径上所有结点, 使pItem可见
    * @param   [in] pItem,   待展开的节点的地址( 由InsertItem返回 )
    * @remark  pItem本身并不展开
    */
    void ExpandTo( HTREELISTITEM pItem )
    {
        m_tree.ExpandTo( pItem );
        _Refresh( 0, -1 );
    }

    /**
    * @brief   为各子结点设置显示的文字
    * @param   [in] pItem,    结点( 由InsertItem返回 )
    * @param   [in] nSubItem, 结点的第几列
    * @param   [in] lpszText, 文字
    * @remark  
    */
    //BOOL	  SetItemText( HTREELISTITEM pItem, int nSubItem, LPCTSTR lpszText );

    /**
    * @brief   获取结点的显示文字
    * @param   [in]  pItem,    结点( 由InsertItem返回 )
    * @param   [in]  nSubItem, 结点的第几列
    * @param   [out] strText,  返回文字
    * @remark  
    */
    //BOOL      GetItemText( HTREELISTITEM pItem, int nSubItem, ATL::CString& strText );

    /**
    * @brief   设置结点图片
    * @param   [in]  pItem,    结点( 由InsertItem返回 )
    * @param   [in]  nImage,   和该树列表相关的Imagelist中的图片Index, -1为没有图片
    * @param   [in]  nSelectedImage,   结点被选中时的图片
    * @remark  
    */
    //void      SetItemImage( HTREELISTITEM pItem, int nImage, int nSelectedImage, BOOL bRedraw = TRUE );

    /**
    * @brief   获取结点图片
    * @param   [in]   pItem,    结点( 由InsertItem返回 )
    * @param   [out]  nImage,   和该树列表相关的Imagelist中的图片Index, -1为没有图片
    * @param   [out]  nSelectedImage,   结点被选中时的图片
    * @remark  
    */
    //void      GetItemImage( HTREELISTITEM pItem, int& nImage, int& nSelectedImage );

    /**
    * @brief   设置某列是否使用checkbox
    * @param   [in] nColumn,    列, 0-based index
    * @param   [in] bCheckable, TRUE表示该列使用checkbox, FALSE表示不使用(默认不使用)
    * @remark  
    */
    void SetColumnCheckable( int nColumn, BOOL bCheckable )
    {
        HDITEM  hdItem = {0};
        hdItem.mask  = HDI_LPARAM;

        CHeaderCtrl header = GetHeader();
        header.GetItem( nColumn, &hdItem );

        hdItem.lParam = bCheckable;
        header.SetItem( nColumn, &hdItem );
    }

    /**
    * @brief   检查某列是否使用了checkbox
    * @param   [in] nColumn,    列, 0-based index
    * @return  如果该列使用了CheckBox返回TRUE, 否则返回FALSE
    * @remark  
    */
    BOOL IsColumnCheckable( int nColumn ) const
    {
        HDITEM hdItem = {0};
        hdItem.mask = HDI_LPARAM;

        CHeaderCtrl header = GetHeader();
        header.GetItem( nColumn, &hdItem );

        return (BOOL)hdItem.lParam;
    }

    /**
    * @brief   获取子结点的Check状态
    * @param   [in] pItem,    结点( 由InsertItem返回 )
    * @param   [in] nSubItem, 子结点
    * @return  返回该子结点是否check
    * @remark  
    */
    BOOL GetSubItemCheckStatus( HTREELISTITEM pItem, int nSubItem ) const
    {
        if (pItem == NULL)
            return FALSE;

        return pItem->GetData().m_bChecked;
    }

    /**
    * @brief   设置子结点的Check状态
    * @param   [in] pItem,        结点( 由InsertItem返回 )
    * @param   [in] nSubItem,     子结点
    * @param   [in] bCheckStatus, check状态
    * @return  成功返回TURE, 失败(该列没有使用checkbox)返回FALSE
    * @remark  
    */
    BOOL	  SetSubItemCheckStatus( HTREELISTITEM pItem, int nSubItem, BOOL bCheckStatus )
    {
        if( IsColumnCheckable( nSubItem ) )
        {
            pItem->GetData().m_bChecked = bCheckStatus;

            int nItem = m_tree.GetVisibleItemIndex( pItem );
            if( nItem != -1 )
            {
                RedrawItems( nItem, nItem );
            }

            return TRUE;
        }
        else
        {
            return FALSE;
        }

    }

    /**
    * @brief   在指定的结点上设置一个32位的关联数据
    * @param   [in] pItem,      结点( 由InsertItem返回 )
    * @param   [in] dwData,     关联数据
    * @remark  
    */
    void	  SetItemData( HTREELISTITEM pItem, DWORD dwData )
    {
        pItem->GetData().m_dwData = dwData;
    }

    /**
    * @brief   获取指定的结点上的关联数据
    * @param   [in] pItem,      结点( 由InsertItem返回 )
    * @return  关联数据
    * @remark  
    */
    DWORD	  GetItemData( HTREELISTITEM pItem ) const
    {
        return pItem->GetData().m_dwData;
    }

    /**
    * @brief   删除所有结点
    * @remark  
    */
    BOOL	  DeleteAllItems()
    {
        BOOL bRet = TRUE;

        m_tree.Clear();
        _Refresh( 0 );

        return bRet;
    }

    /**
    * @brief   刷新所有结点
    * @remark  
    */
    void Refresh()
    {
        _Refresh( 0, -1 );
    }


    /**
    * @brief   获取当前所选择的结点
    * @remark  
    */
    HTREELISTITEM GetSelectedItem() const
    {
        int nCurSel = GetSelectedIndex();

        if( nCurSel >= 0 )
        {
            return m_tree.GetVisibleItem( nCurSel );
        }
        else
        {
            return NULL;
        }
    }




    enum ENUM_HIT_SPEC{ eNormal = 0, eFlagBtn, eCheckBox, eImage };
    struct FBHITTESTINFO
    {
        HTREELISTITEM pItem;
        int           nItem;
        int           nSubItem;
        ENUM_HIT_SPEC eHitSpec;
    };

    /**
    * @brief   点击测试, 测试某点 在树-列表的哪个位置
    * @param   [in]  pt,           待测试的点
    * @param   [out] pHitTestInfo, 位置信息
    * @remark  
    */
    void HitTest( const POINT& pt, FBHITTESTINFO* pInfo )
    {
        memset( pInfo, 0, sizeof( FBHITTESTINFO ) );

        LVHITTESTINFO lvhti;
        lvhti.pt = pt;
        SubItemHitTest( &lvhti );

        //-------------------------------------
        // 常规信息
        pInfo->nItem    = lvhti.iItem;
        pInfo->nSubItem = lvhti.iSubItem;
        if( lvhti.iItem < 0 )
        {
            return;
        }

        pInfo->pItem = m_tree.GetVisibleItem( pInfo->nItem  );

        //-------------------------------------
        // 特殊信息
        CRect rcUnit;
        GetSubItemRect( lvhti.iItem, lvhti.iSubItem, LVIR_BOUNDS, &rcUnit );
        int xOffset = rcUnit.left;


        // 测试是否在 树的展开收缩标记上
        if( m_bTreeView && lvhti.iSubItem == 0 )        
        {
            int nLayer = m_tree.GetItemLayer( pInfo->pItem );
            nLayer--;
            xOffset   += nLayer * m_nTreeIndent;

            if( pt.x > xOffset && pt.x < xOffset + m_nTreeIndent &&
                m_tree.HasChildren( pInfo->pItem ) 
                )
            {
                pInfo->eHitSpec = eFlagBtn;
                return;
            }

            xOffset += m_nTreeIndent;
        }

        // 测试是否在 check box上
        if( IsColumnCheckable( lvhti.iSubItem ) )
        {
            if( pt.x > xOffset && pt.x < xOffset + m_nTreeIndent )
            {
                pInfo->eHitSpec = eCheckBox;
                return;
            }

            xOffset += m_nTreeIndent;
        }


        // 测试是否在 图标上
        if( lvhti.iSubItem == 0 )  
        {
            if( pt.x > xOffset && pt.x < xOffset + m_nTreeIndent )
            {
                pInfo->eHitSpec = eImage;
                return;
            }
        }

        return;

    }

    /**
    * @brief   设置图像列表
    * @param   [in]  hImageList,  图像列表
    * @remark  
    */
    CImageList SetImageList( HIMAGELIST hImageList )
    {
        if( hImageList == NULL )
        {
            return NULL;
        }


        IMAGEINFO info = {0};
        ImageList_GetImageInfo( hImageList, 0, &info );
        CRect rcImage = info.rcImage;

        m_sizeImage.cx = rcImage.Width();
        m_sizeImage.cy = rcImage.Height();

        m_nTreeIndent = max( m_nTreeIndent, m_sizeImage.cx );

        return theBaseClass::SetImageList( hImageList, LVSIL_SMALL );
    }

    /**       
    * @brief   设置结点被选中后的颜色
    * @param   [in]  crSelectedItem,  颜色
    * @remark  
    */
    void  SetSelectedItemColor( COLORREF crSelectedItem )
    {
        m_crSelectedItem = crSelectedItem;
    }



    //------------------------------------------------------------------------------------------
    // 消息处理
    //------------------------------------------------------------------------------------------
    BEGIN_MSG_MAP( CSkinRichEditCtrl )
        MESSAGE_HANDLER( WM_CREATE,        OnCreate )
        MESSAGE_HANDLER( WM_LBUTTONDOWN,   OnLButtonDown )
        MESSAGE_HANDLER( WM_LBUTTONDBLCLK, OnLButtonDblClk )
        CHAIN_MSG_MAP_ALT( WTL::COwnerDraw<CSkinTreeListCtrl>, 1 )
        CHAIN_MSG_MAP( WTL::CDoubleBufferImpl<CSkinTreeListCtrl> )
        DEFAULT_REFLECTION_HANDLER()
    END_MSG_MAP()

    LRESULT OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
    {
        _Init();

        return 0;
    }
    virtual LRESULT OnLButtonDown( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
    {
        if (::GetFocus() != m_hWnd)
            SetFocus();

        int xPos = GET_X_LPARAM( lParam ); 
        int yPos = GET_Y_LPARAM( lParam ); 

        FBHITTESTINFO fbHitInfo;
        HitTest( CPoint( xPos, yPos ), &fbHitInfo );

        if( fbHitInfo.nItem < 0 || fbHitInfo.pItem == NULL)
        {
            return 0;
        }


        if( fbHitInfo.eHitSpec == eCheckBox ) 
        { 
            BOOL bLastCheckStatus = GetSubItemCheckStatus( fbHitInfo.pItem, fbHitInfo.nSubItem );
            OnClickedCheckbox( fbHitInfo.pItem, fbHitInfo.nItem, fbHitInfo.nSubItem, bLastCheckStatus );
        }
        else if( fbHitInfo.eHitSpec == eFlagBtn )
        {
            OnClickedFlagBtn( fbHitInfo.pItem, fbHitInfo.nItem, m_tree.IsItemExpanded( fbHitInfo.pItem ) );
        }
        else if( fbHitInfo.eHitSpec == eImage )
        {
            OnClickedImage( fbHitInfo.pItem, fbHitInfo.nItem );
        }
        else
        {
            OnClickedItem( fbHitInfo.pItem, fbHitInfo.nItem , fbHitInfo.nSubItem);
        }

        Invalidate(FALSE);

        return 0;
    }


    LRESULT OnLButtonDblClk( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
    {
        if (::GetFocus() != m_hWnd)
            SetFocus();

        int xPos = GET_X_LPARAM( lParam ); 
        int yPos = GET_Y_LPARAM( lParam );

        FBHITTESTINFO fbHitInfo;
        HitTest( CPoint( xPos, yPos ), &fbHitInfo );

        if( fbHitInfo.nItem < 0 )
        {
            return 0;
        }


        if( fbHitInfo.eHitSpec == eCheckBox ) 
        { 
            BOOL bLastCheckStatus = GetSubItemCheckStatus( fbHitInfo.pItem, fbHitInfo.nSubItem );
            OnClickedCheckbox( fbHitInfo.pItem, fbHitInfo.nItem, fbHitInfo.nSubItem, bLastCheckStatus );
        }
        else if( fbHitInfo.eHitSpec == eImage )
        {
            OnClickedImage( fbHitInfo.pItem, fbHitInfo.nItem );
        }
        else
        {
            OnClickedFlagBtn( fbHitInfo.pItem, fbHitInfo.nItem, m_tree.IsItemExpanded( fbHitInfo.pItem ) );
        }

        Invalidate(FALSE);

        DefWindowProc();

        return 0;
    }


    void DoPaint( CDCHandle dcPaint )
    {

        DefWindowProc( WM_ERASEBKGND, (WPARAM)dcPaint.m_hDC, 0 );
        DefWindowProc( WM_PAINT, (WPARAM)dcPaint.m_hDC, 0 );
        //::DefWindowProc(GetHeader(), WM_ERASEBKGND, (WPARAM)dcPaint.m_hDC, 0 );
        //::DefWindowProc(GetHeader(), WM_PAINT, (WPARAM)dcPaint.m_hDC, 0);

        GetHeader().Invalidate(FALSE);
    }


    virtual void DrawItem( LPDRAWITEMSTRUCT    lpdis )
    {
        CDCHandle  dc			= lpdis->hDC;
        CRect      rcItem		= lpdis->rcItem;
        int        nIndex		= lpdis->itemID;
        HTREELISTITEM pItem		= m_tree.GetVisibleItem( nIndex );
        int        nLayer		= m_tree.GetItemLayer( pItem );
        int        nColumnCount = GetHeader().GetItemCount();
        BOOL       bDisabled    = !IsWindowEnabled();

        if( nIndex == -1 )
        {
            return;
        }

        nLayer--;

        for( int nCol = 0; nCol < nColumnCount; nCol++ ) 
        { 
            CRect  rcTreeUnit;
            GetSubItemRect( nIndex, nCol, LVIR_BOUNDS, &rcTreeUnit );

            //-------------------------------------
            // 刷背景
            {
                int nSavedDC = dc.SaveDC();

                CBrush brushBkground;
                CPen   penBkground;

                if( bDisabled ) 
                {
                    COLORREF crBkground = ::GetSysColor( COLOR_BTNFACE );

                    brushBkground.CreateSolidBrush( crBkground );
                    penBkground.CreatePen( PS_SOLID, 1, crBkground );

                    dc.SelectBrush( brushBkground );
                    dc.SelectPen( penBkground );
                }
                else if( lpdis->itemState & ODS_SELECTED )
                {
                    COLORREF crBkground = m_crSelectedItem;

                    brushBkground.CreateSolidBrush( crBkground );
                    penBkground.CreatePen( PS_SOLID, 1, crBkground );

                    dc.SelectBrush( brushBkground );
                    dc.SelectPen( penBkground );
                }
                else
                {
                    dc.SelectStockPen( WHITE_PEN );
                    dc.SelectStockBrush( WHITE_BRUSH );
                }

                dc.Rectangle( &rcTreeUnit );
                dc.RestoreDC( nSavedDC );
            }


            //-------------------------------------
            // 画线
            if( m_bTreeView && nCol == 0 && m_tree.Size() > 1 )
            {
                int nSavedDC = dc.SaveDC();

                CPoint ptStart;
                CPoint ptEnd;

                const int xFlagBtnCenter = rcTreeUnit.left + m_nTreeIndent * nLayer + m_nTreeIndent / 2;

                {
                    ptStart.x = xFlagBtnCenter;
                    ptStart.y = rcTreeUnit.top + rcTreeUnit.Height() / 2;
                    ptEnd.x   = ptStart.x + m_nTreeIndent;
                    ptEnd.y   = ptStart.y;
                    DrawTreeline( dc, ptStart, ptEnd );
                }

                if( nIndex != 0 )
                {
                    ptStart.x = xFlagBtnCenter;
                    ptStart.y = rcTreeUnit.top;
                    ptEnd.x   = ptStart.x;
                    ptEnd.y   = ptStart.y + ( ( m_tree.GetNextSibling( pItem ) != NULL  ) ? rcTreeUnit.Height() : rcTreeUnit.Height() / 2 );
                    DrawTreeline( dc, ptStart, ptEnd );
                }


                {
                    HTREELISTITEM pParentItem = m_tree.GetParent( pItem );
                    int       xLinePos    = xFlagBtnCenter;
                    while( pParentItem != NULL )
                    {
                        xLinePos   -= m_nTreeIndent;

                        if( m_tree.GetNextSibling( pParentItem ) != NULL )
                        {				
                            ptStart.x = xLinePos;
                            ptStart.y = rcTreeUnit.top;
                            ptEnd.x   = ptStart.x;
                            ptEnd.y   = ptStart.y + rcTreeUnit.Height();
                            DrawTreeline( dc, ptStart, ptEnd );
                        }

                        pParentItem = m_tree.GetParent( pParentItem );
                    }
                }

                dc.RestoreDC( nSavedDC );
            }


            //-------------------------------------
            // 画树展开标记
            // 标记宽度 = CTRL_WIDTH_TREEFLAG
            if( m_bTreeView && nCol == 0 )
            {
                rcTreeUnit.left += nLayer * m_nTreeIndent; 

                int x = ( m_nTreeIndent       - TREE_FLAG_WIDTH ) / 2;
                int y = ( rcTreeUnit.Height() - TREE_FLAG_WIDTH ) / 2;

                int nStatus = m_tree.HasChildren( pItem ) ? 
                    ( m_tree.IsItemExpanded( pItem ) ? TFS_EXPANDED : TFS_RETRACTILE ) : TFS_NOCHILD;

                DrawTreeExpandFlag( 
                    dc, 
                    rcTreeUnit.left + x, 
                    rcTreeUnit.top  + y, 
                    nStatus
                    );

                rcTreeUnit.left += m_nTreeIndent;
            }


            //-------------------------------------
            // 画Checkbox
            // checkbox宽度 = CTRL_WIDTH_CHECKBOX
            if( IsColumnCheckable( nCol) )
            {
                BOOL bChecked = GetSubItemCheckStatus( pItem, nCol );
                int  nStatus  = 0;
                nStatus  = bDisabled ? CBS_DISABLED : CBS_NORMAL;
                nStatus |= ( bChecked ? CBS_CHECKED : 0 );


                int x = ( m_nTreeIndent       - CHECKBOX_WIDTH ) / 2;
                int y = ( rcTreeUnit.Height() - CHECKBOX_WIDTH ) / 2;

                DrawCheckBox( dc, rcTreeUnit.left + x, rcTreeUnit.top + y, nStatus );

                rcTreeUnit.left += m_nTreeIndent;
            }


            //-------------------------------------
            // 画图标
            if( nCol == 0 )
            {
                HICON   hIcon = NULL;
                HBITMAP hBmp = NULL;

                if ( (hIcon = pItem->GetData().GetIcon(pItem, ( lpdis->itemState & ODS_SELECTED ) )) != NULL )
                {
                    int x = rcTreeUnit.left + ( m_nTreeIndent       - m_sizeImage.cx ) / 2;
                    int y = rcTreeUnit.top  + ( rcTreeUnit.Height() - m_sizeImage.cy ) / 2;

                    ::DrawIconEx( dc, x, y, hIcon, 16, 16, 0, NULL, DI_NORMAL);

                    rcTreeUnit.left += 22;
                }
                else if ( (hBmp = pItem->GetData().GetBitmap(pItem, ( lpdis->itemState & ODS_SELECTED ) )) != NULL )
                {
                    int x = rcTreeUnit.left + ( m_nTreeIndent       - m_sizeImage.cx ) / 2;
                    int y = rcTreeUnit.top  + ( rcTreeUnit.Height() - m_sizeImage.cy ) / 2;

                    CDC dcMem;
                    dcMem.CreateCompatibleDC(dc);

                    HBITMAP hOldBmp = dcMem.SelectBitmap( hBmp );

                    dc.BitBlt( x, y, 16, 16, dcMem, 0, 0, SRCCOPY );

                    dcMem.SelectBitmap( hOldBmp );

                    rcTreeUnit.left += 22;
                }
                else
                {
                    CImageList imagelist = GetImageList( LVSIL_SMALL );
                    if( !imagelist.IsNull() )
                    {
                        int nImage = ( lpdis->itemState & ODS_SELECTED ) ? 
                            pItem->GetData().GetImageIndex(pItem, TRUE) : 
                        pItem->GetData().GetImageIndex(pItem, FALSE);


                        if( nImage >= 0 && m_sizeImage.cy <= rcTreeUnit.Height() && m_sizeImage.cx <= rcTreeUnit.Width() )
                        {
                            int x = rcTreeUnit.left + ( m_nTreeIndent       - m_sizeImage.cx ) / 2;
                            int y = rcTreeUnit.top  + ( rcTreeUnit.Height() - m_sizeImage.cy ) / 2;

                            imagelist.Draw( dc, nImage, CPoint( x, y ), ILD_TRANSPARENT  );

                            rcTreeUnit.left += m_nTreeIndent + 6;
                        }
                    }
                }

            }


            //-------------------------------------
            // 显示文字
            // 宽度 = 剩余所有宽度
            {
                dc.SetTextColor( (lpdis->itemState & ODS_SELECTED) ? 
                    GetSysColor(COLOR_HIGHLIGHTTEXT) : GetSysColor(COLOR_WINDOWTEXT));

                ATL::CString strText = pItem->GetData().GetItemText( pItem, nCol );

                dc.DrawText( 
                    strText, 
                    strText.GetLength(), 
                    rcTreeUnit, 
                    DT_VCENTER | DT_LEFT | DT_SINGLELINE | DT_WORD_ELLIPSIS
                    );
            }
        }
    }


    void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
    {

    }
    void DeleteItem( LPDELETEITEMSTRUCT  lpDeleteItemStruct )
    {
        ATLASSERT( FALSE );
    }

protected:
    virtual void OnClickedCheckbox( HTREELISTITEM pItem, int nItem, int nSubItem, BOOL bLastCheckStatus )
    {
        SetSubItemCheckStatus( pItem, nSubItem, !bLastCheckStatus );
        CheckSubBox(pItem, nSubItem, !bLastCheckStatus);
    }

    virtual void OnClickedFlagBtn( HTREELISTITEM pItem, int nItem, BOOL bLastFlagStatus )
    {
        Expand( pItem, !bLastFlagStatus );
    }

    virtual void OnClickedImage( HTREELISTITEM pItem, int nItem )
    {
        SelectItem( nItem );
    }

    virtual void OnClickedItem( HTREELISTITEM pItem, int nItem , int nSubItem)
    {
        SelectItem( nItem );

        if ( pItem != NULL )
            pItem->GetData().OnClicked(pItem, nSubItem);

        SendMessage( GetParent(), WM_SUBITEMCLICKED, (WPARAM)pItem, (LPARAM)nItem );
    }



    //------------------------------------------------------------------------------------------
    //  辅助功能
    //------------------------------------------------------------------------------------------

    void _Init()
    {
        const DWORD dwStyle = LVS_OWNERDRAWFIXED | LVS_OWNERDATA | LVS_REPORT | LVS_SINGLESEL;
        ModifyStyle( 0, dwStyle, 0 );
    }

    void  _Refresh( HTREELISTITEM pFirstItem = NULL, HTREELISTITEM pLastItem = NULL )
    {
        int nFirstItem = ( pFirstItem == NULL ) ? 0  : m_tree.GetVisibleItemIndex( pFirstItem );
        int nLastItem  = ( pLastItem  == NULL ) ? -1 : m_tree.GetVisibleItemIndex( pLastItem );

        if( nFirstItem != -1 )
        {
            _Refresh( nFirstItem, nLastItem );
        }
    }
    void  _Refresh( int nFirstItem = 0, int nLastItem = -1 )
    {
        if( IsWindow() )
        {
            int nItemCount = m_tree.GetVisibleItemCount();
            if( nItemCount == 0 )
            {
                SetItemCount( 0 );
            }
            else
            {
                SetItemCountEx( nItemCount, LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL );

                if( nLastItem == -1 )
                {
                    nLastItem = nItemCount - 1;
                }

                //ATLASSERT( nFirstItem <= nLastItem );

                RedrawItems( nFirstItem, nLastItem );
            }
        }
    }

    void  CheckSubBox(HTREELISTITEM pItem, int nSubItem, BOOL bLastCheckStatus)
    {
        HTREELISTITEM subItem = m_tree.GetFirstChild(pItem);
        if (subItem != NULL)
        {
            CheckSubBox(subItem, nSubItem, bLastCheckStatus);
            SetSubItemCheckStatus(subItem, nSubItem, bLastCheckStatus);

            subItem = m_tree.GetNextSibling(subItem);
            while (subItem)
            {
                CheckSubBox(subItem, nSubItem, bLastCheckStatus);
                SetSubItemCheckStatus(subItem, nSubItem, bLastCheckStatus);
                subItem = m_tree.GetNextSibling(subItem);
            }
        }
    }


protected:
    KExpandableTree  m_tree;
    CSize            m_sizeImage;
    int              m_nTreeIndent;
    COLORREF         m_crSelectedItem;
    
    BOOL             m_bTreeView;


    //------------------------------------------------------------------------------------------
    // 辅助绘图函数
    //------------------------------------------------------------------------------------------
protected:

    /**
    * @brief   画树的连接线( 直线 虚线 )
    * @param   [in] hdc,     目标dc
    * @param   [in] ptStart, 起点
    * @param   [in] ptEnd,   终点
    * @remark  
    */
    template<class T>
    void Swap( T* pT1, T* pT2 )
    {
        T temp = *pT1;
        *pT1   = *pT2;
        *pT2   = temp;
    }


    void DrawTreeline( HDC hdc, POINT ptStart, POINT ptEnd )
    {
        CDCHandle dc( hdc );
        COLORREF  crDot = ::GetSysColor( COLOR_BTNSHADOW );
        const int nStep = 2;


        // 控制所有点都在y为偶数的坐标上 (0 - based)
        if( ptStart.y & 1 ) // 奇数
        {
            ptStart.y++;
            ptEnd.y++;
        }


        if( ptEnd.y - ptStart.y == 0 )  // 水平线
        {
            if( ptEnd.x < ptStart.x )
            {
                Swap( &ptStart, &ptEnd );
            }

            while( ptEnd.x > ptStart.x )  
            {
                dc.SetPixel( ptStart.x, ptStart.y, crDot );
                ptStart.x += nStep;
            }
        }

        else if( ptEnd.x - ptStart.x == 0 ) // 垂直线
        { 
            if( ptEnd.y < ptStart.y )
            {
                Swap( &ptStart, &ptEnd );
            }

            while( ptEnd.y > ptStart.y )  
            {
                dc.SetPixel( ptStart.x, ptStart.y, crDot );
                ptStart.y += nStep;
            }
        }

        else
        {
            ATLASSERT( FALSE && "只画直线" );
        }
    }



};


}; // namespace KSGUI