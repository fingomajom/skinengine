/********************************************************************
	created:	2008/10/24
	created:	24:10:2008   16:32
	filename: 	d:\lidengwang\lse\trunk\src\driver\hookpointmgt.h
	file path:	d:\lidengwang\lse\trunk\src\driver
	file base:	hookpointmgt
	file ext:	h
	author:		lidengwang
	
	purpose:	
*********************************************************************/

#pragma once


BOOL install_all_hook();
BOOL uninstall_all_hook();


typedef enum{

    hook_nt_open_process = 0,

    hook_query_system_information = 1,

    hook_ke_user_mode_callback = 2,

    hook_nt_create_section = 3,

    hook_nt_open_section = 3,

    hook_function_count

} em_hook_func_type ;


PROC get_stub_func_address( em_hook_func_type funcid );



typedef enum _SECTION_INFORMATION_CLASS
{
    SectionBasicInformation,
    SectionImageInformation,
}SECTION_INFORMATION_CLASS;

typedef struct _INITIAL_TEB 
{
    PVOID					StackBase;
    PVOID					StackLimit;
    PVOID					StackCommit;
    PVOID					StackCommitMax;
    PVOID					StackReserved;
} INITIAL_TEB, *PINITIAL_TEB;

//////////////////////////////////////////////////////////////////////////


NTSTATUS NTAPI SzHookNtOpenProcess(
    PHANDLE     ProcessHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PCLIENT_ID  ClientId);

NTSTATUS NTAPI SzHookQuerySystemInformation(
    ULONG SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength);

NTSTATUS NTAPI SzHookKeUserModeCallback(
    ULONG  ApiNumber, 
    PVOID  InputBuffer, 
    ULONG  InputLength, 
    PVOID *OutputBuffer,
    PULONG OutputLength);

NTSTATUS NTAPI SzHookCreateSection(
    PHANDLE SectionHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PLARGE_INTEGER MaximumSize,
    ULONG SectionPageProtection,
    ULONG AllocationAttributes,
    HANDLE FileHandle);

NTSTATUS NTAPI SzHookOpenSection(
    OUT PHANDLE SectionHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes );


//////////////////////////////////////////////////////////////////////////


typedef NTSTATUS ( NTAPI * PNTTERMINATEPROCESS )(
    IN HANDLE   ProcessHandle OPTIONAL,
    IN NTSTATUS ExitStatus);


NTSTATUS NTAPI SzHookNtTerminateProcess(
    IN HANDLE   ProcessHandle OPTIONAL,
    IN NTSTATUS ExitStatus);

PNTTERMINATEPROCESS SysNtTerminateProcess;


typedef NTSTATUS ( *PNTCREATETHREAD )(
    OUT PHANDLE             ThreadHandle,
    IN ACCESS_MASK          DesiredAccess,
    IN POBJECT_ATTRIBUTES   ObjectAttributes OPTIONAL,
    IN HANDLE               ProcessHandle,
    OUT PCLIENT_ID          ClientId,
    IN PCONTEXT             ThreadContext,
    IN PINITIAL_TEB         InitialTeb,
    IN BOOLEAN              CreateSuspended );

NTSTATUS SzHookNtCreateThread(
    OUT PHANDLE             ThreadHandle,
    IN ACCESS_MASK          DesiredAccess,
    IN POBJECT_ATTRIBUTES   ObjectAttributes OPTIONAL,
    IN HANDLE               ProcessHandle,
    OUT PCLIENT_ID          ClientId,
    IN PCONTEXT             ThreadContext,
    IN PINITIAL_TEB         InitialTeb,
    IN BOOLEAN              CreateSuspended);

PNTCREATETHREAD SysNtCreateThread;

typedef NTSTATUS ( *NTREADVIRTUALMEMORY ) (
    IN  HANDLE      ProcessHandle,
    IN  PVOID       BaseAddress,
    OUT PVOID       Buffer,
    IN  ULONG       BufferLength,
    OUT PULONG      ReturnLength OPTIONAL);

NTSTATUS SzHookNtReadVirtualMemory (
    IN  HANDLE      ProcessHandle,
    IN  PVOID       BaseAddress,
    OUT PVOID       Buffer,
    IN  ULONG       BufferLength,
    OUT PULONG      ReturnLength OPTIONAL);

NTREADVIRTUALMEMORY SysNtReadVirtualMemory;


typedef NTSTATUS ( *NTWRITEVIRTUALMEMORY )(
    IN  HANDLE      ProcessHandle,
    IN  PVOID       BaseAddress,
    IN  PVOID       Buffer,
    IN  ULONG       BufferLength,
    OUT PULONG      ReturnLength OPTIONAL);

NTSTATUS SzHookNtWriteVirtualMemory(
    IN HANDLE       ProcessHandle,
    IN PVOID        BaseAddress,
    IN PVOID        Buffer,
    IN ULONG        BufferLength,
    OUT PULONG      ReturnLength OPTIONAL);

NTWRITEVIRTUALMEMORY SysNtWriteVirtualMemory;
