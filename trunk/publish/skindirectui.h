/********************************************************************
* CreatedOn: 2008-4-6   17:41
* FileName: skindirectui.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once 

#include "skinxmlwin.h"


namespace KSGUI{


class duidrawbase
{

public:

    virtual ~duidrawbase() {}

    virtual BOOL initduidraw( skinxmlwin& xmlwin )
    {

    #ifdef _DIRECTUI_USE_IDNAME_
        xmlwin.GetIdName(strIdName);
    #endif

        if ( !xmlwin.GetLeft  ( (int&)rcClient.left   ) ) return FALSE;
        if ( !xmlwin.GetTop   ( (int&)rcClient.top    ) ) return FALSE;
        if ( !xmlwin.GetWidth ( (int&)rcClient.right  ) ) return FALSE;
        if ( !xmlwin.GetHeight( (int&)rcClient.bottom ) ) return FALSE;

        rcClient.right  += rcClient.left;
        rcClient.bottom += rcClient.top;

        return TRUE;
    }

    virtual void drawdc( HDC hDC ) = 0;

protected:

#ifdef _DIRECTUI_USE_IDNAME_
    KSG::CString strIdName;
#endif

    RECT rcClient;

};

class skinduidrawtext : public duidrawbase
{

public:

    skinduidrawtext()
    {
        clrTextColor = 0x0L;
        dwDrawFlag   = DT_LEFT | DT_TOP;
    }
    
    virtual BOOL initduidraw( skinxmlwin& xmlwin )
    {
        if (!duidrawbase::initduidraw(xmlwin))
            return FALSE;

        if ( !xmlwin.GetCaption( strCaption ) ) return FALSE;
        if ( !xmlwin.GetFont( hFont.m_hFont ) ) {};
        if ( !xmlwin.GetTextColor( clrTextColor ) ) {};
        if ( !xmlwin.GetObject(_T("DrawTextFlags"), dwDrawFlag ) ) {};

        return TRUE;
    }

    virtual void drawdc( HDC hDC )
    {
        CSkinDCHandle dc(hDC);

        HFONT hOldFont = NULL;        

        COLORREF clrOld = dc.SetTextColor(clrTextColor);

        if (hFont.m_hFont != NULL)
            hOldFont = dc.SelectFont(hFont);

        dc.DrawText( strCaption, -1, &rcClient, dwDrawFlag );

        if (hFont.m_hFont != NULL)
            dc.SelectFont(hOldFont);

        dc.SetTextColor(clrOld);
    }

protected:
    
    KSGUI::CString strCaption;
    CFont          hFont;
    COLORREF       clrTextColor;
    DWORD          dwDrawFlag;
};

class skinduidrawbmp : public duidrawbase
{

public:

    skinduidrawbmp()
    {
    }

    virtual BOOL initduidraw( skinxmlwin& xmlwin )
    {
        if (!duidrawbase::initduidraw(xmlwin))
            return FALSE;

        KSGUI::CString strBitbmpId;

        if ( !xmlwin.GetObject(_T("Bitbmp"), strBitbmpId ) ) return FALSE;

        m_bmp.Attach( SkinLoadBitmap(strBitbmpId) );

        if (m_bmp.m_hBitmap == NULL)
            return FALSE;

        return TRUE;
    }

    virtual void drawdc( HDC hDC )
    {
        CSkinDCHandle dc(hDC);

        POINT pt = { rcClient.left, rcClient.top };
        
        dc.SkinDrawBitmap(pt, m_bmp);
    }

protected:

    CBitmap m_bmp;
};

class skinduidrawrect : public duidrawbase
{

public:

    skinduidrawrect()
    {
        nPenStyle   = PS_SOLID;
        nPenWidth   = 1;
        nRoundWidth = 0;

        clrBkGnd     = 0x00FFFFFFL;
        clrTextColor = 0x0L;
    }

    virtual BOOL initduidraw( skinxmlwin& xmlwin )
    {
        if (!duidrawbase::initduidraw(xmlwin))
            return FALSE;

        KSGUI::CString strBitbmpId;

        if ( !xmlwin.GetTextColor( clrTextColor ) ) {};
        if ( !xmlwin.GetBkColor( clrBkGnd ) ) {};

        if ( !xmlwin.GetObject(_T("PenStyle")  , nPenStyle ) ) {};
        if ( !xmlwin.GetObject(_T("PenWidth")  , nPenWidth ) ) {};
        if ( !xmlwin.GetObject(_T("RoundWidth"), nRoundWidth ) ) {};

        return TRUE;
    }

    virtual void drawdc( HDC hDC )
    {
        CSkinDCHandle dc(hDC);

        CPen   pen;
        CBrush brush;
        
        pen.CreatePen(nPenStyle, nPenWidth, clrTextColor);
        brush.CreateSolidBrush(clrBkGnd);

        HPEN hOldPen = dc.SelectPen(pen);
        HBRUSH hBrush = dc.SelectBrush(brush);

        if (nRoundWidth == 0)
        {
            dc.Rectangle(&rcClient);
        }
        else
        {
            POINT pt = { nRoundWidth, nRoundWidth }; 

            dc.RoundRect(&rcClient, pt);
        }

        dc.SelectBrush(hBrush);
        dc.SelectPen(hOldPen);
    }

protected:

    int nPenStyle;
    int nPenWidth;

    int nRoundWidth;

    COLORREF clrBkGnd;
    COLORREF clrTextColor;
};



class skindirectui
{
public:
    
    duidrawbase* getdirectui( LPCTSTR pszclsname )
    {
        if (!_tcscmp(pszclsname, _T("skinduidrawtext")))
        {
            return new skinduidrawtext();
        }
        else if(!_tcscmp(pszclsname, _T("skinduidrawbmp")))
        {
            return new skinduidrawbmp();
        }
        else if(!_tcscmp(pszclsname, _T("skinduidrawrect")))
        {
            return new skinduidrawrect();
        }

        return NULL;
    }

    bool createdirectui( skinxmlwin& xmlwin )
    {
        KSGUI::CString strSkinClassName;

        if (!xmlwin.GetSkinClassName(strSkinClassName))
            return false;

        duidrawbase* pbase = getdirectui( strSkinClassName );
        if (pbase == NULL)
            return false;
        
        if (!pbase->initduidraw(xmlwin))
        {
            delete pbase;
        }
        else
        {
            vtdrawlist.push_back(pbase);
        }

        return true;
    }

    void drawdirectui( HDC hDC )
    {
        for (size_t idx = 0; idx < vtdrawlist.size(); idx++)
        {
            duidrawbase* pbase = vtdrawlist[idx];
            ATLASSERT(pbase != NULL);
            if (pbase == NULL)
                continue;
            
            pbase->drawdc(hDC);
        }

    }

    ~skindirectui()
    {
        free();
    }

    void free()
    {
        for (size_t idx = 0; idx < vtdrawlist.size(); idx++)
        {
            duidrawbase* pbase = vtdrawlist[idx];
            ATLASSERT(pbase != NULL);
            if (pbase == NULL)
                continue;

            delete pbase;
        }

        vtdrawlist.clear();
    }

    
    std::vector<duidrawbase*> vtdrawlist;
};

} // namespace KSGUI
