/********************************************************************
* CreatedOn: 2007-12-17   17:08
* FileName: skinxmlobject.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/


#pragma once

#include <skintinyxml.h>

namespace KSE{

class skinxmlobject
{
public:
    
    virtual bool  AttachXmlNode(const SkinXmlElement& xmlElement) = 0;


};

} // amespace KSE{