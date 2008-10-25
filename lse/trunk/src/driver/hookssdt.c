#include "precomp.h"
#pragma hdrstop


/////////////////////////////////////////////////////////////////////////////


NTKERNELAPI NTSTATUS KeAddSystemServiceTable( 
    PVOID    lpAddressTable, 
    BOOLEAN  bUnknown,
    ULONG    dwNumEntries, 
    PVOID    lpParameterTable,
    ULONG    dwTableID 
    );


PMDL                     KeServiceDescriptorTableShadowMDL	= NULL;
PULONG                   KeServiceTable						= NULL;
PSYSTEM_SERVICE_TABLE    KeServiceDescriptorTableShadow		= NULL;

/////////////////////////////////////////////////////////////////////////////

#define SSTForId(x)     ( (( ULONG) x) >> 12       )
#define IndexForId(x)   ( (( ULONG) x) &  0xFFFUL  )
#define ValidId(x)      ( (((ULONG) x) & ~0x3FFFUL ) == 0UL )

/////////////////////////////////////////////////////////////////////////////

ULONG ServiceIdFromFn(PVOID pfnHandler)
{
    PUCHAR	pbHandler;
    ULONG	ulService;

    pbHandler = (PUCHAR) pfnHandler;

    if (*pbHandler != 0xB8) 
    {
        return 0UL;
    }

    ulService = *(PULONG) (pbHandler + 1);

    if (!ValidId(ulService)) 
    {
        return 0UL;
    }

    return ulService;
}

/////////////////////////////////////////////////////////////////////////////

PVOID HookSystemService(ULONG ulService, PVOID pfnNewHandler)
{
    ULONG ulSST, ulIndex;
    PVOID *pfnHandler, pfnOldHandler;
    SYSTEM_SERVICE_TABLE *pDescriptor;

    if (!ValidId (ulService)) 
    {
        return NULL;
    }

    ulSST = SSTForId(ulService);
    ulIndex = IndexForId(ulService);

    pDescriptor = &KeServiceDescriptorTable[ulSST];
    if (pDescriptor == NULL) 
    {
        return NULL;
    }

    if (pDescriptor->ServiceLimit < ulIndex) 
    {
        return NULL;
    }

    pfnHandler = (PVOID *)&pDescriptor->ServiceTable[ulIndex];

    pfnOldHandler = *pfnHandler;

    __try 
    {
        WPOFF();
        pfnOldHandler = InterlockedExchangePointer(pfnHandler, pfnNewHandler);
        WPON();
    }__except (EXCEPTION_EXECUTE_HANDLER) 
    {
        pfnOldHandler = pfnHandler;
    }

    return pfnOldHandler;
}

/////////////////////////////////////////////////////////////////////////////

PVOID HookSystemServiceByFn(PVOID pfnHandler, PVOID pfnNewHandler)
{
    ULONG ulService;

    ulService = ServiceIdFromFn(pfnHandler);
    if (ulService == 0UL)
        return NULL;

    return HookSystemService(ulService, pfnNewHandler);
}

/////////////////////////////////////////////////////////////////////////////

PVOID GetServiceAddress(PVOID pFunc)
{
    ULONG ulService;
    ULONG ulSST, ulIndex;
    PVOID *pfnHandler, pfnOldHandler;
    SYSTEM_SERVICE_TABLE *pDescriptor;

    ulService = ServiceIdFromFn(pFunc);
    if (ulService == 0UL)
        return NULL;

    if (!ValidId (ulService)) 
    {
        return NULL;
    }

    ulSST = SSTForId(ulService);
    ulIndex = IndexForId(ulService);

    pDescriptor = &KeServiceDescriptorTable[ulSST];
    if (pDescriptor == NULL) 
    {
        return NULL;
    }

    if (pDescriptor->ServiceLimit < ulIndex) 
    {
        return NULL;
    }

    pfnHandler = (PVOID*)&pDescriptor->ServiceTable[ulIndex];

    pfnOldHandler = *pfnHandler;

    return pfnOldHandler;
}

PSYSTEM_SERVICE_TABLE SzFindShadowTable(void)
{
    UCHAR* pCheckArea = (UCHAR*) KeAddSystemServiceTable;
    PSYSTEM_SERVICE_TABLE  pSrvTable = NULL;

    int i;

    for (i = 0; i < 100; i++)
    {
        __try
        {
            pSrvTable = (PSYSTEM_SERVICE_TABLE)(*(ULONG*)pCheckArea);
            if (
                !MmIsAddressValid(pSrvTable)            ||
                (pSrvTable == KeServiceDescriptorTable) ||
                (RtlCompareMemory(
                pSrvTable,
                KeServiceDescriptorTable, 
                sizeof (PSYSTEM_SERVICE_TABLE)) != sizeof (PSYSTEM_SERVICE_TABLE))
                )
            {
                pCheckArea++;
                pSrvTable = NULL;
            }
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            pSrvTable = NULL;
        }
        if (pSrvTable)
            break;
    }


    return pSrvTable;
}


//////////////////////////////////////////////////////////////////////////