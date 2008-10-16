/********************************************************************
* CreatedOn: 2008-9-28   15:47
* FileName: hookssdt.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once


ULONG ServiceIdFromFn(PVOID pfnHandler);
PVOID HookSystemService(ULONG ulService, PVOID pfnNewHandler);
PVOID HookSystemServiceByFn(PVOID pfnHandler, PVOID pfnNewHandler);

PVOID GetServiceAddress( PVOID pFunc );

/////////////////////////////////////////////////////////////////////////////

extern NTSYSAPI KPROCESSOR_MODE NTAPI KeGetPreviousMode();

//////////////////////////////////////////////////////////////////////////

#pragma pack(push, 1)


typedef struct _SYSTEM_SERVICE_TABLE
{
    PULONG  ServiceTable;       //Service Table Pointer
    PULONG  CounterTableBase;   //Table Item Counter. This table is only updated in checked builds.
    ULONG   ServiceLimit;       //Number of services contained in this table.
    PUCHAR  ArgumentTable;      //Table containing the number of bytes of parameters the handler function takes.
} SYSTEM_SERVICE_TABLE, *PSYSTEM_SERVICE_TABLE;

#pragma pack(pop)

__declspec(dllimport) SYSTEM_SERVICE_TABLE KeServiceDescriptorTable[4];

#define HOOK_SYSCALL(_Function, _Hook, _Orig )  \
{ \
    (*_Orig) = (PVOID) InterlockedExchange( (PLONG) &KeServiceDescriptorTable->ServiceTable[ FUNCTION_PTR(_Function) ], (LONG) _Hook );}

#define UNHOOK_SYSCALL(_Function, _Orig )  \
{ \
    InterlockedExchange( (PLONG) &KeServiceDescriptorTable->ServiceTable[ FUNCTION_PTR(_Function) ], (LONG) _Orig ); }


#define HOOK_SYSCALL_INDEX(_Index, _Hook, _Orig )  \
{ \
    (*_Orig) = (PVOID) InterlockedExchange( (PLONG) &KeServiceDescriptorTable->ServiceTable[ _Index ], (LONG) _Hook );}


#define UNHOOK_SYSCALL_INDEX(_Index, _Orig )  \
{ \
    InterlockedExchange( (PLONG) &KeServiceDescriptorTable->ServiceTable[ _Index ], (LONG) _Orig ); }


#define HOOK_SYSCALL_SHADOW_INDEX(_Index, _Hook, _Orig )  \
{ \
    (*_Orig) = (PVOID) InterlockedExchange( (PLONG) &KeServiceTable[ _Index ], (LONG) _Hook );}

#define UNHOOK_SYSCALL_SHADOW_INDEX(_Index, _Orig )  \
{ \
    InterlockedExchange( (PLONG) &KeServiceTable[ _Index ], (LONG) _Orig ); }


