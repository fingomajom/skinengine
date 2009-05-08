// MenuData.h: interface for the CMenuData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined( _ITREESURF__MENUDATA_H__INCLUDED_ )
#define _ITREESURF__MENUDATA_H__INCLUDED_

#if _MSC_VER > 1000         
#pragma once                    
#endif // _MSC_VER > 1000                  


class CMenuData  
{
public:
	BOOL m_bIni;
	CImageList* m_pImgList;
	CMenuData();
	virtual ~CMenuData();
	int m_nBitmap;
	char *m_szMenuText;
	char *m_szURL;
	//UINT nFlags, nID;
};

BOOL SetMenuText(LPCSTR lpMenuText, CMenuData* md);
BOOL SetURL(LPCSTR lpszURL, CMenuData* md);

#endif // !defined( _ITREESURF__MENUDATA_H__INCLUDED_ )
