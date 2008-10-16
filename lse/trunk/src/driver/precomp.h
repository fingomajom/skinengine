/********************************************************************
* CreatedOn: 2008-8-25   14:38
* FileName: precomp.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/


#include <ntifs.h>
#include <string.h>


typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef ULONG DWORD;
typedef BYTE *PBYTE;
typedef BYTE *LPBYTE;
typedef int	BOOL;
typedef unsigned int UINT;
typedef char CHAR;
typedef char *PCHAR;
typedef PVOID POBJECT;
typedef void *HINSTANCE;
typedef void *HOOKPROC;
typedef void *HHOOK;
typedef void *HWND;
typedef void *HMENU;
typedef void *LPVOID;
typedef LARGE_INTEGER QWORD;
typedef void *HDESK;

typedef int (__stdcall *PROC)();

#define MAX_PATH    266


#define MEM_TAG     'pttd'

#if DBG
#define DbgPrint(arg) DbgPrint arg
#else
#define DbgPrint(arg) 
#endif


#include "..\public\pttdrvcom.h"

#include "reg.h"
#include "pttdrv.h"
#include "selfdestroy.h"
#include "inlinehook.h"
#include "hookssdt.h"
#include "knlosinfo.h"
#include "hookpointmgt.h"
