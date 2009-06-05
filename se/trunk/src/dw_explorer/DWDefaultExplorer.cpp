#include "StdAfx.h"
#include "DWDefaultExplorer.h"



typedef struct tag_RegStruct
{
    DWORD dwOSVersion ;		// 哪个操作系统版本要修改
    HKEY hRootKey ;			// 是哪个根键下的
    LPTSTR strPath;			// 子键
    LPTSTR strValueName ;	// 键值的名称
    DWORD dwValueType ;		// 键值的类型
    const PBYTE pValues;	// 键值
    DWORD dwValueSize ;		// 键值大小
    BOOL bIgnoreFail ;		// 失败时是否忽略失败
} REGSTRUCT , *PREGSTRUCT ;

// %%SogouPath%% 搜狗浏览器的路径
// %%SogouPathName%% 搜狗浏览器的路径+可执行文件的路径
// const REGSTRUCT regSetSogouDefault_HKLM[]={
// };
// const REGSTRUCT regSetSogouDefault_HKCR[]={
// };
// const REGSTRUCT regSetSogouDefault_HKCR[]={
// };

const REGSTRUCT RegSetSogouDefault[] = { 

    //	{WinNTAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\setask.exe") , NULL , REG_SZ , (const PBYTE)_T("%%SogouPathName%%") , 0 } ,
    //	{WinNTAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\setask.exe") , _T("Path") , REG_SZ , (const PBYTE)_T("%%SogouPath%%") , 0 } ,  

    {WinVistaAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\SogouExplorer") , NULL , REG_NONE , NULL , 0 } ,
    {WinVistaAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\SogouExplorer\\Capabilities") , _T("ApplicationDescription") , REG_SZ , (const PBYTE)_T("Sogou Explorer") ,0 } ,
    {WinVistaAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\SogouExplorer\\Capabilities") , _T("ApplicationName") , REG_SZ , (const PBYTE)_T("SogouExplorer") , 0 } ,
    {WinVistaAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\SogouExplorer\\Capabilities\\FileAssociations") , _T(".htm") , REG_SZ , (const PBYTE)_T("SogouExplorer.AssocFile.HTM") , 0 } ,
    {WinVistaAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\SogouExplorer\\Capabilities\\FileAssociations") , _T(".html") , REG_SZ , (const PBYTE)_T("SogouExplorer.AssocFile.HTM") , 0 } ,
    {WinVistaAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\SogouExplorer\\Capabilities\\FileAssociations") , _T(".mht") , REG_SZ , (const PBYTE)_T("SogouExplorer.AssocFile.HTM") , 0 } ,
    {WinVistaAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\SogouExplorer\\Capabilities\\FileAssociations") , _T(".xml") , REG_SZ , (const PBYTE)_T("SogouExplorer.AssocFile.HTM") , 0 } ,
    {WinVistaAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\SogouExplorer\\Capabilities\\FileAssociations") , _T(".mhtml") , REG_SZ , (const PBYTE)_T("SogouExplorer.AssocFile.HTM") , 0 } ,
    {WinVistaAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\SogouExplorer\\Capabilities\\Startmenu") , _T("StartmenuInternet") , REG_SZ , (const PBYTE)_T("SogouExplorer") , 0 } ,
    //	{WinVistaAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\SogouExplorer\\Capabilities\\UrlAssociations") , _T("ftp") , REG_SZ , (const PBYTE)_T("SogouExplorer.HTTP") , 0 } ,
    {WinVistaAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\SogouExplorer\\Capabilities\\UrlAssociations") , _T("http") , REG_SZ , (const PBYTE)_T("SogouExplorer.HTTP") , 0 } ,
    {WinVistaAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\SogouExplorer\\Capabilities\\UrlAssociations") , _T("https") , REG_SZ , (const PBYTE)_T("SogouExplorer.HTTP") , 0 } ,

    {WinVistaAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\RegisteredApplications") , _T("SogouExplorer") , REG_SZ ,(const PBYTE) _T("Software\\SogouExplorer\\Capabilities") , 0 } ,

    {WinVistaAll , HKEY_CLASSES_ROOT , _T("SogouExplorer.AssocFile.HTM") , NULL, REG_NONE , NULL , 0 } ,
    {WinVistaAll , HKEY_CLASSES_ROOT , _T("SogouExplorer.AssocFile.HTM\\DefaultIcon") , NULL, REG_SZ , (const PBYTE)_T("%%SogouPathName%%,0") , 0 } ,
    {WinVistaAll , HKEY_CLASSES_ROOT , _T("SogouExplorer.AssocFile.HTM\\Shell") , NULL, REG_SZ , (const PBYTE)_T("open") , 0 } ,
    {WinVistaAll , HKEY_CLASSES_ROOT , _T("SogouExplorer.AssocFile.HTM\\Shell\\open") , NULL, REG_SZ , (const PBYTE)_T("open") , 0 } ,
    {WinVistaAll , HKEY_CLASSES_ROOT , _T("SogouExplorer.AssocFile.HTM\\Shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%SogouPathName%%\" %1") , 0 } ,

    {WinVistaAll , HKEY_CLASSES_ROOT , _T("SogouExplorer.HTTP") , NULL, REG_SZ , (const PBYTE)_T("URL:HyperText Transfer Protocol") , 0 } ,
    {WinVistaAll , HKEY_CLASSES_ROOT , _T("SogouExplorer.HTTP\\DefaultIcon") , NULL, REG_SZ , (const PBYTE)_T("%%SogouPathName%%,0") , 0 } ,
    {WinVistaAll , HKEY_CLASSES_ROOT , _T("SogouExplorer.HTTP\\Shell") , NULL, REG_SZ , (const PBYTE)_T("open") , 0 } ,
    {WinVistaAll , HKEY_CLASSES_ROOT , _T("SogouExplorer.HTTP\\Shell\\open") , NULL, REG_SZ , (const PBYTE)_T("open") , 0 } ,
    {WinVistaAll , HKEY_CLASSES_ROOT , _T("SogouExplorer.HTTP\\Shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%SogouPathName%%\" %1") , 0 } ,

    {WinNTAll , HKEY_CLASSES_ROOT , _T("Http\\shell") , NULL, REG_SZ , (const PBYTE)_T("SogouExplorer") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Http\\shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%SogouPathName%%\" %1") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Http\\shell\\open\\ddeexec\\Application") , NULL, REG_SZ , (const PBYTE)_T("SogouExplorer") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Http\\shell\\open\\ddeexec\\Applicationr") , NULL, REG_SZ , (const PBYTE)_T("SogouExplorer") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Http\\shell\\SogouExplorer") , NULL, REG_SZ , (const PBYTE)_T("用&SogouExplorer浏览") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Http\\shell\\SogouExplorer\\Command") , NULL, REG_SZ , (const PBYTE)_T("\"%%SogouPathName%%\" %1") , 0 } ,

    {WinNTAll , HKEY_CLASSES_ROOT , _T("Https\\shell") , NULL, REG_SZ , (const PBYTE)_T("SogouExplorer") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Https\\shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%SogouPathName%%\" %1") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Https\\shell\\open\\ddeexec\\Application") , NULL, REG_SZ , (const PBYTE)_T("SogouExplorer") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Https\\shell\\open\\ddeexec\\Applicationr") , NULL, REG_SZ , (const PBYTE)_T("SogouExplorer") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Https\\shell\\SogouExplorer") , NULL, REG_SZ , (const PBYTE)_T("用&SogouExplorer浏览") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Https\\shell\\SogouExplorer\\Command") , NULL, REG_SZ , (const PBYTE)_T("\"%%SogouPathName%%\" %1") , 0 } ,

    {WinNTAll , HKEY_CLASSES_ROOT , _T("Htmlfile\\shell") , NULL, REG_SZ , (const PBYTE)_T("SogouExplorer") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Htmlfile\\shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%SogouPathName%%\" %1") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Htmlfile\\shell\\open\\ddeexec\\Application") , NULL, REG_SZ , (const PBYTE)_T("SogouExplorer") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Htmlfile\\shell\\open\\ddeexec\\Applicationr") , NULL, REG_SZ , (const PBYTE)_T("SogouExplorer") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Htmlfile\\shell\\SogouExplorer") , NULL, REG_SZ , (const PBYTE)_T("用&SogouExplorer浏览") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Htmlfile\\shell\\SogouExplorer\\Command") , NULL, REG_SZ , (const PBYTE)_T("\"%%SogouPathName%%\" %1") , 0 } ,

    {WinNTAll , HKEY_CLASSES_ROOT , _T("Mhtmlfile\\shell") , NULL, REG_SZ , (const PBYTE)_T("SogouExplorer") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Mhtmlfile\\shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%SogouPathName%%\" %1") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Mhtmlfile\\shell\\open\\ddeexec\\Application") , NULL, REG_SZ , (const PBYTE)_T("SogouExplorer") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Mhtmlfile\\shell\\open\\ddeexec\\Applicationr") , NULL, REG_SZ , (const PBYTE)_T("SogouExplorer") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Mhtmlfile\\shell\\SogouExplorer") , NULL, REG_SZ , (const PBYTE)_T("用&SogouExplorer浏览") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Mhtmlfile\\shell\\SogouExplorer\\Command") , NULL, REG_SZ , (const PBYTE)_T("\"%%SogouPathName%%\" %1") , 0 } ,

    {WinNTAll , HKEY_CLASSES_ROOT , _T("xmlfile\\shell") , NULL, REG_SZ , (const PBYTE)_T("SogouExplorer") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("xmlfile\\shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%SogouPathName%%\" %1") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("xmlfile\\shell\\open\\ddeexec\\Application") , NULL, REG_SZ , (const PBYTE)_T("SogouExplorer") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("xmlfile\\shell\\SogouExplorer") , NULL, REG_SZ , (const PBYTE)_T("用&SogouExplorer浏览") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("xmlfile\\shell\\SogouExplorer\\Command") , NULL, REG_SZ , (const PBYTE)_T("\"%%SogouPathName%%\" %1") , 0 } ,

    {WinNTAll , HKEY_CLASSES_ROOT , _T("file\\shell") , NULL, REG_SZ , (const PBYTE)_T("SogouExplorer") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("file\\shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%SogouPathName%%\" %1") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("file\\shell\\open\\ddeexec\\Application") , NULL, REG_SZ , (const PBYTE)_T("SogouExplorer") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("file\\shell\\open\\ddeexec\\Applicationr") , NULL, REG_SZ , (const PBYTE)_T("SogouExplorer") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("file\\shell\\SogouExplorer") , NULL, REG_SZ , (const PBYTE)_T("用&SogouExplorer浏览") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("file\\shell\\SogouExplorer\\Command") , NULL, REG_SZ , (const PBYTE)_T("\"%%SogouPathName%%\" %1") , 0 } ,

    // 	{WinNTAll , HKEY_CLASSES_ROOT , _T("ftp\\shell") , NULL, REG_SZ , (const PBYTE)_T("SogouExplorer") , 0 } ,
    // 	{WinNTAll , HKEY_CLASSES_ROOT , _T("ftp\\shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%SogouPathName%%\" %1") , 0 } ,
    // 	{WinNTAll , HKEY_CLASSES_ROOT , _T("ftp\\shell\\open\\ddeexec\\Application") , NULL, REG_SZ , (const PBYTE)_T("SogouExplorer") , 0 } ,
    // 	{WinNTAll , HKEY_CLASSES_ROOT , _T("ftp\\shell\\open\\ddeexec\\Applicationr") , NULL, REG_SZ , (const PBYTE)_T("SogouExplorer") , 0 } ,
    // 	{WinNTAll , HKEY_CLASSES_ROOT , _T("ftp\\shell\\SogouExplorer") , NULL, REG_SZ , (const PBYTE)_T("用&SogouExplorer浏览") , 0 } ,
    // 	{WinNTAll , HKEY_CLASSES_ROOT , _T("ftp\\shell\\SogouExplorer\\Command") , NULL, REG_SZ , (const PBYTE)_T("\"%%SogouPathName%%\" %1") , 0 } ,
    // 
    // 	{WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.AssocFile.HTM\\shell") , NULL, REG_SZ , (const PBYTE)_T("opennew") , 0 } ,
    // 	{WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.AssocFile.HTM\\shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%SogouPathName%%\" %1") , 0 } ,
    // 	{WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.AssocFile.HTM\\shell\\opennew\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%SogouPathName%%\" %1") , 0 } ,
    // 
    // 	{WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.AssocFile.MHT\\shell") , NULL, REG_SZ , (const PBYTE)_T("opennew") , 0 } ,
    // 	{WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.AssocFile.MHT\\shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%SogouPathName%%\" %1") , 0 } ,
    // 	{WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.AssocFile.MHT\\shell\\opennew\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%SogouPathName%%\" %1") , 0 } ,
    // 
    // 	{WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.AssocFile.URL\\shell") , NULL, REG_SZ , (const PBYTE)_T("opennew") , 0 } ,
    // 	{WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.AssocFile.URL\\shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%SogouPathName%%\" %1") , 0 } ,
    // 	{WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.AssocFile.URL\\shell\\opennew\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%SogouPathName%%\" %1") , 0 } ,
    // 
    // 	{WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.FTP\\shell") , NULL, REG_SZ , (const PBYTE)_T("open") , 0 } ,
    // 	{WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.FTP\\shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%SogouPathName%%\" %1") , 0 } ,
    // 
    // 	{WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.HTTP\\shell") , NULL, REG_SZ , (const PBYTE)_T("open") , 0 } ,
    // 	{WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.HTTP\\shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%SogouPathName%%\" %1") , 0 } ,
    // 
    // 	{WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.HTTPS\\shell") , NULL, REG_SZ , (const PBYTE)_T("open") , 0 } ,
    // 	{WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.HTTPS\\shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%SogouPathName%%\" %1") , 0 } ,

    //	{WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\ftp\\UserChoice") , _T("Progid"), REG_SZ , (const PBYTE)_T("SogouExplorer.HTTP") , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\http\\UserChoice") , _T("Progid"), REG_SZ , (const PBYTE)_T("SogouExplorer.HTTP") , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\https\\UserChoice") , _T("Progid"), REG_SZ , (const PBYTE)_T("SogouExplorer.HTTP") , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.htm\\UserChoice") , NULL, REG_SZ , NULL , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.htm\\UserChoice") , _T("Progid"), REG_SZ , (const PBYTE)_T("SogouExplorer.AssocFile.HTM") , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.html\\UserChoice") , NULL, REG_SZ , NULL , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.html\\UserChoice") , _T("Progid"), REG_SZ , (const PBYTE)_T("SogouExplorer.AssocFile.HTM") , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.xml\\UserChoice") , NULL, REG_SZ , NULL , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.xml\\UserChoice") , _T("Progid"), REG_SZ , (const PBYTE)_T("SogouExplorer.AssocFile.HTM") , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.mhtml\\UserChoice") , NULL, REG_SZ , NULL , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.mhtml\\UserChoice") , _T("Progid"), REG_SZ , (const PBYTE)_T("SogouExplorer.AssocFile.HTM") , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.mht\\UserChoice") , NULL, REG_SZ , NULL , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.mht\\UserChoice") , _T("Progid"), REG_SZ , (const PBYTE)_T("SogouExplorer.AssocFile.HTM") , 0 } ,

    {WinNTAll , HKEY_CURRENT_USER , _T("SOFTWARE\\Clients\\StartMenuInternet") , NULL , REG_SZ , (const PBYTE)_T("SogouExplorer.exe") , 0 } ,

    {WinNTAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\Clients\\StartMenuInternet") , NULL , REG_SZ , (const PBYTE)_T("SogouExplorer.exe") , 0 } ,
    {WinNTAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\Clients\\StartMenuInternet\\SogouExplorer.exe") , NULL , REG_SZ , (const PBYTE)_T("Sogou Explorer") , 0 } ,
    {WinNTAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\Clients\\StartMenuInternet\\SogouExplorer.exe\\Shell") , NULL , REG_SZ , (const PBYTE)_T("open") , 0 } ,
    {WinNTAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\Clients\\StartMenuInternet\\SogouExplorer.exe\\Shell\\open") , NULL , REG_SZ , (const PBYTE)_T("open") , 0 } ,
    {WinNTAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\Clients\\StartMenuInternet\\SogouExplorer.exe\\Shell\\open\\command") , NULL , REG_SZ , (const PBYTE)_T("%%SogouPathName%%") , 0 } ,

    {WinNTAll , HKEY_CLASSES_ROOT , _T("CLSID\\{0002DF01-0000-0000-C000-000000000046}\\LocalServer32") , NULL, REG_SZ , (const PBYTE)_T("%%SogouPathName%%") , 0 , TRUE } ,

};

// %%IEPath%%
// %%IEPathName%%
const REGSTRUCT RegSetIEDefault[] = { 

    {WinNTAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\setask.exe") , NULL , REG_SZ , (const PBYTE)_T("%%IEPathName%%") , 0 } ,
    {WinNTAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\setask.exe") , _T("Path") , REG_SZ , (const PBYTE)_T("%%IEPath%%") , 0 } ,  
    {WinNTAll , HKEY_CLASSES_ROOT , _T("CLSID\\{0002DF01-0000-0000-C000-000000000046}\\LocalServer32") , NULL, REG_SZ , (const PBYTE)_T("%%IEPathName%%") , 0 , TRUE } ,

    {WinNTAll , HKEY_CURRENT_USER , _T("SOFTWARE\\Clients\\StartMenuInternet") , NULL , REG_SZ , (const PBYTE)_T("setask.exe") , 0 , TRUE } ,
    {WinNTAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\Clients\\StartMenuInternet") , NULL , REG_SZ , (const PBYTE)_T("setask.exe") , 0 , TRUE } ,

    {WinNTAll , HKEY_CLASSES_ROOT , _T("Http\\shell") , NULL, REG_SZ , (const PBYTE)_T("open") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Http\\shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%IEPathName%%\" %1") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Http\\shell\\open\\ddeexec\\Application") , NULL, REG_SZ , (const PBYTE)_T("IExplore") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Http\\shell\\open\\ddeexec\\Applicationr") , NULL, REG_SZ , (const PBYTE)_T("IExplore") , 0 } ,

    {WinNTAll , HKEY_CLASSES_ROOT , _T("Https\\shell") , NULL, REG_SZ , (const PBYTE)_T("open") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Https\\shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%IEPathName%%\" %1") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Https\\shell\\open\\ddeexec\\Application") , NULL, REG_SZ , (const PBYTE)_T("IExplore") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Https\\shell\\open\\ddeexec\\Applicationr") , NULL, REG_SZ , (const PBYTE)_T("IExplore") , 0 } ,

    {WinNTAll , HKEY_CLASSES_ROOT , _T("Htmlfile\\shell") , NULL, REG_SZ , (const PBYTE)_T("open") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Htmlfile\\shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%IEPathName%%\" %1") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Htmlfile\\shell\\open\\ddeexec\\Application") , NULL, REG_SZ , (const PBYTE)_T("IExplore") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Htmlfile\\shell\\open\\ddeexec\\Applicationr") , NULL, REG_SZ , (const PBYTE)_T("IExplore") , 0 } ,

    {WinNTAll , HKEY_CLASSES_ROOT , _T("Mhtmlfile\\shell") , NULL, REG_SZ , (const PBYTE)_T("open") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Mhtmlfile\\shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%IEPathName%%\" %1") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Mhtmlfile\\shell\\open\\ddeexec\\Application") , NULL, REG_SZ , (const PBYTE)_T("IExplore") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Mhtmlfile\\shell\\open\\ddeexec\\Applicationr") , NULL, REG_SZ , (const PBYTE)_T("IExplore") , 0 } ,

    {WinNTAll , HKEY_CLASSES_ROOT , _T("xmlfile\\shell") , NULL, REG_SZ , (const PBYTE)_T("open") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("xmlfile\\shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%IEPathName%%\" %1") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("xmlfile\\shell\\open\\ddeexec\\Application") , NULL, REG_SZ , (const PBYTE)_T("IExplore") , 0 } ,

    {WinNTAll , HKEY_CLASSES_ROOT , _T("file\\shell") , NULL, REG_SZ , (const PBYTE)_T("open") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("file\\shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%IEPathName%%\" %1") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("file\\shell\\open\\ddeexec\\Application") , NULL, REG_SZ , (const PBYTE)_T("IExplore") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("file\\shell\\open\\ddeexec\\Applicationr") , NULL, REG_SZ , (const PBYTE)_T("IExplore") , 0 } ,

    {WinNTAll , HKEY_CLASSES_ROOT , _T("ftp\\shell") , NULL, REG_SZ , (const PBYTE)_T("open") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("ftp\\shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%IEPathName%%\" %1") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("ftp\\shell\\open\\ddeexec\\Application") , NULL, REG_SZ , (const PBYTE)_T("IExplore") , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("ftp\\shell\\open\\ddeexec\\Applicationr") , NULL, REG_SZ , (const PBYTE)_T("IExplore") , 0 } ,

    {WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.AssocFile.HTM\\shell") , NULL, REG_SZ , (const PBYTE)_T("opennew") , 0 } ,
    {WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.AssocFile.HTM\\shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%IEPathName%%\" %1") , 0 } ,
    {WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.AssocFile.HTM\\shell\\opennew\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%IEPathName%%\" %1") , 0 } ,

    {WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.AssocFile.MHT\\shell") , NULL, REG_SZ , (const PBYTE)_T("opennew") , 0 } ,
    {WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.AssocFile.MHT\\shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%IEPathName%%\" %1") , 0 } ,
    {WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.AssocFile.MHT\\shell\\opennew\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%IEPathName%%\" %1") , 0 } ,

    {WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.AssocFile.URL\\shell") , NULL, REG_SZ , (const PBYTE)_T("opennew") , 0 } ,
    {WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.AssocFile.URL\\shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%IEPathName%%\" %1") , 0 } ,
    {WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.AssocFile.URL\\shell\\opennew\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%IEPathName%%\" %1") , 0 } ,

    {WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.FTP\\shell") , NULL, REG_SZ , (const PBYTE)_T("open") , 0 } ,
    {WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.FTP\\shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%IEPathName%%\" %1") , 0 } ,

    {WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.HTTP\\shell") , NULL, REG_SZ , (const PBYTE)_T("open") , 0 } ,
    {WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.HTTP\\shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%IEPathName%%\" %1") , 0 } ,

    {WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.HTTPS\\shell") , NULL, REG_SZ , (const PBYTE)_T("open") , 0 } ,
    {WinVistaAll , HKEY_CLASSES_ROOT , _T("IE.HTTPS\\shell\\open\\command") , NULL, REG_SZ , (const PBYTE)_T("\"%%IEPathName%%\" %1") , 0 } ,

    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\ftp\\UserChoice") , _T("Progid"), REG_SZ , (const PBYTE)_T("IE.FTP") , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\http\\UserChoice") , _T("Progid"), REG_SZ , (const PBYTE)_T("IE.HTTP") , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\https\\UserChoice") , _T("Progid"), REG_SZ , (const PBYTE)_T("IE.HTTPS") , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.htm\\UserChoice") , NULL, REG_SZ , NULL , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.htm\\UserChoice") , _T("Progid"), REG_SZ , (const PBYTE)_T("IE.AssocFile.HTM") , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.html\\UserChoice") , NULL, REG_SZ , NULL , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.html\\UserChoice") , _T("Progid"), REG_SZ , (const PBYTE)_T("IE.AssocFile.HTM") , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.xml\\UserChoice") , NULL, REG_SZ , NULL , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.xml\\UserChoice") , _T("Progid"), REG_SZ , (const PBYTE)_T("IE.AssocFile.HTM") , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.mhtml\\UserChoice") , NULL, REG_SZ , NULL , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.mhtml\\UserChoice") , _T("Progid"), REG_SZ , (const PBYTE)_T("IE.AssocFile.HTM") , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.mht\\UserChoice") , NULL, REG_SZ , NULL , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.mht\\UserChoice") , _T("Progid"), REG_SZ , (const PBYTE)_T("IE.AssocFile.HTM") , 0 } ,

};

const REGSTRUCT CleanKey[] = {

    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.htm\\UserChoice") ,NULL, REG_SZ , NULL , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.html\\UserChoice") ,NULL, REG_SZ , NULL , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.xml\\UserChoice") ,NULL, REG_SZ , NULL , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.mhtml\\UserChoice") , NULL, REG_SZ , NULL , 0 } ,
    {WinVistaAll , HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.mht\\UserChoice") , NULL, REG_SZ , NULL , 0 } ,

    {WinNTAll , HKEY_CLASSES_ROOT , _T("Http\\shell\\SogouExplorer\\Command") , NULL, REG_SZ , NULL , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Http\\shell\\SogouExplorer") , NULL, REG_SZ , NULL , 0 } ,

    {WinNTAll , HKEY_CLASSES_ROOT , _T("Https\\shell\\SogouExplorer\\Command") , NULL, REG_SZ , NULL , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Https\\shell\\SogouExplorer") , NULL, REG_SZ , NULL , 0 } ,

    {WinNTAll , HKEY_CLASSES_ROOT , _T("Htmlfile\\shell\\SogouExplorer\\Command") , NULL, REG_SZ , NULL , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Htmlfile\\shell\\SogouExplorer") , NULL, REG_SZ , NULL , 0 } ,

    {WinNTAll , HKEY_CLASSES_ROOT , _T("Mhtmlfile\\shell\\SogouExplorer\\Command") , NULL, REG_SZ , NULL , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("Mhtmlfile\\shell\\SogouExplorer") , NULL, REG_SZ , NULL , 0 } ,

    {WinNTAll , HKEY_CLASSES_ROOT , _T("xmlfile\\shell\\SogouExplorer\\Command") , NULL, REG_SZ , NULL , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("xmlfile\\shell\\SogouExplorer") , NULL, REG_SZ , NULL , 0 } ,

    {WinNTAll , HKEY_CLASSES_ROOT , _T("file\\shell\\SogouExplorer\\Command") , NULL, REG_SZ , NULL , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("file\\shell\\SogouExplorer") , NULL, REG_SZ , NULL , 0 } ,

    {WinNTAll , HKEY_CLASSES_ROOT , _T("ftp\\shell\\SogouExplorer\\Command") , NULL, REG_SZ , NULL , 0 } ,
    {WinNTAll , HKEY_CLASSES_ROOT , _T("ftp\\shell\\SogouExplorer") , NULL, REG_SZ , NULL , 0 } ,

    {WinVistaAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\RegisteredApplications\\SogouExplorer") , NULL , REG_SZ ,NULL , 0 } ,

    {WinNTAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\Clients\\StartMenuInternet\\SogouExplorer.exe\\Shell\\open\\command") , NULL , REG_SZ , NULL , 0 } ,
    {WinNTAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\Clients\\StartMenuInternet\\SogouExplorer.exe\\Shell\\open") , NULL , REG_SZ , NULL , 0 } ,
    {WinNTAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\Clients\\StartMenuInternet\\SogouExplorer.exe\\Shell") , NULL , REG_SZ , NULL , 0 } ,
    {WinNTAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\Clients\\StartMenuInternet\\SogouExplorer.exe") , NULL , REG_SZ , NULL , 0 } ,

    {WinVistaAll , HKEY_CLASSES_ROOT , _T("SogouExplorer.AssocFile.HTM\\Shell\\open\\command") , NULL, REG_SZ , NULL , 0 } ,
    {WinVistaAll , HKEY_CLASSES_ROOT , _T("SogouExplorer.AssocFile.HTM\\Shell\\open") , NULL, REG_SZ , NULL , 0 } ,
    {WinVistaAll , HKEY_CLASSES_ROOT , _T("SogouExplorer.AssocFile.HTM\\Shell") , NULL, REG_SZ , NULL , 0 } ,
    {WinVistaAll , HKEY_CLASSES_ROOT , _T("SogouExplorer.AssocFile.HTM\\DefaultIcon") , NULL, REG_SZ , NULL , 0 } ,
    {WinVistaAll , HKEY_CLASSES_ROOT , _T("SogouExplorer.AssocFile.HTM") , NULL, REG_NONE , NULL , 0 } ,

    {WinVistaAll , HKEY_CLASSES_ROOT , _T("SogouExplorer.HTTP\\DefaultIcon") , NULL, REG_SZ , NULL , 0 } ,
    {WinVistaAll , HKEY_CLASSES_ROOT , _T("SogouExplorer.HTTP\\Shell\\open\\command") , NULL, REG_SZ , NULL , 0 } ,
    {WinVistaAll , HKEY_CLASSES_ROOT , _T("SogouExplorer.HTTP\\Shell\\open") , NULL, REG_SZ , NULL , 0 } ,
    {WinVistaAll , HKEY_CLASSES_ROOT , _T("SogouExplorer.HTTP\\Shell") , NULL, REG_SZ , NULL , 0 } ,
    {WinVistaAll , HKEY_CLASSES_ROOT , _T("SogouExplorer.HTTP") , NULL, REG_SZ , NULL , 0 } ,

    {WinVistaAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\SogouExplorer\\Capabilities\\FileAssociations") , NULL , REG_SZ ,NULL , 0 } ,
    {WinVistaAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\SogouExplorer\\Capabilities\\Startmenu") , NULL , REG_SZ , NULL , 0 } ,
    {WinVistaAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\SogouExplorer\\Capabilities\\UrlAssociations") , NULL , REG_SZ , NULL , 0 } ,
    {WinVistaAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\SogouExplorer\\Capabilities") , NULL , REG_SZ , NULL ,0 } ,
    //{WinVistaAll , HKEY_LOCAL_MACHINE , _T("SOFTWARE\\SogouExplorer") , NULL , REG_NONE , NULL , 0 } ,别删除这个太暴力了

} ;



#pragma warning(disable : 4996)

CDWDefaultExplorer::CDWDefaultExplorer()
{
    m_dwServiceID = -1 ;
}

CDWDefaultExplorer::~CDWDefaultExplorer()
{
}

BOOL CDWDefaultExplorer::SetSogouDefault()
{
    if (!TidyFileRelation())
    {
        OutputDebugString(_T("TidyFileRelation failed.")) ;
        return FALSE ;
    }
    if (!SetSogouCommand())
    {
        return FALSE ;
    }

    return TRUE ;
}

BOOL CDWDefaultExplorer::SetIEDefault()
{
    if (!TidyFileRelation())
    {	
        return FALSE ;
    }
    if (!SetIECommand())
    {
        return FALSE ;
    }

    return TRUE ;
}

BOOL CDWDefaultExplorer::TidyFileRelation()
{
    // 	if ( IsSogouDefault() )
    // 		return FALSE ;

    CRegKey RegKey ;
    eOS os = ConvertOSVersion(GetOSVersion()) ;
    int iCount = _countof(CleanKey) ;

    for ( int i=0;i<iCount;i++ )
    {
        const REGSTRUCT& Reg = CleanKey[i] ;
        if ( os&Reg.dwOSVersion )
        {
            LRESULT lRes ;
            if ( NULL == Reg.strValueName || '\0'==Reg.strValueName[0] )
            {
                // 删除项
                lRes = RegKey.Open(Reg.hRootKey , NULL , KEY_ALL_ACCESS) ;
                if ( ERROR_SUCCESS == lRes )
                {
                    RegKey.DeleteSubKey(Reg.strPath) ;
                    RegKey.Close() ;
                }
                else
                {
                    CString str;
                    str.Format(_T("Open key [%s] failed"),Reg.strPath) ;
                    OutputDebugString(str);
                    return FALSE ;
                }
            }
            else
            {
                // 删除值
                lRes = RegKey.Open(Reg.hRootKey , Reg.strPath) ;
                if ( ERROR_SUCCESS == lRes )
                {
                    RegKey.DeleteSubKey(Reg.strValueName) ;
                    RegKey.Close() ;
                }
                else
                    return FALSE ;
            }// end if
        }// end if
    }// end for

    return TRUE ;
}

eOS CDWDefaultExplorer::ConvertOSVersion(DWORD dwVersion)
{
    switch ( dwVersion )
    {
    case WINDOWS_2000:
        return Win2K ;
    case WINDOWS_XP_NO_SP:
        return WinXP;
    case WINDOWS_XP_SP_1:
        return WinXPsp1;
    case WINDOWS_XP_SP_2:
        return WinXPsp2;
    case WINDOWS_XP_SP_3:
        return WinXPsp3;
    case WINDOWS_2003:
        return Win2K3;
    case WINDOWS_VISTA_NO_SP:
        return WinVista;
    case WINDOWS_VISTA_SP_1:
        return WinVistaSp1;
    default:
        return WinUnKnown ;
    }
}

BOOL CDWDefaultExplorer::IsAdmin()
{
    BOOL bAdmin = TRUE;
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    PSID AdministratorsGroup; 
    bAdmin = AllocateAndInitializeSid(
        &NtAuthority,
        2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &AdministratorsGroup); 

    if(bAdmin) 
    {
        if (!CheckTokenMembership( NULL, AdministratorsGroup, &bAdmin)) 
            bAdmin = FALSE;

        FreeSid(AdministratorsGroup); 
    }

    return bAdmin;
}

BOOL CDWDefaultExplorer::SetSogouCommand()
{
    CRegKey RegKey ;
    eOS os = ConvertOSVersion(GetOSVersion()) ;
    int iCount = _countof(RegSetSogouDefault) ;
    TCHAR ModulePathName[1024] , drv[8] , path[MAX_PATH];
    GetModuleFileName(NULL , ModulePathName , 1024) ;
    _tsplitpath_s(ModulePathName , drv , 8 , path , MAX_PATH , NULL, 0 , NULL , 0 );
    const CString csPathName = ModulePathName ;
    const CString csPath = CString(drv)+CString(path) ;
    CString csValue ;
    for ( int i=0;i<iCount;i++ )
    {
        const REGSTRUCT& SetReg = RegSetSogouDefault[i] ;
        if ( os&SetReg.dwOSVersion )
        {
            LONG lRes = RegKey.Open(SetReg.hRootKey , SetReg.strPath) ;
            if ( ERROR_FILE_NOT_FOUND == lRes )
            {
                RegKey.Create(SetReg.hRootKey , SetReg.strPath) ;
                lRes = RegKey.Open(SetReg.hRootKey , SetReg.strPath) ;
            }

            if( ERROR_SUCCESS == lRes )
            {
                if ( SetReg.dwValueType != REG_NONE )
                {
                    PBYTE pValue = (PBYTE)SetReg.pValues ;
                    DWORD dwValueSize = SetReg.dwValueSize ;
                    if ( REG_SZ == SetReg.dwValueType
                        || REG_MULTI_SZ == SetReg.dwValueType)
                    {
                        csValue = SetReg.pValues?(LPCTSTR)SetReg.pValues:L"";
                        csValue.Replace(_T("%%SogouPath%%") , csPath);
                        csValue.Replace(_T("%%SogouPathName%%") , csPathName ) ;
                        pValue = (PBYTE)(LPCTSTR)csValue ;
                        dwValueSize = (csValue.GetLength()+1)*sizeof(TCHAR) ;
                    }
                    lRes = RegKey.SetValue(SetReg.strValueName , SetReg.dwValueType , pValue , dwValueSize ) ;

                    if ( ERROR_SUCCESS != lRes && !SetReg.bIgnoreFail )
                    {	// 设置注册表值失败
                        return FALSE ;
                    }
                }
                RegKey.Close() ;
            }
            else if( !SetReg.bIgnoreFail )
            {	// 操作失败
                return FALSE ;
            }
        }
    }
    return TRUE ;
}

BOOL CDWDefaultExplorer::SetIECommand()
{
    CRegKey RegKey ;
    eOS os = ConvertOSVersion(GetOSVersion()) ;
    int iCount = _countof(RegSetIEDefault) ;
    TCHAR IEPath[MAX_PATH];

    ::ExpandEnvironmentStrings(_T("%ProgramFiles%\\Internet Explorer\\") , IEPath , MAX_PATH) ;
    const CString csIEPath = IEPath ;
    const CString csIEPathName = csIEPath + _T("setask.exe")  ;

    CString csValue ;
    for ( int i=0;i<iCount;i++ )
    {
        const REGSTRUCT& SetReg = RegSetIEDefault[i] ;
        if ( os&SetReg.dwOSVersion )
        {
            LONG lRes = RegKey.Open(SetReg.hRootKey , SetReg.strPath) ;
            if ( ERROR_FILE_NOT_FOUND == lRes )
            {
                RegKey.Create(SetReg.hRootKey , SetReg.strPath) ;
                lRes = RegKey.Open(SetReg.hRootKey , SetReg.strPath) ;
            }

            if( ERROR_SUCCESS == lRes )
            {
                if ( SetReg.dwValueType != REG_NONE )
                {
                    PBYTE pValue = (PBYTE)SetReg.pValues ;
                    DWORD dwValueSize = SetReg.dwValueSize ;
                    if ( REG_SZ == SetReg.dwValueType
                        || REG_MULTI_SZ == SetReg.dwValueType)
                    {
                        csValue = SetReg.pValues?(LPCTSTR)SetReg.pValues:L"";
                        csValue.Replace(_T("%%IEPath%%") , csIEPath);
                        csValue.Replace(_T("%%IEPathName%%") , csIEPathName ) ;
                        pValue = (PBYTE)(LPCTSTR)csValue ;
                        dwValueSize = (csValue.GetLength()+1)*sizeof(TCHAR) ;
                    }
                    lRes = RegKey.SetValue(SetReg.strValueName , SetReg.dwValueType , pValue , dwValueSize ) ;

                    if ( ERROR_SUCCESS != lRes && !SetReg.bIgnoreFail )
                    {	// 设置注册表值失败
                        CString str;
                        str.Format(_T("Set key [%s] value [%s] failed."),SetReg.strPath,csValue) ;
                        OutputDebugString(str);
                        return FALSE ;
                    }
                    else
                    {
                        CString str;
                        str.Format(_T("Set key [%s] value [%s] succeed."),SetReg.strPath,csValue) ;
                        OutputDebugString(str);
                    }

                }
                RegKey.Close() ;
            }
            else if (!SetReg.bIgnoreFail)
            {	// 操作失败
                CString str;
                str.Format(_T("Open key [%s] failed."),SetReg.strPath) ;
                OutputDebugString(str);
                return FALSE ;
            }
        }
    }
    return TRUE ;
}

BOOL CDWDefaultExplorer::CompareValueAtClassesRoot(const CString& cstrRoute, const WCHAR* wszModulePath)
{
    if (cstrRoute.IsEmpty() || !wszModulePath || !wszModulePath[0])
    {
        return FALSE ;
    }

    CRegKey oRegFolder ;
    if (ERROR_SUCCESS != oRegFolder.Open(HKEY_CLASSES_ROOT, cstrRoute))
    {
        return FALSE ;
    }

    int nCount = 0;
    WCHAR wszBuffer[100] = {0};
    DWORD dwBufferLen= _countof(wszBuffer);
    CString csModulePath(wszModulePath) ;
    csModulePath.MakeLower() ;

    CRegKey oRegCommand ;

    if ( ERROR_SUCCESS == oRegFolder.QueryStringValue(NULL , wszBuffer , &dwBufferLen) )
    {
        if ( StrCmpI(wszBuffer,L"SogouExplorer")==0 )
            nCount++;
    }

    if (ERROR_SUCCESS == oRegCommand.Create(oRegFolder, L"open\\command"))
    {		
        dwBufferLen = _countof(wszBuffer) ;
        if (ERROR_SUCCESS == oRegCommand.QueryStringValue(NULL,  wszBuffer, &dwBufferLen))
        {
            ATL::CString strBuf = wszBuffer;
            strBuf.MakeLower();
            if (StrStr(strBuf, csModulePath))
                nCount++;
        }
        oRegCommand.Close() ;
    }

    if (ERROR_SUCCESS == oRegCommand.Create(oRegFolder, L"SogouExplorer\\Command"))
    {
        dwBufferLen = _countof(wszBuffer) ;
        if (ERROR_SUCCESS == oRegCommand.QueryStringValue(NULL, wszBuffer, &dwBufferLen))
        {
            ATL::CString strBuf = wszBuffer;
            strBuf.MakeLower();
            if (StrStr(strBuf, csModulePath))
                nCount++;
        }

        oRegCommand.Close() ;
    }	

    oRegFolder.Close();

    return nCount==3?TRUE : FALSE;

}


bool CDWDefaultExplorer::IsSogouDefault()
{
    bool bRet = false;

    eOS os = ConvertOSVersion(GetOSVersion()) ;

    if ( os&WinVistaAll )
    {
        CRegKey RegKey;
        WCHAR wszBuffer[100] = {0};
        DWORD dwBufferLen= _countof(wszBuffer);
        if ( ERROR_SUCCESS == RegKey.Open(HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\http\\UserChoice") , KEY_QUERY_VALUE) )
        {
            if ( ERROR_SUCCESS == RegKey.QueryStringValue(_T("Progid") , wszBuffer , &dwBufferLen) 
                && StrCmpI(wszBuffer , _T("SogouExplorer.HTTP"))!=0)
            {
                return false;
            }
        }
        dwBufferLen = _countof(wszBuffer) ;
        if ( ERROR_SUCCESS == RegKey.Open(HKEY_CURRENT_USER , _T("Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\https\\UserChoice") , KEY_QUERY_VALUE) )
        {
            if ( ERROR_SUCCESS == RegKey.QueryStringValue(_T("Progid" ) , wszBuffer , &dwBufferLen) 
                && StrCmpI(wszBuffer , _T("SogouExplorer.HTTP"))!=0 )
            {
                return false;
            }
        }
    }// end if

    WCHAR pszAppPath[MAX_PATH] = { 0 } ;
    ::GetModuleFileName(NULL, pszAppPath, MAX_PATH) ;
    if ( CompareValueAtClassesRoot(L"http\\shell", pszAppPath)
        && CompareValueAtClassesRoot(L"https\\shell", pszAppPath)
        && CompareValueAtClassesRoot(L"htmlfile\\shell", pszAppPath)
        && CompareValueAtClassesRoot(L"mhtmlfile\\shell", pszAppPath)
        && CompareValueAtClassesRoot(L"file\\shell", pszAppPath))
    {
        bRet = true;
    }		

    return bRet;
}



long GetOSVersion()
{
    OSVERSIONINFO os = { 0 } ;
    os.dwOSVersionInfoSize = sizeof(os) ;

    if (!GetVersionEx(&os))
    {
        return WINDOWS_OLDEST ;
    }
    if (VER_PLATFORM_WIN32_WINDOWS == os.dwPlatformId)
    {
        return WINDOWS_OLDEST ;
    }
    if (VER_PLATFORM_WIN32_NT == os.dwPlatformId)
    {
        if (4 >= os.dwMajorVersion)
        {
            return WINDOWS_OLDEST ;
        }
        if (5 == os.dwMajorVersion && 0 == os.dwMinorVersion)
        {
            return WINDOWS_2000 ;
        }
        if (5 == os.dwMajorVersion && 1 == os.dwMinorVersion)
        {
            if (!StrCmpNI(L"Service Pack 1", os.szCSDVersion, 127))
            {
                return WINDOWS_XP_SP_1 ;
            }
            if (!StrCmpNI(L"Service Pack 2", os.szCSDVersion, 127))
            {
                return WINDOWS_XP_SP_2 ;
            }
            if (!StrCmpNI(L"Service Pack 3", os.szCSDVersion, 127))
            {
                return WINDOWS_XP_SP_3 ;
            }
            return WINDOWS_XP_NO_SP ;
        }
        if (5 == os.dwMajorVersion && 2 == os.dwMinorVersion)
        {
            return WINDOWS_2003 ;
        }
        if (6 == os.dwMajorVersion)
        {
            if (!StrCmpNI(L"Service Pack 1", os.szCSDVersion, 127))
            {
                return WINDOWS_VISTA_SP_1 ;
            }
            return WINDOWS_VISTA_NO_SP ;
        }
    }

    return WINDOWS_OLDEST ;
}

