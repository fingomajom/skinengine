#include "precomp.h"
#pragma hdrstop


hook_item_info g_hook_function_array[ hook_function_count ] = { 0 };



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



BOOL init_hook()
{
    RtlZeroMemory(g_hook_function_array, sizeof(g_hook_function_array));

    //  进程类
    g_hook_function_array[hook_nt_open_process].bcanhook    = TRUE;
    g_hook_function_array[hook_nt_open_process].ptargetfunc = (void *)NtOpenProcess;
    g_hook_function_array[hook_nt_open_process].phookfunc   = (void *)SzHookNtOpenProcess;


    g_hook_function_array[hook_query_system_information].bcanhook    = TRUE;
    g_hook_function_array[hook_query_system_information].ptargetfunc = GetServiceAddress( ZwQuerySystemInformation );
    g_hook_function_array[hook_query_system_information].phookfunc   = (void *)SzHookQuerySystemInformation;


    //ZwTerminateProcess;
    if ( g_knl_current_offset.dwTerminateProcessSysId != -1 )
    {
        HOOK_SYSCALL_INDEX(g_knl_current_offset.dwTerminateProcessSysId, 
            &SzHookNtTerminateProcess, 
            &SysNtTerminateProcess);
    }


    //// 防注入
    g_hook_function_array[hook_ke_user_mode_callback].bcanhook    = TRUE;
    g_hook_function_array[hook_ke_user_mode_callback].ptargetfunc = (void *)KeUserModeCallback;
    g_hook_function_array[hook_ke_user_mode_callback].phookfunc   = (void *)SzHookKeUserModeCallback;

    g_hook_function_array[hook_nt_create_section].bcanhook    = TRUE;
    g_hook_function_array[hook_nt_create_section].ptargetfunc = GetServiceAddress( ZwCreateSection );
    g_hook_function_array[hook_nt_create_section].phookfunc   = (void *)SzHookCreateSection;


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


    g_hook_function_array[hook_nt_open_section].bcanhook    = TRUE;
    g_hook_function_array[hook_nt_open_section].ptargetfunc = GetServiceAddress( ZwOpenSection );
    g_hook_function_array[hook_nt_open_section].phookfunc   = (void *) SzHookOpenSection;

    return TRUE;
}

BOOL uninit_hook()
{
    if ( g_knl_current_offset.dwTerminateProcessSysId != -1 )
    {
        UNHOOK_SYSCALL_INDEX(g_knl_current_offset.dwTerminateProcessSysId, 
            SysNtTerminateProcess);
    }

    if ( g_knl_current_offset.dwCreateThreadSysId != -1 )
    {
        UNHOOK_SYSCALL_INDEX(g_knl_current_offset.dwCreateThreadSysId, 
            SysNtCreateThread);
    }

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

