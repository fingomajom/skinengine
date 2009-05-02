

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Sat May 02 00:03:12 2009
 */
/* Compiler settings for .\msg_rpc_np.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __msg_rpc_np_h__
#define __msg_rpc_np_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __msg_rpc_np_INTERFACE_DEFINED__
#define __msg_rpc_np_INTERFACE_DEFINED__

/* interface msg_rpc_np */
/* [implicit_handle][version][uuid] */ 

int ReceiveRpcMsg( 
    /* [in] */ int nMsgId,
    /* [in] */ VARIANT pInBuf,
    /* [out] */ VARIANT *ppOutBuf);

int SendRpcMsg( 
    /* [in] */ int nMsgId,
    /* [in] */ VARIANT pInBuf,
    /* [out] */ VARIANT *ppOutBuf);

extern handle_t hMsgRpcNpBinding;


extern RPC_IF_HANDLE msg_rpc_np_v1_0_c_ifspec;
extern RPC_IF_HANDLE msg_rpc_np_v1_0_s_ifspec;
#endif /* __msg_rpc_np_INTERFACE_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


