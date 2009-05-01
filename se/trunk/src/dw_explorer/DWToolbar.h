

#pragma once

#include "DWImage.h"
#include "DWSkinUIMgt.h"

class CDWToolbar : public CWindowImpl<CDWToolbar>
{
protected:

    struct ToolBtnInfo 
    {
        RECT      rcBtn;
        UINT      uID;
        CDWImage* image;
        ATL::CString strCaption;
    };

public:
    
    CDWToolbar() :
        m_nLeftSpace(0),
        m_nTopSpace(0)
    {
        m_nHotIndex = -1;
    }

    BOOL AddToolBtn( LPCTSTR pszCaption, UINT uID, UINT uResId )
    {
        CDWImage* image = new CDWImage();
        if ( image == NULL )
            return FALSE;
        
        if ( !image->LoadFromResource( uResId ) )
        {
            delete image;
            return FALSE;
        }

        return AddToolBtn( pszCaption, uID, image );
    }

    BOOL AddToolBtn( LPCTSTR pszCaption, UINT uID, CDWImage* image )
    {
        ToolBtnInfo info = { 0 };

        info.uID = uID;
        info.image = image;
        info.strCaption = pszCaption;

        m_vtToolBtn.Add( info );

        return TRUE;
    }

    int GetToolbarWidth() const
    {
        return m_nToolbarWidth;
    }
    int GetToolbarHeigth() const
    {
        return m_nToolbarHeigth;
    }

    

    BEGIN_MSG_MAP(CDWToolbar)

        MESSAGE_HANDLER(WM_CREATE , OnCreate )
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)

        MESSAGE_HANDLER(WM_SIZE   , OnSize   )

        MESSAGE_HANDLER(WM_MOUSEMOVE , OnMouseMove)
        MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)

        MESSAGE_HANDLER(WM_LBUTTONDOWN , OnLButtonDown)
        MESSAGE_HANDLER(WM_LBUTTONUP   , OnLButtonUp)

        MESSAGE_HANDLER(WM_PAINT     , OnPaint      )
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkGnd )

    END_MSG_MAP()

    virtual void RePositionBtns()
    {   
        RECT rcBtn = { m_nLeftSpace, m_nTopSpace, m_nLeftSpace, m_nTopSpace};

        for ( int idx = 0; idx < m_vtToolBtn.GetSize(); idx++ )
        {
            ToolBtnInfo& info = m_vtToolBtn[idx];

            ATLASSERT(info.image);
            if ( info.image == NULL )
                continue;

            rcBtn.right += (info.image->GetWidth()) / 4;
            rcBtn.bottom = m_nTopSpace + info.image->GetHeight();
            
            info.rcBtn = rcBtn;

            rcBtn.left = rcBtn.right;

            m_nToolbarWidth = rcBtn.right;
            m_nToolbarHeigth = rcBtn.bottom;
        }
    }

    virtual void OnFinalMessage(HWND /*hWnd*/)
    {
        for ( int idx = 0; idx < m_vtToolBtn.GetSize(); idx++ )
        {
            ToolBtnInfo& info = m_vtToolBtn[idx];
            
            delete info.image;
        }
    }


    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        RePositionBtns();

        return 1L;
    }

    LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        RePositionBtns();

        return 1L;
    }

    LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
    {
        int xPos = GET_X_LPARAM(lParam); 
        int yPos = GET_Y_LPARAM(lParam); 

        POINT pt = { xPos, yPos };
        
        int nHotPos = -1;

        for ( int idx = 0; idx < m_vtToolBtn.GetSize(); idx++ )
        {
            ToolBtnInfo& info = m_vtToolBtn[idx];

            if ( ::PtInRect(&info.rcBtn, pt) )
            {
                nHotPos = idx;
                break;
            }
        }

        if ( m_nHotIndex != nHotPos )
        {
            CClientDC dc(m_hWnd);
            
            if ( m_nHotIndex >= 0 )
                DrawToolBtn( dc, m_vtToolBtn[m_nHotIndex], 0 );
            else
            {
                TRACKMOUSEEVENT evt;
                evt.cbSize = sizeof(evt);
                evt.dwFlags = TME_LEAVE;
                evt.hwndTrack = m_hWnd;

                TrackMouseEvent( &evt );
            }

            if ( nHotPos >= 0 )
                DrawToolBtn( dc, m_vtToolBtn[nHotPos], 1 );

            m_nHotIndex = nHotPos;
        }

        return 1L;
    }

    LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        if ( m_nHotIndex >= 0 )
        {
            CClientDC dc(m_hWnd);

            DrawToolBtn( dc, m_vtToolBtn[m_nHotIndex], 0 );

            m_nHotIndex = -1;
        }
        
        return 1L;
    }

    LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        if ( m_nHotIndex >= 0 )
        {
            CClientDC dc(m_hWnd);

            DrawToolBtn( dc, m_vtToolBtn[m_nHotIndex], 2 );
            
            m_nClickIndex = m_nHotIndex;
        }


        return 1L;
    }
    LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        if ( m_nHotIndex ==  m_nClickIndex )
        {
            CClientDC dc(m_hWnd);

            DrawToolBtn( dc, m_vtToolBtn[m_nHotIndex], 0 );

            m_nClickIndex = -1;

            ::PostMessage( GetParent(), WM_COMMAND, 
                MAKEWPARAM(m_vtToolBtn[m_nHotIndex].uID, BN_CLICKED), 0 );
        }

        return 1L;
    }

    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return 1L;
    }

    void DrawToolBtn( HDC hDC, ToolBtnInfo& info, int nIdex )
    {
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        RECT rcSrcImage = { 0 };
        
        rcSrcImage.left   = info.image->GetWidth() / 4;
        rcSrcImage.bottom = info.image->GetHeight();

        rcSrcImage.right = rcSrcImage.left * (nIdex + 1);
        rcSrcImage.left  = rcSrcImage.left * nIdex;

        info.image->AlphaDraw( hDC, info.rcBtn.left, info.rcBtn.top, &rcSrcImage, skin.clrFrameWindow);
    }

    LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        CPaintDC dc(m_hWnd);

        for ( int idx = 0; idx < m_vtToolBtn.GetSize(); idx++ )
        {
            ToolBtnInfo& info = m_vtToolBtn[idx];

            ATLASSERT(info.image);
            if ( info.image == NULL )
                continue;
            
            DrawToolBtn( dc, info, 0 );
        }

        return 1L;
    }

    LRESULT OnEraseBkGnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        CDWSkinUIMgt& skin = CDWSkinUIMgt::Instace();

        CDCHandle dc = (HDC)wParam;

        RECT rcClient = { 0 };
        GetClientRect(&rcClient);
        
        dc.FillSolidRect( &rcClient, skin.clrFrameWindow );

        return 0L;
    }
    

    DECLARE_WND_CLASS(_T("DWExplorer_Toolbar"));

protected:

    ATL::CSimpleArray<ToolBtnInfo> m_vtToolBtn;
    
    int m_nLeftSpace;
    int m_nTopSpace;
    int m_nToolbarWidth;
    int m_nToolbarHeigth;

    int m_nHotIndex;
    int m_nClickIndex;
};

