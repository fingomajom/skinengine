#include "precomp.h"
#pragma hdrstop



LONG g_lFileHandleCount = 0;



NTSTATUS DeviceShutDown(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    NormalStop();

    Irp->IoStatus.Information = 0;
    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}


NTSTATUS
DeviceOpen(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp)
{
    PIO_STACK_LOCATION  pIrpSp;
    NTSTATUS            NtStatus = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(DeviceObject);


    pIrpSp = IoGetCurrentIrpStackLocation(Irp);

    pIrpSp->FileObject->FsContext = NULL;
    pIrpSp->FileObject->FsContext2 = NULL;

    DbgPrint( ("==>DeviceOpen: \n") );

    if ( InterlockedIncrement(&g_lFileHandleCount) > 1 )
    {
        NtStatus = STATUS_INVALID_HANDLE;
    }
    else
    {
        NtStatus = STATUS_SUCCESS;
    }

    InterlockedExchange(&g_lFileHandleCount, 1);

    Irp->IoStatus.Information = 0;
    Irp->IoStatus.Status = NtStatus;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    DbgPrint( ("<== DeviceOpen\n") );

    return NtStatus;

}

NTSTATUS
DeviceClose(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    DbgPrint( ("==> DeviceClose: \n") );

    Irp->IoStatus.Information = 0;
    Irp->IoStatus.Status      = ntStatus;

    InterlockedExchange(&g_lFileHandleCount, 0);

    IoCompleteRequest (Irp, IO_NO_INCREMENT);

    DbgPrint( ("<== DeviceClose\n") );

    return ntStatus;
}


NTSTATUS
DeviceIoControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp)
{

    NTSTATUS              ntStatus = STATUS_UNSUCCESSFUL;
    PIO_STACK_LOCATION    irpStack = IoGetCurrentIrpStackLocation(Irp);

    LONG                  l_long   = 0;
    PULONG                p_long   = 0;
    PVOID                 p_buffer = Irp->AssociatedIrp.SystemBuffer;

    ULONG uInputBufferLength  = irpStack->Parameters.DeviceIoControl.InputBufferLength;
    ULONG uOutputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;


    DbgPrint( ("==> DeviceIoControl: \n") );


    switch( irpStack->Parameters.DeviceIoControl.IoControlCode )
    {
        //////////////////////////////////////////////////////////////////////////
        // »ñÈ¡·À»ðÇ½ °æ±¾ 
        //////////////////////////////////////////////////////////////////////////
    //case IOCTL_ANTIARP_GET_VERSION:
    default:
        break;

        //-----------------------------------------------------------------------------------
    }

    Irp->IoStatus.Status = ntStatus;

    if ( ntStatus == STATUS_SUCCESS )
        Irp->IoStatus.Information = irpStack->Parameters.DeviceIoControl.OutputBufferLength;
    else
        Irp->IoStatus.Information = 0;

    IoCompleteRequest( Irp, IO_NO_INCREMENT );

    DbgPrint( ("<== DeviceIoControl\n") );

    return ntStatus;
}
