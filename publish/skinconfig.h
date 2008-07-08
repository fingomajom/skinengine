/********************************************************************
* CreatedOn: 2007-12-17   17:07
* FileName: skinconfig.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include <atlpath.h>
#include <framework/kis_pathcfg.h>

namespace KSGUI{

class skinconfigbase
{
public:
    
    virtual LPCTSTR GetSkinResPath()    = 0;
    virtual LPCTSTR GetSkinImagePath()  = 0;

};

class skinconfig : 
    public skinconfigbase
{
public:

    skinconfig()
    {
        InitPaths();
    }

    virtual LPCTSTR GetSkinResPath() 
    {
        return m_pathSkinResPath;
    }
    virtual LPCTSTR GetSkinImagePath() 
    {
        return m_pathSkinImagePath;
    }

    virtual void InitPaths()
    {
        m_pathSkinResPath   = (LPCTSTR)kis::kisGetPath(kis::path_cur_land);
        m_pathSkinImagePath = (LPCTSTR)kis::kisGetPath(kis::path_images);
    }

public:
  
    ATL::CPath m_pathSkinResPath;
    ATL::CPath m_pathSkinImagePath;

};


};