

#pragma once

typedef struct tag_AddrDropListItem
{
    ATL::CString strURL;
    ATL::CString strTitle;

    CIconHandle  hIconLeft;
    CIconHandle  hIconRight;

}ADDRDROPLISTITEM, *LPADDRDROPLISTITEM;

typedef ATL::CAtlList< ADDRDROPLISTITEM > AddrDropItemList;


enum{
    QS_ASYN = -1,
    QS_NONE = 0
};

class CDWSmartAddrProvider
{
public:

    virtual int QueryAddrDropList( HWND hWndN, LPCTSTR pszIText, 
        AddrDropItemList& aList, int nMaxCount ) = 0;
};


class CDWSmartAddrProviderList : public CDWSingleton2<CDWSmartAddrProviderList>
{
    friend class CDWSingleton2<CDWSmartAddrProviderList>;
    CDWSmartAddrProviderList() {}
    CDWSmartAddrProviderList( const CDWSmartAddrProviderList& ); 
public:

    static void AddSmartAddrProvider(CDWSmartAddrProvider* pProvider)
    {
        ATLASSERT(pProvider);
        Instance().m_vtProviderList.Add(pProvider);
    }

    const ATL::CSimpleArray<CDWSmartAddrProvider*> & GetProviderList() const
    {
        return m_vtProviderList;
    }
private:
    ATL::CSimpleArray<CDWSmartAddrProvider*> m_vtProviderList;

};

