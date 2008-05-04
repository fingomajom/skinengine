

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Sat May 03 19:22:31 2008
 */
/* Compiler settings for .\kissvr.idl:
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

MIDL_DEFINE_GUID(IID, IID_ISvrObject,0x1215ABFB,0x12A2,0x4201,0x8F,0x4B,0x8F,0xBD,0x50,0x3C,0x84,0x14);


MIDL_DEFINE_GUID(IID, LIBID_kissvrLib,0x02C621AD,0xEED9,0x4CAD,0xA5,0xBE,0x43,0x73,0x5B,0xC3,0xB2,0x5A);


MIDL_DEFINE_GUID(IID, DIID__ISvrObjectEvents,0xA31A43A2,0x3A43,0x4B71,0x8C,0xD5,0x3A,0xD0,0x02,0x37,0x0C,0x50);


MIDL_DEFINE_GUID(CLSID, CLSID_SvrObject,0x902ED49C,0xE3EF,0x4930,0xB5,0x0B,0x9E,0x6A,0x8B,0xA5,0xA6,0x71);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



