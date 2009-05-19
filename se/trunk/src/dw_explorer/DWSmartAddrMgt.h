
#pragma once

#include "DWSmartAddrProvider.h"
#include "DWSmartAddrFavorite.h"
#include "DWSmartAddrSearch.h"
#include "DWSmartAddrHistroy.h"

class CDWSmartAddrMgt : public CDWSingleton2<CDWSmartAddrMgt>
{
    friend class CDWSingleton2<CDWSmartAddrMgt>;

    CDWSmartAddrMgt() 
    {
        CDWSmartAddrProviderList::AddSmartAddrProvider(&g_DWSmartAddrHistroy);
        CDWSmartAddrProviderList::AddSmartAddrProvider(&g_DWSmartAddrFavorite);
        CDWSmartAddrProviderList::AddSmartAddrProvider(&g_CDWSmartAddrSearch);
    }

public:    

    ~CDWSmartAddrMgt()
    {
    }
    
    int QueryAddrDropList( HWND hWndN, LPCTSTR pszIText, 
        AddrDropItemList& aList, int nMaxCount )
    {
        CDWSmartAddrProviderList& PList = CDWSmartAddrProviderList::Instance();
        
        const ATL::CSimpleArray<CDWSmartAddrProvider*>& ProviderList = 
            CDWSmartAddrProviderList::Instance().GetProviderList();

        for ( int i = 0; i < ProviderList.GetSize(); i++ )
        {
            AddrDropItemList aTempList;
            ATLASSERT(ProviderList[i] != NULL);
            if ( ProviderList[i] == NULL )
                continue;

            if ( ProviderList[i]->QueryAddrDropList( hWndN, pszIText, aTempList, nMaxCount ) > 0 )
                aList.AddTailList( &aTempList );
        }


        return 0;
    }

};