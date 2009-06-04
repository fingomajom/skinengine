/********************************************************************
	created:	2009/05/20
	created:	20:5:2009   0:39
	filename: 	d:\lidengwang\se\trunk\src\dw_explorer\DWSmartAddrHistroy.h
	file path:	d:\lidengwang\se\trunk\src\dw_explorer
	file base:	DWSmartAddrHistroy
	file ext:	h
	author:		lidengwang@s-zone.com
	
	purpose:	
*********************************************************************/

#pragma once

#include "DWSmartAddrProvider.h"
#include "DWURLHistoryMgt.h"

class CDWSmartAddrHistroy : public CDWSmartAddrProvider
{
public:
    CDWSmartAddrHistroy() {}

    virtual int QueryAddrDropList( HWND hWndN, LPCTSTR pszIText, 
        AddrDropItemList& aList, int nMaxCount )
    {
        if ( pszIText == NULL || lstrlenW(pszIText) <= 0 )
            return QS_NONE;        

        return _QueryHistoryDropList( pszIText, aList, nMaxCount);;
    }

    int _QueryHistoryDropList(LPCTSTR pszIText, AddrDropItemList& aList, int nMaxCount)
    {
        int nCount = 0;

        CDWURLHistoryMgt& history = CDWURLHistoryMgt::Instance();

        const URLHistoryInfoOrderMap& hList = history.GetList();

        for ( POSITION pos = hList.GetTailPosition() ; pos != NULL && nCount < nMaxCount; )
        {
            const URLHistoryInfo& info = *hList.GetPrev(pos)->m_value;

            if ( StrStrI(info.strURL, pszIText) != NULL ||
                 StrStrI(info.strTitle, pszIText) != NULL )
            {
                ADDRDROPLISTITEM dropItem;

                dropItem.strTitle = info.strTitle;
                dropItem.strURL   = info.strURL;

                aList.AddTail( dropItem );

                nCount++;
            }
        }

        return nCount;
    }
};

__declspec(selectany) CDWSmartAddrHistroy g_DWSmartAddrHistroy;

