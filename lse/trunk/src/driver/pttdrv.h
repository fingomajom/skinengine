/********************************************************************
* CreatedOn: 2008-9-26   14:27
* FileName: pttdrv.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once


typedef struct _DEVICE_EXTENSION
{
	PDEVICE_OBJECT FsDeviceObject;
	WCHAR wcDriver;
	ULONG FsType;
}DEVICE_EXTENSION, *PDEVICE_EXTENSION;

//////////////////////////////////////////////////////////////////////////

void OnUnLoadDriver(PDRIVER_OBJECT DriverObject);


NTSTATUS DeviceShutDown(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
DeviceOpen(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
DeviceClose(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
DeviceIoControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );



NTSTATUS InitProcessNotify();
NTSTATUS UninitProcessNotify();


BOOL dasm_get_instruction_size(
    BYTE* pInst, 
    UINT* pnSize, 
    UINT* pnSizeIfMoved);

BOOL dasm_move_program(
    BYTE* pSrc, 
    UINT  nSrcSize, 
    BYTE* pDst, 
    UINT* pnDstSize, 
    UINT  nDstMax);
