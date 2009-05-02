/********************************************************************
	created:	2009/04/22
	created:	22:4:2009   11:37
	filename: 	d:\Simple\quickserach\quickserach\QuickSearchConfig.h
	file path:	d:\Simple\quickserach\quickserach
	file base:	QuickSearchConfig.h
	author:		lidengwang@sohu-rd.com	
	purpose:	
*********************************************************************/

#pragma once

#include <atlpath.h>
#include <atlcoll.h>

typedef struct tag_QuickSearchProviderCfgInfo
{
    ATL::CString strFileName;
    ATL::CString strQSName;
    ATL::CString strDescription;
    ATL::CString strRequestInfo;
    ATL::CString strIconFile;
    ATL::CSimpleArray<ATL::CString> m_vtReg;

} QSPCINFO, *LPQSPCINFO;

class CQuickSearchConfig
{

public:

    CQuickSearchConfig(void);
    ~CQuickSearchConfig(void);

    BOOL GetQuickSearchProviderConfig( ATL::CAtlArray<QSPCINFO>& cfg );

    static BOOL CreateIconFile( LPCTSTR pszIconFile, LPCTSTR pszData );

private:

    BOOL AnalyzeQSearchFile( LPCTSTR pszCfgFile, QSPCINFO& info );
};
