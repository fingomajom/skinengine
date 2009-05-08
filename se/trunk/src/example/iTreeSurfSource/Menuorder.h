#if !defined( _ITREESURF__MENUORDER_H__INCLUDED_ )
#define _ITREESURF__MENUORDER_H__INCLUDED_


class CMenuOrderItem
{
public:
	CMenuOrderItem()
	{
		longname = NULL;
		shortname = NULL;
	};
	~CMenuOrderItem()
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
	char * longname;
	char * shortname;
};

class CMenuOrder
{
public:
	CMenuOrder()
	{
		mois = NULL;
		index = NULL;
	};
	~CMenuOrder()
	{
		if(mois)
			delete[] mois;
		if(index)
			delete[] index;
	};

	BOOL LoadMenuOrder(LPCSTR sFolderPath); //relative path
	BOOL WriteMenuOrder(LPCSTR sFolderPath);
	int cbsize;
	int reserved; 
	int len;
	CMenuOrderItem * mois; //array of menu item
	int * index;
	CString lpSubKey;
};

#endif // !defined( _ITREESURF__MENUORDER_H__INCLUDED_ )
