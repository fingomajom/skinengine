// ExternalUtilItem.h: interface for the CExternalUtilItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined( _ITREESURF__EXTERNALUTILITEM_H__INCLUDED_ )
#define _ITREESURF__EXTERNALUTILITEM_H__INCLUDED_

#if _MSC_VER > 1000         
#pragma once                    
#endif // _MSC_VER > 1000                  

class CExternalUtilItem  
{
public:
	CExternalUtilItem();
	virtual ~CExternalUtilItem();

	CString m_strUtilCmd;
	CString m_strUtilName;
	BOOL m_bUtilStart, m_bUtilClose;
	DWORD m_iProcess;
};


class CExternalUtilList  
{
public:
	void Clone(CExternalUtilList* peul);
	void RemoveAll();
	CExternalUtilList();
	virtual ~CExternalUtilList();
	CArray<CExternalUtilItem*, CExternalUtilItem*> m_UtilList;
};

#endif // !defined( _ITREESURF__EXTERNALUTILITEM_H__INCLUDED_ )
