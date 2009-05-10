#include "stdafx.h"
#include "DWMenuorder.h"


#define MenuOrderKey_Root L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MenuOrder\\Favorites"


DWORD GetDWORD(LPBYTE& lpb, LPBYTE lpend)
{
	if(lpb+4>lpend && lpend!=NULL)
		return 0;

	DWORD dw = *((DWORD*)lpb);
	lpb+=4;
	return dw;
}

WORD GetWORD(LPBYTE& lpb, LPBYTE lpend)
{
	if(lpb+2>lpend && lpend!=NULL)
		return 0;

	WORD w = *((WORD*)lpb);
	lpb+=2;
	return w;
}

WCHAR* Get_String(LPBYTE& lpb, LPBYTE lpend)
{
	int sl = lstrlenA((char*)lpb);
	if(lpb+sl>lpend && lpend!=NULL)
		return NULL;

	if(sl>0)
	{
		WCHAR * lptemp;
		__try
		{
			lptemp = new WCHAR[sl+2];
		}
		__except(1)
		{
			lptemp = NULL;
		}

		if(lptemp!=NULL)
		{
            MultiByteToWideChar( CP_OEMCP, 0, 
                (char*)lpb, -1, lptemp, sl + 2 );
		}
		lpb+=sl;
		lpb++;
		return lptemp;
	}
	else
		return NULL;
}

WCHAR* Get_StringW(LPBYTE& lpb, LPBYTE lpend)
{
	int sl = lstrlenW((WCHAR*)lpb);
	if(lpb+(sl*2)>lpend && lpend!=NULL)
		return NULL;

	if(sl>0)
	{
		WCHAR * lptemp;
		__try
		{
			lptemp = new WCHAR[sl+2];
		}
		__except(1)
		{
			return NULL;
		}

		if(lptemp!=NULL)
		{
            ZeroMemory(lptemp, (sl+2)*sizeof(WCHAR) );
			StrCpyN(lptemp, (WCHAR*)lpb, sl+1);
		}
		lpb+=((lstrlenW(lptemp)+1)*2);
		//while(*lpb==0 && (lpb<=lpend || lpend==NULL))
		//	lpb++;
		return lptemp;
	}
	else
		return NULL;
}

void PutWORD(WORD w, LPBYTE& lpb)
{
	*lpb = LOBYTE(w);
	lpb ++;
	*lpb = HIBYTE(w);
	lpb++;
}

void PutDWORD(DWORD dw, LPBYTE& lpb)
{
	PutWORD(LOWORD(dw), lpb);
	PutWORD(HIWORD(dw), lpb);
}

void PutString(WCHAR* lpstr, LPBYTE& lpb)
{
	if(lpstr==NULL)
		return;
	lstrcpyA((char*)lpb, CW2AEX<>(lpstr).m_psz);
	lpb += lstrlenW(lpstr);
	lpb++;
}

int CDWMenuOrder::GetOrder( LPCTSTR pszName ) const
{
    if (!mois)
        return -1;

    for ( int i = 0; i < len; i++ )
    {
        if ( !StrCmpNI(pszName, mois[i].longname, lstrlenW(pszName))) 
            return mois[i].order;
    }

    return -1;
}

BOOL CDWMenuOrder::LoadMenuOrder(LPCTSTR sFolderPath)
{
	if(mois)
		delete[] mois;
	if(index)
		delete[] index;
	mois = NULL;
	index = NULL;
	len = 0;

	HKEY hKey;
	BOOL ret = FALSE;
	lpSubKey = MenuOrderKey_Root;
	lpSubKey += sFolderPath;
	HRESULT hr = RegOpenKey(HKEY_CURRENT_USER, lpSubKey, &hKey);
	if(hr == ERROR_SUCCESS)
	{
		BYTE* lpBuf = NULL;
		DWORD size = 0;
		DWORD type = 0;
		hr = RegQueryValueEx(hKey, L"Order", NULL, &type, NULL, &size);
		if(size>0)
		{
			//__try
			//{
			lpBuf = new BYTE[size];
			//}
			//__except(1)
			//{
			//	RegCloseKey(hKey);
			//	return FALSE;
			//}

			hr = RegQueryValueEx(hKey, L"Order", NULL, &type, lpBuf, &size);

			if(type == REG_BINARY)
			{
				BYTE* lpData = lpBuf;
				lpData += 8;
				cbsize = GetDWORD(lpData, NULL);
				reserved = GetDWORD(lpData, NULL);
				len = GetDWORD(lpData, NULL);
				if(len>0)
				{
					//__try
					{
					    mois = new CDWMenuOrderItem[len];
					    index = new int[len];
					    ZeroMemory(index, len*sizeof(int));
					}
					//__except(1)
					{
					//	len = 0;
					}
				}
				
				int len1, len2;
				for(int i =0; i< len && mois!= NULL; i++)
				{
					BYTE* lpend = lpBuf+size;
					BYTE* lpbt = lpData;
					len1 = mois[i].cbsize = GetDWORD(lpData, lpend);

					lpend = lpbt + len1;
					mois[i].order = GetDWORD(lpData, lpend);

                    len2 = *(WORD*)(lpbt + (len1 - 8));

                    lpData = lpbt+(len2+8+20);

                    mois[i].longname = Get_StringW(lpData, lpend);

					lpData = lpbt + len1;
					ret = TRUE;
				}
			}
			delete[] lpBuf;
		}
		RegCloseKey(hKey);
	}
 
	return ret;
}


BOOL CDWMenuOrder::WriteMenuOrder(LPCTSTR sFolderPath)
{
	//sort dir and files by string
	if(len<=0)
		return FALSE;
	__try
	{
	index = new int[len];
	}
	__except(1)
	{
//		if(e!=NULL)e->Delete();
		return FALSE;
	}

	int i, j;
	index[0]=0;
	for(i = 1; i<len; i++)
	{
		for(j=i-1; j>=0; j--)
		{
			if(StrCmpI(mois[i].longname, mois[index[j]].longname)<0)
			{
				index[j+1]=index[j];
				if(j==0)
					index[j]=i;
			}
			else
			{
				index[j+1]=i;
				break;
			}
		}
	}


	cbsize = 12;
	reserved = 1;
	for(i =0; i<len; i++)
	{
		//calculate length
		if(mois[i].longname!=NULL && mois[i].shortname!=NULL)
			mois[i].cbsize = lstrlenW(mois[i].longname)+1 + lstrlenW(mois[i].shortname)+1;  
		mois[i].cbsize += 22+6;
		cbsize += mois[i].cbsize;
	}

	HKEY hKey;
	BOOL ret = FALSE;
	lpSubKey = MenuOrderKey_Root;
	lpSubKey += sFolderPath;
	HRESULT hr = RegCreateKeyEx(HKEY_CURRENT_USER, lpSubKey, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	int slen;

	if(hr == ERROR_SUCCESS)
	{
		LPBYTE lpbuf;
		__try
		{
			 lpbuf = new BYTE[cbsize + 8];
		}
		__except(1)
		{
//			if(e!=NULL)e->Delete();
			lpbuf = NULL;
		}
		

		if(lpbuf!=NULL)
		{
			LPBYTE lpt = lpbuf;
			PutDWORD(0x08, lpt);
			PutDWORD(0x02, lpt);
			PutDWORD(cbsize, lpt);
			PutDWORD(reserved, lpt);
			PutDWORD(len, lpt);
			for(j=0; j<len; j++)
			{
				i = index[j];
				if(mois[i].itemtype != 0x31)
					continue;
				PutDWORD(mois[i].cbsize, lpt);
				PutDWORD(mois[i].order, lpt);
				slen = 0;
				if(mois[i].longname!=NULL)
					slen = lstrlenW(mois[i].longname);
				if(mois[i].shortname!=NULL)
					slen += lstrlenW(mois[i].shortname);
				PutWORD(slen+1+1+14, lpt);
				PutWORD(mois[i].itemtype, lpt);
				PutDWORD(mois[i].filesize, lpt);
				PutWORD(mois[i].filedate, lpt);
				PutWORD(mois[i].filetime, lpt);
				PutWORD(mois[i].filetype, lpt);
				PutString(mois[i].longname, lpt);
				PutString(mois[i].shortname, lpt);
				PutWORD((WORD)0, lpt); // end with two 0
				PutDWORD((DWORD)0, lpt); //item end
			}
			for(j=0; j<len; j++)
			{
				i = index[j];
				if(mois[i].itemtype != 0x32)
					continue;
				PutDWORD(mois[i].cbsize, lpt);
				PutDWORD(mois[i].order, lpt);
				slen = 0;
				if(mois[i].longname!=NULL)
					slen = lstrlenW(mois[i].longname);
				if(mois[i].shortname!=NULL)
					slen += lstrlenW(mois[i].shortname);
				PutWORD(slen+1+1+14, lpt);
				PutWORD(mois[i].itemtype, lpt);
				PutDWORD(mois[i].filesize, lpt);
				PutWORD(mois[i].filedate, lpt);
				PutWORD(mois[i].filetime, lpt);
				PutWORD(mois[i].filetype, lpt);
				PutString(mois[i].longname, lpt);
				PutString(mois[i].shortname, lpt);
				PutWORD((WORD)0, lpt); // end with two 0
				PutDWORD((DWORD)0, lpt); //item end
			}

			//save
			RegSetValueEx(hKey, L"Order", 0, REG_BINARY, lpbuf, cbsize+8);

			//release
			delete[] lpbuf;

			ret = TRUE;
		}
	}
	RegCloseKey(hKey);

	return ret;
}

