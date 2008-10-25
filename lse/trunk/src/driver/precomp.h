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

typedef LONG_PTR            LRESULT;
typedef unsigned int        UINT_PTR, *PUINT_PTR;
typedef UINT_PTR            WPARAM;
typedef LONG_PTR            LPARAM;
typedef HANDLE HICON;
typedef HICON HCURSOR;


#define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)

#ifndef CALLBACK
#define CALLBACK __stdcall
#endif
typedef int (CALLBACK *PROC)();

typedef struct tagPOINT
{
    LONG x;
    LONG y;
} 	POINT;

typedef struct tagPOINT *PPOINT;

typedef struct tagPOINT *LPPOINT;

typedef struct tagMOUSEINPUT 
{
    LONG					dx;
    LONG					dy;
    DWORD					mouseData;
    DWORD					dwFlags;
    DWORD					time;
    ULONG_PTR				dwExtraInfo;
} MOUSEINPUT, *PMOUSEINPUT, FAR* LPMOUSEINPUT;

typedef struct tagKEYBDINPUT 
{
    WORD					wVk;
    WORD					wScan;
    DWORD					dwFlags;
    DWORD					time;
    ULONG_PTR				dwExtraInfo;
} KEYBDINPUT, *PKEYBDINPUT, FAR* LPKEYBDINPUT;

typedef struct tagHARDWAREINPUT {
    DWORD					uMsg;
    WORD					wParamL;
    WORD					wParamH;
} HARDWAREINPUT, *PHARDWAREINPUT, FAR* LPHARDWAREINPUT;

#define INPUT_MOUSE     0
#define INPUT_KEYBOARD  1
#define INPUT_HARDWARE  2

typedef struct tagINPUT {
    DWORD   type;

    union
    {
        MOUSEINPUT			mi;
        KEYBDINPUT			ki;
        HARDWAREINPUT		hi;
    };
} INPUT, *PINPUT, FAR* LPINPUT;

// 线程监控模块用到的一些结构定义:
typedef struct _THREAD_BASIC_INFORMATION 
{
    NTSTATUS                ExitStatus;
    PVOID                   TebBaseAddress;
    CLIENT_ID               ClientId;
    KAFFINITY               AffinityMask;
    KPRIORITY               Priority;
    KPRIORITY               BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;


typedef int (__stdcall *PROC)();

#define MAX_PATH    260


#define MEM_TAG     'pttd'

#if DBG
#define DbgPrint(arg) DbgPrint arg
#else
#define DbgPrint(arg) 
#endif


#define REG_CURRENT_VERSION_PATH	L"\\REGISTRY\\MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"
#define CSD_VERSION					L"CSDVersion"
#define WINDOWSROOT                 L"SystemRoot"


#include "..\public\pttdrvcom.h"



#include "reg.h"
#include "misc.h"
#include "rule.h"
#include "eventrep.h"
#include "pttdrv.h"
#include "selfdestroy.h"
#include "inlinehook.h"
#include "hookssdt.h"
#include "knlosinfo.h"
#include "hookpointmgt.h"
