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


class CDWEventSvr : public CDWSingleton<CDWEventSvr>
{
    friend class CDWSingleton<CDWEventSvr>;
protected:
    CDWEventSvr( const CDWEventSvr& );

    CDWEventSvr()
    {
    }

    struct CallbackInfo 
    {
        DWORD dwThreadId;
        CDWEventCallback* pCallback;
    };

public:

    void AddCallback( CDWEventCallback* cb )
    {
        CallbackInfo info;
        info.dwThreadId = GetCurrentThreadId();
        info.pCallback  = cb;
        m_listCallback.AddTail( info );
    }

    void RemoveCallback( CDWEventCallback* cb )
    {
        DWORD dwThreadId = GetCurrentThreadId();

        for (POSITION pos = m_listCallback.GetHeadPosition(); pos != NULL; )
        {
            CallbackInfo& info = m_listCallback.GetNext(pos);
            if ( info.dwThreadId != dwThreadId )
                continue;

            if ( info.pCallback == cb )
            {
                m_listCallback.RemoveAt(pos);
                break;
            }
        }
    }


    LRESULT OnMessage( UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0, BOOL bAll = TRUE, BOOL bAllThread = FALSE )
    {
        LRESULT lResult = 0;

        DWORD dwThreadId = GetCurrentThreadId();

        for (POSITION pos = m_listCallback.GetHeadPosition(); pos != NULL; )
        {
            CallbackInfo& info = m_listCallback.GetNext(pos);
            if ( info.dwThreadId != dwThreadId && !bAllThread )
                continue;

            lResult = info.pCallback->OnEventMessage( uMsg, wParam, lParam );
            if ( !bAll && lResult )
                return lResult;
        }

        return 0;
    }
    
private:
    ATL::CAtlList<CallbackInfo> m_listCallback;
};