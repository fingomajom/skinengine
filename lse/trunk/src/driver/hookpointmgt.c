#include "precomp.h"
#pragma hdrstop


hook_item_info g_hook_function_array[ hook_function_count ] = { 0 };

#define CSRSSNAME   L"csrss.exe"


NTSYSAPI NTSTATUS NTAPI ZwQuerySection(
    HANDLE SectionHandle,
    SECTION_INFORMATION_CLASS SectionInformationClass,
    PVOID SectionInformation,
    SIZE_T Length,
    PSIZE_T ResultLength);

NTSYSAPI NTSTATUS NTAPI ZwOpenSymbolicLinkObject(
    PHANDLE LinkHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes);

NTSYSAPI NTSTATUS NTAPI ZwQuerySymbolicLinkObject(
    HANDLE LinkHandle,
    PUNICODE_STRING LinkTarget,
    PULONG ReturnedLength);

NTSYSAPI NTSTATUS NTAPI ObReferenceObjectByName(
    PUNICODE_STRING ObjectName,
    ULONG Attributes,
    PACCESS_STATE AccessState,
    ACCESS_MASK DesiredAccess,
    POBJECT_TYPE ObjectType,
    KPROCESSOR_MODE AccessMode,
    PVOID ParseContext,
    PVOID *Object);

NTSYSAPI NTSTATUS NTAPI ObOpenObjectByName (
    POBJECT_ATTRIBUTES ObjectAttributes,
    POBJECT_TYPE ObjectType,
    KPROCESSOR_MODE AccessMode,
    PACCESS_STATE AccessState,
    ACCESS_MASK DesiredAccess,
    PVOID ParseContext,
    PHANDLE Handle);

NTSYSCALLAPI NTSTATUS NTAPI ZwOpenProcess (
    PHANDLE ProcessHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PCLIENT_ID ClientId); 

NTSYSAPI NTSTATUS NTAPI ZwDeleteFile(
    POBJECT_ATTRIBUTES ObjectAttributes);

NTSYSAPI NTSTATUS NTAPI KeUserModeCallback(
    ULONG ApiNumber, 
    PVOID InputBuffer, 
    ULONG InputLength, 
    PVOID *OutputBuffer,
    PULONG OutputLength);

NTSYSAPI NTSTATUS NTAPI ZwQuerySystemInformation(
    ULONG SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength);

NTKERNELAPI NTSTATUS KeAddSystemServiceTable( 
    PVOID lpAddressTable, 
    BOOLEAN bUnknown,
    ULONG dwNumEntries, 
    PVOID lpParameterTable,
    ULONG dwTableID);


ULONG*	KAVBMapServiceTable(ULONG *ServiceTable, ULONG Size, PMDL *ppMdl)
{
    ULONG	*puReturnAddr = NULL;


    *ppMdl = IoAllocateMdl(ServiceTable, Size, FALSE, FALSE, NULL);
    if ( !(*ppMdl) )
        goto exit0;

    MmBuildMdlForNonPagedPool(*ppMdl);

    puReturnAddr = (ULONG*)MmMapLockedPagesSpecifyCache(
        *ppMdl,
        KernelMode,
        MmNonCached,
        NULL,
        FALSE,
        NormalPagePriority 
        );

exit0:
    return puReturnAddr;

}

int KAVBUnMapServiceTable(ULONG *ServiceTable, PMDL pMdl)
{
    int nResult = FALSE;


    MmUnmapLockedPages(ServiceTable, pMdl);
    if (pMdl)
    {
        ExFreePool(pMdl);
        pMdl = NULL;

    }

    nResult = TRUE;
    return nResult;
}


BOOL init_hook()
{
    BOOL  bResult   = FALSE ;
    PVOID pEprocess = NULL  ;

    DbgPrint (("init_hook"));

    pEprocess = GetEProcessByProcName(CSRSSNAME);
    if ( pEprocess == NULL )
        return bResult;

    DbgPrint (("GetEProcessByProcName"));

    KeAttachProcess(pEprocess);

    KeServiceDescriptorTableShadow = SzFindShadowTable();
    if ( KeServiceDescriptorTableShadow == NULL)
        goto exit0;

    DbgPrint (("SzFindShadowTable"));


    KeServiceDescriptorTableShadow ++;	

    KeServiceTable = KAVBMapServiceTable (	
        KeServiceDescriptorTableShadow->ServiceTable, 
        KeServiceDescriptorTableShadow->ServiceLimit * sizeof(ULONG),
        &KeServiceDescriptorTableShadowMDL);
    if ( KeServiceTable == NULL )
        goto exit0;

    DbgPrint (("KAVBMapServiceTable"));

    RtlZeroMemory(g_hook_function_array, sizeof(g_hook_function_array));

    //  ������
    g_hook_function_array[hook_nt_open_process].bcanhook    = TRUE;
    g_hook_function_array[hook_nt_open_process].ptargetfunc = (void *)NtOpenProcess;
    g_hook_function_array[hook_nt_open_process].phookfunc   = (void *)SzHookNtOpenProcess;


    g_hook_function_array[hook_query_system_information].bcanhook    = TRUE;
    g_hook_function_array[hook_query_system_information].ptargetfunc = GetServiceAddress( ZwQuerySystemInformation );
    g_hook_function_array[hook_query_system_information].phookfunc   = (void *)SzHookQuerySystemInformation;


    //// ��ע��
    g_hook_function_array[hook_ke_user_mode_callback].bcanhook    = TRUE;
    g_hook_function_array[hook_ke_user_mode_callback].ptargetfunc = (void *)KeUserModeCallback;
    g_hook_function_array[hook_ke_user_mode_callback].phookfunc   = (void *)SzHookKeUserModeCallback;

    g_hook_function_array[hook_nt_create_section].bcanhook    = TRUE;
    g_hook_function_array[hook_nt_create_section].ptargetfunc = GetServiceAddress( ZwCreateSection );
    g_hook_function_array[hook_nt_create_section].phookfunc   = (void *)SzHookCreateSection;

    //g_hook_function_array[hook_nt_open_section].bcanhook    = TRUE;
    //g_hook_function_array[hook_nt_open_section].ptargetfunc = GetServiceAddress( ZwOpenSection );
    //g_hook_function_array[hook_nt_open_section].phookfunc   = (void *) SzHookOpenSection;


    WPOFF();

    //ZwTerminateProcess;
    if ( g_knl_current_offset.dwTerminateProcessSysId != -1 )
    {
        HOOK_SYSCALL_INDEX(g_knl_current_offset.dwTerminateProcessSysId, 
            &SzHookNtTerminateProcess, 
            &SysNtTerminateProcess);
    }

    //NtCreateThread
    if ( g_knl_current_offset.dwCreateThreadSysId != -1 )
    {
        HOOK_SYSCALL_INDEX(g_knl_current_offset.dwCreateThreadSysId, 
            &SzHookNtCreateThread, 
            &SysNtCreateThread);
    }

    if ( g_knl_current_offset.dwNtReadVirtualMemorySysId != -1 )
    {
        HOOK_SYSCALL_INDEX(g_knl_current_offset.dwNtReadVirtualMemorySysId, 
            &SzHookNtReadVirtualMemory, 
            &SysNtReadVirtualMemory);
    }

    if ( g_knl_current_offset.dwNtWriteVirtualMemorySysId != -1 )
    {
        HOOK_SYSCALL_INDEX(g_knl_current_offset.dwNtWriteVirtualMemorySysId, 
            &SzHookNtWriteVirtualMemory, 
            &SysNtWriteVirtualMemory);
    }


    SysNtUserQueryWindow = (NTUSERQUERYWINDOW)KeServiceTable[g_knl_current_offset.dwNtUserQueryWindowSysId];

    if ( g_knl_current_offset.dwUserMessageCallSysId != -1 )
    {
        HOOK_SYSCALL_SHADOW_INDEX(g_knl_current_offset.dwUserMessageCallSysId, 
            &SzHookNtUserMessageCall, 
            &SysNtUserMessageCall);
    }

    if ( g_knl_current_offset.dwUserSendInputSysId != -1 )
    {
        HOOK_SYSCALL_SHADOW_INDEX(g_knl_current_offset.dwUserSendInputSysId, 
            &SzHookNtUserSendInput, 
            &SysNtUserSendInput);
    }

    if ( g_knl_current_offset.dwUserPostMessageSysId != -1 )
    {
        HOOK_SYSCALL_SHADOW_INDEX(g_knl_current_offset.dwUserPostMessageSysId, 
            &SzHookNtUserPostMessage, 
            &SysNtUserPostMessage);
    }

    if ( g_knl_current_offset.dwUserPostThreadMessageSysId != -1 )
    {
        HOOK_SYSCALL_SHADOW_INDEX(g_knl_current_offset.dwUserPostThreadMessageSysId, 
            &SzHookNtUserPostThreadMessage, 
            &SysNtUserPostThreadMessage);
    }


    WPON();


    bResult = TRUE;

exit0:

    KeDetachProcess();

    return bResult;
}

BOOL uninit_hook()
{
    //if ( g_knl_current_offset.dwTerminateProcessSysId != -1 )
    //{
    //    UNHOOK_SYSCALL_INDEX(g_knl_current_offset.dwTerminateProcessSysId, 
    //        SysNtTerminateProcess);
    //}

    //if ( g_knl_current_offset.dwCreateThreadSysId != -1 )
    //{
    //    UNHOOK_SYSCALL_INDEX(g_knl_current_offset.dwCreateThreadSysId, 
    //        SysNtCreateThread);
    //}

    //if ( g_knl_current_offset.dwNtReadVirtualMemorySysId != -1 )
    //{
    //    UNHOOK_SYSCALL_INDEX(g_knl_current_offset.dwNtReadVirtualMemorySysId, 
    //        &SysNtReadVirtualMemory);
    //}

    //if ( g_knl_current_offset.dwNtWriteVirtualMemorySysId != -1 )
    //{
    //    UNHOOK_SYSCALL_INDEX(g_knl_current_offset.dwNtWriteVirtualMemorySysId, 
    //        &SysNtWriteVirtualMemory);
    //}


    return TRUE;
}


BOOL install_all_hook()
{
    int nLoop = 0;

    init_hook();

    for (nLoop = 0; nLoop < hook_function_count; nLoop ++)
    {
        if (g_hook_function_array[nLoop].bcanhook && g_hook_function_array[nLoop].ptargetfunc == NULL)
        {
            g_hook_function_array[nLoop].bcanhook = FALSE;
        }
    }

    for (nLoop = 0; nLoop < hook_function_count; nLoop ++)
    {
        if (g_hook_function_array[nLoop].bcanhook == TRUE)
        {
            install_hook( &g_hook_function_array[nLoop] );
        }
    }

    return TRUE;

}

BOOL uninstall_all_hook()
{
    LARGE_INTEGER liSleepTime;
    int nLoop;

    uninit_hook();

    for (nLoop = 0; nLoop < hook_function_count; nLoop ++)
    {
        if (g_hook_function_array[nLoop].bcanhook == TRUE)
        {
            uninstall_hook( &g_hook_function_array[nLoop] );
        }
    }

    liSleepTime.QuadPart = - 2/*seconds*/ *1000*1000*10 ;

    KeDelayExecutionThread (KernelMode, TRUE, &liSleepTime) ;


    return TRUE;
}


PROC get_stub_func_address( em_hook_func_type funcid )
{
    if (funcid >= hook_function_count)
        return NULL;

    return (PROC)g_hook_function_array[funcid].pstubbuf;
}

