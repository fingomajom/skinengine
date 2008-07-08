/* -------------------------------------------------------------------------
// 文件名		:	KFormatDrawer.h
// 创建人		:	冰峰
// 创建时间		:	2008-4-16 9:37:00
// 功能描述     :	
//
// $Id: $
// -----------------------------------------------------------------------*/
#ifndef __KFORMATDRAWER_H__
#define __KFORMATDRAWER_H__

#include "KStreamPage.h"
// -------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// Control动作基类
class Action
{
public:
	typedef enum
	{
		OMR_NoDeal			= 0x00000000,			//未处理
		OMR_HasDeal			= 0x00000001,			//处理了
		OMR_NeedRedraw		= 0x00000002,			//需要重绘
	};
	HWND SetMsgReviecer(HWND hWndReviecer)
 	{
 		ASSERT(!_T("应该在派生类中实现！"));
 		return NULL;
 	}
	int OnMouse(int nCode, const POINT &pt, WPARAM wParam, LPARAM lParam)
	{
		ASSERT(!_T("应该在派生类中实现！"));
		return OMR_NoDeal;
	}
};

//////////////////////////////////////////////////////////////////////////
// KFormatControl 给外面使用的，所谓Control是包括了Draw和Action
template<class TDrawHelper = KSingleLineDrawer, class TPolicy = KDefaultPolicy<TDrawHelper> >
class KFormatControl: public TDrawHelper, public Action
{
public:
	typedef TDrawHelper								TDrawHelper;
	typedef TPolicy									TPolicy;

public:
	KFormatControl()
	{
		m_Policy.SetDrawHelper(this);
	}

	virtual ~KFormatControl()
	{
	}

	//////////////////////////////////////////////////////////////////////////
	// 导出函数
public:

	// 设置显示内容
	BOOL SetRichText(LPCTSTR szContent)
	{
		return m_Policy.SetContent(szContent);
	}


	// Action
	HWND SetMsgReviecer(HWND hWndReviecer)
 	{
 		return m_Policy.SetMsgReviecer(hWndReviecer);
 	}

	int OnMouse(int nCode, const POINT &pt, WPARAM wParam, LPARAM lParam)
	{
		return m_Policy.OnMouse(nCode, pt, wParam, lParam);
	}

	// HitTestEx
	int HitTestEx(const POINT &pt, LPARAM lParam, RECT* lprc = NULL)		//外面不需要放lParam的内存！
	{
		return m_Policy.HitTestEx(pt, lParam, lprc);
	}


private:
	TPolicy				m_Policy;
};

//////////////////////////////////////////////////////////////////////////
// 默认的文字解释类，需要实现Action
template<class TDrawHelper>
class KDefaultPolicy : public Action
{	
public:
	inline BOOL SetDrawHelper(TDrawHelper *pDrawHelper)
	{
		m_pDrawHelper = pDrawHelper;
		return TRUE;
	}
	inline BOOL SetContent(LPCTSTR szContent)
	{
		int nIndex = m_pDrawHelper->AddTextSpan(szContent);
		ASSERT(nIndex != -1);
		return nIndex != -1;
	}

	//////////////////////////////////////////////////////////////////////////
	// HitTestEx
	inline int HitTestEx(POINT pt, LPARAM lParam, RECT* lprc = NULL)
	{
		ASSERT(!_T("未实现！"));
		return ST_NONE;
	}

private:
	TDrawHelper*					m_pDrawHelper;
};

//////////////////////////////////////////////////////////////////////////
// 简单的HTML解析类
#define _SAFE_FIND(exp)														\
{																			\
	if ((exp) == -1)														\
	goto Exit0;																\
}

#define _SAFE_FIND2(exp1, exp2)												\
{																			\
	if ((exp1) == -1)														\
{																			\
	exp2;																	\
	goto Exit0;																\
}																			\
}

//-------------------------------------------------------------------------
typedef enum 
{
	ST_HYPERLINK =				ST_USER_BASE + 1,		//超链接
};

#define WM_USER_HYPERLINK		(WM_USER + 0xFFFE)		//超链接发送的消息id

//////////////////////////////////////////////////////////////////////////
// Html Policy
template<class TDrawHelper = KSingleLineDrawer>
class KSimpleHTMLPolicy : public Action
{
public:
	typedef TDrawHelper											TDrawHelper;

	//////////////////////////////////////////////////////////////////////////
	// 私有变量
private:
	typedef struct tagItem
	{
		int					nType;
		WTL::CString		strName;
	} _Item;
	typedef std::map<UINT, _Item>					_Container;
	typedef typename _Container::iterator			_Iter;

	_Container			m_Container;
	TDrawHelper*		m_pDrawHelper;

	HWND				m_MsgReceiver;				//消息接收者
	HCURSOR				m_OldCursor;
	HCURSOR				m_HandCursor;

	INT					m_bPress : 1;

public:
	KSimpleHTMLPolicy()
	{
		m_MsgReceiver		= NULL;
		m_OldCursor			= NULL;
		m_HandCursor		= ::LoadCursor(NULL, IDC_HAND);

		m_bPress			= FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	// 导出函数
public:
 	inline BOOL SetDrawHelper(TDrawHelper *pDrawHelper)
 	{
 		m_pDrawHelper = pDrawHelper;
 		return TRUE;
 	}
	inline BOOL SetContent(LPCTSTR szContent)
	{
		m_pDrawHelper->Clear();
		return _Parse(szContent);
	}

	//////////////////////////////////////////////////////////////////////////
	// HitTestEx
	inline int HitTestEx(POINT pt, void **ppParam = NULL, RECT* lprc = NULL)
	{
		int nIndex = 0;
		HT_TYPE ht = m_pDrawHelper->HitTest(pt, &nIndex, lprc);
		if (ht != ST_NONE)
		{
			// 给附上元素名称
			_Container::iterator it = m_Container.find(nIndex);
			if (it != m_Container.end())
			{
 				if (ppParam)
 					*ppParam = (void*)(LPCTSTR)it->second.strName;
 				if (it->second.nType == ST_HYPERLINK)
 					return ST_HYPERLINK;
			}
		}

		return ht;
	}


	//////////////////////////////////////////////////////////////////////////
	// Action
	HWND SetMsgReviecer(HWND hWndReviecer)
	{
		HWND hOld = m_MsgReceiver;
		m_MsgReceiver = hWndReviecer;
		return hOld;
	}
	int OnMouse(int nCode, const POINT &pt, WPARAM wParam, LPARAM lParam)
	{
		switch(nCode)
		{
		case WM_MOUSEMOVE:
			return _OnMouseMove(pt, wParam, lParam);
		case WM_LBUTTONDOWN:
			return _OnLButtonDown(pt, wParam, lParam);
		case WM_LBUTTONUP:
			return _OnLButtonUp(pt, wParam, lParam);
		}
		
		return OMR_NoDeal;
	}

	//////////////////////////////////////////////////////////////////////////
	// Helper
private:
	BOOL _Parse(LPCTSTR szText)
	{
		WTL::CString strText = szText;
		WTL::CString strTemp;
		const int nEnd = strText.GetLength();
		int nElementStart = 0;
		int nPos1 = 0, nPos2 = 0;

		//Start
		while (nPos1 < nEnd)
		{
			if (strText.GetAt(nPos1) != '<')					//裸文本
			{
				nPos1 = _ParseText(strText, nPos1);
			}
			else												//HTML元素
			{
				nPos2 = strText.Find(' ', nPos1);				//若一个属性都没有，本句会有问题，先不管，到时改成标准XML
				strTemp = strText.Mid(nPos1 + 1, nPos2 - nPos1 - 1);
				if (strTemp.CompareNoCase(_T("font")) == 0)
					nPos1 = _ParseFont(strText, nPos1);
				else if (strTemp.CompareNoCase(_T("img")) == 0)
					nPos1 = _ParseImage(strText, nPos1);
				else if (strTemp.CompareNoCase(_T("a")) == 0)
					nPos1 = _ParseHyperLink(strText, nPos1);
			}

			++nPos1;											//Next
		}

		return TRUE;
	};

	inline int _ParseText(const WTL::CString& strText, int nStart)
	{
		int nPos1, nPos2;
		nPos1 = nStart;
		nPos2 = strText.Find('<', nPos1);
		if (nPos2 == -1)							//纯文本
			nPos2 = strText.GetLength();

		m_pDrawHelper->AddTextSpan(strText.Mid(nPos1, nPos2 - nPos1));
		return nPos2 - 1;
	}

	inline int _ParseFont(const WTL::CString &strText, int nStart)
	{
		WTL::CString strElement, strTemp;
		int nElementEnd;
		int nPos1, nPos2;
		
		COLORREF	rColor			= Undefine;
		int			nSize			= Undefine;
		UINT		bBold			= Undefine;
		UINT		bItalic			= Undefine;
		UINT		bUnderLine		= Undefine;
		WTL::CString		strID;								//元素的ID

		//属性表
		_SAFE_FIND(nPos1 = strText.Find(' ', nStart));
		_SAFE_FIND(nElementEnd = strText.Find('>', nStart));
		strElement = strText.Mid(nPos1 + 1, nElementEnd - nPos1 - 1);

		nPos1 = 0;
		while (nPos1 != -1)
		{
			nPos2 = strElement.Find('=', nPos1);
			strTemp = strElement.Mid(nPos1, nPos2 - nPos1);

			if (strTemp.CompareNoCase(_T("name")) == 0)
				strID = _GetAttributeString(strElement, nPos1);
			else if (strTemp.CompareNoCase(_T("color")) == 0)
				rColor = _GetAttributeInt(strElement, nPos1);
			else if (strTemp.CompareNoCase(_T("size")) == 0)
				nSize = _GetAttributeInt(strElement, nPos1);
			else if (strTemp.CompareNoCase(_T("bold")) == 0)
				bBold = _GetAttributeInt(strElement, nPos1);
			else if (strTemp.CompareNoCase(_T("italic")) == 0)
				bItalic = _GetAttributeInt(strElement, nPos1);
			else if (strTemp.CompareNoCase(_T("underline")) == 0)
				bUnderLine = _GetAttributeInt(strElement, nPos1);
			else
				ASSERT(FALSE);

			nPos1 = _FindFirstNotChar(strElement, ' ', nPos1);
		}

		//Content
		nPos1 = nElementEnd + 1;
		nPos2 = strText.Find('<', nPos1);
		int nIndex = m_pDrawHelper->AddTextSpan(strText.Mid(nPos1, nPos2 - nPos1), 
										rColor, nSize, bBold, bItalic, bUnderLine);

		if (!strID.IsEmpty())									//元素id对应表
		{
			m_Container[nIndex].nType	= 0;
			m_Container[nIndex].strName = strID;
		}

		return strText.Find('>', nPos2);

	Exit0:
		return -1;
	};

	inline int _ParseImage(const WTL::CString &strText, int nStart)
	{
		WTL::CString strElement, strTemp;
		int nElementEnd;
		int nPos1, nPos2;
		
		WTL::CString strID, strImage;
		INT nImageListIndex = -1;

		//属性表
		_SAFE_FIND(nPos1 = strText.Find(' ', nStart));
		_SAFE_FIND(nElementEnd = strText.Find(_T("/>"), nStart));
		strElement = strText.Mid(nPos1 + 1, nElementEnd - nPos1 - 1);

		nPos1 = 0;
		while (nPos1 != -1)
		{
			nPos2 = strElement.Find('=', nPos1);
			strTemp = strElement.Mid(nPos1, nPos2 - nPos1);

			if (strTemp.CompareNoCase(_T("name")) == 0)
				strID = _GetAttributeString(strElement, nPos1);
			else if (strTemp.CompareNoCase(_T("srcid")) == 0)
				strImage = _GetAttributeString(strElement, nPos1);
			else if (strTemp.CompareNoCase(_T("index")) == 0)
				nImageListIndex = _GetAttributeInt(strElement, nPos1);
			else
				ASSERT(FALSE);

			nPos1 = _FindFirstNotChar(strElement, ' ', nPos1);
		}

		ASSERT(!strImage.IsEmpty());
		if (nImageListIndex == -1)								//单个Bitmap
		{
			LPCTSTR szImageListName;
			KImageListManager *pImageListMgr = KImageListManager::Instance();
			nImageListIndex = pImageListMgr->GetImageListByBitmapName(strImage, &szImageListName);
			ASSERT(nImageListIndex != -1);
			strImage = szImageListName;
		}

		if (nImageListIndex != -1)
		{
			int nIndex = m_pDrawHelper->AddImageSpan(strImage, nImageListIndex);

			if (!strID.IsEmpty())									//元素id对应表
			{
				m_Container[nIndex].nType	= 0;
				m_Container[nIndex].strName = strID;
			}
		}

		return nElementEnd + 1;

	Exit0:
		return -1;
	}

	inline int _ParseHyperLink(const WTL::CString &strText, int nStart)
	{
		WTL::CString strElement, strTemp;
		int nElementEnd;
		int nPos1, nPos2;
		
		COLORREF		rColor			= RGB(0, 0, 255);			//蓝色
		WTL::CString	strID;										//元素的ID

		//属性表
		_SAFE_FIND(nPos1 = strText.Find(' ', nStart));
		_SAFE_FIND(nElementEnd = strText.Find('>', nStart));
		strElement = strText.Mid(nPos1 + 1, nElementEnd - nPos1 - 1);

		nPos1 = 0;
		while (nPos1 != -1)
		{
			nPos2 = strElement.Find('=', nPos1);
			strTemp = strElement.Mid(nPos1, nPos2 - nPos1);

			if (strTemp.CompareNoCase(_T("name")) == 0)
				strID = _GetAttributeString(strElement, nPos1);
			else
				ASSERT(FALSE);

			nPos1 = _FindFirstNotChar(strElement, ' ', nPos1);
		}

		//Content
		nPos1 = nElementEnd + 1;
		nPos2 = strText.Find('<', nPos1);
		int nIndex = m_pDrawHelper->AddTextSpan(strText.Mid(nPos1, nPos2 - nPos1), rColor);

		m_Container[nIndex].nType	= ST_HYPERLINK;
		if (!strID.IsEmpty())									//元素id对应表	
			m_Container[nIndex].strName = strID;

		return strText.Find('>', nPos2);

	Exit0:
		return -1;

	}
	inline int _GetAttributeInt(const WTL::CString &strText, int &nStart)
	{
		WTL::CString strTemp;
		int nPos1, nPos2;
		nPos1 = nStart;
		_SAFE_FIND2(nPos1 = strText.Find('=', nPos1), nStart = -1);
		_SAFE_FIND2(nPos1 = strText.Find('\'', nPos1), nStart = -1);
		_SAFE_FIND2(nPos2 = strText.Find('\'', nPos1 + 1), nStart = -1);

		strTemp = strText.Mid(nPos1 + 1, nPos2 - nPos1 -1); 
		nStart = nPos2 + 1;
		if (strTemp.Find(_T("0x")) != -1)
		{
			int nRes = 0;
			_stscanf_s(strText.Mid(nPos1 + 1, nPos2 - nPos1 -1), _T("%x"), &nRes);
			return RGB(GetBValue(nRes), GetGValue(nRes), GetRValue(nRes));			//把GRB反为RGB
		}
		else
			return _ttoi(strTemp);

Exit0:
		return -1;
	};

	WTL::CString _GetAttributeString(WTL::CString &strText, int &nStart)
	{
		int nPos1, nPos2;
		nPos1 = nStart;
		_SAFE_FIND2(nPos1 = strText.Find('=', nPos1), nStart = -1);
		_SAFE_FIND2(nPos1 = strText.Find('\'', nPos1), nStart = -1);
		_SAFE_FIND2(nPos2 = strText.Find('\'', nPos1 + 1), nStart = -1);

		nStart = nPos2 + 1;
		return strText.Mid(nPos1 + 1, nPos2 - nPos1 -1);

Exit0:
		return _T("");
	};

	inline int _FindFirstNotChar(WTL::CString& strText, TCHAR c, int nStart)
	{
		for (int i = nStart; i < strText.GetLength(); ++i)
		{
			if (strText.GetAt(i) != c)
				return i;
		}

		return -1;
	};


	inline int _OnMouseMove(const POINT &pt, WPARAM wParam, LPARAM lParam)
	{
		static int s_nLastHyperLink = -1;
		int nRet = OMR_NoDeal;
		CRect rc;
		int ht = HitTestEx(pt, NULL, &rc);
		if (ht == ST_HYPERLINK)
		{
			m_OldCursor = ::SetCursor(m_HandCursor);

			//Add Underline
			int nIndex = 0;
			m_pDrawHelper->HitTest(pt, &nIndex);
			m_pDrawHelper->ChangeTextPiece(nIndex, NULL, Undefine, Undefine, Undefine, Undefine, TRUE);
			s_nLastHyperLink = nIndex;
			nRet = OMR_HasDeal | OMR_NeedRedraw;
		}
 		else if (m_OldCursor != NULL)
 		{
			::SetCursor(m_OldCursor);

			// Remove underline
			if (s_nLastHyperLink != -1)
			{
				m_pDrawHelper->ChangeTextPiece(s_nLastHyperLink, NULL, Undefine, Undefine, Undefine, Undefine, FALSE);
				s_nLastHyperLink = -1;
				nRet = OMR_HasDeal | OMR_NeedRedraw;
			}
 		}

		return nRet;
	}
	inline int _OnLButtonDown(const POINT &pt, WPARAM wParam, LPARAM lParam)
	{
		if (!m_MsgReceiver)
			return OMR_NoDeal;

		m_bPress = TRUE;
		return OMR_HasDeal;
	}
	inline int _OnLButtonUp(const POINT &pt, WPARAM wParam, LPARAM lParam)
	{
		if (!m_MsgReceiver)
			return OMR_NoDeal;

// 		if (!m_bPress)
// 			return OMR_NoDeal;

//		m_bPress = FALSE;

		// Normal
		CRect rc;
		LPCTSTR szName = NULL;
		int ht;
		if (lParam)
			ht = HitTestEx(pt);
		else
		{
			ht = HitTestEx(pt, (void**)&szName);
			lParam = (LPARAM)szName;
		}

		if (ht == ST_HYPERLINK)
		{
			::SendMessage(m_MsgReceiver, WM_USER_HYPERLINK, wParam, lParam);
			// 要不要变色？to do...
			return OMR_HasDeal;
		}

		return OMR_NoDeal;
	}

};


//////////////////////////////////////////////////////////////////////////
// Typedef
typedef KFormatControl<KSingleLineDrawer, KDefaultPolicy<KSingleLineDrawer> >		KDefaultControl;
typedef KFormatControl<KSingleLineDrawer, KSimpleHTMLPolicy<KSingleLineDrawer> >	KSingleLineHTMLControl;		//单行
typedef KFormatControl<KMutiLineDrawer, KSimpleHTMLPolicy<KMutiLineDrawer> >		KMutilineHTMLControl;		//支持折行

// -------------------------------------------------------------------------
// $Log: $

#endif /* __KFORMATDRAWER_H__ */
