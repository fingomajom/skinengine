#include "precomp.h"
#pragma hdrstop


NTSTATUS DrvGetValueKey(
    IN  PWCHAR  pwstrKeyPath,
    IN  PWCHAR  pwstrValueName,
    OUT PULONG  pulValueType,
    OUT PVOID   pBuffer,
    IN  ULONG   ulBufferLen,
    OUT PULONG  pulReturn)
{
    NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;

    HANDLE hKey = NULL;

    ULONG  ulAllocateBufferLength = 1024;
    ULONG  ulReturnLength         = 0;

    UNICODE_STRING uniKeyPath   = { 0 };
    UNICODE_STRING uniValueName = { 0 };

    OBJECT_ATTRIBUTES ObjectAttributes = { 0 };
    
    PKEY_VALUE_FULL_INFORMATION pInfoBuffer = NULL; 


    RtlInitUnicodeString(&uniKeyPath, pwstrKeyPath);

    InitializeObjectAttributes( &ObjectAttributes, 
        &uniKeyPath,
        OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
        NULL, NULL);

    do
    {
        ntStatus = ZwOpenKey( &hKey, KEY_READ, &ObjectAttributes );
        if ( ntStatus != STATUS_SUCCESS)
        {
            break;
        }

        DbgPrint( ("ZwOpenKey") );

        RtlInitUnicodeString( &uniValueName, pwstrValueName );

        for( ; 1 ; )
        {
            pInfoBuffer = (PKEY_VALUE_FULL_INFORMATION)ExAllocatePoolWithTag(
                NonPagedPool, 
                ulAllocateBufferLength, 
                MEM_TAG);

            if ( pInfoBuffer == NULL )
            {
                ntStatus = STATUS_INSUFFICIENT_RESOURCES;
                break;
            }
            DbgPrint( ("ExAllocatePoolWithTag") );

            ntStatus = ZwQueryValueKey( hKey, 
                &uniValueName, 
                KeyValueFullInformation,
                pInfoBuffer, 
                ulAllocateBufferLength, 
                &ulReturnLength);

            if ((STATUS_BUFFER_OVERFLOW == ntStatus) || (STATUS_BUFFER_TOO_SMALL == ntStatus))
            {
                if (NULL != pInfoBuffer)
                {
                    ExFreePool(pInfoBuffer);
                    pInfoBuffer = NULL;
                }

                ulAllocateBufferLength = ulReturnLength;

                continue;
            }

            break;
        }

        if ( ntStatus != STATUS_SUCCESS)
        {
            break;
        }
        DbgPrint( ("ZwQueryValueKey") );

        if ( ulBufferLen < pInfoBuffer->DataLength )
        {
            if ( pulReturn != NULL)
            {
                *pulReturn = pInfoBuffer->DataLength;
            }

            ntStatus = STATUS_BUFFER_TOO_SMALL;

            break;
        }
        DbgPrint( ("ulBufferLen") );

        if ( pulValueType != NULL )
        {
            *pulValueType = pInfoBuffer->Type;
        }

        if ( pulReturn != NULL )
        {
            *pulReturn = pInfoBuffer->DataLength;
        }

        RtlCopyMemory( pBuffer, 
            ((PUCHAR)pInfoBuffer + pInfoBuffer->DataOffset), 
            pInfoBuffer->DataLength);

        ntStatus = STATUS_SUCCESS;

    } while ( 0 );

    if ( pInfoBuffer != NULL )
    {
        ExFreePool(pInfoBuffer);
    }
    if ( hKey != NULL )
    {
        ZwClose(hKey);
    }

    return ntStatus;
}


NTSTATUS DrvSetValueKey(
    PWCHAR pwstrKeyPath,
    PWCHAR pwstrValueName,
    ULONG  ulValueType,
    PVOID  pBuffer,
    ULONG  ulBufferLen)
{
    NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;

    UNICODE_STRING    uniKeyPath   = { 0 };
    UNICODE_STRING    uniValueName = { 0 };

    OBJECT_ATTRIBUTES ObjectAttributes = {0};

    HANDLE hKey = NULL;

    RtlInitUnicodeString( &uniKeyPath, pwstrKeyPath );

    InitializeObjectAttributes( &ObjectAttributes,
        &uniKeyPath,
        OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
        NULL, NULL);

    do
    {
        ntStatus = ZwOpenKey( &hKey, KEY_WRITE, &ObjectAttributes);
        if (STATUS_SUCCESS != ntStatus)
        {
            break;
        }

        RtlInitUnicodeString( &uniValueName, pwstrValueName);

        ntStatus = ZwSetValueKey( hKey, &uniValueName, 0, ulValueType, pBuffer, ulBufferLen);

    } while ( 0 );


    if (NULL != hKey)
        ZwClose(hKey);

    return ntStatus;
}


NTSTATUS DrvCreateKey( PWCHAR pwstrKeyPath )
{
    NTSTATUS Status = STATUS_UNSUCCESSFUL;
    HANDLE   hKey   = NULL;

    UNICODE_STRING    uniKeyPath       = {0};
    OBJECT_ATTRIBUTES ObjectAttributes = {0};

    RtlInitUnicodeString(&uniKeyPath, pwstrKeyPath);

    InitializeObjectAttributes( &ObjectAttributes, 
        &uniKeyPath,
        OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
        NULL, NULL);

    Status = ZwCreateKey(&hKey, KEY_WRITE,
        &ObjectAttributes, 0, NULL,
        REG_OPTION_NON_VOLATILE, NULL);

    if (hKey != NULL)
        ZwClose( hKey );

    return Status;

}

NTSTATUS DrvDelRegKey( PWCHAR pwszRegPath )
{
    NTSTATUS Status = STATUS_UNSUCCESSFUL;
    HANDLE   hKey   = NULL;

    UNICODE_STRING    usRegPath   = { 0 };
    OBJECT_ATTRIBUTES ob          = { 0 };


    RtlInitUnicodeString(&usRegPath, pwszRegPath);

    ob.Length = sizeof(OBJECT_ATTRIBUTES);
    ob.ObjectName = &usRegPath;
    ob.RootDirectory = NULL;
    ob.SecurityQualityOfService = NULL;
    ob.SecurityDescriptor = NULL;
    ob.Attributes = OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE;

    do 
    {
        Status = ZwOpenKey(&hKey, KEY_ALL_ACCESS, &ob);
        if (Status != STATUS_SUCCESS)
            break;

        Status = ZwDeleteKey(hKey);

    } while( 0 );
    
    if (hKey != NULL)
        ZwClose( hKey );

    return Status;

}


NTSTATUS DrvDelRegKeyValue(
    PWCHAR pwszRegPath,
    PWCHAR pwszValueName )
{
    NTSTATUS Status = STATUS_UNSUCCESSFUL;
    //HANDLE   hKey   = NULL;

    //UNICODE_STRING usRegPath   = { 0 };
    //UNICODE_STRING usValueName = { 0 };

    //OBJECT_ATTRIBUTES ob = { 0 };


    //RtlInitUnicodeString(&usRegPath  , pwszRegPath);
    //RtlInitUnicodeString(&usValueName, pwszValueName);

    //ob.Length = sizeof(OBJECT_ATTRIBUTES);
    //ob.ObjectName = &usRegPath;
    //ob.RootDirectory = NULL;
    //ob.SecurityQualityOfService = NULL;
    //ob.SecurityDescriptor = NULL;
    //ob.Attributes = OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE;

    //do 
    //{
    //    Status = ZwOpenKey(&hKey, KEY_ALL_ACCESS, &ob);
    //    if (Status != STATUS_SUCCESS)
    //        break;

    //    Status = ZwDeleteValueKey(hKey, &usValueName);

    //} while( 0 );
    //
    //if (hKey != NULL)
    //    ZwClose( hKey );

    return Status;

}
