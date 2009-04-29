

#pragma once


class CDWImage : public CImage
{
public:

    BOOL LoadFromResource( UINT uResId )
    {
        BOOL bResult = FALSE;

        HINSTANCE hInst = _Module.GetResourceInstance();
        IStream*  pstm  = NULL;

        HRSRC hRsrc = ::FindResource( _Module.GetResourceInstance(), 
            MAKEINTRESOURCE(uResId), _T("PNG") );
        if (!hRsrc)
            return bResult;

        DWORD dwResLen = SizeofResource( hInst, hRsrc );
        BYTE* lpRsrc   = (BYTE*)LoadResource( hInst, hRsrc );
        if (!lpRsrc)
            return bResult;

        HGLOBAL hMem = GlobalAlloc(GMEM_FIXED, dwResLen);
        BYTE* pmem = (BYTE*)GlobalLock(hMem);
        if ( hMem != NULL && pmem != NULL )
        {
            memcpy(pmem, lpRsrc, dwResLen);

            CreateStreamOnHGlobal(hMem, FALSE, &pstm);

            if ( SUCCEEDED( Load(pstm) ) )
                bResult = TRUE;

            GlobalUnlock(hMem);
            pstm->Release(); 
        }

        FreeResource(lpRsrc);

        return bResult;
    }


};