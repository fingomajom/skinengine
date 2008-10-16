#include "precomp.h"
#pragma hdrstop


void CreateProcessNotify(
    HANDLE ParentId,
    HANDLE ProcessId,
    BOOLEAN Create)
{

    DbgPrint( ("ParentId = 0x%08x  ProcessId = 0x%08x Create = %d ", ParentId, ProcessId, Create) );

}


NTSTATUS InitProcessNotify()
{
    NTSTATUS ntStatus = PsSetCreateProcessNotifyRoutine(CreateProcessNotify, FALSE);
    
    DbgPrint( ("InitProcessNotify") );

    return ntStatus;
}

//////////////////////////////////////////////////////////////////////////

NTSTATUS UninitProcessNotify()
{
    NTSTATUS ntStatus = PsSetCreateProcessNotifyRoutine(CreateProcessNotify, TRUE);

    DbgPrint( ("UninitProcessNotify") );

    return ntStatus;
}



NTSTATUS NTAPI SzHookKeUserModeCallback(
    ULONG  ApiNumber, 
    PVOID  InputBuffer, 
    ULONG  InputLength, 
    PVOID *OutputBuffer,
    PULONG OutputLength)
{
    NTSTATUS ntResult = STATUS_ACCESS_DENIED;

    PROC     pfnStubFunc   = NULL;
    UINT     uCurProcessId = 0;


    const int ulApiWatch = 66; // ClientLoadLibrary;

    uCurProcessId = (UINT)PsGetCurrentProcessId();
    pfnStubFunc   = get_stub_func_address(hook_ke_user_mode_callback);

    if ( ApiNumber == ulApiWatch )
    {
        DbgPrint ( ("HookKeUserModeCallback ClientLoadLibrary PID=%d", uCurProcessId ) );
    }

    ntResult = (NTSTATUS)pfnStubFunc(ApiNumber, InputBuffer, InputLength, OutputBuffer, OutputLength);


    return ntResult;

}


NTSTATUS NTAPI SzHookNtOpenProcess(
    PHANDLE     ProcessHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PCLIENT_ID  ClientId)
{
    NTSTATUS ntResult = STATUS_ACCESS_DENIED;

    PROC     pfnStubFunc   = NULL;
    UINT     uCurProcessId = 0;

    uCurProcessId = (UINT)PsGetCurrentProcessId();
    pfnStubFunc   = get_stub_func_address(hook_nt_open_process);


    //DbgPrint ( ("HookNtOpenProcess  PID=%d", uCurProcessId ) );

    ntResult = (NTSTATUS)pfnStubFunc(ProcessHandle, DesiredAccess, ObjectAttributes, ClientId );


    return ntResult;
}

NTSTATUS NTAPI SzHookQuerySystemInformation(
    ULONG SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength)
{
    NTSTATUS ntResult = STATUS_ACCESS_DENIED;

    PROC     pfnStubFunc   = NULL;
    UINT     uCurProcessId = 0;

    uCurProcessId = (UINT)PsGetCurrentProcessId();
    pfnStubFunc   = get_stub_func_address(hook_query_system_information);


    //DbgPrint ( ("SzHookQuerySystemInformation  PID=%d", uCurProcessId ) );

    ntResult = (NTSTATUS)pfnStubFunc(SystemInformationClass, 
        SystemInformation, 
        SystemInformationLength, 
        ReturnLength );


    return ntResult;
}


NTSTATUS NTAPI SzHookCreateSection(
    PHANDLE SectionHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PLARGE_INTEGER MaximumSize,
    ULONG SectionPageProtection,
    ULONG AllocationAttributes,
    HANDLE FileHandle)
{
    NTSTATUS ntResult = STATUS_ACCESS_DENIED;

    PROC     pfnStubFunc   = NULL;
    UINT     uCurProcessId = 0;

    uCurProcessId = (UINT)PsGetCurrentProcessId();
    pfnStubFunc   = get_stub_func_address(hook_nt_create_section);


    //DbgPrint ( ("SzHookCreateSection  PID=%d", uCurProcessId ) );

    ntResult = (NTSTATUS)pfnStubFunc(
        SectionHandle,
        DesiredAccess,
        ObjectAttributes,
        MaximumSize,
        SectionPageProtection,
        AllocationAttributes,
        FileHandle);

    return ntResult;
}


NTSTATUS NTAPI SzHookOpenSection(
    OUT PHANDLE SectionHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes )
{
    NTSTATUS ntResult = STATUS_ACCESS_DENIED;

    PROC     pfnStubFunc   = NULL;
    UINT     uCurProcessId = 0;

    uCurProcessId = (UINT)PsGetCurrentProcessId();
    pfnStubFunc   = get_stub_func_address(hook_nt_open_section);


    DbgPrint ( ("SzHookOpenSection  PID=%d", uCurProcessId ) );

    ntResult = (NTSTATUS)pfnStubFunc(
        SectionHandle,
        DesiredAccess,
        ObjectAttributes);

    return ntResult;
}

NTSTATUS NTAPI SzHookNtTerminateProcess(
    IN HANDLE   ProcessHandle OPTIONAL,
    IN NTSTATUS ExitStatus)
{
    DbgPrint ( ("SzHookNtTerminateProcess  PID=%d", (UINT)PsGetCurrentProcessId() ) );

    return SysNtTerminateProcess( ProcessHandle, ExitStatus );
}

NTSTATUS SzHookNtCreateThread(
    OUT PHANDLE             ThreadHandle,
    IN ACCESS_MASK          DesiredAccess,
    IN POBJECT_ATTRIBUTES   ObjectAttributes OPTIONAL,
    IN HANDLE               ProcessHandle,
    OUT PCLIENT_ID          ClientId,
    IN PCONTEXT             ThreadContext,
    IN PINITIAL_TEB         InitialTeb,
    IN BOOLEAN              CreateSuspended)
{
    //DbgPrint ( ("SzHookNtCreateThread  PID=%d", (UINT)PsGetCurrentProcessId() ) );

    return SysNtCreateThread( 
        ThreadHandle,
        DesiredAccess,
        ObjectAttributes,
        ProcessHandle,
        ClientId,
        ThreadContext,
        InitialTeb,
        CreateSuspended);
}


NTSTATUS SzHookNtReadVirtualMemory (
    IN  HANDLE      ProcessHandle,
    IN  PVOID       BaseAddress,
    OUT PVOID       Buffer,
    IN  ULONG       BufferLength,
    OUT PULONG      ReturnLength OPTIONAL)
{
    DbgPrint ( ("SzHookNtReadVirtualMemory  PID=%d", (UINT)PsGetCurrentProcessId() ) );

    return SysNtReadVirtualMemory( 
        ProcessHandle,
        BaseAddress,
        Buffer,
        BufferLength,
        ReturnLength);
}

NTSTATUS SzHookNtWriteVirtualMemory(
    IN HANDLE       ProcessHandle,
    IN PVOID        BaseAddress,
    IN PVOID        Buffer,
    IN ULONG        BufferLength,
    OUT PULONG      ReturnLength OPTIONAL)
{
    DbgPrint ( ("SzHookNtWriteVirtualMemory  PID=%d", (UINT)PsGetCurrentProcessId() ) );

    return SysNtWriteVirtualMemory( 
        ProcessHandle,
        BaseAddress,
        Buffer,
        BufferLength,
        ReturnLength);
}

