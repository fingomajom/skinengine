// iTreeSurfHelper.h: interface for the CiTreeSurfHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined( _ITREESURF__ITREESURFHELPER_H__INCLUDED_ )
#define _ITREESURF__ITREESURFHELPER_H__INCLUDED_

#if _MSC_VER > 1000         
#pragma once                    
#endif // _MSC_VER > 1000                  

#include "iTreeSurfView.h"

//function used for Exp obj to get the views
//LPDISPATCH GetCurrentIDSP(int nFlag = 0);
CiTreeSurfView* GetCurrentView(int nFlag = 0, BOOL bInNewWindow = FALSE);
CiTreeSurfView * CreateNewView();

#endif // !defined( _ITREESURF__ITREESURFHELPER_H__INCLUDED_ )
