/********************************************************************
* CreatedOn: 2008-1-8   23:50
* FileName:  skinctrls.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#include "skinxmlctrls.h"
#include "skinwin.h"
#include <atlctrls.h>


///////////////////////////////////////////////////////////////////////////////
// Classes in this file:
//
// CSkinStaticT<TBase> - CSkinStatic


namespace KSG{


template <class TBase>
class CSkinStaticT : public CStaticT<TBase>
{
public:

    CSkinStaticT(HWND hWnd = NULL) : CStaticT<TBase>(hWnd)
    { }

    CSkinStaticT< TBase >& operator =(HWND hWnd)
    {
        m_hWnd = hWnd;
        return *this;
    }

    HWND SkinCreate( 
        const SkinXmlElement& xmlElement,
        HWND hWndParent, _U_MENUorID MenuOrID = 0U ) throw()
    {
        return TBase::SkinCreate(xmlElement, 
            hWndParent, 
            GetWndClassName(),
            MenuOrID);
    }
};

typedef CSkinStaticT< KSG::SkinWindow > CSkinStatic;


//////////////////////////////////////////////////////////////////////////

template <class TBase>
class CSkinButtonT : public CButtonT<TBase>
{
public:

    CSkinButtonT(HWND hWnd = NULL) : CButtonT<TBase>(hWnd)
    { }

    CSkinButtonT< TBase >& operator =(HWND hWnd)
    {
        m_hWnd = hWnd;
        return *this;
    }

    HWND SkinCreate( 
        const SkinXmlElement& xmlElement,
        HWND hWndParent, _U_MENUorID MenuOrID = 0U ) throw()
    {
        return TBase::SkinCreate(xmlElement, 
            hWndParent, 
            GetWndClassName(),
            MenuOrID);
    }
};

typedef CSkinButtonT< KSG::SkinWindow > CSkinButton;



//////////////////////////////////////////////////////////////////////////

}; // namespace KSG