#include "precomp.h"
#pragma hdrstop


RULE_LIST g_BlackRuleList;
RULE_LIST g_WhiteRuleList;
RULE_LIST g_ProtectRuleList;


VOID InitRuleList( LP_RULE_LIST RuleList )
{
    InitializeListHead  (&RuleList->RuleList);
    KeInitializeSpinLock(&RuleList->RuleLock);
}


VOID UninitRuleList( LP_RULE_LIST RuleList )
{
    KIRQL        OldIrql;
    PLIST_ENTRY  ListEntry = NULL;
    LP_RULE_INFO RuleInfo  = NULL;

    KeAcquireSpinLock(&RuleList->RuleLock, &OldIrql);

    while ( ! IsListEmpty(&RuleList->RuleList) )
    {
        ListEntry = RemoveTailList( &RuleList->RuleList );

        RuleInfo  = CONTAINING_RECORD( ListEntry, RULE_INFO, ListEntry );

        ExFreePool( RuleInfo );
    }

    InitializeListHead(&RuleList->RuleList);

    KeReleaseSpinLock(&RuleList->RuleLock, OldIrql);

}

BOOL AppendRule( LP_RULE_LIST RuleList, LP_RULE_INFO RuleInfo )
{
    BOOL  bResult = FALSE;
    KIRQL OldIrql;

    LP_RULE_INFO NewRuleItem = NULL;

    NewRuleItem = 
        ExAllocatePoolWithTag(NonPagedPool, 
        sizeof(RULE_INFO), 
        MEM_TAG);

    if ( RuleList != NULL && RuleInfo != NULL && NewRuleItem != NULL )
    {
        RtlMoveMemory( NewRuleItem, RuleList, sizeof(RULE_INFO) );

        KeAcquireSpinLock(&RuleList->RuleLock, &OldIrql);

        InsertTailList( &RuleList->RuleList, &NewRuleItem->ListEntry);

        KeReleaseSpinLock(&RuleList->RuleLock, OldIrql);

        bResult = TRUE;

    }

    return bResult;
}

VOID RemoveRule( LP_RULE_LIST RuleList, LP_RULE_INFO RuleInfo )
{
    KIRQL        OldIrql;
    PLIST_ENTRY  FindEntry = NULL;


    KeAcquireSpinLock(&RuleList->RuleLock, &OldIrql);

    FindEntry = FindRule( RuleList, RuleInfo);
    if ( FindEntry != NULL )
    {
        RemoveEntryList(FindEntry);
        ExFreePool( RuleInfo );
    }

    KeReleaseSpinLock(&RuleList->RuleLock, OldIrql);
}

PLIST_ENTRY FindRule( LP_RULE_LIST RuleList, LP_RULE_INFO RuleInfo )
{
    PLIST_ENTRY  EntryResult = NULL;
    PLIST_ENTRY  NextEntry   = NULL;
    LP_RULE_INFO FRuleInfo   = NULL;

    
    if (  RuleList != NULL && 
          RuleInfo != NULL && 
         !IsListEmpty( &RuleList->RuleList ) )
    {

        NextEntry = RuleList->RuleList.Flink;

        while ( NextEntry != &RuleList->RuleList )
        {
            FRuleInfo = CONTAINING_RECORD(NextEntry, RULE_INFO, ListEntry);

            if ( FRuleInfo->uContentType == RuleInfo->uContentType )
            {
                switch( FRuleInfo->uContentType )
                {
                case CT_ID:

                    if ( FRuleInfo->uProcessId == RuleInfo->uProcessId )
                    {
                        EntryResult = NextEntry;
                    }

                    break;

                case CT_NAME:
                    if ( !_wcsnicmp( FRuleInfo->wszFileName, 
                                    RuleInfo->wszFileName, MAX_PATH) )
                    {
                        EntryResult = NextEntry;
                    }

                    break;

                case CT_PATH:
                    if ( !_wcsnicmp( FRuleInfo->wszPath, 
                                     RuleInfo->wszFileName, MAX_PATH) )
                    {
                        EntryResult = NextEntry;
                    }

                    break;

                case CT_PATHFILE:
                    if ( !_wcsnicmp( FRuleInfo->wszPathFile, 
                                     RuleInfo->wszPathFile, MAX_PATH) )
                    {
                        EntryResult = NextEntry;
                    }
                    
                    break;

                default:
                    break;
                }
            }

            if ( EntryResult != NULL )
                break;

            NextEntry = NextEntry->Flink;
        }
    }

    return EntryResult;
}


PLIST_ENTRY MatchingRule( 
    UINT  uRuleType,
    UINT  uContentType,
    VOID* pContent)
{
    LP_RULE_LIST RuleList    = NULL;
    PLIST_ENTRY  EntryResult = NULL;
    PLIST_ENTRY  NextEntry   = NULL;
    LP_RULE_INFO FRuleInfo   = NULL;

    switch( uRuleType )
    {
        case RT_BLACKRULE:
            RuleList = &g_BlackRuleList;
            break;
        case RT_WHITERULE:
            RuleList = &g_WhiteRuleList;
            break;
        case RT_PROTECTRULE:
            RuleList = &g_ProtectRuleList;
            break;
    }

    if (  RuleList != NULL && 
          pContent != NULL && 
         !IsListEmpty( &RuleList->RuleList ) )
    {

        NextEntry = RuleList->RuleList.Flink;

        while ( NextEntry != &RuleList->RuleList )
        {
            FRuleInfo = CONTAINING_RECORD(NextEntry, RULE_INFO, ListEntry);

            if ( FRuleInfo->uContentType == uContentType )
            {
                switch( uContentType )
                {
                case CT_ID:

                    if ( FRuleInfo->uProcessId == *((UINT*)pContent) )
                    {
                        EntryResult = NextEntry;
                    }

                    break;

                case CT_NAME:

                    if ( !_wcsnicmp( FRuleInfo->wszFileName + ( wcslen(FRuleInfo->wszFileName) - wcslen(pContent) ), 
                                     pContent, wcslen(pContent)) )
                    {
                        EntryResult = NextEntry;
                    }

                    break;

                case CT_PATH:
                    if ( !_wcsnicmp( FRuleInfo->wszPath, 
                        pContent , wcslen(FRuleInfo->wszPath) ) )
                    {
                        EntryResult = NextEntry;
                    }

                    break;

                case CT_PATHFILE:
                    if ( !_wcsnicmp( FRuleInfo->wszPathFile, 
                        pContent, MAX_PATH ) )
                    {
                        EntryResult = NextEntry;
                    }

                    break;

                default:
                    break;
                }
            }

            if ( EntryResult != NULL )
                break;

            NextEntry = NextEntry->Flink;
        }
    }

    return EntryResult;
}


BOOL AppendRule_I(
    LP_RULE_LIST RuleList, 
    UINT  uContentType,
    UINT  uEnable,
    VOID* pContent)
{
    BOOL bResult = FALSE;

    RULE_INFO RuleInfo = { 0 };

    RuleInfo.uContentType = uContentType;
    RuleInfo.uEnable      = uEnable;
    
    switch( uContentType )
    {
    case CT_ID:

        RuleInfo.uProcessId = *((ULONG*)pContent);

        DbgPrint( ("AppendRule_I(%x, CT_ID, %d, %d)", RuleList, uEnable, RuleInfo.uProcessId) );

        break;

    case CT_NAME:

        wcsncpy( RuleInfo.wszFileName, 
            pContent, MAX_PATH);

        DbgPrint( ("AppendRule_I(%x, CT_NAME, %d, %ws)", RuleList, uEnable, RuleInfo.wszFileName) );

        break;

    case CT_PATH:

        wcsncpy( RuleInfo.wszPath, 
            pContent, MAX_PATH);

        DbgPrint( ("AppendRule_I(%x, CT_PATH, %d, %ws)", RuleList, uEnable, RuleInfo.wszPath) );

        break;

    case CT_PATHFILE:

        wcsncpy( RuleInfo.wszPathFile, 
            pContent, MAX_PATH);

        DbgPrint( ("AppendRule_I(%x, CT_PATHFILE, %d, %ws)", RuleList, uEnable, RuleInfo.wszPathFile) );

        break;
    default:
        return FALSE;
    }

    bResult = AppendRule( RuleList, &RuleInfo );

    DbgPrint( ("<<==AppendRule_I() = %s", bResult ? "TRUE" : "FALSE" ) );


    return bResult;
}



BOOL InitDefaultRuleList( UINT uRuleType )
{
    BOOL bResult = TRUE;

    if ( uRuleType & RT_BLACKRULE )
    {   
    }

    if ( uRuleType & RT_WHITERULE )
    {   
        AppendRule_I( &g_WhiteRuleList, CT_NAME, TRUE, L"\\explorer.exe" );
        AppendRule_I( &g_WhiteRuleList, CT_NAME, TRUE, L"\\system32\\svchost.exe" );
        AppendRule_I( &g_WhiteRuleList, CT_NAME, TRUE, L"\\system32\\lsass.exe" );
        AppendRule_I( &g_WhiteRuleList, CT_NAME, TRUE, L"\\system32\\conime.exe" );
        AppendRule_I( &g_WhiteRuleList, CT_NAME, TRUE, L"\\system32\\ctfmon.exe" );
        AppendRule_I( &g_WhiteRuleList, CT_NAME, TRUE, L"\\system32\\winlogon.exe" );
        AppendRule_I( &g_WhiteRuleList, CT_NAME, TRUE, L"\\system32\\csrss.exe" );
        AppendRule_I( &g_WhiteRuleList, CT_NAME, TRUE, L"\\system32\\rundll32.exe" );

        AppendRule_I( &g_WhiteRuleList, CT_NAME, TRUE, L"\\system32\\msctf.dll" );
        AppendRule_I( &g_WhiteRuleList, CT_NAME, TRUE, L"\\system32\\browseui.dll" );
        AppendRule_I( &g_WhiteRuleList, CT_NAME, TRUE, L"\\system32\\uxtheme.dll" );
        AppendRule_I( &g_WhiteRuleList, CT_NAME, TRUE, L"\\system32\\ieframe.dll" );
        AppendRule_I( &g_WhiteRuleList, CT_NAME, TRUE, L"\\system32\\ieui.dll" );


        AppendRule_I( &g_WhiteRuleList, CT_NAME, TRUE, L"\\lserv.exe" );
    }

    if ( uRuleType & RT_PROTECTRULE )
    {   
    }

    return bResult;
}

