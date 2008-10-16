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

BOOL install_hook     ( lp_hook_item_info phookinfo );
BOOL uninstall_hook   ( lp_hook_item_info phookinfo );
BOOL safe_install_hook( lp_hook_item_info phookinfo );


#define WPOFF() \
    _asm mov eax, cr0 \
    _asm and eax, NOT 10000H \
    _asm mov cr0, eax

#define WPON() \
    _asm mov eax, cr0 \
    _asm or eax, 10000H \
    _asm mov cr0, eax



