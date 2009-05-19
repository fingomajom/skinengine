

#pragma once


class CDWSkinUIMgt : public CDWSingleton<CDWSkinUIMgt>
{
    friend CDWSingleton<CDWSkinUIMgt>;
protected:
    CDWSkinUIMgt( const CDWSkinUIMgt& );

    CDWSkinUIMgt()
    {
        clrFrameNcBorder = RGB( 90 , 90 , 90  );
        clrFrameWindow   = RGB( 127, 127, 255 );

        fontDefault.CreatePointFont( 90, L"Tahoma" );

        iconNull.LoadIcon(IDI_ICON_FAV_NULL_URL, 0, 0, LR_DEFAULTSIZE);
        iconFavDir.LoadIcon(IDI_ICON_FAV_DIRECTORY, 0, 0, LR_DEFAULTSIZE);


        png_dropdown.LoadFromResource( IDR_PNG_DROPDOWN );
    }
public:
    

    COLORREF clrFrameNcBorder;
    COLORREF clrFrameWindow;
    CFont    fontDefault;

    CIcon    iconFavDir;
    CIcon    iconNull;

    CDWImage png_dropdown;
};