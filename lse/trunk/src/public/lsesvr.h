

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Thu Oct 16 11:35:00 2008
 */
/* Compiler settings for .\lsesvr.idl:
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

#ifndef __lsesvr_h__
#define __lsesvr_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ISvrObject_FWD_DEFINED__
#define __ISvrObject_FWD_DEFINED__
typedef interface ISvrObject ISvrObject;
#endif 	/* __ISvrObject_FWD_DEFINED__ */


#ifndef ___ISvrObjectEvents_FWD_DEFINED__
#define ___ISvrObjectEvents_FWD_DEFINED__
typedef interface _ISvrObjectEvents _ISvrObjectEvents;
#endif 	/* ___ISvrObjectEvents_FWD_DEFINED__ */


#ifndef __SvrObject_FWD_DEFINED__
#define __SvrObject_FWD_DEFINED__

#ifdef __cplusplus
typedef class SvrObject SvrObject;
#else
typedef struct SvrObject SvrObject;
#endif /* __cplusplus */

#endif 	/* __SvrObject_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __ISvrObject_INTERFACE_DEFINED__
#define __ISvrObject_INTERFACE_DEFINED__

/* interface ISvrObject */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISvrObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DCCD45AE-FC29-414C-8149-F8870C628865")
    ISvrObject : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct ISvrObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISvrObject * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISvrObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISvrObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISvrObject * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISvrObject * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISvrObject * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISvrObject * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } ISvrObjectVtbl;

    interface ISvrObject
    {
        CONST_VTBL struct ISvrObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISvrObject_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISvrObject_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISvrObject_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISvrObject_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISvrObject_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISvrObject_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISvrObject_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISvrObject_INTERFACE_DEFINED__ */



#ifndef __lsesvrLib_LIBRARY_DEFINED__
#define __lsesvrLib_LIBRARY_DEFINED__

/* library lsesvrLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_lsesvrLib;

#ifndef ___ISvrObjectEvents_DISPINTERFACE_DEFINED__
#define ___ISvrObjectEvents_DISPINTERFACE_DEFINED__

/* dispinterface _ISvrObjectEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__ISvrObjectEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("66EBAFDC-31F2-4220-A2A7-5BBD24239EA9")
    _ISvrObjectEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _ISvrObjectEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _ISvrObjectEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _ISvrObjectEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _ISvrObjectEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _ISvrObjectEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _ISvrObjectEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _ISvrObjectEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _ISvrObjectEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _ISvrObjectEventsVtbl;

    interface _ISvrObjectEvents
    {
        CONST_VTBL struct _ISvrObjectEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _ISvrObjectEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _ISvrObjectEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _ISvrObjectEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _ISvrObjectEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _ISvrObjectEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _ISvrObjectEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _ISvrObjectEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___ISvrObjectEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_SvrObject;

#ifdef __cplusplus

class DECLSPEC_UUID("A3D222EB-36BF-4EE0-8600-476A491A69BF")
SvrObject;
#endif
#endif /* __lsesvrLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


