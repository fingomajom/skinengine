
#pragma once

#include "SkinResDocument.h"
#include "SkinResPropertyView.h"
#include "SkinTreeControlView.h"

class SkinControlsMgt
{
public:
    
    static SkinControlsMgt& Instance()
    {
        static SkinControlsMgt gs_skincontrolsmgt_instance;

        return gs_skincontrolsmgt_instance;
    }
    
    SkinResPropertyView m_skinResPropertyView;
    SkinTreeControlView m_skinTreeControlView;

    SkinFrame* m_piSkinFrame;

    SkinResDocument m_resDocument;

public:
    
    //BOOL NewRes()
    //{
    //    
    //    return TRUE;
    //}

    //BOOL OpenRes();

    //BOOL SaveRes();

};