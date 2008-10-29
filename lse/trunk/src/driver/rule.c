#include "precomp.h"
#pragma hdrstop


RULE_LIST g_BlackRuleList;
RULE_LIST g_WhiteRuleList;
RULE_LIST g_ProtectRuleList;

RULE_LIST g_RunPIDList;



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
        ExAllocatePool(NonPagedPool, 
        sizeof(RULE_INFO) );


    if ( RuleList != NULL && RuleInfo != NULL && NewRuleItem != NULL )
    {
        RtlMoveMemory( NewRuleItem, RuleInfo, sizeof(RULE_INFO) );

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
    LP_RULE_LIST RuleList,
    UINT         uContentType,
    VOID*        pContent)
{
    PLIST_ENTRY  EntryResult = NULL;
    PLIST_ENTRY  NextEntry   = NULL;
    LP_RULE_INFO FRuleInfo   = NULL;

    int          nTempLen    = 0;
    int          nTempLen1   = 0;

    //DbgPrint (("MatchingRule"));

    if (  RuleList != NULL && 
          pContent != NULL && 
         !IsListEmpty( &RuleList->RuleList ) )
    {

        //DbgPrint (("IsListEmpty"));


        NextEntry = RuleList->RuleList.Flink;

        while ( NextEntry != &RuleList->RuleList )
        {
            FRuleInfo = CONTAINING_RECORD(NextEntry, RULE_INFO, ListEntry);

            //if ( FRuleInfo->uContentType == CT_ID )
            //    DbgPrint (("FRuleInfo [%d], uContentType = %d", FRuleInfo->uProcessId, uContentType  ));
            //else
            //    DbgPrint (("FRuleInfo [%ws], uContentType = %d", FRuleInfo->wszFileName, uContentType  ));

            switch( uContentType )
            {
            case CT_ID:

                if ( FRuleInfo->uContentType == CT_ID &&
                     FRuleInfo->uProcessId   == *((UINT*)pContent) )
                {
                    EntryResult = NextEntry;
                }
                break;

            case CT_PATHFILE:

                if ( FRuleInfo->uContentType == CT_ID )
                {
                    break;
                }
                else if ( FRuleInfo->uContentType == CT_PATH )
                {
                    nTempLen = wcslen(FRuleInfo->wszPath);

                    if ( !_wcsnicmp( (WCHAR*)pContent , FRuleInfo->wszPath, nTempLen ) )
                    {
                        EntryResult = NextEntry;
                    }
                }
                else if ( FRuleInfo->uContentType == CT_NAME )
                {
                    nTempLen  = wcslen(FRuleInfo->wszFileName);
                    nTempLen1 = wcslen(pContent);

                    if (  nTempLen1 >= nTempLen &&
                         !_wcsnicmp( (WCHAR*)pContent +  ( nTempLen1 - nTempLen ), FRuleInfo->wszFileName, nTempLen ) )
                    {
                        EntryResult = NextEntry;
                    }
                }
                else if ( FRuleInfo->uContentType == CT_PATHFILE )
                {
                    if ( !_wcsnicmp( FRuleInfo->wszPathFile, pContent, MAX_PATH ) )
                    {
                        EntryResult = NextEntry;
                    }
                }

                break;

            case CT_PATH:

                if ( FRuleInfo->uContentType == CT_PATH &&
                     !_wcsnicmp( FRuleInfo->wszPath, pContent , wcslen(FRuleInfo->wszPath) ) )
                {
                    EntryResult = NextEntry;
                    break;
                }

                break;

            case CT_NAME:

                //DbgPrint (("MatchingRule  %ws , %ws , %ws", pContent, FRuleInfo->wszFileName, (WCHAR*)pContent +  ( wcslen(pContent) - wcslen(FRuleInfo->wszFileName) ) ));
                
                nTempLen = wcslen(FRuleInfo->wszFileName);
                nTempLen1 = wcslen(pContent);

                if ( nTempLen1 >= nTempLen &&
                     FRuleInfo->uContentType == CT_NAME &&
                     !_wcsnicmp( (WCHAR*)pContent +  ( nTempLen1 - nTempLen ), FRuleInfo->wszFileName, nTempLen ) )
                {
                    EntryResult = NextEntry;
                }

                break;
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
    BOOL  bResult = TRUE;
    UINT  uLength = 0;
    WCHAR wszBuffer[MAX_PATH] = { 0 };

    if ( uRuleType & RT_BLACKRULE )
    {   
    }

    if ( uRuleType & RT_WHITERULE )
    {
        if ( !GetWindowsDirectory( wszBuffer, sizeof(wszBuffer) ) )
            return FALSE;

        uLength = wcslen(wszBuffer);

        if ( wszBuffer[uLength - 1] == '\\' )
        {
             wszBuffer[uLength - 1] = '\0'; uLength--;
        }

    #define APPEND_SYS_WHITE_RULE( PathFile ) \
        wcsncpy(wszBuffer + uLength, PathFile, MAX_PATH - uLength);     \
        AppendRule_I( &g_WhiteRuleList, CT_PATHFILE, TRUE, wszBuffer);

        
        APPEND_SYS_WHITE_RULE(L"\\explorer.exe");
        APPEND_SYS_WHITE_RULE(L"\\system32\\svchost.exe");
        APPEND_SYS_WHITE_RULE(L"\\system32\\lsass.exe");
        APPEND_SYS_WHITE_RULE(L"\\system32\\conime.exe");
        APPEND_SYS_WHITE_RULE(L"\\system32\\ctfmon.exe");
        APPEND_SYS_WHITE_RULE(L"\\system32\\winlogon.exe");
        APPEND_SYS_WHITE_RULE(L"\\system32\\csrss.exe");
        APPEND_SYS_WHITE_RULE(L"\\system32\\rundll32.exe");

        APPEND_SYS_WHITE_RULE(L"\\system32\\msctf.dll");
        APPEND_SYS_WHITE_RULE(L"\\system32\\browseui.dll");
        APPEND_SYS_WHITE_RULE(L"\\system32\\uxtheme.dll");
        APPEND_SYS_WHITE_RULE(L"\\system32\\ieframe.dll");
        APPEND_SYS_WHITE_RULE(L"\\system32\\ieui.dll");

    #undef APPEND_SYS_WHITE_RULE
            
        uLength = 4;
        AppendRule_I( &g_WhiteRuleList, CT_ID,   TRUE, &uLength );
        uLength = 8;
        AppendRule_I( &g_WhiteRuleList, CT_ID,   TRUE, &uLength );
    }

    if ( uRuleType & RT_PROTECTRULE )
    {
        //AppendRule_I( &g_ProtectRuleList, CT_NAME, TRUE, L"\\lserv.exe" );
        //AppendRule_I( &g_ProtectRuleList, CT_PATHFILE, TRUE, L"c:\\program files\\kingsoft antispy\\kasmain.exe" );

        
        //AppendRule_I( &g_ProtectRuleList, CT_NAME, TRUE, L"\\mfckpstest.exe" );
    }

    return bResult;
}

