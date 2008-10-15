/********************************************************************
* CreatedOn: 2008-9-26   15:06
* FileName: reg.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#ifndef _REG_H_
#define _REG_H_

NTSTATUS DrvGetValueKey(
    IN  PWCHAR  pwstrKeyPath    ,
    IN  PWCHAR  pwstrValueName  ,
    OUT PULONG  pulValueType    ,
    OUT PVOID   pBuffer         ,
    IN  ULONG   ulBufferLen     ,
    OUT PULONG  pulReturn      );


NTSTATUS DrvSetValueKey(
    PWCHAR pwstrKeyPath  ,
    PWCHAR pwstrValueName,
    ULONG  ulValueType   ,
    PVOID  pBuffer       ,
    ULONG  ulBufferLen  );


NTSTATUS DrvCreateKey( PWCHAR pwstrKeyPath );
NTSTATUS DrvDelRegKey( PWCHAR pwszRegPath );


NTSTATUS DrvDelRegKeyValue(
    PWCHAR pwszRegPath    ,
    PWCHAR pwszValueName );



#endif

//-----------------------------------------------------------------------------//
// 
// Parameters:
//
//  pwstrKeyPath key的路径
//
//  pwstrValueName 要读的value
//
//  pulValueType 返回value type
//     REG_BINARY Binary data in any form 
//     REG_DWORD A 4-byte numerical value 
//     REG_DWORD_LITTLE_ENDIAN A 4-byte numerical value whose least significant byte is at the lowest address 
//     REG_DWORD_BIG_ENDIAN A 4-byte numerical value whose least significant byte is at the highest address 
//     REG_EXPAND_SZ A zero-terminated Unicode string, containing unexpanded references to environment variables, such as “%PATH%” 
//     REG_LINK A Unicode string naming a symbolic link. This type is irrelevant to device and intermediate drivers 
//     REG_MULTI_SZ An array of zero-terminated strings, terminated by another zero 
//     REG_NONE Data with no particular type 
//     REG_SZ A zero-terminated Unicode string 
//     REG_RESOURCE_LIST A device driver’s list of hardware resources, used by the driver or one of the physical devices it controls, in the \ResourceMap tree 
//     REG_RESOURCE_REQUIREMENTS_LIST A device driver’s list of possible hardware resources it or one of the physical devices it controls can use, from which the system writes a subset into the \ResourceMap tree 
//     REG_FULL_RESOURCE_DESCRIPTOR A list of hardware resources that a physical device is using, detected and written into the \HardwareDescription tree by the system 
//  Type不匹配返回失败
//
//  pBuffer 接受value， buffer不够返回STATUS_BUFFER_TOO_SMALL 
//
//  ulBufferLength pBuffer大小
//
//  pulReturn 返回需要buffer大小
//
