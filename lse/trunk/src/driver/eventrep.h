/********************************************************************
	created:	2008/10/21
	created:	21:10:2008   10:01
	filename: 	d:\lidengwang\lse\trunk\src\driver\logrep.h
	file path:	d:\lidengwang\lse\trunk\src\driver
	file base:	logrep
	file ext:	h
	author:		lidengwang
	
	purpose:	
*********************************************************************/

#pragma once


#define MAX_EVENT_COUNT  (100)

#pragma pack(push, 1)

typedef struct tag_EVENT_INFO
{
    LIST_ENTRY        ListEntry;
    DRIVER_EVENT_INFO EventInfo;

}EVENT_INFO, *LP_EVENT_INFO;


typedef struct tag_EVENT_LIST
{
    LIST_ENTRY  EventHead;
    KSPIN_LOCK  EventLock;

    UINT        uEventCount;
    PKEVENT     ReportEvent;

}EVENT_LIST, *LP_EVENT_LIST;

#pragma pack(pop)


VOID InitEventList  ( LP_EVENT_LIST EventList );
VOID UninitEventList( LP_EVENT_LIST EventList );

BOOL AppendEvent( LP_EVENT_LIST EventList, LP_EVENT_INFO EventInfo );

BOOL AppendEvent_I(
    LP_EVENT_LIST EventList,
    UINT   uEventType,
    UINT   uSrcPID,
    PWCHAR wszSrcFileName,
    UINT   uTagPID,
    PWCHAR wszTagFileName);

LP_EVENT_INFO PopHeadEvent( LP_EVENT_LIST EventList );


extern EVENT_LIST g_event_list;
