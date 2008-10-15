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

typedef struct
{
    PVOID* pServiceTable;   //Service Table Pointer
    PULONG pulTableCounter; //Table Item Counter. This table is only updated in checked builds.
    ULONG  ulServiceCounter;//Number of services contained in this table.
    PUCHAR rguchParamTable;	//Table containing the number of bytes of parameters the handler function takes.
}SERVICE_TABLE;

#pragma pack(pop)

