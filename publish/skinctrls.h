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
class CSkinStaticT : public TBase
{
};

typedef CSkinStaticT<KSG::SkinWindow< skinxmlstatic > >   CSkinStatic;


}; // namespace KSG