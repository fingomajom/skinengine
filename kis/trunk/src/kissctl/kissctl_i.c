

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Sun May 04 12:01:11 2008
 */
/* Compiler settings for .\kissctl.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_ISvrCaller,0x2E2D8BA6,0x3CB1,0x4927,0xA9,0x99,0x10,0x64,0xB7,0x34,0x7D,0x06);


MIDL_DEFINE_GUID(IID, IID_INotifyCallback,0x44E918C4,0x0540,0x4d63,0xBF,0xEA,0x1B,0xBA,0x0D,0x5C,0xCB,0xFA);


MIDL_DEFINE_GUID(IID, LIBID_kissctlLib,0x0BC7C86A,0x894D,0x46A6,0x99,0x85,0x64,0xDE,0xCF,0x63,0x20,0x32);


MIDL_DEFINE_GUID(IID, DIID__ISvrCallerEvents,0x31B4C9A0,0x4D8E,0x47DE,0x9B,0x37,0x45,0x2C,0xEC,0x61,0xC2,0x57);


MIDL_DEFINE_GUID(CLSID, CLSID_SvrCaller,0x36663AC3,0x0D27,0x4B77,0xA2,0x30,0x7F,0xD7,0x42,0x6B,0xB4,0xD3);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



