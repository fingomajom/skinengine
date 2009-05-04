

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

        iconNull.LoadIcon(IDI_ICON_FAV_NULL_URL, 0, 0, LR_DEFAULTSIZE);
        iconFavDir.LoadIcon(IDI_ICON_FAV_DIRECTORY, 0, 0, LR_DEFAULTSIZE);

    }

    COLORREF clrFrameNcBorder;
    COLORREF clrFrameWindow;
    CFont    fontDefault;

    CIcon    iconFavDir;
    CIcon    iconNull;


};