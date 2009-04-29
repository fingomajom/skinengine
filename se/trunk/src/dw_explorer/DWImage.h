

#pragma once


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

    void AlphaDraw( HDC hDestDC, int xPos, int yPos, LPRECT prcSrc, COLORREF clrAlpha )
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

        BYTE bb = GetBValue(clrAlpha);
        BYTE bg = GetGValue(clrAlpha);
        BYTE br = GetRValue(clrAlpha);

        for ( int y = rcSRC.top; y < rcSRC.bottom; y++ )
        {
            LPBYTE pSrcBuf = (LPBYTE)GetPixelAddress( 0, y );
            LPBYTE pDesBuf = (LPBYTE)image.GetPixelAddress( 0, y - rcSRC.top );

            for ( int x = rcSRC.left; x < rcSRC.right; x++)
            {
                int nSPos = (x) * 4;
                int nDPos = (x - rcSRC.left) * 4;

                BYTE ab = pSrcBuf[nSPos];
                BYTE ag = pSrcBuf[nSPos+1];
                BYTE ar = pSrcBuf[nSPos+2];
                BYTE aa = pSrcBuf[nSPos+3];

                float ff = ((float)aa / 255);

                pDesBuf[nDPos]   = BYTE((1-ff)*bb+(ff)*ab);
                pDesBuf[nDPos+1] = BYTE((1-ff)*bg+(ff)*ag);
                pDesBuf[nDPos+2] = BYTE((1-ff)*br+(ff)*ar);
                pDesBuf[nDPos+3] = 0;
            }
        }

        image.Draw( hDestDC, xPos, yPos );

    }

};