
#pragma once

namespace KSE{

#include <tinyxml.h>

#ifdef _DEBUG
    #pragma comment(lib, "tinyxmld.lib")
#else
    #pragma comment(lib, "tinyxml.lib")
#endif


typedef TiXmlNode           SkinXmlNode;
typedef TiXmlDocument       SkinXmlDocument;
typedef TiXmlElement        SkinXmlElement;
typedef TiXmlComment        SkinXmlComment;
typedef TiXmlUnknown        SkinXmlUnknown;
typedef TiXmlAttribute      SkinXmlAttribute;
typedef TiXmlText           SkinXmlText;
typedef TiXmlDeclaration    SkinXmlDeclaration;
typedef TiXmlParsingData    SkinXmlParsingData;




};