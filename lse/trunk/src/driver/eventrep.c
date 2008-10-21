#include "precomp.h"
#pragma hdrstop


EVENT_LIST g_event_list = { 0 };


VOID InitEventList( LP_EVENT_LIST EventList )
{
    InitializeListHead  (&EventList->EventHead);
    KeInitializeSpinLock(&EventList->EventLock);

    EventList->uEventCount = 0;
}


VOID UninitEventList( LP_EVENT_LIST EventList )
{
    KIRQL         OldIrql;
    PLIST_ENTRY   ListEntry = NULL;
    LP_EVENT_INFO EventInfo  = NULL;

    KeAcquireSpinLock(&EventList->EventLock, &OldIrql);

    while ( ! IsListEmpty(&EventList->EventHead) )
    {
        ListEntry = RemoveTailList( &EventList->EventHead );

        EventInfo  = CONTAINING_RECORD( ListEntry, EVENT_INFO, ListEntry );

        ExFreePool( EventInfo );
    }

    InitializeListHead(&EventList->EventHead);
    EventList->uEventCount = 0;

    KeReleaseSpinLock(&EventList->EventLock, OldIrql);

}

BOOL AppendEvent( LP_EVENT_LIST EventList, LP_EVENT_INFO EventInfo )
{
    BOOL  bResult = FALSE;
    KIRQL OldIrql;

    LP_EVENT_INFO NewRuleItem = NULL;

    KeAcquireSpinLock(&EventList->EventLock, &OldIrql);

    if ( EventList->uEventCount >= MAX_EVENT_COUNT )
        goto exit0;


    NewRuleItem = 
        ExAllocatePool(NonPagedPool, 
        sizeof(EVENT_INFO) );


    if ( EventList != NULL && EventInfo != NULL && NewRuleItem != NULL )
    {
        RtlMoveMemory( NewRuleItem, EventInfo, sizeof(EVENT_INFO) );

        InsertTailList( &EventList->EventHead, &NewRuleItem->ListEntry);
        EventList->uEventCount++;

        //DbgPrint( ("Success AppendEvent %d\n" , EventList->uEventCount) );

        bResult = TRUE;
    }

exit0:

    if ( EventList->ReportEvent != NULL && !IsListEmpty(&EventList->EventHead) )
        KeSetEvent(EventList->ReportEvent, 0, 0);    //֪ͨ


    KeReleaseSpinLock(&EventList->EventLock, OldIrql);

    return bResult;
}


BOOL AppendEvent_I( 
    LP_EVENT_LIST EventList,
    UINT   uEventType,
    UINT   uSrcPID,
    PWCHAR wszSrcFileName,
    UINT   uTagPID,
    PWCHAR wszTagFileName)
{
    EVENT_INFO EventInfo = { 0 };

    EventInfo.EventInfo.uEventType = uEventType;
    EventInfo.EventInfo.uSrcPID    = uSrcPID;
    EventInfo.EventInfo.uTagPID    = uTagPID;

    if (wszSrcFileName != NULL)
    {
        wcsncpy(EventInfo.EventInfo.wszSrcFileName,
            wszSrcFileName, MAX_PATH);
    }

    if (wszTagFileName != NULL)
    {
        wcsncpy(EventInfo.EventInfo.wszTagFileName,
            wszTagFileName, MAX_PATH);
    }

    return AppendEvent( EventList, &EventInfo );
}


LP_EVENT_INFO PopHeadEvent( LP_EVENT_LIST EventList )
{
    LP_EVENT_INFO pResult   = NULL;
    PLIST_ENTRY   ListEntry = NULL;

    KIRQL OldIrql;


    if ( EventList != NULL  )
    {
        KeAcquireSpinLock(&EventList->EventLock, &OldIrql);

        if ( !IsListEmpty( &EventList->EventHead ) )
        {
            ListEntry = RemoveHeadList( &EventList->EventHead );
            EventList->uEventCount--;

            pResult  = CONTAINING_RECORD( ListEntry, EVENT_INFO, ListEntry );
        }

        KeReleaseSpinLock(&EventList->EventLock, OldIrql);

    }

    return pResult;
}