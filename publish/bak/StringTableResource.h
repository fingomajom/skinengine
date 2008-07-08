
#pragma once

#include <skinstrres.h>
#include <vector>

using namespace KSG;

class CStringTableResource : public KSG::skinstrres
{
public:
    CStringTableResource();
    ~CStringTableResource(void);

    //////////////////////////////////////////////////////////////////////////
    typedef struct _StringTableItemInfo 
    {
        KSG::CString strIDName;
        KSG::CString strValue;

    }STRINGTABLE_ITEMINFO;

    BOOL SaveString(LPCTSTR pszIDName, LPCTSTR strValue);

    BOOL LoadStringTableList();

    //BOOL GetStringTableList(std::vector<STRINGTABLE_ITEMINFO>& vtItemList);
    //BOOL SetStringTableList(const std::vector<STRINGTABLE_ITEMINFO>& vtItemList);

    BOOL SaveToDocument(SkinXmlElement& root);
    //////////////////////////////////////////////////////////////////////////

    std::vector<CStringTableResource::STRINGTABLE_ITEMINFO>& GetStringTableList();

public:

    std::vector<CStringTableResource::STRINGTABLE_ITEMINFO> m_vtItemList;

};
