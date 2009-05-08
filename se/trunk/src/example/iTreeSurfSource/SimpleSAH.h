// SimpleSAH.h: interface for the CSimpleSAH class.
//
//////////////////////////////////////////////////////////////////////

#if !defined( _ITREESURF__SIMPLESAH_H__INCLUDED_ )
#define _ITREESURF__SIMPLESAH_H__INCLUDED_

#if _MSC_VER > 1000         
#pragma once                    
#endif // _MSC_VER > 1000                  

class CSimpleSAH  
{
public:
	BOOL AddText(CString& text);
	void Close();
	CStdioFile file;
	BOOL LoadHTML(LPCSTR filename, BOOL bUpdate=TRUE);
	BOOL SetAttribute(LPCSTR lpAttriName, LPCSTR lpNewVal);
	CString GetAttribute(LPCSTR lpAttriName);
	BOOL GetNextTags(CStringList& astrTagNames);
	CSimpleSAH();
	virtual ~CSimpleSAH();
	CString strCurTagBuf;


protected:
	CString strHTML;
	long nStartPoint;
	BOOL m_bUpdate;
};

#endif // !defined( _ITREESURF__SIMPLESAH_H__INCLUDED_ )
