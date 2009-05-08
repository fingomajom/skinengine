
#if !defined( _ITREESURF__LANGUAGE_H__INCLUDED_ )
#define _ITREESURF__LANGUAGE_H__INCLUDED_

#include "iTreeSurfDefinition.h"

////////////////////////////////////////////////////////////////////////////
// CLanguageTable

class CLanguageTable
{
// Construction
public:
	CLanguageTable();
	~CLanguageTable();

	bool ReadFromFile();
	void WriteToFile();
	void Initial();

  
// Attributes
public:
	CString _csLanguageConfigFile;
	CString _csLanguageRootFolder;

	CString _csLanguageName;
	CString _csLanguageFolder;
	CString _csLanguageDllFile;


// Implementation
protected:
};

#endif // !defined( _ITREESURF__LANGUAGE_H__INCLUDED_ )
