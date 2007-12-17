/********************************************************************
* CreatedOn: 2007-12-17   17:08
* FileName: skingdi.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include <atlgdi.h>


namespace KSE{

template <bool t_bManaged>
class CSkinDCT : public CDCT<t_bManaged>
{
public:

    CSkinDCT(HDC hDC = NULL) : CDCT(hDC)
    {
    }

    
    BOOL SkinDrawBitmap( POINT pt, HBITMAP hBmp, DWORD dwRop = SRCCOPY)
    {
        CBitmapHandle bmp = hBmp;
        BITMAP bm = { 0 };

        if (hBmp == NULL || !bmp.GetBitmap(&bm))
            return FALSE;
        
        CDCT<true> dcMem;
        dcMem.CreateCompatibleDC(m_hDC);

        HBITMAP hOldBmp = dcMem.SelectBitmap(hBmp);

        BitBlt(pt.x, pt.y, bm.bmWidth, bm.bmHeight, dcMem, 0, 0, dwRop );

        dcMem.SelectBitmap(hOldBmp);

        return TRUE;
    }

    BOOL SkinDrawBitmap( POINT pt, int nWidth, int nHeight, HBITMAP hBmp, DWORD dwRop = SRCCOPY)
    {
        CBitmapHandle bmp = hBmp;
        BITMAP bm = { 0 };

        if (hBmp == NULL || !bmp.GetBitmap(&bm))
            return FALSE;

        CDCT<true> dcMem;
        dcMem.CreateCompatibleDC(m_hDC);

        HBITMAP hOldBmp = dcMem.SelectBitmap(hBmp);

        BitBlt(pt.x, pt.y, nWidth, nHeight, dcMem, 0, 0, dwRop );

        dcMem.SelectBitmap(hOldBmp);

        return TRUE;
    }


    BOOL SkinDrawBitmap( LPRECT lpRect, HBITMAP hBmp, DWORD dwRop = SRCCOPY)
    {
        CBitmapHandle bmp = hBmp;
        BITMAP bm = { 0 };

        if (hBmp == NULL || !bmp.GetBitmap(&bm))
            return FALSE;

        CDCT<true> dcMem;
        dcMem.CreateCompatibleDC(m_hDC);

        HBITMAP hOldBmp = dcMem.SelectBitmap(hBmp);
        
        StretchBlt( 
            lpRect->left, lpRect->top,
            lpRect->right - lpRect->left, lpRect->bottom - lpRect->top,
            dcMem, 
            0, 0, bm.bmWidth, bm.bmHeight, dwRop);

        dcMem.SelectBitmap(hOldBmp);

        return TRUE;
    }


};

typedef CSkinDCT<false>   CSkinDCHandle;
typedef CSkinDCT<true>    CSkinDC;

} // amespace KSE