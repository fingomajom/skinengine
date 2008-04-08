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

    virtual BOOL initduidraw( SkinXmlElement& element )
    {
        skinxmlwin xmlwin(element);

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

    RECT rcClient;

};

class duidrawtext
{

public:

    duidrawtext()
    {
        clrTextColor = 0x0L;
        dwDrawFlag   = DT_LEFT | DT_TOP;
    }
    
    virtual BOOL initduidraw( SkinXmlElement& element )
    {
        if (!duidrawbase::initduidraw(element))
            return FALSE;

        if ( !xmlwin.GetLeft  ( (int&)rcClient.left   ) ) return FALSE;


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
    
    KSG::CString strCaption;
    CFont        hFont;
    COLORREF     clrTextColor;
    DWORD        dwDrawFlag;
};


class skindirectui
{
public:
    

    
};

} // namespace KSGUI
