/********************************************************************
* CreatedOn: 2006-7-7   9:49
* FileName: IDataBuffer.h
* CreatedBy: lidengwang<lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL: $
* Purpose:
*********************************************************************/

#pragma once


struct __declspec(uuid("B0B1411F-5D5E-4de1-B4B3-89A276084E2E")) IDataBuffer;
interface IDataBuffer : public IUnknown
{
public:
    virtual void*        STDMETHODCALLTYPE GetDataBuffer()  = 0; 
    virtual unsigned int STDMETHODCALLTYPE GetBufferSize()  = 0;
    virtual unsigned int STDMETHODCALLTYPE GetReserveSize() = 0;
};

inline int LSEMemoryInit  ( void *pvContext ) { return 0; }
inline int LSEMemoryUnInit( void *pvContext ) { return 0; } 

inline IDataBuffer *LSECreateDataBuffer(unsigned int uSize);


inline IDataBuffer* SafeArray2DataBuffer( SAFEARRAY  * pSafeArray   );
inline SAFEARRAY  * DataBuffer2SafeArray( IDataBuffer* piDataBuffer );

//////////////////////////////////////////////////////////////////////////

template<typename T>
class LSEDataBufferImpl
{
public:
    LSEDataBufferImpl() 
    { 
        m_piDataBuffer = LSECreateDataBuffer(sizeof(T));

        ATLASSERT(m_piDataBuffer != NULL);
        ATLASSERT(m_piDataBuffer->GetBufferSize() == sizeof(T));

        m_piDataBuffer->AddRef();
    }

    LSEDataBufferImpl(T* psrc)
    {
        m_piDataBuffer = KanCreateDataBuffer(
            sizeof(T));

        ATLASSERT(m_piDataBuffer != NULL);
        ATLASSERT(m_piDataBuffer->GetBufferSize() == sizeof(T));
        
        *((T*)m_piDataBuffer->GetDataBuffer()) = *psrc;

        m_piDataBuffer->AddRef();

    }

    LSEDataBufferImpl(const T& src)
    {
        m_piDataBuffer = KanCreateDataBuffer(
            sizeof(T));

        ATLASSERT(m_piDataBuffer != NULL);
        ATLASSERT(m_piDataBuffer->GetBufferSize() == sizeof(T));

        *((T*)m_piDataBuffer->GetDataBuffer()) = src;

        m_piDataBuffer->AddRef();
    }

    ~LSEDataBufferImpl()
    {
        if (m_piDataBuffer != NULL)
        {
            m_piDataBuffer->Release();
            m_piDataBuffer = NULL;
        }
    }

    LSEDataBufferImpl& operator = (const T& cpy)
    {
        ATLASSERT(m_piDataBuffer != NULL);
        ATLASSERT(m_piDataBuffer->GetDataBuffer() == sizeof(T));

        *((T*)m_piDataBuffer->GetDataBuffer()) = src;

        return *this;
    }

    LSEDataBufferImpl& operator = (const LSEDataBufferImpl<T>& cpy)
    {
        ATLASSERT(m_piDataBuffer != NULL);
        ATLASSERT(m_piDataBuffer->GetDataBuffer() == sizeof(T));

        *((T*)m_piDataBuffer->GetDataBuffer()) = *((T*)src);

        return *this;
    }

    operator T*() const
    {
        return( (T*)m_piDataBuffer->GetDataBuffer() );
    }

    T* operator->() const
    {
        return( (T*)m_piDataBuffer->GetDataBuffer() );
    }

    IDataBuffer* GetDataBuffer() const
    {
        return m_piDataBuffer;
    }

public:
    IDataBuffer* m_piDataBuffer;
};


class LSEDataBuffer :
    public IDataBuffer
{
public:
    LSEDataBuffer() :
      m_dwRef(0),
          m_pDataBuffer(NULL),
          m_uBufSize(0),
          m_uReserveSize(0)
      {
      }

      LSEDataBuffer(unsigned int uSize) : m_dwRef(0)
      {
          if (uSize == 0)
          {
              m_pDataBuffer  = NULL;
              m_uBufSize     = (0);
              m_uReserveSize = (0);
          }
          else
          {
              m_pDataBuffer = malloc(uSize);
              m_uBufSize     = uSize;
              m_uReserveSize = uSize;
          }
      }

      ~LSEDataBuffer()
      {
          if (m_pDataBuffer != NULL)
          {
              free(m_pDataBuffer);
              m_pDataBuffer = NULL;
          }

          m_uBufSize     = 0;
          m_uReserveSize = 0;
      }

      void* STDMETHODCALLTYPE GetDataBuffer()
      {
          return m_pDataBuffer;
      }

      unsigned int STDMETHODCALLTYPE GetBufferSize()
      {
          return m_uBufSize;
      }

      unsigned int STDMETHODCALLTYPE GetReserveSize()
      {
          return m_uReserveSize;
      }

      ULONG STDMETHODCALLTYPE AddRef(void) 
      { 
          return (++m_dwRef); 
      }

      ULONG STDMETHODCALLTYPE Release( void) 
      { 
          if (--m_dwRef == 0)
          {
              delete this;
          }

          return (m_dwRef);
      } 

      HRESULT STDMETHODCALLTYPE QueryInterface( 
          /* [in] */ REFIID riid, 
          /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject) 
      {

          if ((riid) == __uuidof(IDataBuffer)) 
          { 
              *ppvObject = static_cast<IDataBuffer*>(this); 
              AddRef();
              return S_OK;
          }
          else if ((riid) == IID_IUnknown)
          {
              *ppvObject = static_cast<IUnknown*>(this); 
              AddRef();
              return S_OK;
          }
          return E_FAIL; 
      }

private:
    UINT m_dwRef;

    void* m_pDataBuffer;
    unsigned int m_uBufSize;
    unsigned int m_uReserveSize;
};

inline IDataBuffer *LSECreateDataBuffer(unsigned int uSize)
{
    IDataBuffer* pRetBuf = new LSEDataBuffer(uSize);

    return pRetBuf;
}


inline IDataBuffer* SafeArray2DataBuffer( SAFEARRAY  * pSafeArray   )
{
    IDataBuffer* piDataBuffer = NULL;

    if (pSafeArray == NULL)
        return piDataBuffer;


    if ( pSafeArray->rgsabound[0].lLbound == 0 && 
         pSafeArray->cDims == 1 &&
         pSafeArray->cbElements == 1)
    {
        piDataBuffer = LSECreateDataBuffer( pSafeArray->rgsabound[0].cElements );

        if (piDataBuffer != NULL)
            memcpy( piDataBuffer->GetDataBuffer(), 
            pSafeArray->pvData, 
            pSafeArray->rgsabound[0].cElements );
    }
    
    return piDataBuffer;
}

inline SAFEARRAY  * DataBuffer2SafeArray( IDataBuffer* piDataBuffer )
{
    LPSAFEARRAY psf = NULL;

    if (piDataBuffer == NULL)
        return psf;

    SAFEARRAYBOUND rgsabound[1] = { 0 };

    rgsabound[0].cElements = piDataBuffer->GetBufferSize();;
    rgsabound[0].lLbound   = 0;

    psf = SafeArrayCreate( VT_UI1, 1, rgsabound);

    if (psf != NULL)
    {
        memcpy( psf->pvData, 
            piDataBuffer->GetDataBuffer(), 
            piDataBuffer->GetBufferSize() );
    }
    
    return psf;
}
