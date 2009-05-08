#if !defined( _ITREESURF__TXTDROPTARGET_H__INCLUDED_ )
#define _ITREESURF__TXTDROPTARGET_H__INCLUDED_

#if _MSC_VER > 1000         
#pragma once                    
#endif // _MSC_VER > 1000                  
// TxtDropTarget.h : header file     
//     


#include <afxole.h>

class CTxtDropTarget : public COleDropTarget
{
public:
    CTxtDropTarget()
        {
    //    ms_pCtl=NULL;
        }
 //   BOOL        Register(CComboBoxEx* pComboBox);
    BOOL        OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
    DROPEFFECT  OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    void        OnDragLeave(CWnd* pWnd);
    DROPEFFECT  OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    DROPEFFECT  OnDragScroll(CWnd* pWnd, DWORD dwKeyState, CPoint point);

private:
//    CComboBoxEx*  ms_pCtl;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined( _ITREESURF__TXTDROPTARGET_H__INCLUDED_ )
