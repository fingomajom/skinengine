/********************************************************************
	created:	2009/05/04
	created:	4:5:2009   2:40
	filename: 	d:\lidengwang\se\trunk\src\dw_explorer\DWEventSvr.h
	file path:	d:\lidengwang\se\trunk\src\dw_explorer
	file base:	DWEventSvr
	file ext:	h
	author:		lidengwang@s-zone.com
	
	purpose:	
*********************************************************************/

#pragma once


#define eid_addr_changed        1
#define edi_open_url            2

#define edi_addr_setfocus       10
#define edi_spr_icon_changed    11

#define edi_status_bar          20

#define edi_skin_changed        101



class CDWEventCallback
{
public:
    virtual LRESULT OnEventMessage( UINT uMsg, WPARAM wParam, LPARAM lParam ) = 0;
};


class CDWEventSvr
{
    CDWEventSvr( const CDWEventSvr& );

    CDWEventSvr()
    {
    }
public:

    static CDWEventSvr& Instance()
    {
        static CDWEventSvr* __CDWEventSvr_Instance__ = new CDWEventSvr();
        return *__CDWEventSvr_Instance__;
    }

    void AddCallback( CDWEventCallback* cb )
    {
        m_listCallback.AddTail( cb );
    }

    LRESULT OnMessage( UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0, BOOL bAll = TRUE )
    {
        LRESULT lResult = 0;

        for (POSITION pos = m_listCallback.GetHeadPosition(); pos != NULL; )
        {
            CDWEventCallback* cb = m_listCallback.GetNext(pos);
            lResult = cb->OnEventMessage( uMsg, wParam, lParam );
            if ( !bAll && lResult )
                return lResult;
        }

        return 0;
    }
    
private:
    ATL::CAtlList<CDWEventCallback*> m_listCallback;
};