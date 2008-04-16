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
    
    virtual BOOL InvalidateParentClient()
    {
        CWindow wndParent = GetParent();
        if (wndParent.m_hWnd == NULL)
            return FALSE;

        RECT rcClient = { 0 };

        GetClientRect(&rcClient);
        ClientToScreen(&rcClient);
        wndParent.ScreenToClient(&rcClient);

        return wndParent.InvalidateRect(&rcClient);
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

        if ( !_tcscmp(szClassName, _T("#32770")) ) // ��һ���Ի���Ļ�
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

public:

    HWND SkinCreate( 
        const SkinXmlElement& xmlElement,
        HWND hWndParent, _U_MENUorID MenuOrID = 0U ) throw()
    {
        BOOL result;

        ATLASSUME(m_hWnd == NULL);

        // Allocate the thunk structure here, where we can fail gracefully.
        result = m_thunk.Init(NULL, NULL);
        if (result == FALSE) {
            SetLastError(ERROR_OUTOFMEMORY);
            return NULL;
        }

        if (T::GetWndClassInfo().m_lpszOrigName == NULL)
            T::GetWndClassInfo().m_lpszOrigName = GetWndClassName();
        ATOM atom = T::GetWndClassInfo().Register(&m_pfnSuperWindowProc);

        _AtlWinModule.AddCreateWndData(&m_thunk.cd, this);

        HWND hWndResult = TBase::SkinCreate(xmlElement, 
            hWndParent,              
            T::GetWndClassInfo().m_wc.lpszClassName,
            MenuOrID);

        return hWndResult;
    }


};



}; // namespace KSGUI