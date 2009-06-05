#pragma once

class DWDefaultExplorer
{
public:
    DWDefaultExplorer(void);
    ~DWDefaultExplorer(void);
};
#pragma once


#define WINDOWS_OLDEST              long(-1000)
#define WINDOWS_2000                long(1)
#define WINDOWS_XP_NO_SP            long(2)
#define WINDOWS_XP_SP_1             long(3)
#define WINDOWS_XP_SP_2             long(4)
#define WINDOWS_XP_SP_3             long(5)
#define WINDOWS_2003			    long(6)
#define WINDOWS_VISTA_NO_SP         long(7)
#define WINDOWS_VISTA_SP_1          long(8)
#define WINDOWS_LATEST              long(1000)


long GetOSVersion();

enum eOS
{
    WinUnKnown = 0 ,
    Win2K = 1 ,
    WinXP = 2 ,
    WinXPsp1 = 4 ,
    WinXPsp2 = 8 ,
    WinXPsp3 = 16 , 
    WinXPAll = WinXP|WinXPsp1|WinXPsp2|WinXPsp3 ,
    Win2K3 = 32 ,
    WinVista = 64 ,
    WinVistaSp1 = 128 ,
    WinVistaAll = WinVista|WinVistaSp1 ,
    WinNTAll = Win2K|WinXPAll|Win2K3|WinVistaAll
};

class CDWDefaultExplorer
{

public :
    CDWDefaultExplorer() ;
    ~CDWDefaultExplorer() ;

public :
    static BOOL SetSogouDefault() ;
    static BOOL SetIEDefault() ;
    static bool IsSogouDefault();

protected :
    static BOOL TidyFileRelation() ;
    static BOOL SetSogouCommand() ;
    static BOOL SetIECommand() ;
    static BOOL CompareValueAtClassesRoot(const CString& cstrRoute, const WCHAR* wszModulePath);
public:
    static eOS ConvertOSVersion(DWORD dwVersion) ;
    static BOOL IsAdmin() ;

private :
    DWORD m_dwServiceID ;
} ;
