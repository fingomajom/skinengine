
#pragma once

#include "DWToolbar.h"


class CDWSysBtnToolBar : public CDWToolbar
{

public:
    CDWSysBtnToolBar() :
        m_image(NULL)
    {
    }

    BEGIN_MSG_MAP(CDWMainFrame)

        MESSAGE_HANDLER(WM_CREATE , OnCreate )

        CHAIN_MSG_MAP(CDWToolbar)

    END_MSG_MAP()

    virtual void OnFinalMessage(HWND hWnd)
    {
        CDWToolbar::OnFinalMessage(hWnd);
        if ( m_image != NULL )
            delete m_image;
    }

    void OnSwitchBtn()
    {
        BOOL bMaxed = ::IsZoomed( GetParent() );        
        ToolBtnInfo& info = m_vtToolBtn[1];
        CDWImage * p = m_image;

        if ( bMaxed )
        {
            if ( info.uID != SC_RESTORE )
            {
                info.uID = SC_RESTORE;
                m_image = info.image;
                info.image = p;
            }
        }
        else
        {
            if ( info.uID != SC_MAXIMIZE )
            {
                info.uID = SC_MAXIMIZE;
                m_image = info.image;
                info.image = p;
            }
        }
    }


    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        AddToolBtn( _T(""), SC_MINIMIZE, IDR_PNG_BTN_SYS_MIN);
        AddToolBtn( _T(""), SC_MAXIMIZE, IDR_PNG_BTN_SYS_MAX);
        //AddToolBtn( _T(""), SC_RESTORE , IDR_PNG_BTN_SYS_MAX2);
        AddToolBtn( _T(""), SC_CLOSE   , IDR_PNG_BTN_SYS_CLOSE);
        
        m_image = new CDWImage;
        if ( m_image )
            m_image->LoadFromResource(IDR_PNG_BTN_SYS_MAX2);

        bHandled = FALSE;
        return 1L;
    }


    CDWImage* m_image;
};