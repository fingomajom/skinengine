/********************************************************************
* CreatedOn: 2007-12-17   17:08
* FileName: skinstr.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include <atlbase.h>
#include <atlapp.h>
#include <atlmisc.h>

namespace KSG{

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
        if (get_pfn_loadstring_str() != NULL)
            return get_pfn_loadstring_str()(pszIDName, *this);

        return FALSE;
    }

    BOOL LoadString(UINT uResID)
    {
        if (get_pfn_loadstring_uint != NULL)
            return get_pfn_loadstring_uint()(uResID, *this);

        return FALSE;
    }

public:

    typedef BOOL(*PFN_LoadString_STR)(LPCTSTR, KSG::CString&);
    typedef BOOL(*PFN_LoadString_UINT)(UINT, KSG::CString&);
    
    static PFN_LoadString_STR& get_pfn_loadstring_str()
    {
        static PFN_LoadString_STR s_pfn_loadstring_str;
        return s_pfn_loadstring_str;
    }

    static PFN_LoadString_UINT& get_pfn_loadstring_uint()
    {
        static PFN_LoadString_UINT s_pfn_loadstring_uint;
        return s_pfn_loadstring_uint;
    }

};


//_declspec(selectany) CString::PFN_LoadString_STR  CString::s_pfn_loadstring_str  = NULL;
//_declspec(selectany) CString::PFN_LoadString_UINT CString::s_pfn_loadstring_uint = NULL;


} // namespace KSG
