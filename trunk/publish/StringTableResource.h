
#pragma once

#include <skinstrres.h>
#include <vector>

class CStringTableResource : public KSE::skinstrres
{
public:
    CStringTableResource();
    ~CStringTableResource(void);

    //////////////////////////////////////////////////////////////////////////
    typedef struct _StringTableItemInfo 
    {
        KSE::CString strIDName;
        KSE::CString strValue;

    }STRINGTABLE_ITEMINFO;

    BOOL SaveString(LPCTSTR pszIDName, LPCTSTR strValue);

    BOOL LoadStringTableList();

    //BOOL GetStringTableList(std::vector<STRINGTABLE_ITEMINFO>& vtItemList);
    //BOOL SetStringTableList(const std::vector<STRINGTABLE_ITEMINFO>& vtItemList);

    BOOL SaveToDocument(CXmlNodeWrapper& root);
    //////////////////////////////////////////////////////////////////////////

    std::vector<CStringTableResource::STRINGTABLE_ITEMINFO>& GetStringTableList();

public:

    std::vector<CStringTableResource::STRINGTABLE_ITEMINFO> m_vtItemList;

};
