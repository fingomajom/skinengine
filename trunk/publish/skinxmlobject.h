

#pragma once

#include <XmlNodeWrapper.h>


namespace KSE{

class skinxmlobject
{
public:
    
    virtual bool  AttachXmlNode(const CXmlNodeWrapper& xmlNode) = 0;


};

} // amespace KSE{