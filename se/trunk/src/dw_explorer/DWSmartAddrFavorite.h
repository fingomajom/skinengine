
#pragma once

#include "DWSmartAddrProvider.h"
#include "DWIEFavoritesMgt.h"

class CDWSmartAddrFavorite : public CDWSmartAddrProvider
{
public:
    CDWSmartAddrFavorite()
    {
        CDWSmartAddrProviderList::AddSmartAddrProvider(this);
    }

    virtual int QueryAddrDropList( HWND hWndN, LPCTSTR pszIText, 
        AddrDropItemList& aList, int nMaxCount )
    {
        if ( pszIText == NULL || lstrlenW(pszIText) <= 0 )
            return QS_NONE;

        CDWIEFavoritesMgt& favmgt = CDWIEFavoritesMgt::Instance();
        
        int nleave = nMaxCount;
        
        _QueryAddrDropList( &favmgt.GetFavoriteList(), pszIText,
            aList, nleave);

        return nMaxCount - nleave;
    }

    int _QueryAddrDropList(CDWFavList* favList, LPCTSTR pszIText, AddrDropItemList& aList, int& nleave)
    {
        if ( favList == NULL || nleave <= 0 )
            return 0;

        for ( size_t idx = 0; idx < favList->GetCount() && nleave > 0; idx++ )
        {
            IEFavoriteItem& favitem = favList->GetAt(idx);

            if ( favitem.pChildList != NULL )
            {
                _QueryAddrDropList( favitem.pChildList, pszIText,
                    aList, nleave);

            }
            else if ( StrStr(favitem.strURL, pszIText) != NULL )
            {
                ADDRDROPLISTITEM dropItem;

                dropItem.strTitle = favitem.strTitle;
                dropItem.strURL   = favitem.strURL;

                nleave--;
                aList.AddTail( dropItem );
            }
        }

        return 0;
    }
};


__declspec(selectany) CDWSmartAddrFavorite g_DWSmartAddrFavorite;
