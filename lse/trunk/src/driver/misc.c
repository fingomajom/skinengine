#include "precomp.h"
#pragma hdrstop

extern  POBJECT_TYPE *PsProcessType;
extern  POBJECT_TYPE *PsThreadType;

#define BASE_PEB_PROCESS_PARAMETER_OFFSET       0x0010
#define BASE_PROCESS_PARAMETER_FULL_IMAGE_NAME  0x0038



NTKERNELAPI NTSTATUS NTAPI ZwQueryInformationProcess(
    HANDLE ProcessHandle,
    PROCESSINFOCLASS ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength); 


NTKERNELAPI NTSTATUS NTAPI PsLookupProcessByProcessId(
    HANDLE ProcessId,
    PEPROCESS *Process);



NTSTATUS  AllocUnicodeStringFromPool(
    PUNICODE_STRING *ppDstString, 
    POOL_TYPE PoolType, 
    USHORT BufferSize)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;

    *ppDstString = (PUNICODE_STRING)ExAllocatePool( PoolType, BufferSize + sizeof(UNICODE_STRING) );
    if ( NULL == *ppDstString )    
    {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit0; 
    }
    RtlZeroMemory(*ppDstString, BufferSize);

    (*ppDstString)->Length = 0;
    (*ppDstString)->MaximumLength = BufferSize;
    (*ppDstString)->Buffer = (PWSTR)( (PUCHAR)(*ppDstString) + sizeof(UNICODE_STRING) );

    status = STATUS_SUCCESS;

Exit0:
    return status;
}

UINT GetWindowsDirectory(
    LPWSTR lpBuffer,
    UINT   uSize)
{
    NTSTATUS status = STATUS_INVALID_PARAMETER;

    ULONG uRetCode      = 0;
    ULONG uType         = 0;
    ULONG uReturnLength = 0;

    status = DrvGetValueKey(
        REG_CURRENT_VERSION_PATH,
        WINDOWSROOT,
        &uType,
        lpBuffer,
        uSize,
        &uReturnLength);

    if ( status == STATUS_SUCCESS )
        uRetCode = uReturnLength;

    return uRetCode;
}

int IsAddressValid(ULONG Address, ULONG Length)
{
    ULONG i = 0;

    for (i = 0; i < Length; i++)
    {
        if (!MmIsAddressValid((PVOID)(Address + i)))
        {
            return FALSE;
        }
    }

    return TRUE;
}

PEPROCESS GetEProcessByHandle( HANDLE hProcess )
{
    NTSTATUS  status = STATUS_UNSUCCESSFUL;
    PEPROCESS pEprocess = NULL;

    status = ObReferenceObjectByHandle(hProcess, 0, *PsProcessType, KernelMode, (PVOID*)&pEprocess, NULL);

    return (NT_SUCCESS(status))?(pEprocess):NULL;
}

ULONG GetPidByProcessHandle(HANDLE ProcessHandle, ULONG *puParentPid)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    PROCESS_BASIC_INFORMATION	pbi;
    ULONG	uReturnLength = 0;

    ULONG uPid = 0;

    status = (NTSTATUS)ZwQueryInformationProcess(
        ProcessHandle, 
        ProcessBasicInformation, 
        &pbi, 
        sizeof(PROCESS_BASIC_INFORMATION), 
        &uReturnLength
        );
    if (STATUS_SUCCESS != status)
    {
        goto Exit0;
    }

    uPid  = pbi.UniqueProcessId;
    if (puParentPid)
    {
        *puParentPid = pbi.InheritedFromUniqueProcessId;
    }

Exit0:
    return uPid;
}


BOOL GetProcessFullPathByID(
    UINT uPID,
    PUNICODE_STRING psUniCodeFullPath)
{
    NTSTATUS nRetCode = 0;

    PEPROCESS pEProcess = NULL;

    nRetCode = PsLookupProcessByProcessId((HANDLE)uPID, &pEProcess);
    if ( !NT_SUCCESS(nRetCode) )
        return FALSE;
    
    return GetProcessFullPath( pEProcess, psUniCodeFullPath );
}


BOOL GetProcessFullPath(
    PEPROCESS       pPeProcess,
    PUNICODE_STRING psUniCodeFullPath)
{
    BOOL        bResult = FALSE;
    KAPC_STATE  APCState;
    ULONG       ulAddress = 0;
    USHORT      usActualLength = 0;
    ULONG       bAttachProcess = FALSE;
    PVOID       pEProcess = NULL;
    USHORT      ReturnLength = 0;
    PUNICODE_STRING	pImageFileName = NULL;

    int nRetCode = 0;

    KPROCESSOR_MODE PreMode = ExGetPreviousMode();

    if (KeGetCurrentIrql() != PASSIVE_LEVEL)		
        return FALSE;

    ulAddress = (ULONG)PsGetCurrentProcess();

    if ( (ULONG)pPeProcess != ulAddress )
    {
        KeStackAttachProcess( pPeProcess, &APCState );
        ulAddress = (ULONG)pPeProcess;
        bAttachProcess = TRUE;
    }

    ulAddress += g_knl_current_offset.dwProcessPebOffset;


    if (!IsAddressValid(ulAddress, 4))
    {
        goto Exit0;
    }

    ulAddress = *(ULONG*)ulAddress;

    ulAddress += BASE_PEB_PROCESS_PARAMETER_OFFSET;

    __try
    {

        if (PreMode == UserMode)
        {
            ProbeForRead((PVOID)ulAddress, 4, 1);

            ulAddress = *(ULONG*)ulAddress;

            ulAddress += BASE_PROCESS_PARAMETER_FULL_IMAGE_NAME;
            ProbeForRead((PVOID)ulAddress, 8, 1);
            pImageFileName = (PUNICODE_STRING)ulAddress;
            ProbeForRead(pImageFileName->Buffer, pImageFileName->Length, 1);

        }
        else
        {
            if (!IsAddressValid(ulAddress, 4))
            {
                goto Exit0;
            }

            ulAddress = *(ULONG*)ulAddress;

            ulAddress += BASE_PROCESS_PARAMETER_FULL_IMAGE_NAME;
            if (!IsAddressValid(ulAddress, 8))
            {
                goto Exit0;
            }

            pImageFileName = (PUNICODE_STRING)ulAddress;

            if (!IsAddressValid((ULONG)pImageFileName->Buffer, pImageFileName->Length))
            {
                goto Exit0;
            }
        }
    }
    __except(1)
    {
        goto Exit0;
    }


    //
    // 所有内存操作都应该try, 因为内存的有效性严正和对内存的操作不是原子操作,
    // 在这个过程中间, 内存有可能被释放
    //
    __try
    {
        usActualLength = pImageFileName->Length;
        if ( usActualLength <= 0 )
        {
            goto Exit0;
        }

        ReturnLength = usActualLength + sizeof(UNICODE_NULL);


        psUniCodeFullPath->Buffer = ExAllocatePoolWithTag(NonPagedPool, ReturnLength, MEM_TAG);
        if (psUniCodeFullPath->Buffer == NULL)
            goto Exit0;

        RtlCopyMemory(psUniCodeFullPath->Buffer, pImageFileName->Buffer, usActualLength);

        psUniCodeFullPath->Length = usActualLength;
        psUniCodeFullPath->MaximumLength = ReturnLength;

        nRetCode = _wcsnicmp(psUniCodeFullPath->Buffer, L"\\??\\", 4);
        if (nRetCode == 0)
        {
            RtlCopyMemory(psUniCodeFullPath->Buffer, psUniCodeFullPath->Buffer + 4, usActualLength - 4 * sizeof(WCHAR));
            psUniCodeFullPath->Length -= 8;
            psUniCodeFullPath->MaximumLength -= 8;
            usActualLength -= 8;
        }
        //
        // 结尾置NULL
        //
        psUniCodeFullPath->Buffer[ usActualLength / sizeof(WCHAR) ] = UNICODE_NULL;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        goto Exit0;
    }

    bResult = TRUE;

Exit0:
    if ( bAttachProcess )
    {
        KeUnstackDetachProcess(&APCState);
        bAttachProcess = FALSE;
    }
    return bResult;
}


PVOID GetEProcessByProcName(WCHAR *pwszName)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    PVOID Process = NULL;
    SYSTEM_PROCESS_INFORMATION *pSystemProcessInfo = NULL, *p = NULL;
    ULONG ulBufferLength = 0;
    ULONG uPid = 0;

    ULONG	uNameLength = wcslen(pwszName);

    ulBufferLength = 0x1000;

    pSystemProcessInfo = (SYSTEM_PROCESS_INFORMATION*) ExAllocatePool(NonPagedPool, ulBufferLength);
    if (NULL == pSystemProcessInfo)
    {
        status = STATUS_UNSUCCESSFUL;
        goto Exit0;
    }

    do 
    {
        status = ZwQuerySystemInformation(
            SystemProcessInformation,
            pSystemProcessInfo,
            ulBufferLength,
            NULL);
        if (STATUS_SUCCESS == status)
        {
            break;
        }
        if ( pSystemProcessInfo ) { ExFreePool(pSystemProcessInfo); pSystemProcessInfo = NULL; }
        ulBufferLength *= 2;
        pSystemProcessInfo = (SYSTEM_PROCESS_INFORMATION*) ExAllocatePool(NonPagedPool, ulBufferLength);
        if (NULL == pSystemProcessInfo)
        {
            status = STATUS_UNSUCCESSFUL;
            goto Exit0;
        }
    } while((STATUS_INFO_LENGTH_MISMATCH == status) || (STATUS_BUFFER_TOO_SMALL == status));

    if (STATUS_SUCCESS != status)
    {
        if ( pSystemProcessInfo ) { ExFreePool(pSystemProcessInfo); pSystemProcessInfo = NULL; }
        goto Exit0;
    }

    p = pSystemProcessInfo;
    __try
    {
        do 
        {
            if (p->ProcessId == 0 || p->ProcessId == 4)
            {
                goto Next;
            }
            if (!_wcsnicmp(pwszName, p->ProcessName.Buffer, uNameLength))
            {
                uPid = p->ProcessId;
                break;
            }
Next:
            p = (SYSTEM_PROCESS_INFORMATION*)(
                (UCHAR*)p + p->NextEntryDelta);
        } while(p->NextEntryDelta);

    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        goto Exit0;
    }

    if (uPid)
    {
        PsLookupProcessByProcessId((HANDLE)uPid, (PEPROCESS *)&Process);
    }

Exit0:
    if ( pSystemProcessInfo ) { ExFreePool(pSystemProcessInfo); pSystemProcessInfo = NULL; }

    return Process;
}


//-----------------------------------------------------------------------------//
/**
*@brief 根据文件句柄得到文件全路径
* 
*@param[in]		FileHandle 文件句柄
*@param[out]	pwszFullPathName 文件名缓冲区
*@param[in]		Length 文件名缓冲区大小
*@return 返回STATUS_SUCCESS时成功,其它值失败，
*/
//BOOL GetFullPathNameByHandle(HANDLE FileHandle, WCHAR *pwszFullPathName, ULONG uLength)
//{
//    int nResult  = FALSE;
//    int nRetCode = FALSE;
//
//    FILE_OBJECT *pFileObject = NULL;
//
//    nRetCode = ObReferenceObjectByHandle(
//        FileHandle,
//        0,
//        *IoFileObjectType,
//        ExGetPreviousMode(), 
//        (PVOID*)&pFileObject,
//        0);
//    if ( !NT_SUCCESS(nRetCode) )
//        goto Exit0;
//
//    nRetCode = GetFullPathNameByFileObject(pFileObject, pwszFullPathName, uLength);
//    if ( !NT_SUCCESS(nRetCode) )
//        goto Exit0;
//
//    nResult = TRUE;
//
//Exit0:
//    if (pFileObject) ObDereferenceObject(pFileObject);
//
//    return nResult;
//}


//-----------------------------------------------------------------------------//
/**
*@brief 根据文件对象得到文件全路径
* 
*@param[in]		pFileObject 文件对象
*@param[out]	pwszFullPathName 文件名缓冲区
*@param[in]		Length 文件名缓冲区大小
*@return 返回STATUS_SUCCESS时成功,其它值失败，
*/
//BOOL GetFullPathNameByFileObject(PFILE_OBJECT pFileObject, WCHAR *pwszFullPathName, ULONG uLength)
//{
//    int nResult  = FALSE;
//    int nRetCode = FALSE;
//
//    UNICODE_STRING VolumeDeviceFileName = { 0 };
//
//    ULONG actualLen = 0;
//    WCHAR c = 0;
//
//    if (!pFileObject || !pFileObject->FileName.Buffer || !pFileObject->FileName.Length)
//    {
//        goto Exit0;
//    }
//
//    VolumeDeviceFileName.Length = 0;
//    VolumeDeviceFileName.MaximumLength = (USHORT)uLength;
//    VolumeDeviceFileName.Buffer = ExAllocatePool( NonPagedPool, uLength + sizeof(WCHAR) );
//
//    if ( VolumeDeviceFileName.Buffer == NULL )
//        goto Exit0;
//
//    nRetCode = ObQueryNameString( 
//        (PVOID)pFileObject,
//        (POBJECT_NAME_INFORMATION)&VolumeDeviceFileName,
//        uLength,
//        (PULONG)&actualLen );
//    if(!NT_SUCCESS(nRetCode))
//    {
//        nResult = FALSE;
//        goto Exit0;
//    }
//
//    if (VolumeDeviceFileName.Length > uLength)
//    {
//        nResult = FALSE;
//        goto Exit0;
//    }
//
//    if (uLength < (ULONG)(6 + pFileObject->FileName.Length))
//    {
//        nResult = FALSE;
//        goto Exit0;
//    }
//
//    __try
//    {
//        c = GetDriverLetterFromVolumeDeviceFileName(&VolumeDeviceFileName);
//        
//        if ( 0 != c )
//        {
//            pwszFullPathName[0] = c;
//            pwszFullPathName[1] = L':';
//            RtlCopyMemory(pwszFullPathName + 2, pFileObject->FileName.Buffer, pFileObject->FileName.Length);
//            pwszFullPathName[pFileObject->FileName.Length / sizeof(WCHAR) + 2] = 0;
//        }
//        else
//        {
//            if (pFileObject->FileName.Length < uLength)
//            {
//                RtlCopyMemory(pwszFullPathName, pFileObject->FileName.Buffer, pFileObject->FileName.Length);
//                pwszFullPathName[pFileObject->FileName.Length / 2] = 0;
//            }
//        }
//
//    }
//    __except(EXCEPTION_EXECUTE_HANDLER)
//    {
//        goto Exit0;
//    }
//
//
//    nResult = TRUE;
//
//Exit0:
//
//    if ( VolumeDeviceFileName.Buffer != NULL )
//        ExFreePool( VolumeDeviceFileName.Buffer );
//
//    return nResult;
//}
//

BOOL GetFullPathNameByHandle(HANDLE FileHandle, WCHAR *pwszFullPathName, ULONG Length)
{
    int krResult = FALSE;
    NTSTATUS status = STATUS_UNSUCCESSFUL;

    FILE_OBJECT    *pFileObject = NULL;
    PUNICODE_STRING pVolumeDeviceFileName = NULL;

    ULONG actualLen = 0;
    WCHAR c = 0;

    status = ObReferenceObjectByHandle(
        FileHandle,
        0,
        *IoFileObjectType,
        ExGetPreviousMode(), 
        (PVOID*)&pFileObject,
        0);
    if(!NT_SUCCESS(status))
        goto Exit0;

    if (!pFileObject || !pFileObject->FileName.Buffer || !pFileObject->FileName.Length)
    {
        goto Exit0;
    }

    status = AllocUnicodeStringFromPool( &pVolumeDeviceFileName, NonPagedPool, (USHORT)Length );
    if ( !NT_SUCCESS(status) )  goto Exit0;

    status = ObQueryNameString( 
        (PVOID)pFileObject,
        (POBJECT_NAME_INFORMATION)pVolumeDeviceFileName,
        Length,
        (PULONG)&actualLen );
    if(!NT_SUCCESS(status))
        goto Exit0;

    if (pVolumeDeviceFileName->Length >  Length)
        goto Exit0;
    if (Length < (ULONG)(6 + pFileObject->FileName.Length))
        goto Exit0;

    __try
    {
        c = GetDriverLetterFromVolumeDeviceFileName(pVolumeDeviceFileName);
        if (0 != c)
        {
            pwszFullPathName[0] = c;
            pwszFullPathName[1] = L':';
            RtlCopyMemory(pwszFullPathName + 2, pFileObject->FileName.Buffer, pFileObject->FileName.Length);
            pwszFullPathName[pFileObject->FileName.Length / 2 + 2] = 0;
        }
        else
        {
            if (pFileObject->FileName.Length < Length)
            {
                RtlCopyMemory(pwszFullPathName, pFileObject->FileName.Buffer, pFileObject->FileName.Length);
                pwszFullPathName[pFileObject->FileName.Length / 2] = 0;
            }
        }

    }
    __except(1)
    {
        goto Exit0;
    }


    krResult = TRUE;

Exit0:
    if (pFileObject)  ObDereferenceObject(pFileObject);
    if (pVolumeDeviceFileName) ExFreePool(pVolumeDeviceFileName);

    return krResult;
}


WCHAR GetDriverLetterFromVolumeDeviceFileName( 
    PUNICODE_STRING pVolumeDeviceFileName )
{
    NTSTATUS            status = STATUS_UNSUCCESSFUL;

    WCHAR               c;
    UNICODE_STRING      symbolicLinkName;
    OBJECT_ATTRIBUTES   oa;
    HANDLE              hSymbolicLink = INVALID_HANDLE_VALUE;
    PUNICODE_STRING     pTempString = NULL;
    ULONG               actualLength = 0;
    WCHAR				str[10] = L"\\??\\C:" ;

    RtlInitUnicodeString( &symbolicLinkName, str);

    for ( c = L'A' ; c <= L'Z'; c++ )
    {
        symbolicLinkName.Buffer[4] = c;

        if ( hSymbolicLink != INVALID_HANDLE_VALUE )
        {
            ZwClose( hSymbolicLink );
            hSymbolicLink = INVALID_HANDLE_VALUE;
        }

        InitializeObjectAttributes(&oa, &symbolicLinkName, OBJ_CASE_INSENSITIVE, 0, 0);
        status = ZwOpenSymbolicLinkObject(&hSymbolicLink, GENERIC_READ, &oa);
        if ( !NT_SUCCESS(status) )    
            continue;

        status = AllocUnicodeStringFromPool( &pTempString, NonPagedPool, 1 );
        if ( !NT_SUCCESS(status) )    
            goto Exit0;

        status = ZwQuerySymbolicLinkObject( hSymbolicLink, pTempString, &actualLength );
        if ( status != STATUS_BUFFER_TOO_SMALL )   
        { 
            status = STATUS_UNSUCCESSFUL;
            goto Exit0;
        }

        ExFreePool(pTempString); 
        pTempString = NULL;

        status = AllocUnicodeStringFromPool( &pTempString, NonPagedPool, (USHORT)actualLength);
        if ( !NT_SUCCESS(status) )    
            goto Exit0;

        status = ZwQuerySymbolicLinkObject( hSymbolicLink, pTempString, &actualLength );
        if ( !NT_SUCCESS(status) )  
            goto Exit0;

        if ( pTempString->Length == 
            RtlCompareMemory(pTempString->Buffer, pVolumeDeviceFileName->Buffer, pTempString->Length) 
            )
        {
            break;
        }

        ExFreePool(pTempString);     
        pTempString = NULL;
    }


Exit0:
    if ( pTempString != NULL )       
        ExFreePool(pTempString);
    if ( hSymbolicLink != INVALID_HANDLE_VALUE )    
        ZwClose( hSymbolicLink );

    return ( c > L'Z' ) ? 0 : c;
}
