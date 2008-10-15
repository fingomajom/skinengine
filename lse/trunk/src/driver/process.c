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



NTSTATUS NTAPI HookKeUserModeCallback(
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


NTSTATUS NTAPI HookNtOpenProcess(
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


    DbgPrint ( ("HookNtOpenProcess  PID=%d", uCurProcessId ) );

    ntResult = (NTSTATUS)pfnStubFunc(ProcessHandle, DesiredAccess, ObjectAttributes, ClientId );


    return ntResult;
}
