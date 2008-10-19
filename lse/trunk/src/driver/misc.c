#include "precomp.h"
#pragma hdrstop

extern  POBJECT_TYPE *PsProcessType;
extern  POBJECT_TYPE *PsThreadType;

#define BASE_PEB_PROCESS_PARAMETER_OFFSET       0x0010
#define BASE_PROCESS_PARAMETER_FULL_IMAGE_NAME  0x0038


NTKERNELAPI NTSTATUS NTAPI PsLookupProcessByProcessId(
    HANDLE ProcessId,
    PEPROCESS *Process);


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


BOOL GetProcessFullPathByID(
    UINT uPID,
    PUNICODE_STRING pUniCodeFullPath)
{
    NTSTATUS nRetCode = 0;

    PEPROCESS pEProcess = NULL;

    nRetCode = PsLookupProcessByProcessId((HANDLE)uPID, &pEProcess);
    if ( !NT_SUCCESS(nRetCode) )
        return FALSE;
    
    return GetProcessFullPath( pEProcess, pUniCodeFullPath );
}


BOOL GetProcessFullPath(
    PEPROCESS       pPeProcess,
    PUNICODE_STRING pUniCodeFullPath)
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


        pUniCodeFullPath->Buffer = ExAllocatePoolWithTag(NonPagedPool, ReturnLength, MEM_TAG);
        if (pUniCodeFullPath->Buffer == NULL)
            goto Exit0;

        RtlCopyMemory(pUniCodeFullPath->Buffer, pImageFileName->Buffer, usActualLength);

        pUniCodeFullPath->Length = usActualLength;
        pUniCodeFullPath->MaximumLength = ReturnLength;

        nRetCode = _wcsnicmp(pUniCodeFullPath->Buffer, L"\\??\\", 4);
        if (nRetCode == 0)
        {
            RtlCopyMemory(pUniCodeFullPath->Buffer, pUniCodeFullPath->Buffer + 4, usActualLength - 4 * sizeof(WCHAR));
            pUniCodeFullPath->Length -= 8;
            pUniCodeFullPath->MaximumLength -= 8;
        }
        //
        // 结尾置NULL
        //
        pUniCodeFullPath->Buffer[ usActualLength / sizeof(WCHAR) ] = UNICODE_NULL;
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

