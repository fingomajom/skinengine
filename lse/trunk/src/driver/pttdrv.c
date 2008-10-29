
#include "precomp.h"
#pragma hdrstop


PDEVICE_OBJECT g_pDeviceObject = NULL;


NTSTATUS StartWorking();
NTSTATUS StopWorking();



NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	NTSTATUS ntStatus = 0;
    int      idx      = 0;

    UNICODE_STRING sDriverString = { 0 };
	UNICODE_STRING sDeviceString = { 0 };
	
	PDEVICE_OBJECT    pDeviceObject   = NULL;
	PDEVICE_EXTENSION DeviceExtension = NULL;

    do 
    {
        if ( IsSelfDestroy() )
        {
            ntStatus = STATUS_UNSUCCESSFUL;
            break;
        }

        RtlInitUnicodeString(&sDriverString, PTTDRV_NT_DEVICE_NAME);
        RtlInitUnicodeString(&sDeviceString, PTTDRV_DOS_DEVICE_NAME);

        ntStatus = IoCreateDevice(DriverObject, 
            sizeof(DEVICE_EXTENSION), 
            &sDriverString, 
            FILE_DEVICE_UNKNOWN, 
            0, 
            FALSE, 
            &pDeviceObject);
        if(ntStatus != STATUS_SUCCESS)
            break;

        ntStatus = IoCreateSymbolicLink(&sDeviceString, &sDriverString);
        if(ntStatus != STATUS_SUCCESS)
        {
            IoDeleteDevice(pDeviceObject);
            break;
        }

        g_pDeviceObject = pDeviceObject;

        DriverObject->DriverUnload = OnUnLoadDriver;

        DriverObject->MajorFunction[IRP_MJ_CREATE]         = DeviceOpen;
        DriverObject->MajorFunction[IRP_MJ_CLOSE]          = DeviceClose;
        DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceIoControl;
        DriverObject->MajorFunction[IRP_MJ_SHUTDOWN]       = DeviceShutDown;

        IoRegisterShutdownNotification(pDeviceObject);


        ntStatus = StartWorking();


    } while( 0 );


    if ( ntStatus == STATUS_SUCCESS )
        DbgPrint( ("DriverEntry Successed") );
    else
        DbgPrint( ("DriverEntry Failed [0x%x]", ntStatus ) );

	return ntStatus;
}


void OnUnLoadDriver(PDRIVER_OBJECT DriverObject)
{
    NTSTATUS ntStatus = STATUS_DEVICE_CONFIGURATION_ERROR;

    UNICODE_STRING  uszDeviceString = { 0 };


    StopWorking();

	IoDeleteDevice(DriverObject->DeviceObject);

	RtlInitUnicodeString(&uszDeviceString, PTTDRV_DOS_DEVICE_NAME);
	IoDeleteSymbolicLink(&uszDeviceString);


    DbgPrint( ("OnUnLoadDriver") );
}



NTSTATUS StartWorking()
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    do 
    {
        if ( !InitKernelVersion() )
            break;

        InitRuleList( &g_BlackRuleList   );
        InitRuleList( &g_WhiteRuleList   );
        InitRuleList( &g_ProtectRuleList );
        InitRuleList( &g_RunPIDList      );

       
        InitDefaultRuleList( RT_BLACKRULE | RT_WHITERULE | RT_PROTECTRULE );
        
        InitEventList( & g_event_list );

        ntStatus = InitProcessNotify();
        if ( ntStatus != STATUS_SUCCESS )
            break;

        InitLoadImageNotify();


        ntStatus = STATUS_UNSUCCESSFUL;

        if ( !install_all_hook() )
            break;


        ntStatus = STATUS_SUCCESS;

    } while( 0 );


    return ntStatus;
}


NTSTATUS StopWorking()
{
    NTSTATUS ntStatus = STATUS_SUCCESS;


    UninitProcessNotify();
    UninitLoadImageNotify();

    //uninstall_all_hook();

    return ntStatus;
}
