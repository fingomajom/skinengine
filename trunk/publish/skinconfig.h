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

    virtual LPCTSTR GetWorkDirectory()
    {
        if ( m_pathWorkDirectory.m_strPath.GetLength() > 2 )
            return m_pathWorkDirectory.m_strPath;

        TCHAR szBuffer[MAX_PATH] = { 0 };

        GetModuleFileName(NULL, szBuffer, MAX_PATH);
        
        m_pathWorkDirectory.m_strPath = szBuffer;
        m_pathWorkDirectory.RemoveFileSpec();

        return m_pathWorkDirectory.m_strPath;
    }

    virtual void InitPaths()
    {
        GetWorkDirectory();

        m_pathSkinResPath   = m_pathWorkDirectory;
        m_pathSkinImagePath = m_pathWorkDirectory;
    }

public:
    
    ATL::CPath m_pathWorkDirectory;
    ATL::CPath m_pathSkinResPath;
    ATL::CPath m_pathSkinImagePath;

};


};