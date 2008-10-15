#include "precomp.h"
#pragma hdrstop


/////////////////////////////////////////////////////////////////////////////

__declspec(dllimport) SERVICE_TABLE KeServiceDescriptorTable[4];

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
    SERVICE_TABLE *pDescriptor;

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

    if (pDescriptor->ulServiceCounter < ulIndex) 
    {
        return NULL;
    }

    pfnHandler = &pDescriptor->pServiceTable[ulIndex];

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
    SERVICE_TABLE *pDescriptor;

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

    if (pDescriptor->ulServiceCounter < ulIndex) 
    {
        return NULL;
    }

    pfnHandler = &pDescriptor->pServiceTable[ulIndex];

    pfnOldHandler = *pfnHandler;

    return pfnOldHandler;
}

//////////////////////////////////////////////////////////////////////////