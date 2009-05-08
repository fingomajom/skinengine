
#pragma once

class CDWMenuOrderItem
{
public:
	CDWMenuOrderItem()
	{
		longname = NULL;
		shortname = NULL;
	};
	~CDWMenuOrderItem()
	{
		if(longname)
			delete[] longname;
		if(shortname)
			delete[] shortname;
	};

	int cbsize;
	int order;
	WORD len;
	WORD itemtype;
	int filesize;
	WORD filedate;
	WORD filetime;
	WORD filetype;
	WCHAR * longname;
	WCHAR * shortname;
};

class CDWMenuOrder
{
public:
	CDWMenuOrder()
	{
		mois = NULL;
		index = NULL;
	};
	~CDWMenuOrder()
	{
		if(mois)
			delete[] mois;
		if(index)
			delete[] index;
	};

	BOOL LoadMenuOrder(LPCTSTR sFolderPath); 
	BOOL WriteMenuOrder(LPCTSTR sFolderPath);

    int  GetOrder( LPCTSTR pszName ) const;

	int cbsize;
	int reserved; 
	int len;
	CDWMenuOrderItem * mois; //array of menu item
	int * index;
    ATL::CString lpSubKey;
};
