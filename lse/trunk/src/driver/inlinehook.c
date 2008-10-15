#include "precomp.h"
#pragma hdrstop



hook_item_info g_hook_function_array[ hook_function_count ] = { 0 };


//////////////////////////////////////////////////////////////////////////

NTSYSAPI NTSTATUS NTAPI KeUserModeCallback(
    ULONG ApiNumber, 
    PVOID InputBuffer, 
    ULONG InputLength, 
    PVOID *OutputBuffer,
    PULONG OutputLength);


NTSYSCALLAPI NTSTATUS NTAPI ZwOpenProcess (
    PHANDLE ProcessHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PCLIENT_ID ClientId); 

//////////////////////////////////////////////////////////////////////////


BOOL init_hook()
{
    RtlZeroMemory(g_hook_function_array, sizeof(g_hook_function_array));

    g_hook_function_array[hook_ke_user_mode_callback].bcanhook    = TRUE;
    g_hook_function_array[hook_ke_user_mode_callback].ptargetfunc = (void *)KeUserModeCallback;
    g_hook_function_array[hook_ke_user_mode_callback].phookfunc   = (void *)HookKeUserModeCallback;

    g_hook_function_array[hook_nt_open_process].bcanhook    = TRUE;
    g_hook_function_array[hook_nt_open_process].ptargetfunc = GetServiceAddress(ZwOpenProcess);
    g_hook_function_array[hook_nt_open_process].phookfunc   = (void *)HookNtOpenProcess;


    return TRUE;
}

BOOL uninit_hook()
{
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



BOOL install_hook( lp_hook_item_info phookinfo )
{
    DWORD dwJmpDst;
    int	  nInstSize;
    int   nInstSizeIfMoved;
    UINT  unLoop;
    BOOL  bRetCode;
    PVOID pLockImage = NULL;
    KIRQL OldIrql;
    PMDL  pAllocMDL;	

    phookinfo->nheadsize = 0;
    phookinfo->nstubsize = 5; // == sizeof(JMP rel32)

    while (phookinfo->nheadsize < 5) // need at least 5 char to path
    {
        bRetCode = dasm_get_instruction_size(phookinfo->ptargetfunc + phookinfo->nheadsize, 
            &nInstSize, &nInstSizeIfMoved);

        if (bRetCode == FALSE)
        {
            DbgPrint(("pttdrv: GetInstructionSize failed (target=0x%08X)\n", phookinfo->ptargetfunc));
            return FALSE;
        }

        phookinfo->nheadsize += nInstSize;
        phookinfo->nstubsize += nInstSizeIfMoved;

        if (phookinfo->nstubsize >= max_stub_size) 
        {
            DbgPrint(("pttdrv: Stub too big (target=0x%08X)\n", phookinfo->ptargetfunc));
            return FALSE;
        }      
    }

    memcpy(phookinfo->pheadbuf, phookinfo->ptargetfunc, phookinfo->nheadsize);

    // create stub
    bRetCode = dasm_move_program(phookinfo->ptargetfunc, 
        phookinfo->nheadsize, 
        phookinfo->pstubbuf, 
        &phookinfo->nstubsize, 
        max_stub_size);
    if (bRetCode == FALSE)
    {
        DbgPrint(("pttdrv: MoveProgram failed (target=0x%08X)\n", phookinfo->ptargetfunc));
        return FALSE;
    }

    // if it was Inline hook by anyothers? modify by nimaozhi 2008-4-29
    // 处理了与360的兼容
    if ( (UCHAR)phookinfo->ptargetfunc[0] == 0xE9)
    {
        ULONG ulDstAddr = (ULONG)phookinfo->ptargetfunc + 5 + *(PULONG)(phookinfo->ptargetfunc + 1);
        ULONG ulSrcAddr = (ULONG)phookinfo->pstubbuf + 5;
        dwJmpDst = ulDstAddr - ulSrcAddr;		
        phookinfo->pstubbuf[0] = 0xE9;
        memcpy(&phookinfo->pstubbuf[1], &dwJmpDst, 4);
    }

    // add jmp at the end of the stub
    dwJmpDst = (DWORD)phookinfo->ptargetfunc - (DWORD)phookinfo->pstubbuf - phookinfo->nstubsize;
    phookinfo->pstubbuf[phookinfo->nstubsize] = 0xE9; // JMP rel32
    memcpy(&phookinfo->pstubbuf[phookinfo->nstubsize+1], &dwJmpDst, 4);
    phookinfo->nstubsize += 5;

    DbgPrint(("pttdrv: Install a Hook %x", phookinfo->ptargetfunc));
    pAllocMDL = IoAllocateMdl(phookinfo->ptargetfunc, phookinfo->nheadsize, FALSE, FALSE, NULL);
    __try
    {
        MmProbeAndLockPages(pAllocMDL, KernelMode, IoModifyAccess); 
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        IoFreeMdl(pAllocMDL);
        return FALSE;
    }

    KeRaiseIrql(HIGH_LEVEL, &OldIrql);
    WPOFF();

    phookinfo->ptargetfunc[0] = 0xE9; // JMP rel32
    dwJmpDst = (DWORD)phookinfo->phookfunc - (DWORD)phookinfo->ptargetfunc - 5; // 5==sizeof(JMP rel32)
    memcpy(phookinfo->ptargetfunc + 1, &dwJmpDst, 4);

    for (unLoop = 5; unLoop < phookinfo->nheadsize; unLoop ++)
        phookinfo->ptargetfunc[unLoop] = 0x90; // NOP

    WPON();
    KeLowerIrql(OldIrql);

    MmUnlockPages(pAllocMDL);
    IoFreeMdl(pAllocMDL);

    DbgPrint(("Success\n"));

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////

BOOL uninstall_hook( lp_hook_item_info phookinfo )
{
    PVOID pLockImage = NULL;
    KIRQL OldIrql;

    if (phookinfo->nheadsize <= 0 || phookinfo->ptargetfunc[0] != 0xE9) 
    {
        DbgPrint(("pttdrv: Function not hooked (pfnTarget=0x%08X\n", phookinfo->ptargetfunc));
        return FALSE;
    }

    pLockImage = MmLockPagableDataSection(phookinfo->ptargetfunc);
    KeRaiseIrql(HIGH_LEVEL, &OldIrql);
    WPOFF();

    memcpy(phookinfo->ptargetfunc, phookinfo->pheadbuf, phookinfo->nheadsize);

    WPON();
    KeLowerIrql(OldIrql);
    MmUnlockPagableImageSection(pLockImage);

    return TRUE;
}



//////////////////////////////////////////////////////////////////////////


#define CPU_NUM		32

typedef struct _PATCH_DPC_CONTEXT 
{
    PKSPIN_LOCK		pPatchSpinLock;
    PULONG			pulCpuCount;		

} PATCH_DPC_CONTEXT, *PPATCH_DPC_CONTEXT;

//
// global 
//
KSPIN_LOCK	        g_PatchSpinLock = 0; 
ULONG		        g_ulCpuCount	= 0;
PATCH_DPC_CONTEXT	g_DpcContext    = { 0 };

KDPC		g_Dpc[CPU_NUM];



VOID DpcRoutine(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2 )
{
    KIRQL               OldIrql;
    PPATCH_DPC_CONTEXT  pDpcContext;

    pDpcContext = (PPATCH_DPC_CONTEXT)DeferredContext;

    __asm cli;

    KeRaiseIrql(DISPATCH_LEVEL, &OldIrql);


    InterlockedIncrement((PLONG)pDpcContext->pulCpuCount);

    //	KPrintf("Ready to spin lock.\n");

    KeAcquireSpinLockAtDpcLevel(pDpcContext->pPatchSpinLock);

    KeReleaseSpinLockFromDpcLevel(pDpcContext->pPatchSpinLock);

    KeLowerIrql(OldIrql);

    __asm sti;
}

NTSTATUS DvrBMapPagedCode(ULONG *pStartAddress, ULONG Size, PMDL *ppMdl)
{
    NTSTATUS nStatus = STATUS_UNSUCCESSFUL;

    *ppMdl = IoAllocateMdl(pStartAddress, Size, FALSE, FALSE, NULL);
    if (*ppMdl == NULL)
    {
        goto Exit0;
    }

    __try
    {
        MmProbeAndLockPages(*ppMdl, KernelMode, IoReadAccess);
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        goto Exit0;
    }
    nStatus = STATUS_SUCCESS;

Exit0:
    return nStatus;

}

NTSTATUS DrvBUnMapPagedCode(PMDL pMdl)
{
    NTSTATUS nStatus = STATUS_UNSUCCESSFUL;

    MmUnlockPages(pMdl);

    if (pMdl)
    {
        IoFreeMdl(pMdl);		
        pMdl = NULL;
    }

    nStatus = STATUS_SUCCESS;
    return nStatus;
}

//-----------------------------------------------------------------------------//
BOOL safe_install_hook( lp_hook_item_info phookinfo )
{
    NTSTATUS Status = STATUS_UNSUCCESSFUL;

    KIRQL		OldIrql;
    ULONG		ulTotalProcessorNumber	= 0;
    ULONG		ulCurProcessorNumber	= 0;
    ULONG		ulProcessorNumber		= 0;
    ULONG		ulProcessorCount		= 0;
    KAFFINITY	affinity;	
    int         i = 0;

    PMDL  FunMDL	= NULL;	
    Status = DvrBMapPagedCode((PULONG)phookinfo->ptargetfunc, max_head_size, &FunMDL);
    if ( Status != STATUS_SUCCESS)
    {
        goto Exit0;
    }
    affinity = KeQueryActiveProcessors();	
    for (i = 0; i < 31; i++)
    {
        if ((affinity >> i) & 0x1)
        {
            ulTotalProcessorNumber ++;
        }
    }

    if (1 == ulTotalProcessorNumber)
    {		
        __asm cli;
        KeRaiseIrql(DISPATCH_LEVEL, &OldIrql);
        Status = install_hook(phookinfo);
        if (STATUS_SUCCESS != Status)
        {
            //DbgPrint("ERROR: HookKeUserModeCallback failed\n");
        }
        KeLowerIrql(OldIrql);
        __asm sti;

        goto Exit0;
    };


    g_DpcContext.pPatchSpinLock		= &g_PatchSpinLock;
    g_DpcContext.pulCpuCount		= &g_ulCpuCount;
    ////
    KeInitializeSpinLock(&g_PatchSpinLock);

    for (i = 0; i < CPU_NUM; i++)
    {
        KeInitializeDpc(&g_Dpc[i], DpcRoutine, &g_DpcContext);
    }


    g_ulCpuCount = 0;

    i = CPU_NUM;

    __asm cli;
    KeAcquireSpinLock(&g_PatchSpinLock, &OldIrql);

    ulCurProcessorNumber = KeGetCurrentProcessorNumber();

    while (i > 0)
    {
        if (1 == (affinity & 0x1))
        {
            ulProcessorCount++;
            if (ulProcessorNumber != ulCurProcessorNumber)
            {
                KeSetTargetProcessorDpc(&g_Dpc[ulProcessorNumber], (CCHAR)ulProcessorNumber);
                KeSetImportanceDpc(&g_Dpc[ulProcessorNumber], HighImportance);
                KeInsertQueueDpc(&g_Dpc[ulProcessorNumber], NULL, NULL);

            }
        }
        ulProcessorNumber ++;
        affinity >>= 1;
        i--;
    }

    for (i = 0; i < 15; i++)
    {		
        int j = 0;
        for(j = 0; j < 1000000; j++)
        {
            __asm nop;
        }
        if (g_ulCpuCount == (ulProcessorCount - 1))
        {
            //
            // 所有其他cpu都spinlock在g_PatchSpinLock, 在这里做恢复inline的事
            //
            // 如果没有执行到这里Status为STATUS_UNSUCCESSFUL, 可能不是所有cpu都准备好了
            //

            // To do ...
            Status = install_hook(phookinfo);
            if (STATUS_SUCCESS != Status)
            {

            }
            //

            break;
        }
    }

    KeReleaseSpinLock(&g_PatchSpinLock, OldIrql);

    __asm sti;

Exit0:
    if (FunMDL)
    {
        Status = DrvBUnMapPagedCode(FunMDL);
    }

    return Status == STATUS_SUCCESS;
}
