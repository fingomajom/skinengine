/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Thu Jul 25 11:28:03 2002
 */
/* Compiler settings for F:\MyNet\TreeSoft\iTreeSurf\iTreeSurfSource\iTreeSurf.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


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

#ifndef __iTreeSurf_i_h__
#define __iTreeSurf_i_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IiTreeSurfNSHandle_FWD_DEFINED__
#define __IiTreeSurfNSHandle_FWD_DEFINED__
typedef interface IiTreeSurfNSHandle IiTreeSurfNSHandle;
#endif 	/* __IiTreeSurfNSHandle_FWD_DEFINED__ */


#ifndef __iTreeSurfNSHandle_FWD_DEFINED__
#define __iTreeSurfNSHandle_FWD_DEFINED__

#ifdef __cplusplus
typedef class iTreeSurfNSHandle iTreeSurfNSHandle;
#else
typedef struct iTreeSurfNSHandle iTreeSurfNSHandle;
#endif /* __cplusplus */

#endif 	/* __iTreeSurfNSHandle_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IiTreeSurfNSHandle_INTERFACE_DEFINED__
#define __IiTreeSurfNSHandle_INTERFACE_DEFINED__

/* interface IiTreeSurfNSHandle */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IiTreeSurfNSHandle;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("25E81C73-A6C1-46B2-A83B-C05565643062")
    IiTreeSurfNSHandle : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IiTreeSurfNSHandleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IiTreeSurfNSHandle __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IiTreeSurfNSHandle __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IiTreeSurfNSHandle __RPC_FAR * This);
        
        END_INTERFACE
    } IiTreeSurfNSHandleVtbl;

    interface IiTreeSurfNSHandle
    {
        CONST_VTBL struct IiTreeSurfNSHandleVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IiTreeSurfNSHandle_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IiTreeSurfNSHandle_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IiTreeSurfNSHandle_Release(This)	\
    (This)->lpVtbl -> Release(This)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IiTreeSurfNSHandle_INTERFACE_DEFINED__ */



#ifndef __iTreeSurfLib_LIBRARY_DEFINED__
#define __iTreeSurfLib_LIBRARY_DEFINED__

/* library iTreeSurfLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_iTreeSurfLib;

EXTERN_C const CLSID CLSID_iTreeSurfNSHandle;

#ifdef __cplusplus

class DECLSPEC_UUID("B96C53BD-FADC-45C7-A72D-EB5230433A46")
iTreeSurfNSHandle;
#endif
#endif /* __iTreeSurfLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
