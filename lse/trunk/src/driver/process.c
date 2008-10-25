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

    UNICODE_STRING sPathSrcProcess =    { 0, 0, 0 };
    UNICODE_STRING sPathParentProcess = { 0, 0, 0 };
    UNICODE_STRING sPathTagProcess =    { 0, 0, 0 };

    if ( g_lDriverStatus != DRIVER_STATUS_RUNNING )
        return;

    if ( !GetProcessFullPath( PsGetCurrentProcess(), &sPathSrcProcess ))
        ;

    if ( !GetProcessFullPathByID( (UINT)ParentId, &sPathParentProcess ) )
        ;//return;

    if ( !GetProcessFullPathByID( (UINT)ProcessId, &sPathTagProcess ) )
        ;//return;

    DbgPrint( ("%s SID = %d PPID=%d  PID = %d SID = %ws PPID=%ws PID=%ws", 
        bCreate ? "CreateP" : "DestoryP", 
        PsGetCurrentProcessId(), ParentId, ProcessId, 
        sPathSrcProcess.Buffer, 
        sPathParentProcess.Buffer, 
        sPathTagProcess.Buffer) );


    if ( bCreate )
    {
        AppendEvent_I( &g_event_list,
            LOG_TYPE_CREATE_PROCESS, 
            (UINT)ParentId , sPathSrcProcess.Buffer,
            (UINT)ProcessId, sPathTagProcess.Buffer);       

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

    UNICODE_STRING sPathSrcProcess = { 0, 0, 0 };

    const int ulApiWatch = 66; // ClientLoadLibrary;

    WCHAR wszDllPath[MAX_PATH] = { 0 };

    uCurProcessId = (UINT)PsGetCurrentProcessId();
    pfnStubFunc   = get_stub_func_address(hook_ke_user_mode_callback);

    if ( g_lDriverStatus != DRIVER_STATUS_RUNNING )
        goto call_default;

    if ( ApiNumber != ulApiWatch )
        goto call_default;

    do 
    {
        if ( !GetProcessFullPath( PsGetCurrentProcess(), &sPathSrcProcess ) )
            break;

        DbgPrint (("SzHookKeUserModeCallback sPathSrcProcess = %ws", sPathSrcProcess.Buffer ));

        if ( g_dwOsVersion == os_vista_sp0 ||
             g_dwOsVersion == os_vista_sp1 )
        {
            wcsncpy(wszDllPath, (WCHAR *) ((char *)InputBuffer + 0x30), InputLength - 0x30);
        }
        else
        {
            wcsncpy(wszDllPath, (WCHAR *) ((char *)InputBuffer + 0x28), InputLength - 0x28);
        }

        //////////////////////////////////////////////////////////////////////////
        //
        AppendEvent_I( &g_event_list,
            LOG_TYPE_USERMODECALLBACK, 
            uCurProcessId, sPathSrcProcess.Buffer,
            uCurProcessId, wszDllPath);
        //////////////////////////////////////////////////////////////////////////

        // 是否是要保护对象
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) == NULL )
            break;

        if ( MatchingRule( &g_WhiteRuleList, CT_PATHFILE, wszDllPath ) != NULL )
            break;

        // 未出现在黑名单中
        if ( MatchingRule( &g_BlackRuleList, CT_PATHFILE, wszDllPath ) != NULL )
            goto checkend;

        DbgPrint (("SzHookKeUserModeCallback [%ws] [%ws]", sPathSrcProcess.Buffer,wszDllPath ));

checkend:

        if ( g_drv_config.usEnableBlockUnknown )
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

    UNICODE_STRING sPathSrcProcess = { 0, 0, 0 };
    UNICODE_STRING sPathTagProcess = { 0, 0, 0 };


    uCurProcessId = (UINT)PsGetCurrentProcessId();
    pfnStubFunc   = get_stub_func_address(hook_nt_open_process);

    if ( g_lDriverStatus != DRIVER_STATUS_RUNNING )
        goto call_default;

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

        if ( !GetProcessFullPath( PsGetCurrentProcess(), &sPathSrcProcess ) )
            break;

        //////////////////////////////////////////////////////////////////////////
        //
        AppendEvent_I( &g_event_list,
            LOG_TYPE_OPENPROCESS, 
            uCurProcessId , sPathSrcProcess.Buffer,
            (UINT)ClientId->UniqueProcess, sPathTagProcess.Buffer);       
        //////////////////////////////////////////////////////////////////////////

        DbgPrint (("SzHookNtOpenProcess sPathTagProcess = %ws", sPathTagProcess.Buffer ));

        // 是否是要保护对象
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathTagProcess.Buffer ) == NULL )
            break;

        DbgPrint (("SzHookNtOpenProcess g_ProtectRuleList MatchingRule = %ws ", sPathTagProcess.Buffer ));



        if ( MatchingRule( &g_WhiteRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            break;
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            break;

        // 未出现在黑名单中
        if ( MatchingRule( &g_BlackRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            goto skip_default;

        if (sPathSrcProcess.Buffer != NULL && sPathTagProcess.Buffer != NULL)
        {
            DbgPrint (("SzHookNtOpenProcess skipdefault cur = %ws, tag = %ws", sPathSrcProcess.Buffer, sPathTagProcess.Buffer ));
        }

        if ( g_drv_config.usEnableBlockUnknown )
            goto skip_default;

    } while( 0 );

call_default:

    ntResult = (NTSTATUS)pfnStubFunc(ProcessHandle, DesiredAccess, ObjectAttributes, ClientId );

skip_default:

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


    UNICODE_STRING sPathSrcProcess = { 0, 0, 0 };
    UNICODE_STRING sPathTagProcess = { 0, 0, 0 };

    SYSTEM_PROCESS_INFORMATION* pcurp  = NULL;
    SYSTEM_PROCESS_INFORMATION* pprevp = NULL;


    uCurProcessId = (UINT)PsGetCurrentProcessId();
    pfnStubFunc   = get_stub_func_address(hook_query_system_information);

    if ( g_lDriverStatus != DRIVER_STATUS_RUNNING )
    {
        return (NTSTATUS)pfnStubFunc(SystemInformationClass, 
            SystemInformation, 
            SystemInformationLength, 
            ReturnLength );
    }

    if ( SystemInformationClass == 5 )
    {
        if ( GetProcessFullPath( PsGetCurrentProcess(), &sPathSrcProcess ) )
        {
            if ( MatchingRule( &g_BlackRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
                return ntResult;
        }
    }

    ntResult = (NTSTATUS)pfnStubFunc(SystemInformationClass, 
        SystemInformation, 
        SystemInformationLength, 
        ReturnLength );

    if ( !g_drv_config.usEnableHideProcess )
        goto call_default;

    if ( SystemInformationClass != 5 || !NT_SUCCESS(ntResult) )
        goto call_default;

    if ( sPathSrcProcess.Buffer == NULL )
        goto call_default;

    //////////////////////////////////////////////////////////////////////////
    //
    AppendEvent_I( &g_event_list,
        LOG_TYPE_QUERYSYSTEMINFORMATION, 
        uCurProcessId , sPathSrcProcess.Buffer,
        0, NULL);       
    //////////////////////////////////////////////////////////////////////////

    
    if ( MatchingRule( &g_WhiteRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
        goto call_default;
    if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
        goto call_default;
    
    DbgPrint (("SzHookQuerySystemInformation sPathSrcProcess = %ws", sPathSrcProcess.Buffer ));


    pcurp = (SYSTEM_PROCESS_INFORMATION *)SystemInformation;

    while (pcurp != NULL)
    {
        if ( GetProcessFullPathByID( pcurp->ProcessId, &sPathTagProcess ) )
        {
            if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathTagProcess.Buffer ) != NULL )
            {
                DbgPrint (("SzHookQuerySystemInformation Hide = %ws", sPathTagProcess.Buffer ));

                if ( pprevp != NULL )
                {
                    if ( pcurp->NextEntryDelta != 0 )
                    {
                        pprevp->NextEntryDelta += pcurp->NextEntryDelta;
                    }
                    else
                    {
                        pprevp->NextEntryDelta = 0;
                    }
                }
                else
                {
                    if ( pcurp->NextEntryDelta != 0 )
                    {
                        (char *)SystemInformation += pcurp->NextEntryDelta;
                    }
                    else
                    {
                        SystemInformation = NULL;					
                    }
                }
            }
            else
                pprevp = pcurp;
        }

        if (pcurp->NextEntryDelta) 
            ((char *)pcurp += pcurp->NextEntryDelta);
        else 
            pcurp = NULL;

    }


call_default:

    if (sPathSrcProcess.Buffer != NULL) ExFreePool(sPathSrcProcess.Buffer);

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

    ULONG actualLen = 0;

    UNICODE_STRING sPathSrcProcess = { 0, 0, 0 };

    WCHAR wszFullName[MAX_PATH * 2 ] = { 0 };

    //UNICODE_STRING sFileObject = { 0, MAX_PATH * 2 * sizeof(WCHAR), wszFullName + 2 };
    //UNICODE_STRING DriverName  = { 0, 0, 0 };

    //PFILE_OBJECT   pFileObject = NULL;


    uCurProcessId = (UINT)PsGetCurrentProcessId();
    pfnStubFunc   = get_stub_func_address(hook_nt_create_section);

    if ( g_lDriverStatus != DRIVER_STATUS_RUNNING )
        goto call_default;

    do 
    {
        if ( !(DesiredAccess & CheckDesireAccess)      &&
             !(SectionPageProtection & CheckSectionPageProtection) && 0 )
        {
            break;
        }

        if ( !GetProcessFullPath( PsGetCurrentProcess(), &sPathSrcProcess ) )
            break;

        if ( !GetFullPathNameByHandle(FileHandle, wszFullName, sizeof(wszFullName) - 2 ) )
            break;

        //ntRetCode = ObReferenceObjectByHandle(FileHandle, 
        //    0, *IoFileObjectType, ExGetPreviousMode(), &pFileObject, NULL);
        //if ( !NT_SUCCESS(ntRetCode) || pFileObject == NULL )
        //    break;

        //ntRetCode = ObQueryNameString( 
        //    (PVOID)pFileObject,
        //    (POBJECT_NAME_INFORMATION)&sFileObject,
        //    sFileObject.MaximumLength,
        //    (PULONG)&actualLen );
        //if ( !NT_SUCCESS(ntRetCode) )
        //    break;

        //ntRetCode = RtlVolumeDeviceToDosName(pFileObject->DeviceObject, &DriverName);
        //if ( !NT_SUCCESS(ntRetCode) ||
        //    DriverName.Length == 0 || 
        //    DriverName.Buffer == NULL)
        //    break;

        //if ( (DriverName.Buffer[0] >= 'a' && DriverName.Buffer[0] <= 'z') ||
        //     (DriverName.Buffer[0] >= 'A' && DriverName.Buffer[0] <= 'Z') )
        //{
        //    wszFullName[0] = DriverName.Buffer[0];
        //    wszFullName[1] = L':';
        //}
        //else
        //    break;
        //    wszFullName[0] = L'C';
        //    wszFullName[1] = L':';

        //////////////////////////////////////////////////////////////////////////
        //
        AppendEvent_I( &g_event_list,
            LOG_TYPE_CREATESECTION, 
            uCurProcessId , sPathSrcProcess.Buffer,
            uCurProcessId , wszFullName);       
        //////////////////////////////////////////////////////////////////////////


        DbgPrint (("SzHookCreateSection sPathSrcProcess = %ws %ws", 
            sPathSrcProcess.Buffer, wszFullName ));

        // 是否是要保护对象
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) == NULL )
            break;

        if ( MatchingRule( &g_WhiteRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            break;
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            break;

        // 未出现在黑名单中
        if ( MatchingRule( &g_BlackRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            goto skip_default;


        DbgPrint (("SzHookCreateSection FullName = %ws", wszFullName ));


        //if ( g_drv_config.usEnableBlockUnknown )
        //    goto skip_default;

    } while( 0 );

call_default:

    ntResult = (NTSTATUS)pfnStubFunc(
        SectionHandle,
        DesiredAccess,
        ObjectAttributes,
        MaximumSize,
        SectionPageProtection,
        AllocationAttributes,
        FileHandle);


skip_default:

    if (sPathSrcProcess.Buffer != NULL) ExFreePool(sPathSrcProcess.Buffer);

    //if (DriverName.Buffer != NULL) ExFreePool(DriverName.Buffer);
    //if (FullName.Buffer != NULL) ExFreePool(FullName.Buffer);

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

    if ( g_lDriverStatus != DRIVER_STATUS_RUNNING )
        goto call_default;


    //DbgPrint ( ("SzHookOpenSection  PID=%d", uCurProcessId ) );

call_default:

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

    UNICODE_STRING sPathSrcProcess = { 0, 0, 0 };
    UNICODE_STRING sPathTagProcess = { 0, 0, 0 };

    KPROCESSOR_MODE mode = ExGetPreviousMode();
    
    if ( g_lDriverStatus != DRIVER_STATUS_RUNNING )
        goto call_default;

    do 
    {
        if ( !GetProcessFullPath( PsGetCurrentProcess(), &sPathSrcProcess ) )
            break;

        pEprocess = GetEProcessByHandle(ProcessHandle);
        if (pEprocess == NULL)
            break;

        if ( !GetProcessFullPath( pEprocess, &sPathTagProcess ) )
            break;

        //////////////////////////////////////////////////////////////////////////
        //
        AppendEvent_I( &g_event_list,
            LOG_TYPE_TERMINATEPROCESS, 
            (UINT)PsGetCurrentProcessId() , sPathSrcProcess.Buffer,
            GetPidByProcessHandle(ProcessHandle, NULL) , sPathTagProcess.Buffer);       
        //////////////////////////////////////////////////////////////////////////

        DbgPrint (("SzHookNtTerminateProcess sPathSrcProcess = %ws", sPathSrcProcess.Buffer ));


        //if ( MatchingRule( &g_WhiteRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
        //    break;
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            break;

        // 未出现在黑名单中
        if ( MatchingRule( &g_BlackRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            goto skip_default;



        DbgPrint (("SzHookNtTerminateProcess sPathTagProcess = %ws", sPathTagProcess.Buffer ));


        // 是否是要保护对象
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathTagProcess.Buffer ) != NULL )
        {
            if ( g_dwOsVersion == os_vista_sp0 ||
                g_dwOsVersion == os_vista_sp1)
            {
                if (0xC000042C == ExitStatus)  // STATUS_ELEVATION_REQUIRED
                {
                    // vista UAC提权
                    goto call_default;
                }
                else if (mode == KernelMode )
                {
                    goto call_default;
                }
            }

            if ( g_drv_config.usEnableBlockUnknown )
                goto skip_default;
        }


    } while( 0 );

call_default:

    nResult = SysNtTerminateProcess( ProcessHandle, ExitStatus );

skip_default:

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

    UNICODE_STRING sPathSrcProcess = { 0, 0, 0 };
    UNICODE_STRING sPathTagProcess = { 0, 0, 0 };

    KPROCESSOR_MODE mode = ExGetPreviousMode();

    if ( g_lDriverStatus != DRIVER_STATUS_RUNNING )
        goto call_default;

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

        //////////////////////////////////////////////////////////////////////////
        //
        AppendEvent_I( &g_event_list,
            LOG_TYPE_CREATETHREAD, 
            (UINT)PsGetCurrentProcessId() , sPathSrcProcess.Buffer,
            GetPidByProcessHandle(ProcessHandle, NULL) , sPathTagProcess.Buffer);       
        //////////////////////////////////////////////////////////////////////////


        DbgPrint (("SzHookNtCreateThread sPathSrcProcess = %ws %ws", sPathSrcProcess.Buffer, sPathTagProcess.Buffer ));


        // 是否是要保护对象
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathTagProcess.Buffer ) == NULL )
            break;

        if ( MatchingRule( &g_WhiteRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            break;
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            break;

        // 未出现在黑名单中
        if ( MatchingRule( &g_BlackRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            goto skip_default;
        
        if ( g_drv_config.usEnableBlockUnknown )
            goto skip_default;

    } while( 0 );

call_default:

    nResult = SysNtCreateThread( 
        ThreadHandle,
        DesiredAccess,
        ObjectAttributes,
        ProcessHandle,
        ClientId,
        ThreadContext,
        InitialTeb,
        CreateSuspended);

skip_default:

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

    UNICODE_STRING sPathSrcProcess = { 0, 0, 0 };
    UNICODE_STRING sPathTagProcess = { 0, 0, 0 };

    KPROCESSOR_MODE mode = ExGetPreviousMode();

    if ( g_lDriverStatus != DRIVER_STATUS_RUNNING )
        goto call_default;

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

        //////////////////////////////////////////////////////////////////////////
        //
        AppendEvent_I( &g_event_list,
            LOG_TYPE_READVIRTUALMEMORY, 
            (UINT)PsGetCurrentProcessId() , sPathSrcProcess.Buffer,
            GetPidByProcessHandle(ProcessHandle, NULL) , sPathTagProcess.Buffer);       
        //////////////////////////////////////////////////////////////////////////


        // 是否是要保护对象
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathTagProcess.Buffer ) == NULL )
            break;

        if ( MatchingRule( &g_WhiteRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            break;
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            break;

        // 未出现在黑名单中
        if ( MatchingRule( &g_BlackRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            goto skip_default;
        

        if ( g_drv_config.usEnableBlockUnknown )
            goto skip_default;

    } while( 0 );

call_default:

    nResult = SysNtReadVirtualMemory( 
        ProcessHandle,
        BaseAddress,
        Buffer,
        BufferLength,
        ReturnLength);

skip_default:

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

    UNICODE_STRING sPathSrcProcess = { 0, 0, 0 };
    UNICODE_STRING sPathTagProcess = { 0, 0, 0 };

    KPROCESSOR_MODE mode = ExGetPreviousMode();

    if ( g_lDriverStatus != DRIVER_STATUS_RUNNING )
        goto call_default;

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

        //////////////////////////////////////////////////////////////////////////
        //
        AppendEvent_I( &g_event_list,
            LOG_TYPE_WRITEVIRTUALMEMORY, 
            (UINT)PsGetCurrentProcessId() , sPathSrcProcess.Buffer,
            GetPidByProcessHandle(ProcessHandle, NULL) , sPathTagProcess.Buffer);       
        //////////////////////////////////////////////////////////////////////////

        // 是否是要保护对象
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathTagProcess.Buffer ) == NULL )
            break;

        if ( MatchingRule( &g_WhiteRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            break;
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            break;

        // 未出现在黑名单中
        if ( MatchingRule( &g_BlackRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            goto skip_default;

        if ( g_drv_config.usEnableBlockUnknown )
            goto skip_default;

    } while( 0 );

call_default:

    nResult = SysNtWriteVirtualMemory( 
        ProcessHandle,
        BaseAddress,
        Buffer,
        BufferLength,
        ReturnLength);

skip_default:

    if (sPathSrcProcess.Buffer != NULL) ExFreePool(sPathSrcProcess.Buffer);
    if (sPathTagProcess.Buffer != NULL) ExFreePool(sPathTagProcess.Buffer);

    return nResult;

}


#define WM_DESTROY      0x0002
#define WM_CLOSE        0x0010
#define WM_NCDESTROY    0x0082
#define WM_QUIT         0x0012


LRESULT NTAPI SzHookNtUserMessageCall(
    IN HWND        hwnd,
    IN UINT        msg,
    IN WPARAM      wParam,
    IN LPARAM      lParam,
    IN ULONG_PTR   xParam,
    IN DWORD       xpfnProc,
    IN BOOL        bAnsi)
{
    LRESULT nResult = FALSE;

    PEPROCESS pEprocess = NULL;

    UINT uTagPID = 0;

    UNICODE_STRING sPathSrcProcess = { 0, 0, 0 };
    UNICODE_STRING sPathTagProcess = { 0, 0, 0 };

    KPROCESSOR_MODE mode = ExGetPreviousMode();

    if ( g_lDriverStatus != DRIVER_STATUS_RUNNING )
        goto call_default;

    uTagPID = (ULONG) SysNtUserQueryWindow(hwnd, 0);
    if ( uTagPID == 0 )
        goto call_default;

    if ( uTagPID == (UINT)PsGetCurrentProcessId() )
        goto call_default;


    if ( msg == WM_DESTROY ||
         msg == WM_CLOSE ||
         msg == WM_NCDESTROY ||
         msg == WM_QUIT )
    {
        if ( !GetProcessFullPath( PsGetCurrentProcess(), &sPathSrcProcess ) )
            goto call_default;
        if ( !GetProcessFullPathByID( uTagPID, &sPathTagProcess ) )
            goto call_default;

        //////////////////////////////////////////////////////////////////////////
        //
        AppendEvent_I( &g_event_list,
            LOG_TYPE_USERMESSAGECALL, 
            (UINT)PsGetCurrentProcessId() , sPathSrcProcess.Buffer,
            uTagPID, sPathTagProcess.Buffer);       
        //////////////////////////////////////////////////////////////////////////

        DbgPrint (("SzHookNtUserMessageCall msg = %d sPathSrcProcess = %ws sPathTagProcess = %ws", 
            msg, sPathSrcProcess.Buffer, sPathTagProcess.Buffer ));

        // 是否是要保护对象
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathTagProcess.Buffer ) == NULL )
            goto call_default;

        //if ( MatchingRule( &g_WhiteRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
        //    goto call_default;
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            goto call_default;

        // 未出现在黑名单中
        if ( MatchingRule( &g_BlackRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            goto skip_default;

        if ( g_drv_config.usEnableBlockUnknown )
            goto skip_default;

    }

call_default:

    nResult = SysNtUserMessageCall( 
        hwnd,
        msg,
        wParam,
        lParam,
        xParam,
        xpfnProc,
        bAnsi);

skip_default:

    if (sPathSrcProcess.Buffer != NULL) ExFreePool(sPathSrcProcess.Buffer);
    if (sPathTagProcess.Buffer != NULL) ExFreePool(sPathTagProcess.Buffer);

    return nResult;
}



BOOL NTAPI SzHookNtUserPostMessage(
    IN HWND     hwnd,
    IN UINT     msg,
    IN WPARAM   wParam,
    IN LPARAM   lParam)
{
    BOOL nResult = FALSE;

    PEPROCESS pEprocess = NULL;

    UINT uTagPID = 0;

    UNICODE_STRING sPathSrcProcess = { 0, 0, 0 };
    UNICODE_STRING sPathTagProcess = { 0, 0, 0 };

    KPROCESSOR_MODE mode = ExGetPreviousMode();

    if ( g_lDriverStatus != DRIVER_STATUS_RUNNING )
        goto call_default;
    
    uTagPID = (ULONG) SysNtUserQueryWindow(hwnd, 0);
    if ( uTagPID == 0 )
        goto call_default;

    if ( uTagPID == (UINT)PsGetCurrentProcessId() )
        goto call_default;


    if ( msg == WM_DESTROY ||
         msg == WM_CLOSE ||
         msg == WM_NCDESTROY ||
         msg == WM_QUIT )
    {
        if ( !GetProcessFullPath( PsGetCurrentProcess(), &sPathSrcProcess ) )
            goto call_default;
        if ( !GetProcessFullPathByID( uTagPID, &sPathTagProcess ) )
            goto call_default;

        //////////////////////////////////////////////////////////////////////////
        //
        AppendEvent_I( &g_event_list,
            LOG_TYPE_USERPOSTMESSAGE, 
            (UINT)PsGetCurrentProcessId() , sPathSrcProcess.Buffer,
            uTagPID, sPathTagProcess.Buffer);       
        //////////////////////////////////////////////////////////////////////////

        DbgPrint (("SzHookNtUserPostMessage msg = %d sPathSrcProcess = %ws sPathTagProcess = %ws", 
            msg, sPathSrcProcess.Buffer, sPathTagProcess.Buffer ));

        // 是否是要保护对象
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathTagProcess.Buffer ) == NULL )
            goto call_default;

        //if ( MatchingRule( &g_WhiteRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
        //    goto call_default;
        if ( MatchingRule( &g_ProtectRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            goto call_default;

        // 未出现在黑名单中
        if ( MatchingRule( &g_BlackRuleList, CT_PATHFILE, sPathSrcProcess.Buffer ) != NULL )
            goto skip_default;

        if ( g_drv_config.usEnableBlockUnknown )
            goto skip_default;

    }

call_default:

    nResult = SysNtUserPostMessage( 
        hwnd,
        msg,
        wParam,
        lParam);

skip_default:

    if (sPathSrcProcess.Buffer != NULL) ExFreePool(sPathSrcProcess.Buffer);
    if (sPathTagProcess.Buffer != NULL) ExFreePool(sPathTagProcess.Buffer);

    return nResult;
}

BOOL NTAPI SzHookNtUserPostThreadMessage(
    IN DWORD    id,
    IN UINT     msg,
    IN WPARAM   wParam,
    IN LPARAM   lParam)
{
    BOOL nResult = FALSE;

    PEPROCESS pEprocess = NULL;

    UNICODE_STRING sPathSrcProcess = { 0, 0, 0 };
    UNICODE_STRING sPathTagProcess = { 0, 0, 0 };

    KPROCESSOR_MODE mode = ExGetPreviousMode();

    if ( g_lDriverStatus != DRIVER_STATUS_RUNNING )
        goto call_default;

    if ( msg != WM_QUIT )
        goto call_default;


    if ( !GetProcessFullPath( PsGetCurrentProcess(), &sPathSrcProcess ) )
        goto call_default;
    
    DbgPrint (("SzHookNtUserPostThreadMessage msg = %d sPathTagProcess = %ws sPathTagProcess = %ws", 
        msg, sPathTagProcess.Buffer, sPathTagProcess.Buffer ));


call_default:

    nResult = SysNtUserPostThreadMessage( 
        id,
        msg,
        wParam,
        lParam);

skip_default:

    if (sPathSrcProcess.Buffer != NULL) ExFreePool(sPathSrcProcess.Buffer);
    if (sPathTagProcess.Buffer != NULL) ExFreePool(sPathTagProcess.Buffer);

    return nResult;
}


UINT NTAPI SzHookNtUserSendInput (
    IN UINT				cInputs,
    IN CONST INPUT		*pInputs,
    IN int				cbSize)
{
    UINT nResult = 0;

    PEPROCESS pEprocess = NULL;

    UNICODE_STRING sPathSrcProcess = { 0, 0, 0 };
    UNICODE_STRING sPathTagProcess = { 0, 0, 0 };

    KPROCESSOR_MODE mode = ExGetPreviousMode();

    if ( g_lDriverStatus != DRIVER_STATUS_RUNNING )
        goto call_default;

call_default:

    nResult = SysNtUserSendInput( 
        cInputs,
        pInputs,
        cbSize );

skip_default:

    if (sPathSrcProcess.Buffer != NULL) ExFreePool(sPathSrcProcess.Buffer);
    if (sPathTagProcess.Buffer != NULL) ExFreePool(sPathTagProcess.Buffer);

    return nResult;
}
