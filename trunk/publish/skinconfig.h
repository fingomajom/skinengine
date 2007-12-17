
#pragma once

namespace KSE{

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

    virtual LPCTSTR GetSkinResPath() 
    {
        return _T("C:\\");
    }
    virtual LPCTSTR GetSkinImagePath() 
    {
        return NULL;
    }

};


};