/********************************************************************
* CreatedOn: 2008-1-6   12:08
* FileName:  skinwin.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include <map>

namespace KSGUI{

class SkinWindow : public CWindow
{
public:

    SkinWindow(HWND hWnd = NULL)  
        : CWindow(hWnd)
    {
    }

    virtual ~SkinWindow()  
    {
    }
    
    HWND SkinCreate( 
        const SkinXmlElement& xmlElement,
        HWND hWndParent, LPCTSTR szClassName, _U_MENUorID MenuOrID = 0U ) throw()
    {
        ATLASSUME(m_hWnd == NULL);

        skinxmlwin xmlWin(xmlElement);

        RECT rcClient = rcDefault;
        KSGUI::CString strCaption;

        DWORD dwStyle   = 0;
        DWORD dwExStyle = 0;
        DWORD dwId      = 0;

        GetRealRect(hWndParent, xmlElement, rcClient);
        
        xmlWin.GetCaption(strCaption);

        xmlWin.GetStyle(dwStyle);
        xmlWin.GetExStyle(dwExStyle);

        xmlWin.GetItemId(dwId);

        if (MenuOrID.m_hMenu == 0U)
            MenuOrID.m_hMenu = (HMENU)dwId;

        m_hWnd = ::CreateWindowEx(dwExStyle, szClassName, strCaption,
            dwStyle, rcClient.left, rcClient.top, rcClient.right - rcClient.left,
            rcClient.bottom - rcClient.top, hWndParent, MenuOrID.m_hMenu,
            _AtlBaseModule.GetModuleInstance(), NULL);

        return m_hWnd;
    }


    virtual BOOL UpdateSkinCaption(const SkinXmlElement& xmlElement)
    {
        skinxmlwin xmlWin(xmlElement);

        KSGUI::CString strCaption;

        if (xmlWin.GetCaption(strCaption))
            SetWindowText(strCaption);

        return TRUE;
    }


    virtual BOOL UpdateSkin(const SkinXmlElement& xmlElement)
    {

        return TRUE;
    }
    

    /*  */
    virtual BOOL GetRealRect( HWND hWndParent, const SkinXmlElement& xmlElement, RECT& rcClient )
    {
        if ( !xmlElement.IsValid() )
            return FALSE;

        skinxmlwin xmlWin(xmlElement);

        if (!xmlWin.GetLeft((int&)rcClient.left))
            return FALSE;
        if (!xmlWin.GetTop((int&)rcClient.top))
            return FALSE;

        rcClient.right  = rcClient.left;
        rcClient.bottom = rcClient.top;

        if (xmlWin.GetWidth((int&)rcClient.right))
            rcClient.right += rcClient.left;
        if (xmlWin.GetHeight((int&)rcClient.bottom))
            rcClient.bottom += rcClient.top;

        if ( !::IsWindow( hWndParent ) )
            return TRUE;

        TCHAR szClassName[7] = { 0 };

        GetClassName(hWndParent, szClassName, 7);

        if ( !_tcscmp(szClassName, _T("#32770")) ) // 是一个对话框的话
        {
            MapDialogRect(hWndParent, &rcClient);
        }

        return TRUE;
    }

};

class SkinWindowCreator;

template<class T, class TBase , class WinCreator = SkinWindowCreator>
class SkinWindowImpl : public CWindowImpl<T, TBase>
{
};



}; // namespace KSGUI