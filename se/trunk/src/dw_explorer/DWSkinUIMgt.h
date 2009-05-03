

#pragma once


struct CDWSkinUIMgt
{
public:

    static CDWSkinUIMgt& Instace()
    {
        static CDWSkinUIMgt __CDWSkinUIMgt_Instance__;
        return __CDWSkinUIMgt_Instance__;
    }
        
    CDWSkinUIMgt()
    {
        clrFrameNcBorder = RGB( 90 , 90, 90);
        clrFrameWindow   = RGB( 196, 80, 220 );

        fontDefault.CreatePointFont( 90, L"Tahoma" );
    }

    COLORREF clrFrameNcBorder;
    COLORREF clrFrameWindow;
    CFont    fontDefault;
};