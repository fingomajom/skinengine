

#pragma once

#include "DWConfigDB.h"

class CDWSkinUIMgt : public CDWSingleton<CDWSkinUIMgt>
{
    friend CDWSingleton<CDWSkinUIMgt>;
protected:
    CDWSkinUIMgt( const CDWSkinUIMgt& );

    CDWSkinUIMgt()
    {
        clrFrameNcBorder = RGB( 90 , 90 , 90  );
        clrFrameWindow   = RGB( 7  , 120 , 247 );

        CDWBaseConfig cfg;
        clrFrameWindow = cfg.get_Window_Color( clrFrameWindow );

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