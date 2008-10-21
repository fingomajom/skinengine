/********************************************************************
* CreatedOn: 2008-9-26   14:33
* FileName: pttdrvcom.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#ifndef __PTTDRVCOM_H__
#define __PTTDRVCOM_H__

#include <devioctl.h>


#define PTTDRV_DRIVER_VERSION_1     (1)
#define PTTDRV_DRIVER_VERSION_2     (2)
#define PTTDRV_DRIVER_VERSION_3     (3)
#define PTTDRV_DRIVER_VERSION_4     (4)


#define PTTDRV_DEVICE_NAME          L"szpttdrv"
#define PTTDRV_NT_DEVICE_NAME       L"\\Device\\szpttdrv"
#define PTTDRV_DOS_DEVICE_NAME      L"\\DosDevices\\szpttdrv"


#define PTTDRV_DRIVER               0x8000


enum{

    DRIVER_STATUS_INVALID = 0,
    DRIVER_STATUS_STOP,
    DRIVER_STATUS_RUNNING,
    DRIVER_STATUS_RESET

};

enum{
    RT_BLACKRULE    = 0x1,
    RT_WHITERULE    = 0x2,
    RT_PROTECTRULE  = 0x4
};

enum {
    CT_ID = 0,
    CT_NAME,
    CT_PATH,
    CT_PATHFILE
};

#pragma pack(push, 1)

typedef struct tag_DRIVER_RULE_INFO
{
    UINT  uRuleType;
    UINT  uContentType;
    UINT  uEnable;

union{
    UINT  uProcessId;
    WCHAR wszFileName[MAX_PATH];
    WCHAR wszPath[MAX_PATH];
    WCHAR wszPathFile[MAX_PATH];
};

}DRIVER_RULE_INFO, *LP_DRIVER_RULE_INFO;


typedef struct tag_DRIVER_EVENT_INFO
{
    UINT  uEventType;

    UINT  uSrcPID;
    WCHAR wszSrcFileName[MAX_PATH];

    UINT  uTagPID;
    WCHAR wszTagFileName[MAX_PATH];

}DRIVER_EVENT_INFO, *LP_DRIVER_EVENT_INFO;

#pragma pack(pop)



//////////////////////////////////////////////////////////////////////////
// 获取驱动版本 
//////////////////////////////////////////////////////////////////////////
#define IOCTL_PTTDRV_GET_VERSION			        \
    CTL_CODE(PTTDRV_DRIVER, 0x01, METHOD_BUFFERED, FILE_ANY_ACCESS)

//////////////////////////////////////////////////////////////////////////
// 获取状态
//////////////////////////////////////////////////////////////////////////
#define IOCTL_PTTDRV_GET_STATUS			        \
    CTL_CODE(PTTDRV_DRIVER, 0x10, METHOD_BUFFERED, FILE_ANY_ACCESS)

//////////////////////////////////////////////////////////////////////////
//  设置状态
//////////////////////////////////////////////////////////////////////////
#define IOCTL_PTTDRV_SET_STATUS			        \
    CTL_CODE(PTTDRV_DRIVER, 0x11, METHOD_BUFFERED, FILE_ANY_ACCESS)


//////////////////////////////////////////////////////////////////////////
// 设置规则
//////////////////////////////////////////////////////////////////////////
#define IOCTL_PTTDRV_APPEND_RULE_INFO           \
    CTL_CODE(PTTDRV_DRIVER, 0x21, METHOD_BUFFERED, FILE_ANY_ACCESS)

//////////////////////////////////////////////////////////////////////////
// 清除规则
//////////////////////////////////////////////////////////////////////////
#define IOCTL_PTTDRV_CLEAR_RULE                 \
    CTL_CODE(PTTDRV_DRIVER, 0x22, METHOD_BUFFERED, FILE_ANY_ACCESS)



//////////////////////////////////////////////////////////////////////////
// 设置日志事件句柄
//////////////////////////////////////////////////////////////////////////
#define IOCTL_PTTDRV_SET_REPORT_EVENT          \
    CTL_CODE(PTTDRV_DRIVER, 0x31, METHOD_BUFFERED, FILE_ANY_ACCESS)


//////////////////////////////////////////////////////////////////////////
// 获取日志
//////////////////////////////////////////////////////////////////////////
#define IOCTL_PTTDRV_GET_REPORT_INFO          \
    CTL_CODE(PTTDRV_DRIVER, 0x32, METHOD_BUFFERED, FILE_ANY_ACCESS)

#endif // __PTTDRVCOM_H__


