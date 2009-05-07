

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
        clrFrameWindow   = RGB( 60, 130, 255 );

        fontDefault.CreatePointFont( 90, L"Tahoma" );

        iconNull.LoadIcon(IDI_ICON_FAV_NULL_URL, 0, 0, LR_DEFAULTSIZE);
        iconFavDir.LoadIcon(IDI_ICON_FAV_DIRECTORY, 0, 0, LR_DEFAULTSIZE);


        png_dropdown.LoadFromResource( IDR_PNG_DROPDOWN );
    }

    COLORREF clrFrameNcBorder;
    COLORREF clrFrameWindow;
    CFont    fontDefault;

    CIcon    iconFavDir;
    CIcon    iconNull;

    CDWImage png_dropdown;
};