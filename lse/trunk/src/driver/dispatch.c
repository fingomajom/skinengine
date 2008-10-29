#include "precomp.h"
#pragma hdrstop



LONG g_lFileHandleCount = 0;
//LONG g_lDriverStatus    = DRIVER_STATUS_STOP;
LONG g_lDriverStatus    = DRIVER_STATUS_RUNNING;

DRIVER_CONFIG g_drv_config = { 0xFFFFFFFF, 1, 1, 1, 0 };


BOOL AddRule( LP_DRIVER_RULE_INFO RuleInfo );
BOOL ClearRule( UINT uRuleType );

NTSTATUS DeviceShutDown(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    ResumeErrorCount();

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

    PVOID                 p_buffer = Irp->AssociatedIrp.SystemBuffer;

    ULONG uInputBufferLength  = irpStack->Parameters.DeviceIoControl.InputBufferLength;
    ULONG uOutputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;


    DbgPrint( ("==> DeviceIoControl: \n") );


    switch( irpStack->Parameters.DeviceIoControl.IoControlCode )
    {
    case IOCTL_PTTDRV_GET_VERSION:
        //////////////////////////////////////////////////////////////////////////
        // 获取驱动版本 
        //////////////////////////////////////////////////////////////////////////
        {
            DbgPrint( ("DeviceIoControl IOCTL_PTTDRV_GET_VERSION\n") );

            if ( uOutputBufferLength >= sizeof( ULONG ) )
            {
                if (p_buffer != NULL)
                {
                    *((PLONG)p_buffer) = ( ULONG ) PTTDRV_DRIVER_VERSION_1 ;
                }
                else
                    break;

                ntStatus = STATUS_SUCCESS;

                DbgPrint( ("Success DeviceIoControl IOCTL_PTTDRV_GET_VERSION\n") );
            }
        }
        break;

    case IOCTL_PTTDRV_GET_STATUS:
        //////////////////////////////////////////////////////////////////////////
        // 获取状态 
        //////////////////////////////////////////////////////////////////////////
        {
            DbgPrint( ("DeviceIoControl IOCTL_PTTDRV_GET_VERSION\n") );

            if ( uOutputBufferLength >= sizeof( ULONG ) )
            {
                if (p_buffer != NULL)
                {
                    *((PLONG)p_buffer) = ( ULONG ) g_lDriverStatus ;
                }
                else
                    break;

                ntStatus = STATUS_SUCCESS;

                DbgPrint( ("Success DeviceIoControl IOCTL_PTTDRV_GET_VERSION\n") );
            }
        }
        break;
    case IOCTL_PTTDRV_SET_STATUS:
        //////////////////////////////////////////////////////////////////////////
        // 设置状态 
        //////////////////////////////////////////////////////////////////////////
        {
            DbgPrint( ("DeviceIoControl IOCTL_PTTDRV_SET_STATUS\n") );

            if ( uInputBufferLength >= sizeof( ULONG ) )
            {
                if (p_buffer != NULL)
                {
                    InterlockedExchange( &g_lDriverStatus,
                        *((PLONG)p_buffer));
                }
                else
                    break;

                ntStatus = STATUS_SUCCESS;

                DbgPrint( ("Success DeviceIoControl IOCTL_PTTDRV_SET_STATUS\n") );
            }
        }
        break;
        
    case IOCTL_PTTDRV_GET_CONFIG:
        //////////////////////////////////////////////////////////////////////////
        // 设置 
        //////////////////////////////////////////////////////////////////////////
        {
            DbgPrint( ("DeviceIoControl IOCTL_PTTDRV_GET_CONFIG\n") );

            if ( uOutputBufferLength >= sizeof( DRIVER_CONFIG ) )
            {
                if (p_buffer != NULL)
                {
                    *((DRIVER_CONFIG*)p_buffer) = g_drv_config;
                }
                else
                    break;

                ntStatus = STATUS_SUCCESS;

                DbgPrint( ("Success DeviceIoControl IOCTL_PTTDRV_GET_CONFIG\n") );
            }
        }
        break;

    case IOCTL_PTTDRV_SET_CONFIG:
        //////////////////////////////////////////////////////////////////////////
        // 设置 
        //////////////////////////////////////////////////////////////////////////
        {
            DbgPrint( ("DeviceIoControl IOCTL_PTTDRV_SET_CONFIG\n") );

            if ( uInputBufferLength >= sizeof( DRIVER_CONFIG ) )
            {
                if (p_buffer != NULL)
                {
                    g_drv_config = *((DRIVER_CONFIG*)p_buffer);
                }
                else
                    break;

                ntStatus = STATUS_SUCCESS;

                DbgPrint( ("Success DeviceIoControl IOCTL_PTTDRV_SET_CONFIG\n") );
            }
        }
        break;

    case IOCTL_PTTDRV_APPEND_RULE_INFO:
        //////////////////////////////////////////////////////////////////////////
        // 设置规则
        //////////////////////////////////////////////////////////////////////////
        {
            DbgPrint( ("DeviceIoControl IOCTL_PTTDRV_APPEND_RULE_INFO %d,%d,%x\n", uInputBufferLength,sizeof( DRIVER_RULE_INFO ), p_buffer) );

            if ( uInputBufferLength == sizeof( DRIVER_RULE_INFO ) && p_buffer != NULL )
            {
                if ( !AddRule( (LP_DRIVER_RULE_INFO)p_buffer ) )
                    break;

                ntStatus = STATUS_SUCCESS;

                DbgPrint( ("Success DeviceIoControl IOCTL_PTTDRV_APPEND_RULE_INFO\n") );
            }
        }
        break;

    case IOCTL_PTTDRV_CLEAR_RULE:        
        //////////////////////////////////////////////////////////////////////////
        // 清除规则
        //////////////////////////////////////////////////////////////////////////
        {
            DbgPrint( ("DeviceIoControl IOCTL_PTTDRV_CLEAR_RULE %d,%x\n", uInputBufferLength, p_buffer) );

            if ( uInputBufferLength == sizeof( UINT ) && p_buffer != NULL )
            {
                if ( !ClearRule( *(UINT*)p_buffer ) )
                    break;

                ntStatus = STATUS_SUCCESS;

                DbgPrint( ("Success DeviceIoControl IOCTL_PTTDRV_CLEAR_RULE\n") );
            }

        }

        break;

    case IOCTL_PTTDRV_SET_REPORT_EVENT:        
        //////////////////////////////////////////////////////////////////////////
        // 设置日志事件句柄
        //////////////////////////////////////////////////////////////////////////
        {
            DbgPrint( ("DeviceIoControl IOCTL_PTTDRV_SET_REPORT_EVENT %d,%x\n", uInputBufferLength, p_buffer) );

            if ( uInputBufferLength == sizeof( HANDLE ) && p_buffer != NULL )
            {
                ntStatus = ObReferenceObjectByHandle( 
                    *((HANDLE*)p_buffer),
                    0, (POBJECT_TYPE) NULL, UserMode, 
                    (PVOID) & g_event_list.ReportEvent, 
                    (POBJECT_HANDLE_INFORMATION) NULL);

                if ( !NT_SUCCESS(ntStatus) )
                    break;

                ntStatus = STATUS_SUCCESS;

                DbgPrint( ("Success DeviceIoControl IOCTL_PTTDRV_SET_REPORT_EVENT\n") );
            }
        }

        break;

    case IOCTL_PTTDRV_GET_REPORT_INFO:        
        //////////////////////////////////////////////////////////////////////////
        // 获取日志
        //////////////////////////////////////////////////////////////////////////
        {
            DbgPrint( ("DeviceIoControl IOCTL_PTTDRV_GET_REPORT_INFO %d,%x\n", uOutputBufferLength, p_buffer) );

            if ( uOutputBufferLength >= sizeof( DRIVER_EVENT_INFO ) && p_buffer != NULL )
            {
                LP_EVENT_INFO pInfo = PopHeadEvent(&g_event_list);

                if ( pInfo != NULL )
                {
                    *((LP_DRIVER_EVENT_INFO)p_buffer) = pInfo->EventInfo;
                }
                else
                    break;

                ntStatus = STATUS_SUCCESS;

                DbgPrint( ("Success DeviceIoControl IOCTL_PTTDRV_GET_REPORT_INFO\n") );
            }
        }

        break;

    default:
        break;

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


BOOL AddRule( LP_DRIVER_RULE_INFO RuleInfo )
{
    BOOL bResult = FALSE;


    switch( RuleInfo->uRuleType )
    {
    case RT_BLACKRULE:
        bResult = AppendRule_I(&g_BlackRuleList, 
            RuleInfo->uContentType, 
            RuleInfo->uEnable,
            &RuleInfo->uProcessId);
        break;
    case RT_WHITERULE:
        bResult = AppendRule_I(&g_WhiteRuleList, 
            RuleInfo->uContentType, 
            RuleInfo->uEnable,
            &RuleInfo->uProcessId);
        break;
    case RT_PROTECTRULE:
        bResult = AppendRule_I(&g_ProtectRuleList, 
            RuleInfo->uContentType, 
            RuleInfo->uEnable,
            &RuleInfo->uProcessId);
        break;
    default:
        bResult = FALSE;
    }

    return bResult;
}


BOOL ClearRule( UINT uRuleType )
{
    BOOL bResult = TRUE;
    
    if ( uRuleType & RT_BLACKRULE )
    {   
        UninitRuleList( &g_BlackRuleList );
    }

    if ( uRuleType & RT_WHITERULE )
    {   
        UninitRuleList( &g_WhiteRuleList );
    }

    if ( uRuleType & RT_PROTECTRULE )
    {   
        UninitRuleList( &g_ProtectRuleList );
    }

    InitDefaultRuleList( uRuleType );

    return bResult;
}
