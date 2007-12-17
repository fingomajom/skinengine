
#pragma once

#include <atlbase.h>
#include <atlapp.h>
#include <atlmisc.h>

namespace KSE{

class CString : public WTL::CString
{
public:

    CString()
    {
        Init();
    }

    CString(const CString& stringSrc) :
        WTL::CString(stringSrc)
    {   
    }

    CString(TCHAR ch, int nRepeat = 1) :
        WTL::CString(ch, nRepeat)
    {
    }

    CString(LPCTSTR lpsz) :
        WTL::CString(lpsz)
    {
    }

#ifdef _UNICODE
    CString(LPCSTR lpsz) :
        WTL::CString(lpsz)
    {
    }
#else // !_UNICODE
    CString(LPCWSTR lpsz) :
        WTL::CString(lpsz)
    {
    }

#endif // !_UNICODE

    CString(LPCTSTR lpch, int nLength) :
        WTL::CString(lpch, nLength)
    {
    }

#ifdef _UNICODE
    CString(LPCSTR lpsz, int nLength) :
        WTL::CString(lpsz, nLength)
    {
    }
#else // !_UNICODE
    CString(LPCWSTR lpsz, int nLength) :
        WTL::CString(lpsz, nLength)
    {
    }
#endif // !_UNICODE

    CString(const unsigned char* lpsz) :
        WTL::CString(lpsz)
    {
    }

public:

    BOOL LoadString(LPCTSTR pszIDName)
    {
        if (s_pfn_loadstring_str != NULL)
            return s_pfn_loadstring_str(pszIDName, *this);

        return FALSE;
    }

    BOOL LoadString(UINT uResID)
    {
        if (s_pfn_loadstring_uint != NULL)
            return s_pfn_loadstring_uint(uResID, *this);

        return FALSE;
    }

public:

    typedef BOOL(*PFN_LoadString_STR)(LPCTSTR, KSE::CString&);
    typedef BOOL(*PFN_LoadString_UINT)(UINT, KSE::CString&);
    
    static PFN_LoadString_STR  s_pfn_loadstring_str;
    static PFN_LoadString_UINT s_pfn_loadstring_uint;
};


_declspec(selectany) CString::PFN_LoadString_STR  CString::s_pfn_loadstring_str  = NULL;
_declspec(selectany) CString::PFN_LoadString_UINT CString::s_pfn_loadstring_uint = NULL;


} // namespace KSE
