#include "stdafx.h"
#include "iTreeSurf.h"
#include "LanguageTable.h"


////////////////////////////////////////////////////////////////////////////
// CLanguageTable Contruction

CLanguageTable::CLanguageTable()
{
	Initial();
}

CLanguageTable::~CLanguageTable()
{

}


void CLanguageTable::Initial()
{
	TCHAR szfname[MAX_PATH];
	DWORD nSize = MAX_PATH;
	int i=0;

    GetModuleFileName( NULL , szfname , nSize );
    for( i = _tcslen( szfname );i >= 0;i -- )
    {
        if( szfname[i] == '\\' )
            break;
    }
	szfname[i + 1] = 0;
    _tcscat( szfname , _LANGUAGE_INI_FILE_ );
	_csLanguageConfigFile = szfname;

	szfname[0] = 0;
    GetModuleFileName( NULL , szfname , nSize );
    for( i = _tcslen( szfname );i >= 0;i -- )
    {
        if( szfname[i] == '\\' )
            break;
    }
	szfname[i + 1] = 0;
    _tcscat( szfname , _RELATIVE_LANGUAGE_ROOT_ );
	_csLanguageRootFolder = szfname;

	_csLanguageName = "";
	_csLanguageFolder = "";
	_csLanguageDllFile = "";
}

bool CLanguageTable::ReadFromFile( )
{

	TCHAR szCurrentLang[256];
	GetPrivateProfileString(
		_INI_SECTION_CURRENT,
		_INI_VAR_CURRENT_LANG,
		"",
		(char*)szCurrentLang,
		256,
		_csLanguageConfigFile );

	CString csCurrentLang = szCurrentLang;
	if( csCurrentLang.IsEmpty() )
	{
		//Autoselect language when no language set
		LANGID langID = GetUserDefaultLangID();
		switch( PRIMARYLANGID(langID) )
		{
		case LANG_CHINESE:
			switch( SUBLANGID(langID) )
			{
			case SUBLANG_CHINESE_TRADITIONAL:
				csCurrentLang = "CHINESE_T";
				break;

			case SUBLANG_CHINESE_SIMPLIFIED:
			default:
				csCurrentLang = "CHINESE";
				break;
			}
			break;

		case LANG_RUSSIAN:
			csCurrentLang = "RUSSIAN";
			break;

		case LANG_SPANISH:
			csCurrentLang = "SPANISH";
			break;

		default:
			csCurrentLang = "ENGLISH";
			break;
		}

	}

	TCHAR szLangName[256];
	GetPrivateProfileString(
		csCurrentLang,
		_INI_VAR_LANG_NAME,
		"",
		(char*)szLangName,
		256,
		_csLanguageConfigFile );

	TCHAR szLangFolder[256];
	GetPrivateProfileString(
		csCurrentLang,
		_INI_VAR_LANG_FOLDER,
		"",
		(char*)szLangFolder,
		256,
		_csLanguageConfigFile );

	TCHAR szLangDll[256];
	GetPrivateProfileString(
		csCurrentLang,
		_INI_VAR_LANG_DLL,
		"",
		(char*)szLangDll,
		256,
		_csLanguageConfigFile );

	_csLanguageName = szLangName;
	_csLanguageFolder = szLangFolder;
	_csLanguageDllFile = szLangDll;

	if( 
		_csLanguageName.IsEmpty()
		||
		_csLanguageFolder.IsEmpty()
		||
		_csLanguageDllFile.IsEmpty()
		)
	{
		_csLanguageName = "";
		_csLanguageFolder = "";
		_csLanguageDllFile = "";
		return false;
	}

	CString csDll = _csLanguageRootFolder + _csLanguageFolder;
	csDll += "\\";
	csDll += _csLanguageDllFile;
	csDll += ".dll";
	_csLanguageDllFile = csDll;

	return true;
}


void CLanguageTable::WriteToFile()
{
}

