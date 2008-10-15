/********************************************************************
* CreatedOn: 2008-9-28   14:55
* FileName: inlinehook.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once


//////////////////////////////////////////////////////////////////////////

#define max_stub_size   20
#define max_head_size   20



//////////////////////////////////////////////////////////////////////////

typedef struct tag_hook_item_info
{
    BOOL  bcanhook;

    PBYTE ptargetfunc;
    PBYTE phookfunc;

    BYTE  pstubbuf[max_stub_size];
    UINT  nstubsize;

    BYTE  pheadbuf[max_head_size];
    UINT  nheadsize;

}hook_item_info, *lp_hook_item_info;

//////////////////////////////////////////////////////////////////////////


BOOL install_all_hook();
BOOL uninstall_all_hook();

//////////////////////////////////////////////////////////////////////////

BOOL install_hook     ( lp_hook_item_info phookinfo );
BOOL uninstall_hook   ( lp_hook_item_info phookinfo );
BOOL safe_install_hook( lp_hook_item_info phookinfo );


//BOOL init_hook();
//BOOL uninit_hook();



typedef enum{

    hook_ke_user_mode_callback = 0,

    hook_nt_open_process = 1,

    hook_function_count

} em_hook_func_type ;


PROC get_stub_func_address( em_hook_func_type funcid );



#define WPOFF() \
    _asm mov eax, cr0 \
    _asm and eax, NOT 10000H \
    _asm mov cr0, eax

#define WPON() \
    _asm mov eax, cr0 \
    _asm or eax, 10000H \
    _asm mov cr0, eax


//////////////////////////////////////////////////////////////////////////

NTSTATUS NTAPI HookKeUserModeCallback(
    ULONG  ApiNumber, 
    PVOID  InputBuffer, 
    ULONG  InputLength, 
    PVOID *OutputBuffer,
    PULONG OutputLength);

NTSTATUS NTAPI HookNtOpenProcess(
    PHANDLE     ProcessHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PCLIENT_ID  ClientId);
