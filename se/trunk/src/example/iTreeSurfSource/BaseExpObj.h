// BaseExpObj.h: interface for the CBaseExpObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined( _ITREESURF__BASEEXPOBJ_H__INCLUDED_ )
#define _ITREESURF__BASEEXPOBJ_H__INCLUDED_

#if _MSC_VER > 1000         
#pragma once                    
#endif // _MSC_VER > 1000                  

#include "iTreeSurfHelper.h"

//the base object of explore object.
class CBaseExpObj  
{
public:
	virtual BOOL Update(int nLevel) =0;
	virtual void Destroy() =0;
	virtual BOOL Initialize() =0;
	virtual BOOL Create(CWnd* pParent) =0;
	virtual CWnd* GetWindow() =0;
	virtual LPCSTR GetTitle() = 0;
	CBaseExpObj();
	virtual ~CBaseExpObj();

};

#endif // !defined( _ITREESURF__BASEEXPOBJ_H__INCLUDED_ )
