

#pragma once

#include "DWGDI.h"

class CDWImage : public CImage
{
public:

    BOOL LoadFromResource( UINT uResId )
    {
        BOOL bResult = FALSE;

        HINSTANCE hInst = _Module.GetResourceInstance();
        IStream*  pstm  = NULL;

        HRSRC hRsrc = ::FindResource( _Module.GetResourceInstance(), 
            MAKEINTRESOURCE(uResId), _T("PNG") );
        if (!hRsrc)
            return bResult;

        DWORD dwResLen = SizeofResource( hInst, hRsrc );
        BYTE* lpRsrc   = (BYTE*)LoadResource( hInst, hRsrc );
        if (!lpRsrc)
            return bResult;

        HGLOBAL hMem = GlobalAlloc(GMEM_FIXED, dwResLen);
        BYTE* pmem = (BYTE*)GlobalLock(hMem);
        if ( hMem != NULL && pmem != NULL )
        {
            memcpy(pmem, lpRsrc, dwResLen);

            CreateStreamOnHGlobal(hMem, FALSE, &pstm);

            if ( SUCCEEDED( Load(pstm) ) )
                bResult = TRUE;

            GlobalUnlock(hMem);
            pstm->Release(); 
        }

        FreeResource(lpRsrc);

        return bResult;
    }

    void AlphaDraw( HDC hDestDC, int xPos, int yPos, LPRECT prcSrc, int nFlags = 0, float fAlpha=1.0f )
    {
        RECT rcSRC = { 0 };

        if ( CImage::IsNull() )
            return;

        if ( prcSrc != NULL )
        {
            rcSRC = *prcSrc;
        }
        else
        {
            rcSRC.right  = GetWidth();
            rcSRC.bottom = GetHeight();
        }

        CImage image;
        image.Create( rcSRC.right - rcSRC.left, rcSRC.bottom - rcSRC.top, GetBPP() );

        ::BitBlt( image.GetDC(), 
            0, 0, 
            rcSRC.right - rcSRC.left, 
            rcSRC.bottom - rcSRC.top, 
            hDestDC,
            xPos,
            yPos,
            SRCCOPY);
        image.ReleaseDC();

        BYTE dr,dg,db;
        BYTE sr,sg,sb;

        for ( int y = rcSRC.top; y < rcSRC.bottom; y++ )
        {
            LPBYTE pSrcBuf = (LPBYTE)GetPixelAddress( 0, y );
            LPBYTE pDesBuf = (LPBYTE)image.GetPixelAddress( 0, y - rcSRC.top );

            for ( int x = rcSRC.left; x < rcSRC.right; x++)
            {
                int nSPos = (x) * 4;
                int nDPos = (x - rcSRC.left) * 4;

                float ff = ((float)pSrcBuf[nSPos+3] / 255) / fAlpha;

                dr = pDesBuf[nDPos];
                dg = pDesBuf[nDPos+1];
                db = pDesBuf[nDPos+2];

                switch (nFlags)
                {
                case 1:
                    sr = sg = sb = ((pSrcBuf[nSPos] + pSrcBuf[nSPos+1] + pSrcBuf[nSPos+2])/3);
                    break;
                case 0:
                default:
                    sr = pSrcBuf[nSPos];
                    sg = pSrcBuf[nSPos+1];
                    sb = pSrcBuf[nSPos+2];
                }


                pDesBuf[nDPos]   = BYTE((1-ff)*dr+(ff)*sg);
                pDesBuf[nDPos+1] = BYTE((1-ff)*dg+(ff)*sg);
                pDesBuf[nDPos+2] = BYTE((1-ff)*db+(ff)*sb);
                pDesBuf[nDPos+3] = 0;
            }
        }

        image.Draw( hDestDC, xPos, yPos );
    }


    void AlphaDraw( HDC hDestDC, int xPos, int yPos, LPRECT prcSrc, COLORREF clrDest, int nFlags = 0, float fAlpha=1.0f )
    {
        RECT rcSRC = { 0 };

        if ( CImage::IsNull() )
            return;

        if ( prcSrc != NULL )
        {
            rcSRC = *prcSrc;
        }
        else
        {
            rcSRC.right  = GetWidth();
            rcSRC.bottom = GetHeight();
        }

        CImage image;
        image.Create( rcSRC.right - rcSRC.left, rcSRC.bottom - rcSRC.top, GetBPP() );

        ::BitBlt( image.GetDC(), 
            0, 0, 
            rcSRC.right - rcSRC.left, 
            rcSRC.bottom - rcSRC.top, 
            hDestDC,
            xPos,
            yPos,
            SRCCOPY);
        image.ReleaseDC();

        BYTE dr,dg,db;
        BYTE sr,sg,sb;
        BYTE ar,ag,ab;

        ar = GetRValue(clrDest);
        ag = GetGValue(clrDest);
        ab = GetBValue(clrDest);

        for ( int y = rcSRC.top; y < rcSRC.bottom; y++ )
        {
            LPBYTE pSrcBuf = (LPBYTE)GetPixelAddress( 0, y );
            LPBYTE pDesBuf = (LPBYTE)image.GetPixelAddress( 0, y - rcSRC.top );

            for ( int x = rcSRC.left; x < rcSRC.right; x++)
            {
                int nSPos = (x) * 4;
                int nDPos = (x - rcSRC.left) * 4;

                float ff = ((float)pSrcBuf[nSPos+3] / 255) / fAlpha;

                dr = pDesBuf[nDPos];
                dg = pDesBuf[nDPos+1];
                db = pDesBuf[nDPos+2];

                switch (nFlags)
                {
                case 1:
                    sr = sg = sb = (pSrcBuf[nSPos] + pSrcBuf[nSPos+1] + pSrcBuf[nSPos+2])/3;
                    break;
                case 0:
                default:
                    sr = pSrcBuf[nSPos];
                    sg = pSrcBuf[nSPos+1];
                    sb = pSrcBuf[nSPos+2];
                }

                sr = ( sr + ar ) / 2;
                sg = ( sg + ag ) / 2;
                sb = ( sb + ab ) / 2;

                pDesBuf[nDPos]   = BYTE((1-ff)*dr+(ff)*sg);
                pDesBuf[nDPos+1] = BYTE((1-ff)*dg+(ff)*sg);
                pDesBuf[nDPos+2] = BYTE((1-ff)*db+(ff)*sb);
                pDesBuf[nDPos+3] = 0;
            }
        }

        image.Draw( hDestDC, xPos, yPos );
    }

};