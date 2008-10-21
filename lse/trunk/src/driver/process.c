#include "precomp.h"
#pragma hdrstop



#define PROCESS_TERMINATE         (0x0001)  
#define PROCESS_CREATE_THREAD     (0x0002)  
#define PROCESS_SET_SESSIONID     (0x0004)  
#define PROCESS_VM_OPERATION      (0x0008)  
#define PROCESS_VM_READ           (0x0010)  
#define PROCESS_VM_WRITE          (0x0020)  
#define PROCESS_DUP_HANDLE        (0x0040)  
#define PROCESS_CREATE_PROCESS    (0x0080)  
#define PROCESS_SET_QUOTA         (0x0100)  
#define PROCESS_SET_INFORMATION   (0x0200)  
#define PROCESS_QUERY_INFORMATION (0x0400)  
#define PROCESS_SUSPEND_RESUME    (0x0800)  



void CreateProcessNotify(
    HANDLE ParentId,
    HANDLE ProcessId,
    BOOLEAN bCreate)
{
    KIRQL       OldIrql;
    PLIST_ENTRY MatchEntry = NULL;

    DbgPrint( ("ParentId = 0x%08x  ProcessId = 0x%08x Create = %d ", ParentId, ProcessId, bCreate) );


    if ( bCreate )
    {
        KeAcquireSpinLock(&g_RunPIDList.RuleLock, &OldIrql);
        
        MatchEntry = MatchingRule( &g_RunPIDList, CT_ID, &ParentId );

        KeReleaseSpinLock(&g_RunPIDList.RuleLock, OldIrql);

        if ( MatchEntry != NULL )
            AppendRule_I( &g_RunPIDList, CT_ID, TRUE, &ProcessId );
        else
        {

        }
    }
    else
    {
        KeAcquireSpinLock(&g_RunPIDList.RuleLock, &OldIrql);

        MatchEntry = MatchingRule( &g_RunPIDList, CT_ID, &ProcessId );
        if ( MatchEntry != NULL )
        {
            RemoveEntryList(MatchEntry);
            ExFreePool( MatchEntry );
        }

        KeReleaseSpinLock(&g_RunPIDList.RuleLock, OldIrql);
    }
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
    NTSTATUS ntResult = STATUS_SUCCESS;

    PROC     pfnStubFunc   = NULL;
    UINT     uCurProcessId = 0;

    UNICODE_STRING sPathSrcProcess = { 0 };

    const int ulApiWatch = 66; // ClientLoadLibrary;

    WCHAR wszDllPath[MAX_PATH] = { 0 };

    uCurProcessId = (UINT)PsGetCurrentProcessId();
    pfnStubFunc   = get_stub_func_address(hook_ke_user_mode_callback);

    if ( ApiNumber != ulApiWatch )
        goto call_default;


    do 
    {
        if ( !GetProcessFullPath( PsGetCurrentProcess(), &sPathSrcProcess ) )
            break;

        //DbgPrint (("SzHookKeUserModeCallback sPathSrcProcess = %ws", sPathSrcProcess.Buffer ));

        // �Ƿ���Ҫ��������
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) == NULL )
            break;

        if ( g_dwOsVersion == os_vista_sp0 ||
             g_dwOsVersion == os_vista_sp1 )
        {
            wcsncpy(wszDllPath, (WCHAR *) ((char *)InputBuffer + 0x30), InputLength - 0x30);
        }
        else
        {
            wcsncpy(wszDllPath, (WCHAR *) ((char *)InputBuffer + 0x28), InputLength - 0x28);
        }

        if ( MatchingRule( &g_WhiteRuleList, CT_PATHFILE, wszDllPath ) != NULL )
            break;

        // δ�����ں�������
        if ( MatchingRule( &g_BlackRuleList, CT_PATHFILE, wszDllPath ) != NULL )
            goto checkend;

        DbgPrint (("SzHookKeUserModeCallback [%ws] [%ws]", sPathSrcProcess.Buffer,wszDllPath ));

checkend:

        AppendEvent_I( &g_event_list,
            0, 
            uCurProcessId, sPathSrcProcess.Buffer,
            0, wszDllPath);

        goto skip_default;

    } while( 0 );


call_default:

    //if (ApiNumber == 66)
    //    DbgPrint (("->SzHookKeUserModeCallback return[0x%x] [0x%x] [0x%x]", ntResult, *OutputBuffer, *OutputLength));

    ntResult = (NTSTATUS)pfnStubFunc(ApiNumber, InputBuffer, InputLength, OutputBuffer, OutputLength);

    //if (ApiNumber == 66)
    //    DbgPrint (("-<SzHookKeUserModeCallback return[0x%x] [0x%x] [0x%x]", ntResult, *OutputBuffer, *OutputLength));

skip_default:

    if (sPathSrcProcess.Buffer != NULL) ExFreePool(sPathSrcProcess.Buffer);

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

    UNICODE_STRING sPathSrcProcess = { 0 };
    UNICODE_STRING sPathTagProcess = { 0 };

    uCurProcessId = (UINT)PsGetCurrentProcessId();
    pfnStubFunc   = get_stub_func_address(hook_nt_open_process);
    

    do 
    {
        if ( !(DesiredAccess & PROCESS_TERMINATE)      &&
             !(DesiredAccess & PROCESS_SUSPEND_RESUME) &&
             !(DesiredAccess & PROCESS_CREATE_THREAD) )
        {
             break;
        }

        DbgPrint (("SzHookNtOpenProcess DesiredAccess = 0x%x", DesiredAccess ));

        if ( ProcessHandle == ClientId->UniqueProcess )
            break;

        if ( !GetProcessFullPathByID( (UINT)ClientId->UniqueProcess, &sPathTagProcess ) )
            break;

        DbgPrint (("SzHookNtOpenProcess sPathTagProcess = %ws", sPathTagProcess.Buffer ));


        // �Ƿ���Ҫ��������
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathTagProcess.Buffer ) == NULL )
            break;

        DbgPrint (("SzHookNtOpenProcess g_ProtectRuleList MatchingRule = %ws ", sPathTagProcess.Buffer ));


        if ( !GetProcessFullPath( PsGetCurrentProcess(), &sPathSrcProcess ) )
            break;

        if ( MatchingRule( &g_WhiteRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            break;
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            break;

        // δ�����ں�������
        if ( MatchingRule( &g_BlackRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            goto skipdefault;

        if (sPathSrcProcess.Buffer != NULL && sPathTagProcess.Buffer != NULL)
        {
            DbgPrint (("SzHookNtOpenProcess skipdefault cur = %ws, tag = %ws", sPathSrcProcess.Buffer, sPathTagProcess.Buffer ));
        }


        goto skipdefault;

    } while( 0 );

    ntResult = (NTSTATUS)pfnStubFunc(ProcessHandle, DesiredAccess, ObjectAttributes, ClientId );

skipdefault:

    if (sPathSrcProcess.Buffer != NULL) ExFreePool(sPathSrcProcess.Buffer);
    if (sPathTagProcess.Buffer != NULL) ExFreePool(sPathTagProcess.Buffer);

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
    NTSTATUS ntResult  = STATUS_ACCESS_DENIED;
    NTSTATUS ntRetCode = STATUS_ACCESS_DENIED;

    PROC     pfnStubFunc   = NULL;
    UINT     uCurProcessId = 0;

    ULONG	CheckDesireAccess = SECTION_MAP_EXECUTE ;
    ULONG	CheckSectionPageProtection = PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;


    UNICODE_STRING sPathSrcProcess = { 0 };
    UNICODE_STRING sPathTagProcess = { 0 };

    UNICODE_STRING DriverName = { 0 };
    UNICODE_STRING FullName   = { 0 };

    WCHAR wszFullName[MAX_PATH] = { 0 };

    PFILE_OBJECT pFileObject = NULL;


    uCurProcessId = (UINT)PsGetCurrentProcessId();
    pfnStubFunc   = get_stub_func_address(hook_nt_create_section);

    do 
    {
        if ( !(DesiredAccess & CheckDesireAccess)      &&
             !(SectionPageProtection & CheckSectionPageProtection) && 0 )
        {
            break;
        }

        if ( !GetProcessFullPath( PsGetCurrentProcess(), &sPathSrcProcess ) )
            break;

        DbgPrint (("SzHookCreateSection sPathSrcProcess = %ws", sPathSrcProcess.Buffer ));

        if ( MatchingRule( &g_WhiteRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            break;
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            break;

        // δ�����ں�������
        if ( MatchingRule( &g_BlackRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            goto skipdefault;

        ntRetCode = ObReferenceObjectByHandle(FileHandle, 0, 0, KernelMode, &pFileObject, NULL);
        if ( !NT_SUCCESS(ntRetCode) ||
              pFileObject == NULL ||
              pFileObject->FileName.Length == 0 ||
              pFileObject->FileName.Buffer == NULL)
            break;

        ntRetCode = RtlVolumeDeviceToDosName(pFileObject->DeviceObject, &DriverName);
        if ( !NT_SUCCESS(ntRetCode) ||
              DriverName.Length == 0 || 
              DriverName.Buffer == NULL)
            break;

        if ( (DriverName.Length + pFileObject->FileName.Length) > MAX_PATH * 2)
            break;

        wcsncpy(wszFullName, DriverName.Buffer, MAX_PATH);
        wcsncpy(wszFullName + wcslen(wszFullName), pFileObject->FileName.Buffer, MAX_PATH);

        wszFullName[MAX_PATH - 1] = L'\0';

        RtlInitUnicodeString(&FullName, wszFullName);
        FullName.Length += sizeof(WCHAR);

        DbgPrint (("SzHookCreateSection FullName = %ws", FullName.Buffer ));


    } while( 0 );


    ntResult = (NTSTATUS)pfnStubFunc(
        SectionHandle,
        DesiredAccess,
        ObjectAttributes,
        MaximumSize,
        SectionPageProtection,
        AllocationAttributes,
        FileHandle);

skipdefault:

    if (sPathSrcProcess.Buffer != NULL) ExFreePool(sPathSrcProcess.Buffer);
    if (sPathTagProcess.Buffer != NULL) ExFreePool(sPathTagProcess.Buffer);

    if (DriverName.Buffer != NULL) ExFreePool(DriverName.Buffer);
    if (FullName.Buffer != NULL) ExFreePool(FullName.Buffer);

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


    //DbgPrint ( ("SzHookOpenSection  PID=%d", uCurProcessId ) );

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

    int nResult = STATUS_ACCESS_DENIED;

    PEPROCESS pEprocess = NULL;

    //DbgPrint ( ("SzHookNtTerminateProcess  PID=%d %d", (UINT)PsGetCurrentProcessId(), ProcessHandle ) );

    UNICODE_STRING sPathSrcProcess = { 0 };
    UNICODE_STRING sPathTagProcess = { 0 };

    KPROCESSOR_MODE mode = ExGetPreviousMode();
    
    do 
    {
        if ( !GetProcessFullPath( PsGetCurrentProcess(), &sPathSrcProcess ) )
            break;

        DbgPrint (("SzHookNtTerminateProcess sPathSrcProcess = %ws", sPathSrcProcess.Buffer ));


        if ( MatchingRule( &g_WhiteRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            break;
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            break;

        // δ�����ں�������
        if ( MatchingRule( &g_BlackRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            goto skipdefault;


        pEprocess = GetEProcessByHandle(ProcessHandle);
        if (pEprocess == NULL)
            break;

        if ( !GetProcessFullPath( pEprocess, &sPathTagProcess ) )
            break;

        DbgPrint (("SzHookNtTerminateProcess sPathTagProcess = %ws", sPathTagProcess.Buffer ));


        // �Ƿ���Ҫ��������
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathTagProcess.Buffer ) != NULL )
        {
            if ( g_dwOsVersion == os_vista_sp0 ||
                g_dwOsVersion == os_vista_sp1)
            {
                if (0xC000042C == ExitStatus)  // STATUS_ELEVATION_REQUIRED
                {
                    // vista UAC��Ȩ
                    goto call_default;
                }
                else if (mode == KernelMode )
                {
                    goto call_default;
                }
            }

            goto skipdefault;
        }


    } while( 0 );

call_default:

    nResult = SysNtTerminateProcess( ProcessHandle, ExitStatus );

skipdefault:

    if (sPathSrcProcess.Buffer != NULL) ExFreePool(sPathSrcProcess.Buffer);
    if (sPathTagProcess.Buffer != NULL) ExFreePool(sPathTagProcess.Buffer);


    return nResult;
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

    int nResult = STATUS_ACCESS_DENIED;

    PEPROCESS pEprocess = NULL;

    UNICODE_STRING sPathSrcProcess = { 0 };
    UNICODE_STRING sPathTagProcess = { 0 };

    KPROCESSOR_MODE mode = ExGetPreviousMode();

    do 
    {
        pEprocess = GetEProcessByHandle(ProcessHandle);
        if (pEprocess == NULL)
            break;

        if ( PsGetCurrentProcess() == pEprocess )
            break;

        if ( !GetProcessFullPath( PsGetCurrentProcess(), &sPathSrcProcess ) )
            break;

        if ( !GetProcessFullPath( pEprocess, &sPathTagProcess ) )
            break;

        DbgPrint (("SzHookNtCreateThread sPathSrcProcess = %ws %ws", sPathSrcProcess.Buffer, sPathTagProcess.Buffer ));


        // �Ƿ���Ҫ��������
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathTagProcess.Buffer ) == NULL )
            break;

        if ( MatchingRule( &g_WhiteRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            break;
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            break;

        // δ�����ں�������
        if ( MatchingRule( &g_BlackRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            goto skipdefault;
        
        goto skipdefault;

    } while( 0 );


    nResult = SysNtCreateThread( 
        ThreadHandle,
        DesiredAccess,
        ObjectAttributes,
        ProcessHandle,
        ClientId,
        ThreadContext,
        InitialTeb,
        CreateSuspended);

skipdefault:

    if (sPathSrcProcess.Buffer != NULL) ExFreePool(sPathSrcProcess.Buffer);
    if (sPathTagProcess.Buffer != NULL) ExFreePool(sPathTagProcess.Buffer);

    return nResult;
}


NTSTATUS SzHookNtReadVirtualMemory (
    IN  HANDLE      ProcessHandle,
    IN  PVOID       BaseAddress,
    OUT PVOID       Buffer,
    IN  ULONG       BufferLength,
    OUT PULONG      ReturnLength OPTIONAL)
{
    int nResult = STATUS_ACCESS_DENIED;

    PEPROCESS pEprocess = NULL;

    UNICODE_STRING sPathSrcProcess = { 0 };
    UNICODE_STRING sPathTagProcess = { 0 };

    KPROCESSOR_MODE mode = ExGetPreviousMode();

    do 
    {
        pEprocess = GetEProcessByHandle(ProcessHandle);
        if (pEprocess == NULL)
            break;

        if ( PsGetCurrentProcess() == pEprocess )
            break;

        if ( !GetProcessFullPath( PsGetCurrentProcess(), &sPathSrcProcess ) )
            break;

        DbgPrint (("SzHookNtReadVirtualMemory sPathSrcProcess = %ws %ws", sPathSrcProcess.Buffer, sPathTagProcess.Buffer ));

        if ( !GetProcessFullPath( pEprocess, &sPathTagProcess ) )
            break;

        // �Ƿ���Ҫ��������
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathTagProcess.Buffer ) == NULL )
            break;

        if ( MatchingRule( &g_WhiteRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            break;
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            break;

        // δ�����ں�������
        if ( MatchingRule( &g_BlackRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            goto skipdefault;

    } while( 0 );


    nResult = SysNtReadVirtualMemory( 
        ProcessHandle,
        BaseAddress,
        Buffer,
        BufferLength,
        ReturnLength);

skipdefault:

    if (sPathSrcProcess.Buffer != NULL) ExFreePool(sPathSrcProcess.Buffer);
    if (sPathTagProcess.Buffer != NULL) ExFreePool(sPathTagProcess.Buffer);

    return nResult;

}

NTSTATUS SzHookNtWriteVirtualMemory(
    IN HANDLE       ProcessHandle,
    IN PVOID        BaseAddress,
    IN PVOID        Buffer,
    IN ULONG        BufferLength,
    OUT PULONG      ReturnLength OPTIONAL)
{
    int nResult = STATUS_ACCESS_DENIED;

    PEPROCESS pEprocess = NULL;

    UNICODE_STRING sPathSrcProcess = { 0 };
    UNICODE_STRING sPathTagProcess = { 0 };

    KPROCESSOR_MODE mode = ExGetPreviousMode();

    do 
    {
        pEprocess = GetEProcessByHandle(ProcessHandle);
        if (pEprocess == NULL)
            break;

        if ( PsGetCurrentProcess() == pEprocess )
            break;

        if ( !GetProcessFullPath( PsGetCurrentProcess(), &sPathSrcProcess ) )
            break;

        DbgPrint (("SzHookNtWriteVirtualMemory sPathSrcProcess = %ws %ws", sPathSrcProcess.Buffer, sPathTagProcess.Buffer ));

        if ( !GetProcessFullPath( pEprocess, &sPathTagProcess ) )
            break;

        // �Ƿ���Ҫ��������
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathTagProcess.Buffer ) == NULL )
            break;

        if ( MatchingRule( &g_WhiteRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            break;
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            break;

        // δ�����ں�������
        if ( MatchingRule( &g_BlackRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            goto skipdefault;

    } while( 0 );


    nResult = SysNtWriteVirtualMemory( 
        ProcessHandle,
        BaseAddress,
        Buffer,
        BufferLength,
        ReturnLength);

skipdefault:

    if (sPathSrcProcess.Buffer != NULL) ExFreePool(sPathSrcProcess.Buffer);
    if (sPathTagProcess.Buffer != NULL) ExFreePool(sPathTagProcess.Buffer);

    return nResult;

}

