

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __kissctl_h__
#define __kissctl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ISvrCaller_FWD_DEFINED__
#define __ISvrCaller_FWD_DEFINED__
typedef interface ISvrCaller ISvrCaller;
#endif 	/* __ISvrCaller_FWD_DEFINED__ */


#ifndef __INotifyCallback_FWD_DEFINED__
#define __INotifyCallback_FWD_DEFINED__
typedef interface INotifyCallback INotifyCallback;
#endif 	/* __INotifyCallback_FWD_DEFINED__ */


#ifndef ___ISvrCallerEvents_FWD_DEFINED__
#define ___ISvrCallerEvents_FWD_DEFINED__
typedef interface _ISvrCallerEvents _ISvrCallerEvents;
#endif 	/* ___ISvrCallerEvents_FWD_DEFINED__ */


#ifndef __SvrCaller_FWD_DEFINED__
#define __SvrCaller_FWD_DEFINED__

#ifdef __cplusplus
typedef class SvrCaller SvrCaller;
#else
typedef struct SvrCaller SvrCaller;
#endif /* __cplusplus */

#endif 	/* __SvrCaller_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __ISvrCaller_INTERFACE_DEFINED__
#define __ISvrCaller_INTERFACE_DEFINED__

/* interface ISvrCaller */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISvrCaller;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2E2D8BA6-3CB1-4927-A999-1064B7347D06")
    ISvrCaller : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ ULONG uCallerId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Uninitialize( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CallSvrFunc( 
            /* [in] */ ULONG uDesModuleId,
            /* [in] */ BSTR bstrFunctionName,
            /* [in] */ BSTR bstrParameter,
            /* [retval][out] */ BSTR *bstrResult) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RegisterCallback( 
            /* [in] */ IUnknown *piCallback) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISvrCallerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISvrCaller * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISvrCaller * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISvrCaller * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISvrCaller * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISvrCaller * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISvrCaller * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISvrCaller * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            ISvrCaller * This,
            /* [in] */ ULONG uCallerId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Uninitialize )( 
            ISvrCaller * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CallSvrFunc )( 
            ISvrCaller * This,
            /* [in] */ ULONG uDesModuleId,
            /* [in] */ BSTR bstrFunctionName,
            /* [in] */ BSTR bstrParameter,
            /* [retval][out] */ BSTR *bstrResult);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RegisterCallback )( 
            ISvrCaller * This,
            /* [in] */ IUnknown *piCallback);
        
        END_INTERFACE
    } ISvrCallerVtbl;

    interface ISvrCaller
    {
        CONST_VTBL struct ISvrCallerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISvrCaller_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISvrCaller_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISvrCaller_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISvrCaller_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISvrCaller_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISvrCaller_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISvrCaller_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISvrCaller_Initialize(This,uCallerId)	\
    (This)->lpVtbl -> Initialize(This,uCallerId)

#define ISvrCaller_Uninitialize(This)	\
    (This)->lpVtbl -> Uninitialize(This)

#define ISvrCaller_CallSvrFunc(This,uDesModuleId,bstrFunctionName,bstrParameter,bstrResult)	\
    (This)->lpVtbl -> CallSvrFunc(This,uDesModuleId,bstrFunctionName,bstrParameter,bstrResult)

#define ISvrCaller_RegisterCallback(This,piCallback)	\
    (This)->lpVtbl -> RegisterCallback(This,piCallback)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISvrCaller_Initialize_Proxy( 
    ISvrCaller * This,
    /* [in] */ ULONG uCallerId);


void __RPC_STUB ISvrCaller_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISvrCaller_Uninitialize_Proxy( 
    ISvrCaller * This);


void __RPC_STUB ISvrCaller_Uninitialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISvrCaller_CallSvrFunc_Proxy( 
    ISvrCaller * This,
    /* [in] */ ULONG uDesModuleId,
    /* [in] */ BSTR bstrFunctionName,
    /* [in] */ BSTR bstrParameter,
    /* [retval][out] */ BSTR *bstrResult);


void __RPC_STUB ISvrCaller_CallSvrFunc_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISvrCaller_RegisterCallback_Proxy( 
    ISvrCaller * This,
    /* [in] */ IUnknown *piCallback);


void __RPC_STUB ISvrCaller_RegisterCallback_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISvrCaller_INTERFACE_DEFINED__ */


#ifndef __INotifyCallback_INTERFACE_DEFINED__
#define __INotifyCallback_INTERFACE_DEFINED__

/* interface INotifyCallback */
/* [unique][helpstring][nonextensible][uuid][object] */ 


EXTERN_C const IID IID_INotifyCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("44E918C4-0540-4d63-BFEA-1BBA0D5CCBFA")
    INotifyCallback : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE NotifyMessage( 
            /* [in] */ ULONG uDesCallerId,
            /* [in] */ ULONG uSrcModuleId,
            /* [in] */ BSTR bstrFunctionName,
            /* [in] */ BSTR bstrParameter,
            /* [out] */ BSTR *bstrResult) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INotifyCallbackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INotifyCallback * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INotifyCallback * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INotifyCallback * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *NotifyMessage )( 
            INotifyCallback * This,
            /* [in] */ ULONG uDesCallerId,
            /* [in] */ ULONG uSrcModuleId,
            /* [in] */ BSTR bstrFunctionName,
            /* [in] */ BSTR bstrParameter,
            /* [out] */ BSTR *bstrResult);
        
        END_INTERFACE
    } INotifyCallbackVtbl;

    interface INotifyCallback
    {
        CONST_VTBL struct INotifyCallbackVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INotifyCallback_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INotifyCallback_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INotifyCallback_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INotifyCallback_NotifyMessage(This,uDesCallerId,uSrcModuleId,bstrFunctionName,bstrParameter,bstrResult)	\
    (This)->lpVtbl -> NotifyMessage(This,uDesCallerId,uSrcModuleId,bstrFunctionName,bstrParameter,bstrResult)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE INotifyCallback_NotifyMessage_Proxy( 
    INotifyCallback * This,
    /* [in] */ ULONG uDesCallerId,
    /* [in] */ ULONG uSrcModuleId,
    /* [in] */ BSTR bstrFunctionName,
    /* [in] */ BSTR bstrParameter,
    /* [out] */ BSTR *bstrResult);


void __RPC_STUB INotifyCallback_NotifyMessage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INotifyCallback_INTERFACE_DEFINED__ */



#ifndef __kissctlLib_LIBRARY_DEFINED__
#define __kissctlLib_LIBRARY_DEFINED__

/* library kissctlLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_kissctlLib;

#ifndef ___ISvrCallerEvents_DISPINTERFACE_DEFINED__
#define ___ISvrCallerEvents_DISPINTERFACE_DEFINED__

/* dispinterface _ISvrCallerEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__ISvrCallerEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("31B4C9A0-4D8E-47DE-9B37-452CEC61C257")
    _ISvrCallerEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _ISvrCallerEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _ISvrCallerEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _ISvrCallerEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _ISvrCallerEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _ISvrCallerEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _ISvrCallerEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _ISvrCallerEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _ISvrCallerEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _ISvrCallerEventsVtbl;

    interface _ISvrCallerEvents
    {
        CONST_VTBL struct _ISvrCallerEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _ISvrCallerEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _ISvrCallerEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _ISvrCallerEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _ISvrCallerEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _ISvrCallerEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _ISvrCallerEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _ISvrCallerEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___ISvrCallerEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_SvrCaller;

#ifdef __cplusplus

class DECLSPEC_UUID("36663AC3-0D27-4B77-A230-7FD7426BB4D3")
SvrCaller;
#endif
#endif /* __kissctlLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


