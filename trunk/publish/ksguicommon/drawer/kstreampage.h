/* -------------------------------------------------------------------------
// 文件名		:	KStreamPage.h
// 创建人		:	冰峰
// 创建时间		:	2008-4-14 9:12:50
// 功能描述     :	轻量级带格式文本和图片显示，支持多行、折行和点击测试
//
// $Id: $
// -----------------------------------------------------------------------*/
#ifndef __KSTREAMPAGE_H__
#define __KSTREAMPAGE_H__

#include <vector>
#include <map>

#define MAX_KEY				32								//Key最长字符串
#define Undefine			99999999						//整型Undefine
#define GET_H_ALIGN(v)		(v & 0xFF)
#define GET_V_ALIGN(v)		(v & 0xFF00)
#define UNIT_ASSIGN(lhs, rhs, msk, flg)										\
{																			\
	if (rhs != Undefine)													\
	{																		\
		lhs = rhs;															\
		msk |= flg;															\
	}																		\
}

#pragma warning(push)
#pragma warning (disable : 4267 4995 4018 4996)
//////////////////////////////////////////////////////////////////////////
// ImageList 管理器
class KImageListManager
{
public:
	static KImageListManager* Instance()
	{
		static KImageListManager s_Obj;
		return &s_Obj;
	}

	BOOL AddImageList(LPCTSTR szName, HIMAGELIST hImageList)
	{
		_Iter it = m_Container.find(szName);
		if (it == m_Container.end())
		{
			m_Container[szName] =  _Item(hImageList);
			return TRUE;
		}
		else
		{
			ATLTRACE(L"插入了同名的ImageList：%s", szName);		//需要注意同名不同Image的情况
			it->second.nRef++;									//增加计数
		}

		return FALSE;
	}

	HIMAGELIST GetImageList(LPCTSTR szName)
	{
		_Iter it = m_Container.find(szName);
		ASSERT(it != m_Container.end());

		if (it != m_Container.end())
			return m_Container[szName].hImageList;
		else
			return NULL;
	}

	BOOL RemoveImage(LPCTSTR szName)
	{
		_Iter it = m_Container.find(szName);
		if (it != m_Container.end())
		{
			it->second.nRef--;
			if (it->second.nRef <= 0)
			{
				::ImageList_Destroy(m_Container[szName].hImageList);
				m_Container.erase(szName);
			}
			return TRUE;
		}
		else
		{
			//尝试在Self中找
			_Iter_Bitmap it = m_mapBitmaps.find(szName);
			if (it != m_mapBitmaps.end())
			{
				it->second.nRef--;
				if (it->second.nRef <= 0)
				{
					ASSERT(m_Container.find(it->second.strName) != m_Container.end());
					CImageList ImageList = m_Container[it->second.strName].hImageList;
					ImageList.Remove(it->second.nIndex);
					if (ImageList.GetImageCount() == 0)
						RemoveImage(it->second.strName);

					m_mapBitmaps.erase(it);
				}
				return TRUE;
			}
		}

		ASSERT(FALSE);
		return FALSE;
	}

	BOOL AddBitmap(LPCTSTR szName, HBITMAP hBitmap, int cx = -1, int cy = -1, COLORREF rColor = Undefine)
	{
		BOOL bRet;

		//Prepare
		if (cx == -1 || cy == -1)
		{
			ASSERT(cx == -1 && cy == -1);
			CBitmapHandle objBitmap(hBitmap);
			SIZE size;
			bRet = objBitmap.GetSize(size);
			ASSERT(bRet);

			cx = size.cx;
			cy = size.cy;
		}

		//Normal
		_Iter_Bitmap it = m_mapBitmaps.find(szName);
		if (it != m_mapBitmaps.end())
		{
			ATLTRACE(L"插入了同名的ImageList：%s", szName);		//需要注意同名不同Image的情况
			it->second.nRef++;
		}
		else
		{
			WTL::CString strImageList;
			CImageList ImageList = _GetSameSizeImageList(cx, cy);
			if (ImageList.IsNull())
			{
				bRet = ImageList.Create(cx, cy, ILC_COLOR16 | ILC_MASK, 0, 0);
				ASSERT(bRet);
				strImageList = _MakeIdentityName();
				m_Container[strImageList] = _Item(ImageList, TRUE);
			}
			else
			{
				strImageList = _GetImageListNameByHandle(ImageList);
				ASSERT(!strImageList.IsEmpty());
			}

			// Add Bitmap
			int nIndex;
			if (rColor == Undefine)
				nIndex = ImageList.Add(hBitmap);
			else
				nIndex = ImageList.Add(hBitmap, rColor);

			ASSERT(nIndex != -1);
			m_mapBitmaps[szName] = _BitmapItem(strImageList, nIndex);
		}

		return TRUE;
	}

	int GetImageListByBitmapName(LPCTSTR szName, LPCTSTR *pszImageListName)
	{
		_Iter_Bitmap it = m_mapBitmaps.find(szName);
		ASSERT(it != m_mapBitmaps.end());
		if (it == m_mapBitmaps.end())
			return -1;

		ASSERT(m_Container.find(it->second.strName) != m_Container.end());

		// Have Find
		*pszImageListName = it->second.strName;
		return it->second.nIndex;
	}

private:
	KImageListManager() {};									//禁止外面创建

	inline HIMAGELIST _GetSameSizeImageList(int cx, int cy)
	{
		CSize size;
		_Iter it = m_Container.begin();
		for (; it != m_Container.end(); ++it)
		{
			if (it->second.bIsSelf)
			{
				::ImageList_GetIconSize(it->second.hImageList, (int*)&size.cx, (int*)&size.cy);
				if (size.cx == cx && size.cy == cy)
					return it->second.hImageList;
			}
		}

		return NULL;
	};

	inline LPCTSTR	_GetImageListNameByHandle(HIMAGELIST hImageList)
	{
		_Iter it = m_Container.begin();
		for (; it != m_Container.end(); ++it)
		{
			if (it->second.hImageList == hImageList)
				return it->first;
		}

		return NULL;
	}
	inline WTL::CString _MakeIdentityName()
	{
		WTL::CString str;
		str.Format(_T("_ILM_NAME%d"), m_Container.size());
		return str;
	}

private:
	typedef struct tagBitmapItem
	{
		WTL::CString		strName;
		int					nIndex;
		int					nRef;
		tagBitmapItem() {};
		tagBitmapItem(LPCTSTR _szName, int _nIndex)
		{
			strName	= _szName;
			nIndex	= _nIndex;
			nRef	= 1;
		}
	} _BitmapItem;

	typedef struct tagItem
	{
		bool		bIsSelf;
		HIMAGELIST	hImageList;
		int			nRef;
		tagItem() {};
		tagItem(HIMAGELIST _hImageList, bool _bIsSelf = FALSE)
		{
			hImageList	= _hImageList;
			bIsSelf		= _bIsSelf;
			nRef		= 1;
		}
	} _Item;

	typedef std::map<WTL::CString, _Item>			_Container;
	typedef _Container::iterator					_Iter;
	typedef std::map<WTL::CString, _BitmapItem>		_Map_Bitmap;
	typedef _Map_Bitmap::iterator					_Iter_Bitmap;

	_Container									m_Container;
	_Map_Bitmap									m_mapBitmaps;
};


// 元素类型
typedef enum
{
	ST_NONE			= 0x00000000,
	ST_TEXT			= 0x00000001,
	ST_IMG			= 0x00000002,
	ST_USER_BASE	= 0x0000FFFF,			//其它自定义HitTest类型开始点
} ST_TYPE, HT_TYPE;

// 对齐方式
typedef enum
{
	AT_LEFT			= 0x00000000,
	AT_RIGHT		= 0x00000001,
	AT_CENTRE		= 0x00000002,

	AT_VCENTRE		= 0x00000000,
	AT_TOP			= 0x00000100,
	AT_BOTTOM		= 0x00000200,

	AT_DEFAULT		= AT_LEFT | AT_VCENTRE,
} AT_TYPE;

//////////////////////////////////////////////////////////////////////////
// KStreamPage
template<BOOL T_bSingle = TRUE>
class KStreamPage
{
private:
	enum 
	{ 
		IV_SIZE			= 0x00000001, 
		IV_COLOR		= 0x00000002, 
		IV_BOLD			= 0x00000004, 
		IV_ITALIC		= 0x00000008, 
		IV_UNDERLINE	= 0x00000010,
		IV_CHARSET		= 0x00000020,
		IV_FACENAME		= 0x00000040,
		IV_STRIKE		= 0x00000080,
	};

	typedef struct tagFont
	{
		UINT nMask;
		UINT nSize;
		COLORREF rColor;
		UINT bBold : 1;
		UINT bItalic : 1;
		UINT bUnderline : 1;
		UINT bStrikeOut : 1;
		BYTE lfCharSet;
		TCHAR lfFaceName[32];
	} _Font;
	typedef struct tagSpanText 
	{
		_Font		Font;
		LPTSTR		pszContent;
	} _SpanText;
	
	typedef struct tagImage 
	{
		UINT	nWidth;
		UINT	nHeight;
		
		TCHAR	szImageListId[MAX_KEY];
		UINT	nIndex;
		HBITMAP	hBitmap;
	} _Image;

	typedef struct tagSpan
	{
		ST_TYPE		eSpanType;
		union
		{
			_SpanText	SpanText;
			_Image		Image;
		};
	} _Span;

	typedef struct tagDrawPiece
	{
		UINT		nSpanId;
		INT			nOffset;
		INT			nCount;
		CRect		rcBounds;
		tagDrawPiece() {};
		tagDrawPiece(UINT _nSpanId, RECT *lprc)
		{
			nOffset	 = 0;
			nCount	 = -1;
			nSpanId  = _nSpanId;
			rcBounds = *lprc;
		}
	} _DrawPiece;
	typedef struct tagLine 
	{
		INT						nWidth;
		INT						nHeight;
		std::vector<_DrawPiece>	vecDrawPiece;
		tagLine() { nWidth = 0; nHeight = 0; };
	} _Line;
	
	// 绘制流
	typedef std::vector<_Line>						_DrawStream;
	typedef typename _DrawStream::iterator			_Iter_Draw;

	// 格式流
	typedef std::vector<_Span>						_FormatStream;
	typedef typename _FormatStream::iterator		_Iter_Format;

private:
	BOOL				m_bRended;
	SIZE				m_sizePage;
	INT					m_nIndent;							//缩进(有对齐方式时无效)
	_FormatStream		m_FormatStream;
	KImageListManager*	m_ImageListMgr;

	AT_TYPE				m_eAlignType;
	_DrawStream			m_DrawStream;

	// 
	int					m_nOmitType;						//在此字符前加省略号

public:
	KStreamPage()
	{
		m_sizePage.cx		= INT_MAX;
		m_sizePage.cy		= INT_MAX;
		m_nIndent			= 0;

		m_bRended			= FALSE;
		m_eAlignType		= AT_LEFT;

		m_nOmitType			= 0;
		m_ImageListMgr		= KImageListManager::Instance();
	};

	~KStreamPage()
	{
		Clear();
	};

	//////////////////////////////////////////////////////////////////////////
	// 公用函数
public:

	void SetPageSize(INT nWidth, INT nHeight = INT_MAX)
	{
		m_sizePage.cx = nWidth;
		if (nHeight != INT_MAX)
			m_sizePage.cy = nHeight;

		m_bRended	  = FALSE;
	}

	void SetAlignStyle(UINT AlignType)
	{
		m_eAlignType	= (AT_TYPE)AlignType;
		m_bRended		= FALSE;
	}

	void SetIndent(INT nIndent)
	{
		m_nIndent = nIndent;
		m_bRended = FALSE;
	}

	BOOL AddImage(LPCTSTR szName, HIMAGELIST hImageList)
	{
		return m_ImageListMgr->AddImageList(szName, hImageList);
	}
	BOOL AddImage(LPCTSTR szName, HBITMAP hBitmap, COLORREF rMask = Undefine, int cx = -1, int cy = -1)
	{
		return m_ImageListMgr->AddBitmap(szName, hBitmap, cx, cy, rMask);
	}
	BOOL RemoveImage(LPCTSTR szName)
	{
		return m_ImageListMgr->RemoveImage(szName);
	}

	//////////////////////////////////////////////////////////////////////////
	// 清掉内容，重新开始
	void Clear()
	{
		// 先放掉new出来的东西
		int nCount = m_FormatStream.size();
		for (int i = 0; i < nCount; ++i)
		{
			_Span &span = m_FormatStream[i];
			if (span.eSpanType == ST_TEXT)
				delete span.SpanText.pszContent;
		}

		// 容器
		m_FormatStream.clear();
		m_DrawStream.clear();
	}

	//////////////////////////////////////////////////////////////////////////
	// 返回Index,-1表示失败，貌似没什么可能会失败呢
	int AddTextSpan(LPCTSTR szText, COLORREF rColor = Undefine, INT nSize = Undefine,
					INT bBold = Undefine, INT bItalic = Undefine, INT bUnderline = Undefine, 
					INT bStrikeOut = Undefine, INT lfCharSet = Undefine, LPCTSTR lfFaceName = NULL)
	{
		ASSERT(szText);
		_Span span;
		memset(&span, 0, sizeof(_Span));
		span.eSpanType				= ST_TEXT;
		span.SpanText.Font.nMask	= 0;
		_SetTextSpan(span, szText, rColor, nSize,
					bBold, bItalic, bUnderline, 
					bStrikeOut, lfCharSet, lfFaceName);

		m_FormatStream.push_back(span);
		return (int)m_FormatStream.size() - 1;
	}

	int AddImageSpan(LPCTSTR szImageList, UINT nIndex, UINT nWidth = Undefine, UINT nHeight = Undefine)
	{
		ASSERT(szImageList);
		HIMAGELIST hImageList = m_ImageListMgr->GetImageList(szImageList);
		if (!hImageList)
		{
			ASSERT(!_T("Must Set ImageList First"));
			return -1;
		}

		CSize size;
		ImageList_GetIconSize(hImageList, (int*)&size.cx, (int*)&size.cy);

		// Add
		_Span span;
		span.eSpanType		= ST_IMG;
		span.Image.nWidth	= size.cx;
		span.Image.nHeight	= size.cy;
		_SetImageSpan(span, szImageList, nIndex, nWidth, nHeight);

		m_FormatStream.push_back(span);
		return (int)m_FormatStream.size() - 1;
	}


	//////////////////////////////////////////////////////////////////////////
	// Change Format Piece
	BOOL ChangeTextPiece(int nIndex, LPCTSTR szText = NULL, COLORREF rColor = Undefine, INT nSize = Undefine,
					INT bBold = Undefine, INT bItalic = Undefine, INT bUnderline = Undefine, 
					INT bStrikeOut = Undefine, INT lfCharSet = Undefine, LPCTSTR lfFaceName = NULL)
	{
		ASSERT(nIndex < m_FormatStream.size());
		if (nIndex >= m_FormatStream.size())
			return FALSE;

		_Span &span = m_FormatStream[nIndex];
		ASSERT(span.eSpanType == ST_TEXT);

		return _SetTextSpan(span, szText, rColor, nSize,
					bBold, bItalic, bUnderline, 
					bStrikeOut, lfCharSet, lfFaceName);
	}

	BOOL ChangeImagePiece(UINT nIndex, LPCTSTR szImageList, UINT nImageIndex, UINT nWidth = Undefine, UINT nHeight = Undefine)
	{
		ASSERT(nIndex < m_FormatStream.size());
		if (nIndex >= m_FormatStream.size())
			return FALSE;

		_Span &span = m_FormatStream[nIndex];
		ASSERT(span.eSpanType == ST_IMG);
		return _SetImageSpan(span, szImageList, nImageIndex, nWidth, nHeight);
	}

	//////////////////////////////////////////////////////////////////////////
	// CalcBounder
	BOOL CalcBoundary(SIZE *lpsize)
	{
		CDC dc;
		dc.CreateCompatibleDC();
		lpsize->cx	= m_nIndent;
		lpsize->cy	= 0;
		CSize size;
		int nCount = m_FormatStream.size();
		for (int i = 0; i < nCount; ++i)
		{
			_Span& span = m_FormatStream[i];
			switch(span.eSpanType)
			{
			case ST_TEXT:
				_GetTextExtent(CDCHandle(dc), span.SpanText.pszContent, span.SpanText.Font, size);
				lpsize->cx += size.cx;
				if (size.cy > lpsize->cy)
					lpsize->cy = size.cy;
				break;
			case ST_IMG:
				lpsize->cx += span.Image.nWidth;
				if (size.cy > lpsize->cy)
					lpsize->cy = span.Image.nHeight;
				break;
			default:
				ASSERT(FALSE);
			}
		}

		return TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	// Draw
	BOOL Draw(HDC hdc, RECT* lpRect)
	{
		CDCHandle dc(hdc);
		int nSaveDC = dc.SaveDC();
		
		if (!_MakeSureRender(hdc))							//保证已经排好了
			return FALSE;

		HRGN hRgn = CreateRectRgnIndirect(lpRect);
		dc.SelectClipRgn(hRgn);

#ifdef _DEBUG
// 		CPen _debug_pen;
// 		_debug_pen.CreatePen(0, 1, RGB(255, 0, 255));
// 		HPEN _debug_hpen = dc.SelectPen(_debug_pen);
// 		dc.Rectangle(lpRect->left, lpRect->top, 
// 			lpRect->left + m_sizePage.cx, lpRect->top + 200);
// 		dc.SelectPen(_debug_hpen);
#endif
		//
		CRect rc;
		int nSpanIndex = 0;

		int nLines = m_DrawStream.size();
		for (int i = 0; i < nLines; ++i)
		{
			_Line &line = m_DrawStream[i];
			int nPieces = line.vecDrawPiece.size();
			for (int j = 0; j < nPieces; ++j)
			{
				_DrawPiece &piece = line.vecDrawPiece[j];
				_Span &span = m_FormatStream[piece.nSpanId];
				rc =  piece.rcBounds;
				rc.OffsetRect(lpRect->left, lpRect->top);
				switch(span.eSpanType)
				{
				case ST_TEXT:
					_DrawText(dc, span.SpanText, piece, &rc);
					break;
				case ST_IMG:
					_DrawImage(dc, span.Image, piece, &rc);
				    break;
				default:
					ASSERT(FALSE);
				}

				++nSpanIndex;
			}
		}
		
		// End
		::DeleteObject(hRgn);
		dc.SelectClipRgn(NULL);
		dc.RestoreDC(nSaveDC);
		return TRUE;
	}

	//////////////////////////////////////////////////////////////////////////
	// 暂时只支持单行(多行需要考虑折行)
	ST_TYPE HitTest(int x, int y, int* pIndex = NULL, RECT* lprc = NULL)
	{
		int nPos = 0;
		if (x > m_sizePage.cx || y > m_sizePage.cy)				//超出Page的范围
			return ST_NONE;

		int nLines = m_DrawStream.size();
		for (int i = 0; i < nLines; ++i)
		{
			_Line & line = m_DrawStream[i];
			int nPieces = line.vecDrawPiece.size();
			for (int j = 0; j < nPieces; ++j)
			{
				_DrawPiece &piece =line.vecDrawPiece[j];
				if (piece.rcBounds.PtInRect(CPoint(x, y)))
				{
					//Has Find
					if (pIndex)
						*pIndex = piece.nSpanId;
					if (lprc)
						*lprc = piece.rcBounds;

					return m_FormatStream[piece.nSpanId].eSpanType;
				}
			}
		}

		return ST_NONE;
	}
	ST_TYPE HitTest(POINT pt, int* pIndex = NULL, RECT* lprc = NULL)
	{
		return HitTest(pt.x, pt.y, pIndex, lprc);
	}

	//////////////////////////////////////////////////////////////////////////
	// 取得StreamPage中格式片断的矩形
	ST_TYPE GetPieceInfo(int nIndex, RECT *lprc)
	{
		ASSERT(m_bRended);
		if (nIndex >= m_DrawStream.size())
			return ST_NONE;

		// Normal
		int nLines = m_DrawStream.size();
		for (int i = 0; i < nLines; ++i)
		{
			_Line &line = m_DrawStream[i];
			int nPieces = line.vecDrawPiece.size();
			for (int j = 0; j < nPieces; ++j)
			{
				_DrawPiece piece = line.vecDrawPiece[j];
				if (piece.nSpanId == nIndex)
				{
					if (lprc)
						*lprc = piece.rcBounds;

					return m_FormatStream[nIndex].eSpanType;
				}
			}
		}

		return ST_NONE;
	}

	//////////////////////////////////////////////////////////////////////////
	// 设置省略规则
	BOOL SetOmitType(int nOmitType = 0)
	{
		ASSERT(T_bSingle);
		if (T_bSingle)
		{
			m_nOmitType = nOmitType;
			return TRUE;
		}

		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	// Helper
private:
	BOOL _SetTextSpan(_Span &span, LPCTSTR szText = NULL, COLORREF rColor = Undefine, INT nSize = Undefine,
					INT bBold = Undefine, INT bItalic = Undefine, INT bUnderline = Undefine, 
					INT bStrikeOut = Undefine, INT lfCharSet = Undefine, LPCTSTR lfFaceName = NULL)
	{
		if (szText)
		{
			if (span.SpanText.pszContent)
				delete span.SpanText.pszContent;

			span.SpanText.pszContent	= new TCHAR[_tcsclen(szText) + 1 + 3];
			_tcscpy(span.SpanText.pszContent, szText);
		}

		UINT &nMask	= span.SpanText.Font.nMask;
		UNIT_ASSIGN(span.SpanText.Font.nSize, nSize, nMask, IV_SIZE);
		UNIT_ASSIGN(span.SpanText.Font.rColor, rColor, nMask, IV_COLOR);
		UNIT_ASSIGN(span.SpanText.Font.bBold, bBold, nMask, IV_BOLD);
		UNIT_ASSIGN(span.SpanText.Font.bItalic, bItalic, nMask, IV_ITALIC);
		UNIT_ASSIGN(span.SpanText.Font.bUnderline, bUnderline, nMask, IV_UNDERLINE);
		UNIT_ASSIGN(span.SpanText.Font.bStrikeOut, bStrikeOut, nMask, IV_STRIKE);
		UNIT_ASSIGN(span.SpanText.Font.lfCharSet, lfCharSet, nMask, IV_CHARSET);
		if (lfFaceName != NULL)
		{
			nMask |= IV_FACENAME;
			memcpy(span.SpanText.Font.lfFaceName, lfFaceName, 32);
		}

		return TRUE;
	}

	BOOL _SetImageSpan(_Span &span, LPCTSTR szImageList = NULL, UINT nIndex = Undefine, UINT nWidth = Undefine, UINT nHeight = Undefine)
	{
		span.Image.hBitmap		= NULL;						//暂时未用
		if (szImageList)
			_tcsncpy_s(span.Image.szImageListId, szImageList, MAX_KEY);
		if (nIndex != Undefine)
			span.Image.nIndex	= nIndex;
		if (nWidth != Undefine)
			span.Image.nWidth	= nWidth;
		if (nHeight != Undefine)
			span.Image.nHeight	= nHeight;

		return TRUE;
	}
	
	bool _MakeSureRender(HDC hdc)
	{
		if (m_bRended)
			return TRUE;
		
		CDCHandle dc(hdc);
		CSize size;
		CPoint pt;
		pt.x += m_nIndent ;										//加上缩进
		m_DrawStream.clear();
		m_DrawStream.push_back(_Line());
		_Line* pline = &m_DrawStream.back();

		BOOL bBreakFlag = FALSE;
		int nCount = m_FormatStream.size();
		for (int i = 0; !bBreakFlag && i < nCount; ++i)
		{
			_Span& span = m_FormatStream[i];
			switch(span.eSpanType)
			{
			case ST_TEXT:
				_GetTextExtent(dc, span.SpanText.pszContent, span.SpanText.Font, size);
				if (pt.x + size.cx > m_sizePage.cx)		//一行满了
				{
					if (T_bSingle)								// Single Line
					{
						_DealSingleLine(dc, *pline, i, span.SpanText, pt, size);
						bBreakFlag = TRUE;
						break;
					}
					else										// Multi Line
					{
						pline = _BreakLine(dc, *pline, i, span.SpanText, pt);
						if (!pline)
							bBreakFlag = TRUE;

						break;
					}
				}
				
				// Normal
				pline->vecDrawPiece.push_back(_DrawPiece(i, CRect(pt, size)));
				pline->nWidth += size.cx;
				pt.x += size.cx;
				break;
			case ST_IMG:
				size.cx = span.Image.nWidth;
				size.cy = span.Image.nHeight;
				if (pt.y + size.cy >= m_sizePage.cy)			//列满了
				{
					bBreakFlag = TRUE;
					break;
				}
				else if (pt.x + size.cx > m_sizePage.cx)		//一行满了
				{
					if (T_bSingle)
					{
						pline->vecDrawPiece.push_back(_DrawPiece(i, CRect(pt, CSize(m_sizePage.cx - pt.x, span.Image.nHeight))));
						bBreakFlag = TRUE;
						break;
					}
					else
					{
						if (size.cx > m_sizePage.cx)			//本身容不下一张图片
						{
							bBreakFlag = TRUE;
							break;
						}

						_CalcLineHeight(*pline);
						pt.x  = 0;								//行回0
						pt.y += pline->nHeight;					//加一行高
						if (pt.y <= m_sizePage.cy)
						{
							m_DrawStream.push_back(_Line());
							pline = &m_DrawStream.back();
						}
						else
						{
							bBreakFlag = TRUE;
							break;
						}
					}
				}

				pline->vecDrawPiece.push_back(_DrawPiece(i, CRect(pt, CSize(span.Image.nWidth, span.Image.nHeight))));
				pline->nWidth += size.cx;
				pt.x += span.Image.nWidth;
			    break;
			default:
				ASSERT(FALSE);
			}
		}

		// 计算最后一个行高
		if (pline)
		{
			_CalcLineHeight(*pline);

			// 根据对齐方式调整
			_AdjustAlign();
		}

		m_bRended = TRUE;
		return TRUE;
	}

	void _CalcLineHeight(_Line &line)
	{
		int nMax = 0;
		int nCount = line.vecDrawPiece.size();
		for (int i = 0; i < nCount; ++i)
		{
			int nTemp = line.vecDrawPiece[i].rcBounds.Height();
			if (nTemp > nMax)
				nMax = nTemp;
		}

		line.nHeight = nMax;
	}

	inline void _AdjustAlign()
	{
		int nLines = m_DrawStream.size();
		for (int i = 0; i < nLines; ++i)
		{
			_Line &line = m_DrawStream[i];
			int xOffset = _AdjustX(line);
			int nSpans = line.vecDrawPiece.size();
			for (int j = 0; j < nSpans; ++j)
			{
				CRect &rcItem = line.vecDrawPiece[j].rcBounds;
				rcItem.left   += xOffset;
				rcItem.right  += xOffset;
				int yOffset	  = _AdjustY(line, rcItem.Height());
				rcItem.top	  += yOffset;
				rcItem.bottom += yOffset;
			}
		}
	}
	inline int _AdjustX(_Line &line)
	{
		switch(GET_H_ALIGN(m_eAlignType))
		{
		case AT_LEFT:
			return 0;
		case AT_CENTRE:
			ASSERT(m_sizePage.cx != INT_MAX);			//要使用居中对齐，必须设定PageWidth
			return (m_sizePage.cx - line.nWidth) / 2;
		case AT_RIGHT:
			ASSERT(m_sizePage.cx != INT_MAX);
		    return m_sizePage.cx - line.nWidth;
		default:
		    ASSERT(FALSE);
		}

		return 0;
	}
	inline int _AdjustY(_Line &line, int nHeight)
	{
		switch(GET_V_ALIGN(m_eAlignType))
		{
		case AT_TOP:
			return 0;
		case AT_VCENTRE:
			if (T_bSingle)
			{
				ASSERT(m_sizePage.cy != INT_MAX);
				return (m_sizePage.cy - nHeight) / 2;
			}
			else
				return (line.nHeight - nHeight) / 2;		//对多行来说是基线对齐方式
		case AT_BOTTOM:
			if (T_bSingle)
			{
				ASSERT(m_sizePage.cy != INT_MAX);
				return m_sizePage.cy - nHeight;
			}
			else
				return line.nHeight - nHeight;
		default:
		    ASSERT(FALSE);
		}

		return 0;
	}
	void _DrawText(HDC hdc, _SpanText &SpanText, _DrawPiece &piece, RECT *lprc)
	{
		CDCHandle dc(hdc);		
		HFONT		hFont	= dc.GetCurrentFont();
		COLORREF	rColor	= dc.GetTextColor(); 

		CFont font = _SetFont(hdc, SpanText.Font);

//		WTL::CString str(SpanText.pszContent, piece.nCount == -1 ? _tcslen(SpanText.pszContent) : piece.nCount);
		dc.DrawText(SpanText.pszContent + piece.nOffset, piece.nCount, lprc, m_nOmitType);		//自己算对齐方式

		dc.SelectFont(hFont);
		dc.SetTextColor(rColor);
	}

	void _DrawImage(HDC hdc, _Image &Image, _DrawPiece &piece, RECT *lprc)
	{
		if (Image.hBitmap)						//hBitmap优先
		{
			// 暂时未用
		}
		else
		{
			CImageList ImageList = m_ImageListMgr->GetImageList(Image.szImageListId);
			ASSERT(!ImageList.IsNull());
			if (!ImageList.IsNull())
				ImageList.Draw(hdc, Image.nIndex, lprc->left, lprc->top, ILD_TRANSPARENT);
		}
	}

	inline HFONT _SetFont(HDC hdc, const _Font &font)
	{
		CDCHandle dc(hdc);
		CFontHandle hFont = dc.GetCurrentFont();
		LOGFONT logfont;
		hFont.GetLogFont(&logfont);

		if (font.nMask & IV_COLOR)
			dc.SetTextColor(font.rColor);
		if (font.nMask & IV_SIZE)
		{
			logfont.lfWidth  = 0;
			logfont.lfHeight = ::MulDiv(font.nSize, GetDeviceCaps(dc, LOGPIXELSY), 72);
		}
		if (font.nMask & IV_BOLD)
			logfont.lfWeight = font.bBold ? FW_BOLD : FW_NORMAL;
		if (font.nMask & IV_ITALIC)
			logfont.lfItalic = font.bItalic;
		if (font.nMask & IV_UNDERLINE)
			logfont.lfUnderline = font.bUnderline;

//		memcpy(logfont.lfFaceName, L"宋体", 32);
//		logfont.lfQuality = CLEARTYPE_QUALITY;

		CFont objFont;
		objFont.CreateFontIndirect(&logfont);
		dc.SelectFont(objFont);
		return objFont.Detach();
	}

	inline void _GetTextExtent(CDCHandle &dc, LPCTSTR szText, const _Font &font, SIZE &size)
	{
		HFONT		hFont	= dc.GetCurrentFont();
		COLORREF	rColor	= dc.GetTextColor();
		CFont fontTemp		= _SetFont(dc, font);
		dc.GetTextExtent(szText, -1, &size);
		dc.SelectFont(hFont);
		dc.SetTextColor(rColor);
	}

	inline bool _PtInRect(RECT &rc, int x, int y)
	{
		return	x >= rc.left && x < rc.right &&
				y >= rc.top	 && y < rc.bottom;
	}

	inline bool _DealSingleLine(CDCHandle &dc, _Line &line, int nSpanId, const _SpanText &SpanText, const CPoint &pt, CSize &size)
	{
		int nRemainSpace	= m_sizePage.cx - pt.x;
		size.cx				= nRemainSpace;
		int nCharCount		= _GetWrapCharCount(dc, SpanText.pszContent, SpanText.Font, size);

		_DrawPiece piece;
		piece.nOffset		= 0;
		piece.nCount		= min(_tcslen(SpanText.pszContent), m_nOmitType ? nCharCount + 4 : nCharCount);
		piece.nSpanId		= nSpanId;
		piece.rcBounds		= CRect(pt, CSize(nRemainSpace, size.cy));
		line.vecDrawPiece.push_back(piece);
		line.nWidth			+= nRemainSpace;

		return TRUE;
	}
	inline _Line* _BreakLine(CDCHandle &dc, _Line &line, int nSpanId, _SpanText &SpanText, CPoint &pt)
	{
		_Line *pLine			= &line;
		LPTSTR szPos			= SpanText.pszContent;
		int nCharCountRemain	= _tcslen(SpanText.pszContent);
		int nOffset				= 0;
		CSize size;

		while(TRUE)
		{
			int nRemainSpace	= m_sizePage.cx - pt.x;
			size.cx				= nRemainSpace;
			int nCharCount		= _GetWrapCharCount(dc, szPos, SpanText.Font, size);

			if (pt.y + size.cy > m_sizePage.cy)
			{
				m_DrawStream.pop_back();					//放弃最后一行
				return NULL;
			}

			if (nCharCount == 0)							//窄到一个字符都显示不下
			{
				if (pt.x == 0)								//pt.x = 0都跑到这表示page.size太小了
					return NULL;

				_CalcLineHeight(*pLine);					//在插入新行前计算上一行行高
				pt.y += line.nHeight;
				m_DrawStream.push_back(_Line());
				pLine = &m_DrawStream.back();
				pt.x			= 0;						//下一行回0

				continue;
			}

			// Add
			_DrawPiece piece;
			piece.nOffset		= nOffset;
			piece.nCount		= min(nCharCount, nCharCountRemain);
			piece.nSpanId		= nSpanId;
			piece.rcBounds		= CRect(pt, CSize(nRemainSpace, size.cy));
			pLine->vecDrawPiece.push_back(piece);
			pLine->nWidth		+= nRemainSpace;
			
			nCharCountRemain	-= nCharCount;
			nOffset				+= nCharCount;
			szPos				+= nCharCount;

			if (nCharCountRemain > 0)						//增加一行
			{
				_CalcLineHeight(*pLine);
				pt.y += pLine->nHeight;
				m_DrawStream.push_back(_Line());
				pLine = &m_DrawStream.back();
				pt.x			 = 0;						//下一行回0
			}
			else
			{
				pt.x			 = size.cx;
				break;
			}
		}

		return pLine;
	}
	inline int _GetWrapCharCount(CDCHandle &dc, LPCTSTR szPos, const _Font &font, CSize &size)
	{
		HFONT		hFont	= dc.GetCurrentFont();
		COLORREF	rColor	= dc.GetTextColor();
		CFont fontTemp		= _SetFont(dc, font);

		int nCount = 0;
		int nWidth = 0;
		int nHeight = 0;
		int nRemainWidth = size.cx;

		while (*szPos)
		{
			dc.GetTextExtent(szPos++, 1, &size);
			nRemainWidth -= size.cx;
			if (nRemainWidth <= 0)
				break;

			++nCount;
			nWidth += size.cx;
			nHeight = max(nHeight, size.cy);
		}

		dc.SelectFont(hFont);
		dc.SetTextColor(rColor);

		size.cx = nWidth;
		return nCount;
	}
};

//////////////////////////////////////////////////////////////////////////
// 导出类型
//typedef KStreamPage::ST_TYPE			HT_TYPE;			//点击测试类型
//typedef KStreamPage::AT_TYPE			AT_TYPE;			//对齐方式

typedef KStreamPage<TRUE>								KSingleLineDrawer;
typedef KStreamPage<FALSE>								KMutiLineDrawer;

//////////////////////////////////////////////////////////////////////////
// 创建StreamPage
// KStreamPage* _CreateStreamPage(KStreamPageMgr* pStreamPageMgr)
// {
// 	KStreamPage* pStreamPage = new KStreamPage();
// 	pStreamPage->Init(pStreamPageMgr);
// 	return pStreamPage;
// }

#pragma warning(pop)

// -------------------------------------------------------------------------
// $Log: $

#endif /* __KSTREAMPAGE_H__ */
