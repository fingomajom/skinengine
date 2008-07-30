/********************************************************************
* CreatedOn: 2008-7-17   9:21
* FileName: skingdiex.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

namespace KSGUI{

struct color_conv
{
    typedef enum
    {
        color_red,
        color_green,
        color_blue
    };

public:

    BYTE red;
    BYTE green;
    BYTE blue;

    double lightness;
    double saturation;
    double hue;

    color_conv()
    {
    }

    color_conv(double h, double l, double s)
    {
        hue		   = h;
        lightness  = l;
        saturation = s;
    }

    color_conv(BYTE r, BYTE g, BYTE b)
    {
        red   = r;
        green = g;
        blue  = b;
    }

    color_conv(COLORREF color)
    {
        red   = GetRValue(color);
        green = GetGValue(color);
        blue  = GetBValue(color);
    }

    static unsigned char Value(double m1, double m2, double h)
    {
        while (h >= 360) h -= 360;
        while (h <    0) h += 360;

        if (h < 60) 
            m1 = m1 + (m2 - m1) * h / 60;   
        else if (h < 180) 
            m1 = m2;
        else if (h < 240) 
            m1 = m1 + (m2 - m1) * (240 - h) / 60;      

        return (unsigned char)(m1 * 255);
    }

    // lightness  [0..1]
    // saturation [0..1]
    // hue		  [0..360)
    void ToHLS(void)
    {
        double mn, mx; 
        int	   major;

        if ( red < green )
        {
            mn = red; mx = green; major = color_green;
        }
        else
        {
            mn = green; mx = red; major = color_red;
        }

        if ( blue < mn )
            mn = blue;
        else if ( blue > mx )
        {
            mx = blue; major = color_blue;
        }

        if ( mn==mx ) 
        {
            lightness    = mn/255;
            saturation   = 0;
            hue          = 0; 
        }   
        else 
        { 
            lightness = (mn+mx) / 510;

            if ( lightness <= 0.5 )
                saturation = (mx-mn) / (mn+mx); 
            else
                saturation = (mx-mn) / (510-mn-mx);

            switch ( major )
            {
            case color_red  : hue = (green-blue) * 60 / (mx-mn) + 360; break;
            case color_green: hue = (blue-red) * 60  / (mx-mn) + 120;  break;
            case color_blue : hue = (red-green) * 60 / (mx-mn) + 240;
            }

            if (hue >= 360)
                hue = hue - 360;
        }
    }

    void ToRGB(void)
    {
        if (saturation == 0)
        {
            red = green = blue = (unsigned char) (lightness*255);
        }
        else
        {
            double m1, m2;

            if ( lightness <= 0.5 )
                m2 = lightness + lightness * saturation;  
            else                     
                m2 = lightness + saturation - lightness * saturation;

            m1 = 2 * lightness - m2;   

            red   = Value(m1, m2, hue + 120);   
            green = Value(m1, m2, hue);
            blue  = Value(m1, m2, hue - 120);
        }
    }

    COLORREF GetColorRef(void) const
    {
        return RGB(red, green, blue);
    }
};


inline BOOL AlphaBlend(HDC hDC, HBITMAP hbmpSrc, int x, int y)
{
    BITMAPINFOHEADER bmihDraw = { sizeof(BITMAPINFOHEADER), 0, 0, 1, 24 };
    LPVOID pBitsTemp = NULL, pBitsSrc = NULL;
    HBITMAP hBmpSrcTemp = NULL, hBmpTemp = NULL;
    HDC hMemDC = NULL, hMemDCDraw = NULL;
    HGDIOBJ hOld = NULL, hOldDraw = NULL;
    const BYTE* psrc = NULL;
    BYTE *pdst = NULL;
    BOOL bResult = FALSE;

    BITMAP bmpSrc;

    ::GetObject(hbmpSrc, sizeof(BITMAP), &bmpSrc);

    // 只支持32位位图的混合
    if (bmpSrc.bmBitsPixel != 32)
        goto Exit0;

    hMemDC = ::CreateCompatibleDC(hDC);

    bmihDraw.biWidth = bmpSrc.bmWidth;
    bmihDraw.biHeight = bmpSrc.bmHeight;

    if (bmpSrc.bmBits) // 接受的参数本身就是DIB Section
        pBitsSrc = bmpSrc.bmBits;
    else
    {
        bmihDraw.biBitCount = 32;

        hBmpSrcTemp = ::CreateDIBSection(NULL, (BITMAPINFO *)&bmihDraw, DIB_RGB_COLORS, &pBitsSrc, NULL, NULL);
        if (hBmpSrcTemp == NULL)
            goto Exit0;

        hMemDCDraw = ::CreateCompatibleDC(hDC);

        hOldDraw = ::SelectObject(hMemDCDraw, hbmpSrc);
        hOld = ::SelectObject(hMemDC, hBmpSrcTemp);

        ::BitBlt(hMemDC, 0, 0, bmpSrc.bmWidth, bmpSrc.bmHeight, hMemDCDraw, 0, 0, SRCCOPY);

        ::SelectObject(hMemDC, hOld);
        ::SelectObject(hMemDCDraw, hOldDraw);

        ::DeleteDC(hMemDCDraw);

        bmihDraw.biBitCount = 24;
    }

    // 创建一块临时画布, 供混合图像
    hBmpTemp = ::CreateDIBSection(NULL, (BITMAPINFO *)&bmihDraw, DIB_RGB_COLORS, &pBitsTemp, NULL, NULL);

    if (hBmpTemp == NULL)
        goto Exit0;

    hOld = ::SelectObject(hMemDC, hBmpTemp);

    // 先画背景
    ::BitBlt(hMemDC, 0, 0, bmpSrc.bmWidth, bmpSrc.bmHeight, hDC, x, y, SRCCOPY);

    psrc = (const BYTE *) pBitsSrc;
    pdst = (BYTE *) pBitsTemp;

    // 根据Alpha值混合像素
    for (int nPosY = 0; nPosY < abs(bmpSrc.bmHeight); nPosY ++)
    {
        for (int nPosX = bmpSrc.bmWidth; nPosX > 0; nPosX --)
        {
            BYTE alpha  = psrc[3];
            BYTE salpha = 255 - alpha;

            pdst[0] = (BYTE)((psrc[0] * alpha + pdst[0] * salpha) / 255);
            pdst[1] = (BYTE)((psrc[1] * alpha + pdst[1] * salpha) / 255);
            pdst[2] = (BYTE)((psrc[2] * alpha + pdst[2] * salpha) / 255);

            pdst += 3;
            psrc += 4;
        }

        pdst += ((4 - (bmpSrc.bmWidth * 3) % 4) & 3);
    }

    // 真实绘制
    bResult = ::BitBlt(hDC, x, y, bmpSrc.bmWidth, bmpSrc.bmHeight, hMemDC, 0, 0, SRCCOPY);

Exit0:

    if (hMemDC)
    {
        ::SelectObject(hMemDC, hOld);
        ::DeleteObject(hMemDC);
    }

    if (hBmpTemp)
    {
        ::DeleteObject(hBmpTemp);
    }

    if (hBmpSrcTemp)
        ::DeleteObject(hBmpSrcTemp);

    return bResult;
}

inline BOOL ColorateBitmap(HBITMAP hBitmap, double dHue, double dSaturation, double dLightness)
{
    BOOL bResult = FALSE;
    HDC     hDC = ::GetDC(NULL);
    BITMAP  bmp;
    BITMAPINFOHEADER bmih;
    DWORD   dwLineSize = 0;
    BYTE*   pbyPixels = NULL;
    LONG    lPosX = 0, lPosY = 0;
    RGBQUAD *rgbQuad = NULL;
    WORD *pwPixel = NULL;
    COLORREF crPixel = RGB(0, 0, 0);
    int nRet = 0;

    // 二次Gamma校正参数
    double a, b;

    // 二次Gamma校正必须保证亮度基数在0.25到0.75之间
    dLightness = (dLightness > 0.75) ? 0.75 : ((dLightness < 0.25) ? 0.25 : dLightness);

    a = 2 - 4 * dLightness;
    b = 4 * dLightness - 1;

    ::GetObject(hBitmap, sizeof(BITMAP), &bmp);

    // 只支持32位和16位显示
    if (32 != bmp.bmBitsPixel && 16 != bmp.bmBitsPixel)
        goto Exit0;

    memset(&bmih, 0, sizeof(BITMAPINFOHEADER));

    bmih.biSize = sizeof(BITMAPINFOHEADER);
    bmih.biHeight = bmp.bmHeight;
    bmih.biWidth = bmp.bmWidth;
    bmih.biPlanes = 1;
    bmih.biBitCount = bmp.bmBitsPixel;
    bmih.biCompression = 0;

    // 扫描线长度4字节对齐
    dwLineSize = (((bmp.bmWidth * bmih.biBitCount + 31) >> 5) << 2);

    pbyPixels = new BYTE[dwLineSize * bmp.bmHeight];
    if (!pbyPixels)
        goto Exit0;

    ::ZeroMemory(pbyPixels, dwLineSize * bmp.bmHeight);

    nRet = ::GetDIBits(
        hDC, hBitmap, 0, bmp.bmHeight, pbyPixels, 
        (LPBITMAPINFO)&bmih, DIB_RGB_COLORS
        );
    if (0 == nRet)
        goto Exit0;

    for (lPosY = 0; lPosY < bmp.bmHeight; lPosY ++)
    {
        if (32 == bmp.bmBitsPixel)
            rgbQuad = (RGBQUAD *)(pbyPixels + dwLineSize * lPosY);
        else if (16 == bmp.bmBitsPixel)
            pwPixel = (WORD *)(pbyPixels + dwLineSize * lPosY);

        for (lPosX = 0; lPosX < bmp.bmWidth; lPosX ++)
        {
            if (32 == bmp.bmBitsPixel)
                crPixel = RGB(rgbQuad[lPosX].rgbRed, rgbQuad[lPosX].rgbGreen, rgbQuad[lPosX].rgbBlue);
            else if (16 == bmp.bmBitsPixel)
                crPixel = RGB(
                ((pwPixel[lPosX] & 0x7C00) >> 10) << 3, 
                ((pwPixel[lPosX] & 0x3E0) >> 5) << 3, 
                (pwPixel[lPosX] & 0x1F) << 3
                );

            color_conv kcrPixel(crPixel);

            kcrPixel.ToHLS();

            /*          // 直线Gamma校正 效果真差, 不用了
            kcrPixel.lightness =
            (kcrPixel.lightness > 0.5) ? 
            (1 - 2 * (1 - kcrPixel.lightness) * (1 - dLightness)) : 
            (2 * kcrPixel.lightness * dLightness);
            */

            // 二次Gamma校正
            kcrPixel.lightness = (a * kcrPixel.lightness  + b) * kcrPixel.lightness;
            kcrPixel.lightness = (kcrPixel.lightness < 0) ? 0 : kcrPixel.lightness;
            kcrPixel.lightness = (kcrPixel.lightness > 1) ? 1 : kcrPixel.lightness;

            kcrPixel.saturation = dSaturation;
            kcrPixel.hue = dHue;

            kcrPixel.ToRGB();

            crPixel = kcrPixel.GetColorRef();

            if (32 == bmp.bmBitsPixel)
            {
                rgbQuad[lPosX].rgbRed   = GetRValue(crPixel);
                rgbQuad[lPosX].rgbGreen = GetGValue(crPixel);
                rgbQuad[lPosX].rgbBlue  = GetBValue(crPixel);
            }
            else if (16 == bmp.bmBitsPixel)
            {
                pwPixel[lPosX] = (GetRValue(crPixel) >> 3) << 10;
                pwPixel[lPosX] |= (GetGValue(crPixel) >> 3) << 5;
                pwPixel[lPosX] |= GetBValue(crPixel) >> 3;
            }
        }
    }

    nRet = ::SetDIBits(
        hDC, hBitmap, 0, bmp.bmHeight, pbyPixels, 
        (LPBITMAPINFO)&bmih, DIB_RGB_COLORS
        );
    if (0 == nRet)
        goto Exit0;

    bResult = TRUE;

Exit0:

    ::ReleaseDC(NULL, hDC);

    if (pbyPixels)
    {
        delete[] pbyPixels;
        pbyPixels = NULL;
    }

    return bResult;
}

inline BOOL ColorateBitmap(HBITMAP hBitmap, COLORREF crBase)
{
    color_conv  kcrBase(crBase);

    kcrBase.ToHLS();

    return ColorateBitmap(hBitmap, kcrBase.hue, kcrBase.saturation, kcrBase.lightness);
}



};