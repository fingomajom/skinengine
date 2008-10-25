/********************************************************************
* CreatedOn: 2008-9-25   15:50
* FileName: kernelosinfo.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once


enum os_version
{
    os_unknown = 0,
    os_2k_sp0,
    os_2k_sp1,
    os_2k_sp2,
    os_2k_sp3,
    os_2k_sp4,

    os_xp_sp0,
    os_xp_sp1,
    os_xp_sp2,
    os_xp_sp3,
    
    os_2003_sp0,
    os_2003_sp1,
    os_2003_sp2,

    os_vista_sp0,
    os_vista_sp1
};

typedef struct _tag_knl_os_info_offset
{
    DWORD dwEnable;
    
    DWORD dwTerminateProcessSysId;
    DWORD dwCreateThreadSysId;
    DWORD dwNtReadVirtualMemorySysId;
    DWORD dwNtWriteVirtualMemorySysId;
    DWORD dwNtOpenSectionSysId;

    DWORD dwUserMessageCallSysId;
    DWORD dwUserSendInputSysId;
    DWORD dwUserPostMessageSysId;
    DWORD dwUserPostThreadMessageSysId;

    DWORD dwNtUserQueryWindowSysId;
    
    DWORD dwProcessPebOffset;

}knl_os_info_offset, *lp_knl_os_info_offset;


//////////////////////////////////////////////////////////////////////////

#define  PROC_NAME_LEN 64

extern DWORD g_dwOsVersion;
extern knl_os_info_offset g_knl_current_offset;


BOOL InitKernelVersion();

